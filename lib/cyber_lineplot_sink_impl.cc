/* -*- c++ -*- */
/*
 * Copyright 2026 Youssef Mahmoud.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "cyber_lineplot_sink_impl.h"

namespace gr {
  namespace cyberether {

    #pragma message("set the following appropriately and remove this warning")
    using input_type = float;
    cyber_lineplot_sink::sptr
    cyber_lineplot_sink::make()
    {
      return gnuradio::make_block_sptr<cyber_lineplot_sink_impl>(
        );
    }


    /*
     * The private constructor
     */
    cyber_lineplot_sink_impl::cyber_lineplot_sink_impl()
      : gr::sync_block("cyber_lineplot_sink",
              gr::io_signature::make(1 /* min inputs */, 1 /* max inputs */, sizeof(input_type)),
              gr::io_signature::make(0, 0, 0))
    {}

    /*
     * Our virtual destructor.
     */
    cyber_lineplot_sink_impl::~cyber_lineplot_sink_impl()
    {
    }

    int
    cyber_lineplot_sink_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      auto in = static_cast<const input_type*>(input_items[0]);

      #pragma message("Implement the signal processing in your block and remove this warning")
      // Do <+signal processing+>

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace cyberether */
} /* namespace gr */
