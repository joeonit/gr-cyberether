/* -*- c++ -*- */
/*
 * Copyright 2026 Youssef Mahmoud.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_CYBERETHER_CYBER_NULL_SINK_IMPL_H
#define INCLUDED_CYBERETHER_CYBER_NULL_SINK_IMPL_H

#include <gnuradio/cyberether/cyber_null_sink.h>

namespace gr {
  namespace cyberether {

    class cyber_null_sink_impl : public cyber_null_sink
    {
     private:
      // Nothing to declare in this block.

     public:
      cyber_null_sink_impl();
      ~cyber_null_sink_impl();

      // Where all the action really happens
      int work(
              int noutput_items,
              gr_vector_const_void_star &input_items,
              gr_vector_void_star &output_items
      );
    };

  } // namespace cyberether
} // namespace gr

#endif /* INCLUDED_CYBERETHER_CYBER_NULL_SINK_IMPL_H */
