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

namespace gr {
  namespace cyberether {
    using namespace Jetstream;

    using input_type = gr_complex;

    cyber_waterfall_sink::sptr
    cyber_waterfall_sink::make(size_t fft_size, const std::string& name, int height)
    {
      return gnuradio::make_block_sptr<cyber_waterfall_sink_impl>(
          fft_size, name, height);
    }

    cyber_waterfall_sink_impl::cyber_waterfall_sink_impl(
        size_t fft_size, const std::string& name, int height)
      : gr::sync_block("cyber_waterfall_sink",
              gr::io_signature::make(1, 1, sizeof(input_type)),
              gr::io_signature::make(0, 0, 0)),
      d_fft_size(fft_size == 0 ? 1 : fft_size),
      d_height(height <= 0 ? 512 : height),
      d_name(name),
      d_write_ptr(0),
      d_tensor(DeviceType::CPU, TypeToDataType<CF32>(),
               {1, static_cast<U64>(d_fft_size)})
    {
      JST_INFO("[gr-cyberether] cyber_waterfall_sink '{}' constructed: "
               "{} samples per row, {} row history.",
               d_name, d_fft_size, d_height);
    }

    cyber_waterfall_sink_impl::~cyber_waterfall_sink_impl()
    {
      cyber_context::instance().unregister_plot(this);
    }

    bool
    cyber_waterfall_sink_impl::start()
    {
      // Same register-don't-Plot pattern as the line sink: Plot() is issued
      // by cyber_context::present() on the main thread, after the mosaic
      // grid is decided across all sinks.
      Superluminal::PlotConfig config = {
          .buffer    = d_tensor,
          .type      = Superluminal::Type::Waterfall,
          .source    = Superluminal::Domain::Time,        // we feed time samples
          .display   = Superluminal::Domain::Frequency,   // Superluminal does the FFT
          .operation = Superluminal::Operation::Amplitude,
      };
      config.options["height"] = static_cast<I32>(d_height);
      cyber_context::instance().register_plot({ this, d_name, config });
      return sync_block::start();
    }

    bool
    cyber_waterfall_sink_impl::stop()
    {
      cyber_context::instance().unregister_plot(this);
      return sync_block::stop();
    }

    int
    cyber_waterfall_sink_impl::work(int noutput_items,
        gr_vector_const_void_star& input_items,
        gr_vector_void_star& /*output_items*/)
    {
      if (noutput_items <= 0) {
          return 0;
      }

      const input_type* in = static_cast<const input_type*>(input_items[0]);
      size_t n = static_cast<size_t>(noutput_items);

      // Visualisation: keep only the newest fft_size samples if GR hands us a
      // bigger chunk. We always consume everything (no back-pressure on the
      // flowgraph) and write into the buffer as a rolling window. Superluminal
      // FFTs the buffer on each Update() and appends one row to its internal
      // waterfall history.
      if (n > d_fft_size) {
          in += n - d_fft_size;
          n = d_fft_size;
      }

      CF32* display = d_tensor.data<CF32>();
      for (size_t i = 0; i < n; ++i) {
          display[d_write_ptr] = in[i];
          d_write_ptr = (d_write_ptr + 1) % d_fft_size;
      }

      return noutput_items;
    }

  } /* namespace cyberether */
} /* namespace gr */
