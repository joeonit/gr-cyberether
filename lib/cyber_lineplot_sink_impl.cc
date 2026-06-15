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

namespace gr {
  namespace cyberether {
    using namespace Jetstream;

    using input_type = gr_complex;

    cyber_lineplot_sink::sptr
    cyber_lineplot_sink::make(size_t buffer_size, const std::string& name)
    {
      return gnuradio::make_block_sptr<cyber_lineplot_sink_impl>(buffer_size, name);
    }

    cyber_lineplot_sink_impl::cyber_lineplot_sink_impl(size_t buffer_size, const std::string& name)
      : gr::sync_block("cyber_lineplot_sink",
              gr::io_signature::make(1 , 1 , sizeof(input_type)),
              gr::io_signature::make(0, 0, 0)),
      d_buffer_size(buffer_size == 0 ? 1 : buffer_size),
      d_name(name),
      d_display_write_ptr(0),
      d_tensor(DeviceType::CPU, TypeToDataType<CF32>(),
               {1, static_cast<U64>(d_buffer_size)})
    {
      JST_INFO("[gr-cyberether] cyber_lineplot_sink '{}' constructed: {} samples.",
               d_name, d_buffer_size);
    }

    cyber_lineplot_sink_impl::~cyber_lineplot_sink_impl()
    {
      cyber_context::instance().unregister_plot(this);
    }

    bool
    cyber_lineplot_sink_impl::start()
    {
      // Register this sink's plot with the shared context. We deliberately do
      // NOT call Superluminal::Plot() here: the grid layout can only be decided
      // once every sink is known, so the actual Plot() (with this sink's grid
      // cell) is issued later by cyber_context::present() on the main thread.
      // This is what lets multiple sinks tile instead of overlapping.
      const Superluminal::PlotConfig config = {
          .buffer    = d_tensor,
          .type      = Superluminal::Type::Line,
          .source    = Superluminal::Domain::Time,
          .display   = Superluminal::Domain::Time,
          .operation = Superluminal::Operation::Real,
      };
      cyber_context::instance().register_plot({ this, d_name, config });

      return sync_block::start();
    }

    bool
    cyber_lineplot_sink_impl::stop()
    {
      cyber_context::instance().unregister_plot(this);
      return sync_block::stop();
    }

    int
    cyber_lineplot_sink_impl::work(int noutput_items,
        gr_vector_const_void_star& input_items,
        gr_vector_void_star& output_items)
    {
      if (noutput_items <= 0) {
          return 0;
      }

      const input_type* in = static_cast<const input_type*>(input_items[0]);
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
      // memory"). cyber_context::present() drives Superluminal::Update() on a
      // single ~60 Hz thread; work() never calls into Superluminal.
      CF32* display = d_tensor.data<CF32>();
      for (size_t i = 0; i < n; ++i) {
          display[d_display_write_ptr] = in[i];
          d_display_write_ptr = (d_display_write_ptr + 1) % d_buffer_size;
      }

      return noutput_items;
    }

  } /* namespace cyberether */
} /* namespace gr */
