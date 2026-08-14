/* -*- c++ -*- */
/*
 * Copyright 2026 Youssef Mahmoud.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_CYBERETHER_CYBER_SPECTRUM_ANALYZER_SINK_H
#define INCLUDED_CYBERETHER_CYBER_SPECTRUM_ANALYZER_SINK_H

#include <gnuradio/cyberether/api.h>
#include <gnuradio/gr_complex.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace cyberether {

    /*!
     * \brief Spectrum analyzer sink: spectrum line over a waterfall.
     * \ingroup cyberether
     *
     * Complex input only, always time domain: Superluminal runs the FFT and
     * owns both halves of the display. The window is opened by
     * cyberether.present().
     */
    class CYBERETHER_API cyber_spectrum_analyzer_sink : virtual public gr::sync_block
    {
     public:
      typedef std::shared_ptr<cyber_spectrum_analyzer_sink> sptr;

      /*!
       * \param fft_size          samples per FFT frame.
       * \param name              display name shown in the window.
       * \param range_min         lower bound of the amplitude scale, in dB.
       * \param range_max         upper bound of the amplitude scale, in dB.
       * \param averaging         number of frames averaged together.
       * \param max_hold          keep the peak of each bin.
       * \param waterfall_height  history depth of the waterfall half, in rows.
       * \param gui_hint          optional placement in the shared plot grid,
       *                          qtgui-style "row, col[, row_span, col_span]"
       *                          (zero-based cells). Empty (default) auto-places.
       */
      static sptr make(size_t fft_size = 1024,
                       const std::string& name = "spectrum analyzer",
                       float range_min = -100.0f,
                       float range_max = 0.0f,
                       int averaging = 1,
                       bool max_hold = false,
                       int waterfall_height = 512,
                       const std::string& gui_hint = "");
    };

    typedef cyber_spectrum_analyzer_sink cyber_spectrum_analyzer_sink_c;

  } // namespace cyberether
} // namespace gr

#endif /* INCLUDED_CYBERETHER_CYBER_SPECTRUM_ANALYZER_SINK_H */
