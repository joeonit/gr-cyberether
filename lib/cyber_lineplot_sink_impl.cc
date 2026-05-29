/* -*- c++ -*- */
/*
 * Copyright 2026 Youssef Mahmoud.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "cyber_lineplot_sink_impl.h"
#include <jetstream/logger.hh>

namespace gr {
  namespace cyberether {
    using namespace Jetstream;

    // gr_complex is std::complex<float>, which is exactly Jetstream::CF32
    using input_type = gr_complex;

    cyber_lineplot_sink::sptr
    cyber_lineplot_sink::make(size_t buffer_size, const std::string& name)
    {
      return gnuradio::make_block_sptr<cyber_lineplot_sink_impl>(buffer_size, name);
    }


    /*
     * The private constructor
     */
    cyber_lineplot_sink_impl::cyber_lineplot_sink_impl(size_t buffer_size, const std::string& name)
      : gr::sync_block("cyber_lineplot_sink",
              gr::io_signature::make(1 , 1 , sizeof(input_type)),
              gr::io_signature::make(0, 0, 0)),
      d_buffer_size(buffer_size),
      d_name(name),
      d_write_ptr(0),
      d_tensor(DeviceType::CPU, TypeToDataType<CF32>(),
               {1, static_cast<U64>(buffer_size)})
    {}

    // registers the plot, window is not created until present()->Show()
    // calls Superluminal::Start() on the main thread.
    const auto layout = Superluminal::MosaicLayout(1, 1, 1, 1, 0, 0);

    const Result res = Superluminal::Plot(d_name, layout, {
        .buffer    = d_tensor,
        .type      = Superluminal::Type::Line,
        .source    = Superluminal::Domain::Time,
        .display   = Superluminal::Domain::Time,
        .operation = Superluminal::Operation::Real,
    });

    if (res != Result::SUCCESS) {
        throw std::runtime_error("cyber_lineplot_sink_c: Superluminal::Plot failed");
    }

    JST_INFO("[gr-cyberether] cyber_lineplot_sink_c '{}' ready: {} samples, time-domain view.",
             d_name, d_buffer_size);
  }


    /*
     * Our virtual destructor.
     */
    cyber_lineplot_sink_impl::~cyber_lineplot_sink_impl()
    {
    }

    void
    cyber_lineplot_sink_c_impl::present()
    {
      // Creates the window and runs the event loop. Must be the application main thread.
      // Superluminal::Show() internally drives Update(), so work() never touches the render path
      Superluminal::Show();
    }

    bool
    cyber_lineplot_sink_c_impl::is_presenting()
    {
      return Superluminal::Presenting();
    }

    int
    cyber_lineplot_sink_c_impl::work(int noutput_items,
        gr_vector_const_void_star& input_items,
        gr_vector_void_star& output_items)
    {
      const input_type* in = static_cast<const input_type*>(input_items[0]);

      // Ring buffers
      CF32* buf = d_tensor.data<CF32>();
      for (int i = 0; i < noutput_items; ++i) {
          buf[d_write_ptr] = in[i];
          d_write_ptr = (d_write_ptr + 1) % d_buffer_size;
      }


      return noutput_items;
    }

  } /* namespace cyberether */
} /* namespace gr */
