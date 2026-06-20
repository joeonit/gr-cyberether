/* -*- c++ -*- */
/*
 * Copyright 2026 Youssef Mahmoud.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_CYBERETHER_CYBER_WATERFALL_SINK_H
#define INCLUDED_CYBERETHER_CYBER_WATERFALL_SINK_H

#include <gnuradio/cyberether/api.h>
#include <gnuradio/gr_complex.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace cyberether {

    /*!
     * \brief Spectrum waterfall sink (minimal).
     * \ingroup cyberether
     *
     * Accepts a complex time-domain stream, keeps a rolling fft_size-sample
     * frame in a 1D buffer, and lets Superluminal handle the FFT and the
     * scrolling history internally. The window is opened by
     * cyberether.present().
     */
    class CYBERETHER_API cyber_waterfall_sink : virtual public gr::sync_block
    {
     public:
      typedef std::shared_ptr<cyber_waterfall_sink> sptr;

      /*!
       * \param fft_size  samples per FFT frame (and width of the waterfall).
       * \param name      display name shown in the window.
       * \param height    history depth in rows (passed to Superluminal as the
       *                  waterfall module's "height" option). Defaults to 512.
       */
      static sptr make(size_t fft_size = 1024,
                       const std::string& name = "waterfall",
                       int height = 512);
    };

  } // namespace cyberether
} // namespace gr

#endif /* INCLUDED_CYBERETHER_CYBER_WATERFALL_SINK_H */
