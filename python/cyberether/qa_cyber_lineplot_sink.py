#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2026 Youssef Mahmoud.
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
# Headless: present() is never called here, so nothing in this file needs
# a window. It only exercises construction and the work() data path.

from gnuradio import gr, gr_unittest, blocks

try:
    from gnuradio import cyberether
except ImportError:
    import os
    import sys
    dirname, filename = os.path.split(os.path.abspath(__file__))
    sys.path.append(os.path.join(dirname, "bindings"))
    from gnuradio import cyberether


class qa_cyber_lineplot_sink(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_instance(self):
        instance = cyberether.cyber_lineplot_sink_c(1024, "qa")
        self.assertIsNotNone(instance)

    def test_001_complex_run(self):
        src = blocks.vector_source_c([1 + 1j] * 4096, repeat=False)
        snk = cyberether.cyber_lineplot_sink_c(1024, "qa_complex")
        self.tb.connect(src, snk)
        self.tb.run()

    def test_002_float_run(self):
        src = blocks.vector_source_f([0.5] * 4096, repeat=False)
        snk = cyberether.cyber_lineplot_sink_f(1024, "qa_float")
        self.tb.connect(src, snk)
        self.tb.run()


if __name__ == '__main__':
    gr_unittest.run(qa_cyber_lineplot_sink)
