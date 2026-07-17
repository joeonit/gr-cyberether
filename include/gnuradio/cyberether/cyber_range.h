/* -*- c++ -*- */
/*
 * Copyright 2026 Youssef Mahmoud.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_CYBERETHER_CYBER_RANGE_H
#define INCLUDED_CYBERETHER_CYBER_RANGE_H

#include <gnuradio/cyberether/api.h>

#include <functional>
#include <memory>
#include <string>

namespace gr {
  namespace cyberether {

    /*!
     * \brief Slider control rendered inside the CyberEther window.
     *
     * Not a GR block: construct it next to the flowgraph (like qtgui_range)
     * and it occupies a mosaic cell. The callback fires on Superluminal's
     * present thread whenever the user moves the slider.
     */
    class CYBERETHER_API cyber_range
    {
     public:
      typedef std::shared_ptr<cyber_range> sptr;

      static sptr make(const std::string& label, float min, float max,
                       float value, const std::string& gui_hint = "");

      virtual ~cyber_range() = default;

      virtual void set_callback(std::function<void(float)> callback) = 0;
      virtual float value() const = 0;
    };

  } // namespace cyberether
} // namespace gr

#endif /* INCLUDED_CYBERETHER_CYBER_RANGE_H */
