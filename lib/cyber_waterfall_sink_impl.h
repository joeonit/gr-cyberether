/* -*- c++ -*- */
/*
 * Copyright 2026 Youssef Mahmoud.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_CYBERETHER_CYBER_WATERFALL_SINK_IMPL_H
#define INCLUDED_CYBERETHER_CYBER_WATERFALL_SINK_IMPL_H

#include <gnuradio/cyberether/cyber_waterfall_sink.h>
#include <jetstream/superluminal.hh>

namespace gr {
  namespace cyberether {

    template <typename T>
    class cyber_waterfall_sink_impl : public cyber_waterfall_sink<T>
    {
     private:
         const uint64_t    d_fft_size;    // samples per row (FFT length)
         const int         d_height;      // PlotConfig "height" option
         const std::string d_name;
         const Jetstream::Superluminal::Operation d_operation;
         const Jetstream::Superluminal::Domain    d_display;
         uint64_t          d_write_ptr;   // rolling write head into d_tensor
         Jetstream::Tensor d_tensor;      // always CF32, shape {1, fft_size}

     public:
      cyber_waterfall_sink_impl(size_t fft_size, const std::string& name,
                                int height,
                                Jetstream::Superluminal::Operation operation,
                                Jetstream::Superluminal::Domain display);
      ~cyber_waterfall_sink_impl() override;

      bool start() override;
      bool stop()  override;

      int work(
              int noutput_items,
              gr_vector_const_void_star &input_items,
              gr_vector_void_star &output_items
      ) override;
    };

  } // namespace cyberether
} // namespace gr

#endif /* INCLUDED_CYBERETHER_CYBER_WATERFALL_SINK_IMPL_H */
