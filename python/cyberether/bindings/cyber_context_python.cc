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
    using cyber_context = gr::cyberether::cyber_context;

    py::class_<cyber_context, std::shared_ptr<cyber_context>>(m, "cyber_context")
        .def(py::init<>())
        .def("run", &cyber_context::run, py::call_guard<py::gil_scoped_release>())
        .def("is_presenting", &cyber_context::is_presenting)
        ;
}
