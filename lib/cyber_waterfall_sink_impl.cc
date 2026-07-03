/* -*- c++ -*- */
/*
 * Copyright 2026 Youssef Mahmoud.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "cyber_waterfall_sink_impl.h"
#include <gnuradio/cyberether/cyber_context.h>
#include <jetstream/logger.hh>

#include <type_traits>

namespace gr {
  namespace cyberether {
    using namespace Jetstream;

    template <typename T>
    static const char* block_name()
    {
        if constexpr (std::is_same_v<T, gr_complex>) return "cyber_waterfall_sink_c";
        else if constexpr (std::is_same_v<T, float>) return "cyber_waterfall_sink_f";
        else                                          return "cyber_waterfall_sink";
    }

    template <typename T>
    typename cyber_waterfall_sink<T>::sptr
    cyber_waterfall_sink<T>::make(size_t fft_size, const std::string& name,
                                   int height,
                                   Superluminal::Domain display,
                                   const std::string& gui_hint)
    {
      return gnuradio::make_block_sptr<cyber_waterfall_sink_impl<T>>(
          fft_size, name, height, display, gui_hint);
    }

    template <typename T>
    cyber_waterfall_sink_impl<T>::cyber_waterfall_sink_impl(
        size_t fft_size, const std::string& name, int height,
        Superluminal::Domain display, const std::string& gui_hint)
      : gr::sync_block(block_name<T>(),
              gr::io_signature::make(1, 1, sizeof(T)),
              gr::io_signature::make(0, 0, 0)),
      d_fft_size(fft_size == 0 ? 1 : fft_size),
      d_height(height <= 0 ? 512 : height),
      d_name(name),
      d_display(display),
      d_gui_hint(gui_hint),
      d_write_ptr(0),
      d_tensor(DeviceType::CPU, TypeToDataType<CF32>(),
               {1, static_cast<U64>(d_fft_size)})
    {
      JST_INFO("[gr-cyberether] {} '{}' constructed: "
               "{} samples per row, {} row history.",
               block_name<T>(), d_name, d_fft_size, d_height);
    }

    template <typename T>
    cyber_waterfall_sink_impl<T>::~cyber_waterfall_sink_impl()
    {
      cyber_context::instance().unregister_plot(this);
    }

    template <typename T>
    bool
    cyber_waterfall_sink_impl<T>::start()
    {
      // Same register-don't-Plot pattern as the line sink: Plot() is issued
      // by cyber_context::present() on the main thread, after the mosaic
      // grid is decided across all sinks.
      Superluminal::PlotConfig config = {
          .buffer    = d_tensor,
          .type      = Superluminal::Type::Waterfall,
          .source    = Superluminal::Domain::Time,        // we always feed time samples
          .display   = d_display,                          // user choice (Frequency default)
          // operation hardcoded — Superluminal's waterfall builder ignores
          // it and renders |z| regardless. Set to Amplitude to match.
          .operation = Superluminal::Operation::Amplitude,
      };
      config.options["height"] = static_cast<I32>(d_height);
      cyber_context::instance().register_plot({ this, d_name, config, d_gui_hint });
      return sync_block::start();
    }

    template <typename T>
    bool
    cyber_waterfall_sink_impl<T>::stop()
    {
      cyber_context::instance().unregister_plot(this);
      return sync_block::stop();
    }

    template <typename T>
    int
    cyber_waterfall_sink_impl<T>::work(int noutput_items,
        gr_vector_const_void_star& input_items,
        gr_vector_void_star& /*output_items*/)
    {
      if (noutput_items <= 0) {
          return 0;
      }

      const T* in = static_cast<const T*>(input_items[0]);
      size_t n = static_cast<size_t>(noutput_items);

      // Keep only the newest fft_size samples per work() call. We always
      // consume everything (no back-pressure). The buffer is always CF32;
      // float inputs become CF32{x, 0} via if constexpr and Superluminal's
      // FFT path handles them transparently.
      if (n > d_fft_size) {
          in += n - d_fft_size;
          n = d_fft_size;
      }

      CF32* display = d_tensor.data<CF32>();
      for (size_t i = 0; i < n; ++i) {
          if constexpr (std::is_same_v<T, gr_complex>) {
              display[d_write_ptr] = in[i];
          } else {
              display[d_write_ptr] = CF32{static_cast<float>(in[i]), 0.0f};
          }
          d_write_ptr = (d_write_ptr + 1) % d_fft_size;
      }

      return noutput_items;
    }

    // Explicit instantiations — must match the typedefs in the public header.
    template class cyber_waterfall_sink<gr_complex>;
    template class cyber_waterfall_sink<float>;
    template class cyber_waterfall_sink_impl<gr_complex>;
    template class cyber_waterfall_sink_impl<float>;

  } /* namespace cyberether */
} /* namespace gr */
