/* -*- c++ -*- */
/*
 * Copyright 2026 Youssef Mahmoud.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_CYBERETHER_CYBER_CONSTELLATION_SINK_H
#define INCLUDED_CYBERETHER_CYBER_CONSTELLATION_SINK_H

#include <gnuradio/cyberether/api.h>
#include <gnuradio/gr_complex.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace cyberether {

    /*!
     * \brief IQ constellation (scatter) sink: real -> X, imag -> Y.
     * Complex input only. The window is opened by cyberether.present().
     */
    class CYBERETHER_API cyber_constellation_sink : virtual public gr::sync_block
    {
     public:
      typedef std::shared_ptr<cyber_constellation_sink> sptr;

      static sptr make(size_t buffer_size = 2048,
                       const std::string& name = "constellation",
                       const std::string& gui_hint = "");
    };

    typedef cyber_constellation_sink cyber_constellation_sink_c;

  } // namespace cyberether
} // namespace gr

#endif /* INCLUDED_CYBERETHER_CYBER_CONSTELLATION_SINK_H */
