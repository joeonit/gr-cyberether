
/* -*- c++ -*- */
/*
 * Copyright 2026 Youssef Mahmoud.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/*
 * cyber_null_sink does no plotting. Its only job is to prove the D0 build chain:
 *   - the CyberEther/Jetstream headers are found and compile under C++20
 *   - the block links against libjetstream.
 */

#include <gnuradio/io_signature.h>
#include "cyber_null_sink_impl.h"

#include <jetstream/logger.hh>
#include <jetstream/superluminal.hh>

namespace gr {
  namespace cyberether {

    using input_type = gr_complex;

    cyber_null_sink::sptr
    cyber_null_sink::make()
    {
      return gnuradio::make_block_sptr<cyber_null_sink_impl>(
        );
    }


    /*
     * The private constructor
     */
    cyber_null_sink_impl::cyber_null_sink_impl()
      : gr::sync_block("cyber_null_sink",
              gr::io_signature::make(1 /* min inputs */, 1 /* max inputs */, sizeof(input_type)),
              gr::io_signature::make(0, 0, 0))
    {
      // Compile-time proof the Jetstream headers are visible.
      JST_INFO("[gr-cyberether] D0 build test: built against CyberEther v{}.",
               JETSTREAM_VERSION_STR);

      // Link-time proof
      JST_LOG_SET_DEBUG_LEVEL(JST_LOG_DEBUG_DEFAULT_LEVEL);
    }

    /*
     * Our virtual destructor.
     */
    cyber_null_sink_impl::~cyber_null_sink_impl()
    {
    }

    int
    cyber_null_sink_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      // Discard input. A real sink would copy/route samples into a Jetstream.
      return noutput_items;
    }

  } /* namespace cyberether */
} /* namespace gr */
