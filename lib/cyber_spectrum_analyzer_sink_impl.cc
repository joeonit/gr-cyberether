/* -*- c++ -*- */
/*
 * Copyright 2026 Youssef Mahmoud.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "cyber_spectrum_analyzer_sink_impl.h"
#include <gnuradio/cyberether/cyber_context.h>
#include <jetstream/logger.hh>

namespace gr {
  namespace cyberether {
    using namespace Jetstream;

    cyber_spectrum_analyzer_sink::sptr
    cyber_spectrum_analyzer_sink::make(size_t fft_size, const std::string& name,
                                       float range_min, float range_max,
                                       int averaging, bool max_hold,
                                       int waterfall_height,
                                       const std::string& gui_hint)
    {
      return gnuradio::make_block_sptr<cyber_spectrum_analyzer_sink_impl>(
          fft_size, name, range_min, range_max, averaging, max_hold,
          waterfall_height, gui_hint);
    }

    cyber_spectrum_analyzer_sink_impl::cyber_spectrum_analyzer_sink_impl(
        size_t fft_size, const std::string& name,
        float range_min, float range_max, int averaging, bool max_hold,
        int waterfall_height, const std::string& gui_hint)
      : gr::sync_block("cyber_spectrum_analyzer_sink",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(0, 0, 0)),
      d_fft_size(fft_size == 0 ? 1 : fft_size),
      d_name(name),
      d_range_min(range_min),
      d_range_max(range_max),
      d_averaging(averaging < 1 ? 1 : averaging),
      d_max_hold(max_hold),
      d_waterfall_height(waterfall_height < 1 ? 1 : waterfall_height),
      d_gui_hint(gui_hint),
      d_write_ptr(0),
      d_tensor(DeviceType::CPU, TypeToDataType<CF32>(),
               {1, static_cast<U64>(d_fft_size)})
    {
      JST_INFO("[gr-cyberether] cyber_spectrum_analyzer_sink '{}' constructed: "
               "{} samples per frame, {} row history.",
               d_name, d_fft_size, d_waterfall_height);
    }

    cyber_spectrum_analyzer_sink_impl::~cyber_spectrum_analyzer_sink_impl()
    {
      cyber_context::instance().unregister_plot(this);
    }

    bool
    cyber_spectrum_analyzer_sink_impl::start()
    {
      // Register only; cyber_context::present() issues the actual Plot() once
      // every sink is known. Superluminal owns the FFT, the averaging, and the
      // waterfall history, so the sink only feeds it time-domain samples.
      Superluminal::PlotConfig config = {
          .buffer  = d_tensor,
          .type    = Superluminal::Type::SpectrumAnalyzer,
          .source  = Superluminal::Domain::Time,
          .display = Superluminal::Domain::Frequency,
      };
      config.options["rangeMin"]        = d_range_min;
      config.options["rangeMax"]        = d_range_max;
      config.options["averaging"]       = static_cast<I32>(d_averaging);
      config.options["maxHold"]         = static_cast<I32>(d_max_hold ? 1 : 0);
      config.options["waterfallHeight"] = static_cast<I32>(d_waterfall_height);

      cyber_context::instance().register_plot({ this, d_name, config, d_gui_hint });

      return sync_block::start();
    }

    bool
    cyber_spectrum_analyzer_sink_impl::stop()
    {
      cyber_context::instance().unregister_plot(this);
      return sync_block::stop();
    }

    int
    cyber_spectrum_analyzer_sink_impl::work(int noutput_items,
        gr_vector_const_void_star& input_items,
        gr_vector_void_star& /*output_items*/)
    {
      if (noutput_items <= 0) {
          return 0;
      }

      const gr_complex* in = static_cast<const gr_complex*>(input_items[0]);
      size_t n = static_cast<size_t>(noutput_items);

      // Keep only the newest fft_size samples; always consume everything.
      if (n > d_fft_size) {
          in += n - d_fft_size;
          n = d_fft_size;
      }

      // Rolling write; Superluminal reads the tensor in place.
      CF32* display = d_tensor.data<CF32>();
      for (size_t i = 0; i < n; ++i) {
          display[d_write_ptr] = in[i];
          d_write_ptr = (d_write_ptr + 1) % d_fft_size;
      }

      return noutput_items;
    }

  } /* namespace cyberether */
} /* namespace gr */
