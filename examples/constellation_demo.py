#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: CyberEther constellation demo
# Description: Noisy QPSK into every gr-cyberether sink type, arranged in one window with GUI hints; the constellation spans the left column.
# GNU Radio version: v3.11.0.0git-1100-gf06564b3

from gnuradio import analog
from gnuradio import blocks
import numpy
from gnuradio import cyberether
from gnuradio import digital
import threading
from gnuradio import gr
from gnuradio import cyberether
from gnuradio.filter import firdes
from gnuradio.fft import window
import sys
import signal
from argparse import ArgumentParser
from gnuradio.eng_arg import eng_float, intx
from gnuradio import eng_notation




class constellation_demo(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self, "CyberEther constellation demo", catch_exceptions=True)

        ##################################################
        # Variables
        ##################################################
        self.samp_rate = samp_rate = 256000

        ##################################################
        # Blocks
        ##################################################

        self.digital_chunks_to_symbols_xx_0 = digital.chunks_to_symbols_bc([0.707+0.707j, -0.707+0.707j, -0.707-0.707j, 0.707-0.707j], 1)
        self.cyberether_cyber_waterfall_sink_0 = cyberether.cyber_waterfall_sink_c(1024, "waterfall", 256, cyberether.Domain.Frequency, "1, 1, 1, 2")
        self.cyberether_cyber_lineplot_sink_1 = cyberether.cyber_lineplot_sink_c(4096, "spectrum line", cyberether.Domain.Frequency, "0, 2")
        self.cyberether_cyber_lineplot_sink_0 = cyberether.cyber_lineplot_sink_c(4096, "scope", cyberether.Domain.Time, "0, 1")
        self.cyberether_cyber_constellation_sink_0 = cyberether.cyber_constellation_sink_c(2048, "QPSK constellation", "0, 0, 2, 1")
        self.blocks_throttle2_0 = blocks.throttle( gr.sizeof_gr_complex*1, samp_rate, True, 0 if "auto" == "auto" else max( int(float(0.1) * samp_rate) if "auto" == "time" else int(0.1), 1) )
        self.blocks_add_xx_0 = blocks.add_vcc(1)
        self.analog_random_source_x_0 = blocks.vector_source_b(list(map(int, numpy.random.randint(0, 4, 65536))), True)
        self.analog_noise_source_x_0 = analog.noise_source_c(analog.GR_GAUSSIAN, 0.08, 42)


        ##################################################
        # Connections
        ##################################################
        self.connect((self.analog_noise_source_x_0, 0), (self.blocks_add_xx_0, 1))
        self.connect((self.analog_random_source_x_0, 0), (self.digital_chunks_to_symbols_xx_0, 0))
        self.connect((self.blocks_add_xx_0, 0), (self.cyberether_cyber_constellation_sink_0, 0))
        self.connect((self.blocks_add_xx_0, 0), (self.cyberether_cyber_lineplot_sink_0, 0))
        self.connect((self.blocks_add_xx_0, 0), (self.cyberether_cyber_lineplot_sink_1, 0))
        self.connect((self.blocks_add_xx_0, 0), (self.cyberether_cyber_waterfall_sink_0, 0))
        self.connect((self.blocks_throttle2_0, 0), (self.blocks_add_xx_0, 0))
        self.connect((self.digital_chunks_to_symbols_xx_0, 0), (self.blocks_throttle2_0, 0))


    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.blocks_throttle2_0.set_sample_rate(self.samp_rate)




def main(top_block_cls=constellation_demo, options=None):
    tb = top_block_cls()


    def sig_handler(sig=None, frame=None):
        tb.stop()
        tb.wait()

        sys.exit(0)

    signal.signal(signal.SIGINT, sig_handler)
    signal.signal(signal.SIGTERM, sig_handler)

    # cyberether.present(tb, device=...) starts the flowgraph, opens the
    # CyberEther window on the main thread, blocks until the user closes it,
    # then stops/waits the flowgraph. The window event loop owns the main
    # thread for the duration. `device` selects the renderer backend.

    cyberether.present(tb, device=cyberether.DeviceType.Auto)


if __name__ == '__main__':
    main()
