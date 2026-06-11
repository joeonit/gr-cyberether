/* -*- c++ -*- */
/*
 * Copyright 2026 Youssef Mahmoud.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_CYBERETHER_CYBER_LINEPLOT_SINK_H
#define INCLUDED_CYBERETHER_CYBER_LINEPLOT_SINK_H

#include <gnuradio/cyberether/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace cyberether {

    /*!
     * \brief Time-domain line plot sink.
     * \ingroup cyberether
     *
     * Buffers incoming complex samples into a ring buffer that is displayed
     * as a real-valued time-domain line. The window is opened by present().
     */
    class CYBERETHER_API cyber_lineplot_sink : virtual public gr::sync_block
    {
     public:
      typedef std::shared_ptr<cyber_lineplot_sink> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of cyberether::cyber_lineplot_sink.
       *
       * To avoid accidental use of raw pointers, cyberether::cyber_lineplot_sink's
       * constructor is in a private implementation
       * class. cyberether::cyber_lineplot_sink::make is the public interface for
       * creating new instances.
       */
      static sptr make(size_t buffer_size = 4096,
                       const std::string& name = "lineplot");
    };

  } // namespace cyberether
} // namespace gr

#endif /* INCLUDED_CYBERETHER_CYBER_LINEPLOT_SINK_H */
