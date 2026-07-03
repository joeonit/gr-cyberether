/* -*- c++ -*- */
/*
 * Copyright 2026 Youssef Mahmoud.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_CYBERETHER_MOSAIC_LAYOUT_H
#define INCLUDED_CYBERETHER_MOSAIC_LAYOUT_H

#include <gnuradio/cyberether/api.h>

#include <string>
#include <vector>

namespace gr {
  namespace cyberether {

    // One plot's footprint in the shared grid (cells, zero-based).
    struct panel_rect {
      unsigned row      = 0;
      unsigned col      = 0;
      unsigned row_span = 1;
      unsigned col_span = 1;
    };

    // Outcome of parsing a single GUI hint string.
    enum class hint_parse {
      none,
      ok,
      error
    };

    struct layout_result {
      unsigned rows = 0;                   // shared grid height in cells
      unsigned cols = 0;                   // shared grid width in cells
      std::vector<panel_rect>  panels;     // one per input, same order
      std::vector<std::string> warnings;   // caller decides how to log
    };


    CYBERETHER_API hint_parse parse_gui_hint(const std::string& hint,
                                             panel_rect& out,
                                             std::string& error);


    CYBERETHER_API layout_result
    compute_layout(const std::vector<std::string>& hints);

  } // namespace cyberether
} // namespace gr

#endif /* INCLUDED_CYBERETHER_MOSAIC_LAYOUT_H */
