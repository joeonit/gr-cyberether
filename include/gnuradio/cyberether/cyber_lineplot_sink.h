/* -*- c++ -*- */
/*
 * Copyright 2026 Youssef Mahmoud.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_CYBERETHER_CYBER_LINEPLOT_SINK_H
#define INCLUDED_CYBERETHER_CYBER_LINEPLOT_SINK_H

#include <gnuradio/cyberether/api.h>
#include <gnuradio/gr_complex.h>
#include <gnuradio/sync_block.h>
#include <jetstream/superluminal.hh>

namespace gr {
  namespace cyberether {

    /*!
     * \brief Time-domain line plot sink.
     * \ingroup cyberether
     *
     * Buffers incoming samples (complex or float) into a CF32 display tensor
     * shown as a real-valued time-domain line. The window is opened by
     * cyberether.present().
     *
     * The class is templated on the input sample type. Two instantiations are
     * provided: \ref cyber_lineplot_sink_c (complex) and \ref
     * cyber_lineplot_sink_f (float). GRC users pick the type from the block's
     * "Input Type" dropdown.
     */
    template <typename T>
    class CYBERETHER_API cyber_lineplot_sink : virtual public gr::sync_block
    {
     public:
      typedef std::shared_ptr<cyber_lineplot_sink<T>> sptr;

      /*!
       * \param buffer_size  samples in the rolling display buffer.
       * \param name         display name shown in the window.
       * \param display      Time (scope view, default) or Frequency
       *                     (spectrum line — Superluminal inserts the FFT).
       * \param gui_hint     optional placement in the shared plot grid,
       *                     qtgui-style "row, col[, row_span, col_span]"
       *                     (zero-based cells). Empty (default) auto-places.
       */
      static sptr make(size_t buffer_size = 4096,
                       const std::string& name = "lineplot",
                       Jetstream::Superluminal::Domain display =
                           Jetstream::Superluminal::Domain::Time,
                       const std::string& gui_hint = "");
    };

    typedef cyber_lineplot_sink<gr_complex> cyber_lineplot_sink_c;
    typedef cyber_lineplot_sink<float>      cyber_lineplot_sink_f;

  } // namespace cyberether
} // namespace gr

#endif /* INCLUDED_CYBERETHER_CYBER_LINEPLOT_SINK_H */
