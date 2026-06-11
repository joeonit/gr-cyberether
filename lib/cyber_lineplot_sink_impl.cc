/* -*- c++ -*- */
/*
 * Copyright 2026 Youssef Mahmoud.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "cyber_lineplot_sink_impl.h"
#include <jetstream/logger.hh>
#include <atomic>
#include <chrono>
#include <stdexcept>
#include <thread>

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
      d_buffer_size(buffer_size == 0 ? 1 : buffer_size),
      d_name(name),
      d_display_write_ptr(0),
      d_initialized(false),
      d_ring(d_buffer_size),
      d_ring_scratch(d_buffer_size),
      d_tensor(DeviceType::CPU, TypeToDataType<CF32>(),
               {1, static_cast<U64>(d_buffer_size)})
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
    cyber_lineplot_sink_impl::drain_ring_to_tensor()
    {
      const size_t samples_read = d_ring.pop(d_ring_scratch.data(), d_ring_scratch.size());
      CF32* display = d_tensor.data<CF32>();

      for (size_t i = 0; i < samples_read; ++i) {
          display[d_display_write_ptr] = d_ring_scratch[i];
          d_display_write_ptr = (d_display_write_ptr + 1) % d_buffer_size;
      }
    }

    void
    cyber_lineplot_sink_impl::present()
    {
      // Must run on the main thread. Sets up the instance and registers the plot
      // on first call, then runs the render loop until the window is closed.
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

      drain_ring_to_tensor();

      if (Superluminal::Start() != Result::SUCCESS) {
          throw std::runtime_error("cyber_lineplot_sink: Superluminal::Start failed");
      }

      std::atomic<bool> running = true;
      auto update_thread = std::thread([this, &running]() {
          while (running.load(std::memory_order_acquire) && Superluminal::Presenting()) {
              drain_ring_to_tensor();
              if (Superluminal::Update() != Result::SUCCESS) {
                  running.store(false, std::memory_order_release);
                  break;
              }
              std::this_thread::sleep_for(std::chrono::milliseconds(16));
          }
      });

      const Result block_result = Superluminal::Block();

      running.store(false, std::memory_order_release);
      if (update_thread.joinable()) {
          update_thread.join();
      }

      const Result stop_result = Superluminal::Stop();
      const Result terminate_result = Superluminal::Terminate();
      d_initialized = false;

      if (block_result != Result::SUCCESS) {
          throw std::runtime_error("cyber_lineplot_sink: Superluminal::Block failed");
      }
      if (stop_result != Result::SUCCESS) {
          throw std::runtime_error("cyber_lineplot_sink: Superluminal::Stop failed");
      }
      if (terminate_result != Result::SUCCESS) {
          throw std::runtime_error("cyber_lineplot_sink: Superluminal::Terminate failed");
      }
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
      if (noutput_items <= 0) {
          return 0;
      }

      const input_type* in = static_cast<const input_type*>(input_items[0]);
      size_t samples_to_push = static_cast<size_t>(noutput_items);

      // For visualization, keep the newest samples if GNU Radio hands us a chunk
      // larger than the queue. If the consumer falls behind, push() drops the
      // overflow by returning fewer items, but the sink still consumes all input
      // so it does not back-pressure the flowgraph.
      if (samples_to_push > d_buffer_size) {
          in += samples_to_push - d_buffer_size;
          samples_to_push = d_buffer_size;
      }

      d_ring.push(in, samples_to_push);

      return noutput_items;
    }

  } /* namespace cyberether */
} /* namespace gr */
