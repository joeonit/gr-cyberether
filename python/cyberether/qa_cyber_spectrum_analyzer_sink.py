#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2026 Youssef Mahmoud.
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
# Headless: present() is never called here, so nothing needs a window. Only
# construction and the work() data path are exercised.

from gnuradio import gr, gr_unittest, blocks

try:
    from gnuradio import cyberether
except ImportError:
    import os
    import sys
    dirname, filename = os.path.split(os.path.abspath(__file__))
    sys.path.append(os.path.join(dirname, "bindings"))
    from gnuradio import cyberether


class qa_cyber_spectrum_analyzer_sink(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_instance(self):
        instance = cyberether.cyber_spectrum_analyzer_sink_c(1024, "qa")
        self.assertIsNotNone(instance)

    def test_001_defaults_run(self):
        src = blocks.vector_source_c([1 + 1j] * 8192, repeat=False)
        snk = cyberether.cyber_spectrum_analyzer_sink_c(1024, "qa_defaults")
        self.tb.connect(src, snk)
        self.tb.run()

    def test_002_all_options(self):
        # Every parameter set explicitly, including a GUI hint.
        snk = cyberether.cyber_spectrum_analyzer_sink_c(
            2048, "qa_options", -120.0, 10.0, 4, True, 256, "0, 0, 1, 2")
        self.assertIsNotNone(snk)

    def test_003_more_samples_than_fft(self):
        # A chunk larger than fft_size must be truncated to the newest frame,
        # and the sink must still consume everything without back-pressuring.
        src = blocks.vector_source_c([0.5 + 0.5j] * 9000, repeat=False)
        snk = cyberether.cyber_spectrum_analyzer_sink_c(512, "qa_truncate")
        self.tb.connect(src, snk)
        self.tb.run()

    def test_004_degenerate_arguments_clamp(self):
        # Zero fft_size, zero averaging and zero height must clamp rather than
        # produce an empty tensor or a division by zero.
        src = blocks.vector_source_c([1 + 0j] * 1024, repeat=False)
        snk = cyberether.cyber_spectrum_analyzer_sink_c(0, "qa_clamp", -100.0, 0.0, 0, False, 0)
        self.tb.connect(src, snk)
        self.tb.run()


if __name__ == '__main__':
    gr_unittest.run(qa_cyber_spectrum_analyzer_sink)
