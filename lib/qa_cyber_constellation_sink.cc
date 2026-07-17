/* -*- c++ -*- */
/*
 * Copyright 2026 Youssef Mahmoud.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/cyberether/cyber_constellation_sink.h>
#include <gnuradio/attributes.h>
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

namespace gr {
  namespace cyberether {

    BOOST_AUTO_TEST_CASE(test_cyber_constellation_sink_construction)
    {
      // Constructing must not touch Superluminal (no window, no GPU).
      auto sink = cyber_constellation_sink::make(1024, "qa", "0, 0");
      BOOST_REQUIRE(sink);
      BOOST_CHECK_EQUAL(sink->input_signature()->sizeof_stream_item(0),
                        sizeof(gr_complex));
      BOOST_CHECK_EQUAL(sink->output_signature()->max_streams(), 0);
    }

  } /* namespace cyberether */
} /* namespace gr */
