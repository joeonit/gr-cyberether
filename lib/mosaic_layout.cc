/* -*- c++ -*- */
/*
 * Copyright 2026 Youssef Mahmoud.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "mosaic_layout.h"

#include <algorithm>
#include <cmath>
#include <cstddef>

namespace gr {
  namespace cyberether {

    // Superluminal's Mosaic grid coordinates are U8, so nothing may extend
    // past cell 255 in either direction.
    static constexpr unsigned MAX_GRID = 255;

    hint_parse
    parse_gui_hint(const std::string& hint, panel_rect& out, std::string& error)
    {
      // Split on commas, trimming whitespace around each field.
      std::vector<std::string> fields;
      std::size_t start = 0;
      while (true) {
          const std::size_t comma = hint.find(',', start);
          std::string field = hint.substr(
              start, comma == std::string::npos ? std::string::npos : comma - start);
          const std::size_t first = field.find_first_not_of(" \t");
          const std::size_t last  = field.find_last_not_of(" \t");
          fields.push_back(first == std::string::npos
                               ? std::string{}
                               : field.substr(first, last - first + 1));
          if (comma == std::string::npos) {
              break;
          }
          start = comma + 1;
      }

      if (fields.size() == 1 && fields[0].empty()) {
          return hint_parse::none;
      }

      if (fields.size() != 2 && fields.size() != 4) {
          error = "expected 'row, col' or 'row, col, row_span, col_span'";
          return hint_parse::error;
      }

      unsigned values[4] = {0, 0, 1, 1};
      for (std::size_t i = 0; i < fields.size(); ++i) {
          const std::string& field = fields[i];
          if (field.empty() ||
              field.find_first_not_of("0123456789") != std::string::npos) {
              error = "'" + field + "' is not a non-negative integer";
              return hint_parse::error;
          }
          if (field.size() > 3 || std::stoul(field) > MAX_GRID) {
              error = "'" + field + "' is out of range (max " +
                      std::to_string(MAX_GRID) + ")";
              return hint_parse::error;
          }
          values[i] = static_cast<unsigned>(std::stoul(field));
      }

      const panel_rect rect{values[0], values[1], values[2], values[3]};

      if (rect.row_span == 0 || rect.col_span == 0) {
          error = "spans must be >= 1";
          return hint_parse::error;
      }
      if (rect.row + rect.row_span > MAX_GRID ||
          rect.col + rect.col_span > MAX_GRID) {
          error = "panel extends past cell " + std::to_string(MAX_GRID);
          return hint_parse::error;
      }

      out = rect;
      return hint_parse::ok;
    }

    layout_result
    compute_layout(const std::vector<std::string>& hints)
    {
      layout_result res;
      const std::size_t n = hints.size();
      res.panels.resize(n);
      if (n == 0) {
          return res;
      }

      // Pass 1: parse. Malformed hints degrade to auto placement + warning.
      std::vector<bool> hinted(n, false);
      std::size_t auto_count = 0;
      for (std::size_t i = 0; i < n; ++i) {
          panel_rect rect;
          std::string error;
          switch (parse_gui_hint(hints[i], rect, error)) {
          case hint_parse::ok:
              res.panels[i] = rect;
              hinted[i] = true;
              break;
          case hint_parse::error:
              res.warnings.push_back("plot " + std::to_string(i) +
                                     ": ignoring GUI hint '" + hints[i] +
                                     "': " + error + "; auto-placing.");
              ++auto_count;
              break;
          case hint_parse::none:
              ++auto_count;
              break;
          }
      }

      // Pass 2: grid dimensions. Hinted extents define the grid; with no
      // valid hint anywhere, fall back to the near-square auto grid:
      //   1 -> 1x1   2 -> 1x2   3,4 -> 2x2   5,6 -> 2x3   ...
      unsigned rows = 0;
      unsigned cols = 0;
      for (std::size_t i = 0; i < n; ++i) {
          if (hinted[i]) {
              rows = std::max(rows, res.panels[i].row + res.panels[i].row_span);
              cols = std::max(cols, res.panels[i].col + res.panels[i].col_span);
          }
      }
      if (rows == 0) {
          cols = static_cast<unsigned>(
              std::ceil(std::sqrt(static_cast<double>(n))));
          rows = static_cast<unsigned>((n + cols - 1) / cols);
      }

      // Pass 3: occupancy. Hints are honored in registration order; a hint
      // that overlaps an earlier plot is demoted to auto placement.
      std::vector<std::vector<bool>> occupied(rows,
                                              std::vector<bool>(cols, false));
      std::size_t used = 0;
      for (std::size_t i = 0; i < n; ++i) {
          if (!hinted[i]) {
              continue;
          }
          const panel_rect& p = res.panels[i];
          bool clash = false;
          for (unsigned r = p.row; r < p.row + p.row_span && !clash; ++r) {
              for (unsigned c = p.col; c < p.col + p.col_span && !clash; ++c) {
                  clash = occupied[r][c];
              }
          }
          if (clash) {
              res.warnings.push_back("plot " + std::to_string(i) +
                                     ": GUI hint '" + hints[i] +
                                     "' overlaps an earlier plot; auto-placing.");
              res.panels[i] = panel_rect{};
              hinted[i] = false;
              ++auto_count;
              continue;
          }
          for (unsigned r = p.row; r < p.row + p.row_span; ++r) {
              for (unsigned c = p.col; c < p.col + p.col_span; ++c) {
                  occupied[r][c] = true;
              }
          }
          used += static_cast<std::size_t>(p.row_span) * p.col_span;
      }

      // Pass 4: grow the grid toward square until every auto-placed plot has
      // a free cell. Growing changes cell proportions but never moves a
      // hinted plot's cells.
      while (static_cast<std::size_t>(rows) * cols - used < auto_count &&
             (rows < MAX_GRID || cols < MAX_GRID)) {
          if (rows <= cols && rows < MAX_GRID) {
              ++rows;
              occupied.emplace_back(cols, false);
          } else {
              ++cols;
              for (auto& row : occupied) {
                  row.push_back(false);
              }
          }
      }

      // Pass 5: fill auto-placed plots row-major into the free cells.
      unsigned r = 0;
      unsigned c = 0;
      for (std::size_t i = 0; i < n; ++i) {
          if (hinted[i]) {
              continue;
          }
          while (r < rows && occupied[r][c]) {
              if (++c == cols) {
                  c = 0;
                  ++r;
              }
          }
          if (r >= rows) {
              // Only reachable once the grid is capped at 255x255; park the
              // leftovers at (0, 0) rather than fail.
              res.warnings.push_back("plot " + std::to_string(i) +
                                     ": no free cell left; stacking at (0, 0).");
              res.panels[i] = panel_rect{};
              continue;
          }
          res.panels[i] = panel_rect{r, c, 1, 1};
          occupied[r][c] = true;
      }

      res.rows = rows;
      res.cols = cols;
      return res;
    }

  } // namespace cyberether
} // namespace gr
