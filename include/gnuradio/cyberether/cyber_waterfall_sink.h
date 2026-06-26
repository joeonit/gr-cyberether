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
#include <jetstream/superluminal.hh>

namespace gr {
  namespace cyberether {

    /*!
     * \brief Spectrum waterfall sink.
     * \ingroup cyberether
     *
     * Accepts a time-domain stream (complex or float), keeps a rolling
     * fft_size-sample frame in a 1D CF32 buffer, and lets Superluminal handle
     * the FFT and the scrolling history internally. The window is opened by
     * cyberether.present().
     *
     * The class is templated on the input sample type. Two instantiations are
     * provided: \ref cyber_waterfall_sink_c (complex) and \ref
     * cyber_waterfall_sink_f (float). GRC users pick the type from the block's
     * "Input Type" dropdown.
     */
    template <typename T>
    class CYBERETHER_API cyber_waterfall_sink : virtual public gr::sync_block
    {
     public:
      typedef std::shared_ptr<cyber_waterfall_sink<T>> sptr;

      /*!
       * \param fft_size  samples per FFT frame (and width of the waterfall).
       * \param name      display name shown in the window.
       * \param height    history depth in rows (passed to Superluminal as the
       *                  waterfall module's "height" option). Defaults to 512.
       * \param display   Frequency (default — spectrum waterfall) or Time
       *                  (time-amplitude strip; no FFT).
       */
      static sptr make(size_t fft_size = 1024,
                       const std::string& name = "waterfall",
                       int height = 512,
                       Jetstream::Superluminal::Domain display =
                           Jetstream::Superluminal::Domain::Frequency);
    };

    typedef cyber_waterfall_sink<gr_complex> cyber_waterfall_sink_c;
    typedef cyber_waterfall_sink<float>      cyber_waterfall_sink_f;

  } // namespace cyberether
} // namespace gr

#endif /* INCLUDED_CYBERETHER_CYBER_WATERFALL_SINK_H */
