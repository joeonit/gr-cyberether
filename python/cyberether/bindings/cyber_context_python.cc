/*
 * Copyright 2026 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include <gnuradio/cyberether/cyber_context.h>

void bind_cyber_context(py::module& m)
{
    // The entry point used by a GRC snippet / Python launcher:
    //     cyberether.present()
    //
    m.def("present",
          &gr::cyberether::present,
          py::call_guard<py::gil_scoped_release>(),
          "Open the CyberEther window on the calling (main) thread, showing "
          "every registered CyberEther sink, and block until it is closed.");
}
