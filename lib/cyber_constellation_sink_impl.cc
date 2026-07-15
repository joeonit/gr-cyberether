/* -*- c++ -*- */
/*
 * Copyright 2026 Youssef Mahmoud.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "cyber_constellation_sink_impl.h"
#include <gnuradio/cyberether/cyber_context.h>
#include <jetstream/logger.hh>

namespace gr {
  namespace cyberether {
    using namespace Jetstream;

    cyber_constellation_sink::sptr
    cyber_constellation_sink::make(size_t buffer_size, const std::string& name,
                                   const std::string& gui_hint)
    {
      return gnuradio::make_block_sptr<cyber_constellation_sink_impl>(
          buffer_size, name, gui_hint);
    }

    cyber_constellation_sink_impl::cyber_constellation_sink_impl(
        size_t buffer_size, const std::string& name, const std::string& gui_hint)
      : gr::sync_block("cyber_constellation_sink",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(0, 0, 0)),
      d_buffer_size(buffer_size == 0 ? 1 : buffer_size),
      d_name(name),
      d_gui_hint(gui_hint),
      d_display_write_ptr(0),
      d_tensor(DeviceType::CPU, TypeToDataType<CF32>(),
               {1, static_cast<U64>(d_buffer_size)})
    {
      JST_INFO("[gr-cyberether] cyber_constellation_sink '{}' constructed: {} points.",
               d_name, d_buffer_size);
    }

    cyber_constellation_sink_impl::~cyber_constellation_sink_impl()
    {
      cyber_context::instance().unregister_plot(this);
    }

    bool
    cyber_constellation_sink_impl::start()
    {
      // Register only; cyber_context::present() issues the actual Plot() once
      // every sink is known. A constellation is always Time/Time.
      const Superluminal::PlotConfig config = {
          .buffer  = d_tensor,
          .type    = Superluminal::Type::Scatter,
          .source  = Superluminal::Domain::Time,
          .display = Superluminal::Domain::Time,
      };
      cyber_context::instance().register_plot({ this, d_name, config, d_gui_hint });

      return sync_block::start();
    }

    bool
    cyber_constellation_sink_impl::stop()
    {
      cyber_context::instance().unregister_plot(this);
      return sync_block::stop();
    }

    int
    cyber_constellation_sink_impl::work(int noutput_items,
        gr_vector_const_void_star& input_items,
        gr_vector_void_star& /*output_items*/)
    {
      if (noutput_items <= 0) {
          return 0;
      }

      const gr_complex* in = static_cast<const gr_complex*>(input_items[0]);
      size_t n = static_cast<size_t>(noutput_items);

      // Keep only the newest buffer_size samples; always consume everything.
      if (n > d_buffer_size) {
          in += n - d_buffer_size;
          n = d_buffer_size;
      }

      // Rolling write; Superluminal reads the tensor in place.
      CF32* display = d_tensor.data<CF32>();
      for (size_t i = 0; i < n; ++i) {
          display[d_display_write_ptr] = in[i];
          d_display_write_ptr = (d_display_write_ptr + 1) % d_buffer_size;
      }

      return noutput_items;
    }

  } /* namespace cyberether */
} /* namespace gr */
