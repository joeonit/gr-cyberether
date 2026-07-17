#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2026 Youssef Mahmoud.
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

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


class qa_cyber_constellation_sink(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_instance(self):
        instance = cyberether.cyber_constellation_sink_c(1024, "qa", "")
        self.assertIsNotNone(instance)

    def test_001_qpsk_run(self):
        # A finite QPSK burst must run to completion; the sink always
        # consumes everything and never back-pressures.
        rng = np.random.default_rng(1234)
        points = np.array([1+1j, 1-1j, -1+1j, -1-1j], dtype=np.complex64) / np.sqrt(2)
        symbols = rng.choice(points, size=4096).tolist()
        src = blocks.vector_source_c(symbols, repeat=False)
        snk = cyberether.cyber_constellation_sink_c(1024, "qpsk", "0, 0")
        self.tb.connect(src, snk)
        self.tb.run()


if __name__ == '__main__':
    gr_unittest.run(qa_cyber_constellation_sink)
