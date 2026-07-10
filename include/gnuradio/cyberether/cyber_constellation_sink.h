/* -*- c++ -*- */
/*
 * Copyright 2026 Youssef.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_CYBERETHER_CYBER_CONSTELLATION_SINK_H
#define INCLUDED_CYBERETHER_CYBER_CONSTELLATION_SINK_H

#include <gnuradio/cyberether/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace cyberether {

    /*!
     * \brief <+description of block+>
     * \ingroup cyberether
     *
     */
    class CYBERETHER_API cyber_constellation_sink : virtual public gr::sync_block
    {
     public:
      typedef std::shared_ptr<cyber_constellation_sink> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of cyberether::cyber_constellation_sink.
       *
       * To avoid accidental use of raw pointers, cyberether::cyber_constellation_sink's
       * constructor is in a private implementation
       * class. cyberether::cyber_constellation_sink::make is the public interface for
       * creating new instances.
       */
      static sptr make();
    };

  } // namespace cyberether
} // namespace gr

#endif /* INCLUDED_CYBERETHER_CYBER_CONSTELLATION_SINK_H */
