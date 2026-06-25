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
    // Expose Jetstream::DeviceType so flowgraphs / scripts can pick a renderer.
    // "None" is a Python keyword, so the auto-pick value is exposed as "Auto".
    py::enum_<Jetstream::DeviceType>(m, "DeviceType",
        "Superluminal renderer device. Pass to cyberether.present().")
        .value("Auto",   Jetstream::DeviceType::None)
        .value("CPU",    Jetstream::DeviceType::CPU)
        .value("CUDA",   Jetstream::DeviceType::CUDA)
        .value("Metal",  Jetstream::DeviceType::Metal)
        .value("Vulkan", Jetstream::DeviceType::Vulkan)
        .value("WebGPU", Jetstream::DeviceType::WebGPU)
        .export_values();

    // Expose Superluminal::Operation. Sinks use this to choose how to reduce a
    // complex value to a scalar for display (Amplitude/Phase/Real/Imaginary).
    py::enum_<Jetstream::Superluminal::Operation>(m, "Operation",
        "How to reduce a complex value to a scalar for display.")
        .value("Real",      Jetstream::Superluminal::Operation::Real)
        .value("Imaginary", Jetstream::Superluminal::Operation::Imaginary)
        .value("Amplitude", Jetstream::Superluminal::Operation::Amplitude)
        .value("Phase",     Jetstream::Superluminal::Operation::Phase)
        .export_values();

    // Expose Superluminal::Domain. Sinks use this to pick whether their input
    // is in the time or frequency domain (source) and whether the display
    // should be time or frequency (display). When source != display,
    // Superluminal inserts the appropriate forward/inverse FFT.
    py::enum_<Jetstream::Superluminal::Domain>(m, "Domain",
        "Time- or frequency-domain. Used for PlotConfig source/display.")
        .value("Time",      Jetstream::Superluminal::Domain::Time)
        .value("Frequency", Jetstream::Superluminal::Domain::Frequency)
        .export_values();

    // The entry point used by a GRC snippet / Python launcher:
    //     cyberether.present()
    //     cyberether.present(device=cyberether.DeviceType.Metal)
    //
    m.def("present",
          &gr::cyberether::present,
          py::arg("device") = Jetstream::DeviceType::None,
          py::call_guard<py::gil_scoped_release>(),
          "Open the CyberEther window on the calling (main) thread, showing "
          "every registered CyberEther sink, and block until it is closed. "
          "`device` selects the renderer backend (default Auto).");
}
