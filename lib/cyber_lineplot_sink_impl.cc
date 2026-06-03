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
      d_initialized(false),
      d_tensor(DeviceType::CPU, TypeToDataType<CF32>(),
               {1, static_cast<U64>(buffer_size)})
    {
      // Construction only allocates the display buffer. The Superluminal
      // instance and plot are set up lazily in present() on the main thread.
      JST_INFO("[gr-cyberether] cyber_lineplot_sink '{}' constructed: {} samples, time-domain view.",
               d_name, d_buffer_size);
    }


    /*
     * Our virtual destructor.
     */
    cyber_lineplot_sink_impl::~cyber_lineplot_sink_impl()
    {
    }

    void
    cyber_lineplot_sink_impl::present()
    {
      // Must run on the main thread. Sets up the instance and registers the plot
      // on first call, then runs the render loop until the window is closed.
      // Show() drives Update() and tears the instance down on exit.
      if (!d_initialized) {
          if (Superluminal::Initialize() != Result::SUCCESS) {
              throw std::runtime_error("cyber_lineplot_sink: Superluminal::Initialize failed");
          }

          const auto layout = Superluminal::MosaicLayout(1, 1, 1, 1, 0, 0);

          const Result res = Superluminal::Plot(d_name, layout, {
              .buffer    = d_tensor,
              .type      = Superluminal::Type::Line,
              .source    = Superluminal::Domain::Time,
              .display   = Superluminal::Domain::Time,
              .operation = Superluminal::Operation::Real,
          });

          if (res != Result::SUCCESS) {
              throw std::runtime_error("cyber_lineplot_sink: Superluminal::Plot failed");
          }

          d_initialized = true;
      }

      Superluminal::Show();
    }

    bool
    cyber_lineplot_sink_impl::is_presenting()
    {
      return d_initialized && Superluminal::Presenting();
    }

    int
    cyber_lineplot_sink_impl::work(int noutput_items,
        gr_vector_const_void_star& input_items,
        gr_vector_void_star& output_items)
    {
      const input_type* in = static_cast<const input_type*>(input_items[0]);

      // Ring buffer: keep the most recent d_buffer_size samples in the display tensor.
      CF32* buf = d_tensor.data<CF32>();
      for (int i = 0; i < noutput_items; ++i) {
          buf[d_write_ptr] = in[i];
          d_write_ptr = (d_write_ptr + 1) % d_buffer_size;
      }

      return noutput_items;
    }

  } /* namespace cyberether */
} /* namespace gr */
