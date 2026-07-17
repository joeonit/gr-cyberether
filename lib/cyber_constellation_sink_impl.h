/* -*- c++ -*- */
/*
 * Copyright 2026 Youssef Mahmoud.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_CYBERETHER_CYBER_CONSTELLATION_SINK_IMPL_H
#define INCLUDED_CYBERETHER_CYBER_CONSTELLATION_SINK_IMPL_H

#include <gnuradio/cyberether/cyber_constellation_sink.h>
#include <jetstream/superluminal.hh>      //cyberether's superluminal plotting


namespace gr {
  namespace cyberether {

    class cyber_constellation_sink_impl : public cyber_constellation_sink
    {
     private:
         const uint64_t    d_buffer_size;
         const std::string d_name;
         const std::string d_gui_hint;            // "row, col[, spans]"; empty = auto
         uint64_t          d_display_write_ptr;   // rolling write head into d_tensor
         Jetstream::Tensor d_tensor;              // display buffer, CF32, shape {1, N};
                                                  // written by work(), read in place by Superluminal

     public:
      cyber_constellation_sink_impl(size_t buffer_size, const std::string& name,
                                    const std::string& gui_hint);
      ~cyber_constellation_sink_impl() override;

      bool start() override;             // GR lifecycle: register plot with the context
      bool stop() override;              // GR lifecycle: unregister plot

      int work(
              int noutput_items,
              gr_vector_const_void_star &input_items,
              gr_vector_void_star &output_items
      ) override;
    };

  } // namespace cyberether
} // namespace gr

#endif /* INCLUDED_CYBERETHER_CYBER_CONSTELLATION_SINK_IMPL_H */
