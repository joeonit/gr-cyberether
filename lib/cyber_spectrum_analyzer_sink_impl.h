/* -*- c++ -*- */
/*
 * Copyright 2026 Youssef Mahmoud.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_CYBERETHER_CYBER_SPECTRUM_ANALYZER_SINK_IMPL_H
#define INCLUDED_CYBERETHER_CYBER_SPECTRUM_ANALYZER_SINK_IMPL_H

#include <gnuradio/cyberether/cyber_spectrum_analyzer_sink.h>
#include <jetstream/superluminal.hh>      //cyberether's superluminal plotting


namespace gr {
  namespace cyberether {

    class cyber_spectrum_analyzer_sink_impl : public cyber_spectrum_analyzer_sink
    {
     private:
         const uint64_t    d_fft_size;
         const std::string d_name;
         const float       d_range_min;
         const float       d_range_max;
         const int         d_averaging;
         const bool        d_max_hold;
         const int         d_waterfall_height;
         const std::string d_gui_hint;            // "row, col[, spans]"; empty = auto
         uint64_t          d_write_ptr;           // rolling write head into d_tensor
         Jetstream::Tensor d_tensor;              // display buffer, CF32, shape {1, N};
                                                  // written by work(), read in place by Superluminal

     public:
      cyber_spectrum_analyzer_sink_impl(size_t fft_size, const std::string& name,
                                        float range_min, float range_max,
                                        int averaging, bool max_hold,
                                        int waterfall_height,
                                        const std::string& gui_hint);
      ~cyber_spectrum_analyzer_sink_impl() override;

      bool start() override;             // GR lifecycle: register plot with the context
      bool stop() override;              // GR lifecycle: unregister plot

      int work(
              int noutput_items,
              gr_vector_const_void_star &input_items,
              gr_vector_void_star &output_items
      ) override;
    };

  } // namespace cyberether
} // namespace gr

#endif /* INCLUDED_CYBERETHER_CYBER_SPECTRUM_ANALYZER_SINK_IMPL_H */
