/* -*- c++ -*- */
/*
 * Copyright 2026 Youssef Mahmoud.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "cyber_lineplot_sink_impl.h"
#include <gnuradio/cyberether/cyber_context.h>
#include <jetstream/logger.hh>

#include <type_traits>

namespace gr {
  namespace cyberether {
    using namespace Jetstream;

    // Per-T block name: keeps the GR scheduler logs and any block-name lookups
    // distinct between instantiations.
    template <typename T>
    static const char* block_name()
    {
        if constexpr (std::is_same_v<T, gr_complex>) return "cyber_lineplot_sink_c";
        else if constexpr (std::is_same_v<T, float>) return "cyber_lineplot_sink_f";
        else                                          return "cyber_lineplot_sink";
    }

    template <typename T>
    typename cyber_lineplot_sink<T>::sptr
    cyber_lineplot_sink<T>::make(size_t buffer_size, const std::string& name,
                                  Superluminal::Domain display,
                                  const std::string& gui_hint)
    {
      return gnuradio::make_block_sptr<cyber_lineplot_sink_impl<T>>(
          buffer_size, name, display, gui_hint);
    }

    template <typename T>
    cyber_lineplot_sink_impl<T>::cyber_lineplot_sink_impl(
        size_t buffer_size, const std::string& name,
        Superluminal::Domain display, const std::string& gui_hint)
      : gr::sync_block(block_name<T>(),
              gr::io_signature::make(1, 1, sizeof(T)),
              gr::io_signature::make(0, 0, 0)),
      d_buffer_size(buffer_size == 0 ? 1 : buffer_size),
      d_name(name),
      d_display(display),
      d_gui_hint(gui_hint),
      d_display_write_ptr(0),
      d_tensor(DeviceType::CPU, TypeToDataType<CF32>(),
               {1, static_cast<U64>(d_buffer_size)})
    {
      JST_INFO("[gr-cyberether] {} '{}' constructed: {} samples.",
               block_name<T>(), d_name, d_buffer_size);
    }

    template <typename T>
    cyber_lineplot_sink_impl<T>::~cyber_lineplot_sink_impl()
    {
      cyber_context::instance().unregister_plot(this);
    }

    template <typename T>
    bool
    cyber_lineplot_sink_impl<T>::start()
    {
      // Register this sink's plot with the shared context. We deliberately do
      // NOT call Superluminal::Plot() here: the grid layout can only be decided
      // once every sink is known, so the actual Plot() (with this sink's grid
      // cell) is issued later by cyber_context::present() on the main thread.
      // This is what lets multiple sinks tile instead of overlapping.
      const Superluminal::PlotConfig config = {
          .buffer    = d_tensor,
          .type      = Superluminal::Type::Line,
          .source    = Superluminal::Domain::Time,        // always feed time samples
          .display   = d_display,                          // user choice (Time / Frequency)
          // operation is hardcoded — Superluminal's line builder ignores it
          // and renders |z| regardless. Set to Amplitude to match.
          .operation = Superluminal::Operation::Amplitude,
      };
      cyber_context::instance().register_plot({ this, d_name, config, d_gui_hint });

      return sync_block::start();
    }

    template <typename T>
    bool
    cyber_lineplot_sink_impl<T>::stop()
    {
      cyber_context::instance().unregister_plot(this);
      return sync_block::stop();
    }

    template <typename T>
    int
    cyber_lineplot_sink_impl<T>::work(int noutput_items,
        gr_vector_const_void_star& input_items,
        gr_vector_void_star& /*output_items*/)
    {
      if (noutput_items <= 0) {
          return 0;
      }

      const T* in = static_cast<const T*>(input_items[0]);
      size_t n = static_cast<size_t>(noutput_items);

      // For visualization, keep only the newest buffer_size samples if GNU Radio
      // hands us a bigger chunk. We always consume everything, so the sink never
      // back-pressures the flowgraph.
      if (n > d_buffer_size) {
          in += n - d_buffer_size;
          n = d_buffer_size;
      }

      // Write straight into the display tensor as a rolling buffer. Superluminal
      // reads this tensor in place (DMI is a no-op: "data comes from external
      // memory"). The display tensor is always CF32 regardless of input type;
      // float inputs become CF32{x, 0} and are displayed as Operation::Real.
      CF32* display = d_tensor.data<CF32>();
      for (size_t i = 0; i < n; ++i) {
          if constexpr (std::is_same_v<T, gr_complex>) {
              display[d_display_write_ptr] = in[i];
          } else {
              display[d_display_write_ptr] = CF32{static_cast<float>(in[i]), 0.0f};
          }
          d_display_write_ptr = (d_display_write_ptr + 1) % d_buffer_size;
      }

      return noutput_items;
    }

    // Explicit instantiations — must match the typedefs in the public header.
    template class cyber_lineplot_sink<gr_complex>;
    template class cyber_lineplot_sink<float>;
    template class cyber_lineplot_sink_impl<gr_complex>;
    template class cyber_lineplot_sink_impl<float>;

  } /* namespace cyberether */
} /* namespace gr */
