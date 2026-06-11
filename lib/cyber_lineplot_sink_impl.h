/* -*- c++ -*- */
/*
 * Copyright 2026 Youssef Mahmoud.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_CYBERETHER_CYBER_LINEPLOT_SINK_IMPL_H
#define INCLUDED_CYBERETHER_CYBER_LINEPLOT_SINK_IMPL_H

#include <gnuradio/cyberether/cyber_lineplot_sink.h>
#include <jetstream/superluminal.hh>      //cyberether's superluminal plotting
#include <vector>
#include "spsc_ring.h"


namespace gr {
  namespace cyberether {

    class cyber_lineplot_sink_impl : public cyber_lineplot_sink
    {
     private:
         const uint64_t d_buffer_size;
         const std::string d_name;
         uint64_t d_display_write_ptr;
         bool d_initialized;              // Superluminal instance + plot registered (lazy, in present())
         SpscRing<Jetstream::CF32> d_ring; // producer: work(), consumer: Superluminal update loop
         std::vector<Jetstream::CF32> d_ring_scratch;
         Jetstream::Tensor d_tensor;      // persistent display buffer, CF32, shape {1, N}

         void drain_ring_to_tensor();

     public:
      cyber_lineplot_sink_impl(size_t buffer_size, const std::string& name);
      ~cyber_lineplot_sink_impl();


      void present() override;
      bool is_presenting() override;
      // Where all the action really happens
      int work(
              int noutput_items,
              gr_vector_const_void_star &input_items,
              gr_vector_void_star &output_items
      ) override;
    };

  } // namespace cyberether
} // namespace gr

#endif /* INCLUDED_CYBERETHER_CYBER_LINEPLOT_SINK_IMPL_H */
