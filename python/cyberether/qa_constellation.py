#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2026 Youssef Mahmoud.
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
# Headless API test: drives QPSK constellation samples through the public
# sink API without ever opening a window. present() is deliberately never
# called — sinks only register with cyber_context, so everything here is
# CI-safe. Rendering (and the layout translation that happens inside
# present()) is exercised by the C++ layout tests and manual demos instead.

import numpy as np

from gnuradio import gr, gr_unittest, blocks

try:
    from gnuradio import cyberether
except ImportError:
    import os
    import sys
    dirname, filename = os.path.split(os.path.abspath(__file__))
    sys.path.append(os.path.join(dirname, "bindings"))
    from gnuradio import cyberether

# Unit-energy QPSK constellation points.
QPSK = np.array([1 + 1j, 1 - 1j, -1 + 1j, -1 - 1j], dtype=np.complex64) / np.sqrt(2)


def qpsk_symbols(n, seed=1234):
    """n random QPSK symbols as a list of Python complex."""
    rng = np.random.default_rng(seed)
    return rng.choice(QPSK, size=n).tolist()


class qa_constellation(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_qpsk_through_lineplot(self):
        # A finite QPSK burst through the complex line sink must run to
        # completion: the sink always consumes everything (never
        # back-pressures), even when handed more than buffer_size at once.
        symbols = qpsk_symbols(4096)
        src = blocks.vector_source_c(symbols, repeat=False)
        snk = cyberether.cyber_lineplot_sink_c(1024, "qpsk scope")
        self.tb.connect(src, snk)
        self.tb.run()

    def test_002_qpsk_through_waterfall(self):
        symbols = qpsk_symbols(8192)
        src = blocks.vector_source_c(symbols, repeat=False)
        snk = cyberether.cyber_waterfall_sink_c(1024, "qpsk waterfall", 256)
        self.tb.connect(src, snk)
        self.tb.run()

    def test_003_bpsk_float_variant(self):
        # The float sinks take the real projection of the constellation
        # (BPSK-like ±1/sqrt(2) levels).
        levels = [s.real for s in qpsk_symbols(4096)]
        src = blocks.vector_source_f(levels, repeat=False)
        snk = cyberether.cyber_lineplot_sink_f(1024, "bpsk scope")
        self.tb.connect(src, snk)
        self.tb.run()

    def test_004_gui_hint_accepted(self):
        # Valid qtgui-style hints must be accepted by every factory,
        # both the "row, col" and "row, col, row_span, col_span" forms.
        cyberether.cyber_lineplot_sink_c(1024, "a", cyberether.Domain.Time, "0, 0")
        cyberether.cyber_lineplot_sink_f(1024, "b", cyberether.Domain.Time, "1, 0, 1, 2")
        cyberether.cyber_waterfall_sink_c(
            1024, "c", 256, cyberether.Domain.Frequency, "0, 1")
        cyberether.cyber_waterfall_sink_f(
            1024, "d", 256, cyberether.Domain.Frequency, "")

    def test_005_malformed_gui_hint_is_deferred(self):
        # Hints are only parsed inside present(); a malformed hint must not
        # throw at construction or while the flowgraph runs — it degrades to
        # auto-placement with a warning at present() time.
        for bad in ("nonsense", "0", "0, 0, 0, 0", "-1, 2", "1, 2, 3"):
            snk = cyberether.cyber_lineplot_sink_c(
                256, "bad hint", cyberether.Domain.Time, bad)
            tb = gr.top_block()
            src = blocks.vector_source_c(qpsk_symbols(512), repeat=False)
            tb.connect(src, snk)
            tb.run()

    def test_006_spanning_layout_registers(self):
        # The docs/mosaic.md showcase: a waterfall hinted to span the top
        # row with two auto-placed scopes below it, all in one flowgraph.
        # Headless, this exercises multi-sink registration with mixed
        # hinted/unhinted plots.
        symbols = qpsk_symbols(8192)
        wf = cyberether.cyber_waterfall_sink_c(
            1024, "wide waterfall", 128, cyberether.Domain.Frequency, "0, 0, 1, 2")
        s1 = cyberether.cyber_lineplot_sink_c(512, "scope 1")
        s2 = cyberether.cyber_lineplot_sink_c(512, "scope 2")
        src = blocks.vector_source_c(symbols, repeat=False)
        for snk in (wf, s1, s2):
            self.tb.connect(src, snk)
        self.tb.run()

    def test_007_constellation_sink(self):
        # QPSK through the constellation sink, hinted into a cell.
        src = blocks.vector_source_c(qpsk_symbols(4096), repeat=False)
        snk = cyberether.cyber_constellation_sink_c(1024, "qpsk const", "0, 1")
        self.tb.connect(src, snk)
        self.tb.run()

    def test_008_start_stop_cycles(self):
        # start() registers the plot with cyber_context, stop() unregisters
        # it. Two full cycles of the same flowgraph exercise the
        # re-registration (replace-by-owner) path.
        src = blocks.vector_source_c(qpsk_symbols(2048), repeat=False)
        snk = cyberether.cyber_lineplot_sink_c(512, "cycled")
        self.tb.connect(src, snk)
        for _ in range(2):
            self.tb.start()
            self.tb.wait()
            self.tb.stop()
            src.rewind()


if __name__ == '__main__':
    gr_unittest.run(qa_constellation)
