#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: All constellations demo
# Description: Four digital modulations rendered by four constellation sinks in one window - BPSK, QPSK, 8-PSK, 16-QAM - each with the same additive noise, showing the constellation-density/noise trade-off.
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




class all_constellations_demo(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self, "All constellations demo", catch_exceptions=True)

        ##################################################
        # Variables
        ##################################################
        self.samp_rate = samp_rate = 256000
        self.noise_amp = noise_amp = 0.05

        ##################################################
        # Blocks
        ##################################################

        self.digital_chunks_to_symbols_xx_3 = digital.chunks_to_symbols_bc([(a+b*1j)/(10**0.5) for a in (-3,-1,1,3) for b in (-3,-1,1,3)], 1)
        self.digital_chunks_to_symbols_xx_2 = digital.chunks_to_symbols_bc([1, 0.707+0.707j, 1j, -0.707+0.707j, -1, -0.707-0.707j, -1j, 0.707-0.707j], 1)
        self.digital_chunks_to_symbols_xx_1 = digital.chunks_to_symbols_bc([0.707+0.707j, -0.707+0.707j, -0.707-0.707j, 0.707-0.707j], 1)
        self.digital_chunks_to_symbols_xx_0 = digital.chunks_to_symbols_bc([1+0j, -1+0j], 1)
        self.cyberether_cyber_constellation_sink_3 = cyberether.cyber_constellation_sink_c(2048, "16-QAM (4 bits/sym)", "1, 1")
        self.cyberether_cyber_constellation_sink_2 = cyberether.cyber_constellation_sink_c(2048, "8-PSK (3 bits/sym)", "1, 0")
        self.cyberether_cyber_constellation_sink_1 = cyberether.cyber_constellation_sink_c(2048, "QPSK (2 bits/sym)", "0, 1")
        self.cyberether_cyber_constellation_sink_0 = cyberether.cyber_constellation_sink_c(2048, "BPSK (1 bit/sym)", "0, 0")
        self.blocks_throttle2_3 = blocks.throttle( gr.sizeof_gr_complex*1, samp_rate, True, 0 if "auto" == "auto" else max( int(float(0.1) * samp_rate) if "auto" == "time" else int(0.1), 1) )
        self.blocks_throttle2_2 = blocks.throttle( gr.sizeof_gr_complex*1, samp_rate, True, 0 if "auto" == "auto" else max( int(float(0.1) * samp_rate) if "auto" == "time" else int(0.1), 1) )
        self.blocks_throttle2_1 = blocks.throttle( gr.sizeof_gr_complex*1, samp_rate, True, 0 if "auto" == "auto" else max( int(float(0.1) * samp_rate) if "auto" == "time" else int(0.1), 1) )
        self.blocks_throttle2_0 = blocks.throttle( gr.sizeof_gr_complex*1, samp_rate, True, 0 if "auto" == "auto" else max( int(float(0.1) * samp_rate) if "auto" == "time" else int(0.1), 1) )
        self.blocks_add_xx_3 = blocks.add_vcc(1)
        self.blocks_add_xx_2 = blocks.add_vcc(1)
        self.blocks_add_xx_1 = blocks.add_vcc(1)
        self.blocks_add_xx_0 = blocks.add_vcc(1)
        self.analog_random_source_x_3 = blocks.vector_source_b(list(map(int, numpy.random.randint(0, 16, 8192))), True)
        self.analog_random_source_x_2 = blocks.vector_source_b(list(map(int, numpy.random.randint(0, 8, 8192))), True)
        self.analog_random_source_x_1 = blocks.vector_source_b(list(map(int, numpy.random.randint(0, 4, 8192))), True)
        self.analog_random_source_x_0 = blocks.vector_source_b(list(map(int, numpy.random.randint(0, 2, 8192))), True)
        self.analog_noise_source_x_0 = analog.noise_source_c(analog.GR_GAUSSIAN, noise_amp, 42)


        ##################################################
        # Connections
        ##################################################
        self.connect((self.analog_noise_source_x_0, 0), (self.blocks_add_xx_0, 1))
        self.connect((self.analog_noise_source_x_0, 0), (self.blocks_add_xx_1, 1))
        self.connect((self.analog_noise_source_x_0, 0), (self.blocks_add_xx_2, 1))
        self.connect((self.analog_noise_source_x_0, 0), (self.blocks_add_xx_3, 1))
        self.connect((self.analog_random_source_x_0, 0), (self.digital_chunks_to_symbols_xx_0, 0))
        self.connect((self.analog_random_source_x_1, 0), (self.digital_chunks_to_symbols_xx_1, 0))
        self.connect((self.analog_random_source_x_2, 0), (self.digital_chunks_to_symbols_xx_2, 0))
        self.connect((self.analog_random_source_x_3, 0), (self.digital_chunks_to_symbols_xx_3, 0))
        self.connect((self.blocks_add_xx_0, 0), (self.cyberether_cyber_constellation_sink_0, 0))
        self.connect((self.blocks_add_xx_1, 0), (self.cyberether_cyber_constellation_sink_1, 0))
        self.connect((self.blocks_add_xx_2, 0), (self.cyberether_cyber_constellation_sink_2, 0))
        self.connect((self.blocks_add_xx_3, 0), (self.cyberether_cyber_constellation_sink_3, 0))
        self.connect((self.blocks_throttle2_0, 0), (self.blocks_add_xx_0, 0))
        self.connect((self.blocks_throttle2_1, 0), (self.blocks_add_xx_1, 0))
        self.connect((self.blocks_throttle2_2, 0), (self.blocks_add_xx_2, 0))
        self.connect((self.blocks_throttle2_3, 0), (self.blocks_add_xx_3, 0))
        self.connect((self.digital_chunks_to_symbols_xx_0, 0), (self.blocks_throttle2_0, 0))
        self.connect((self.digital_chunks_to_symbols_xx_1, 0), (self.blocks_throttle2_1, 0))
        self.connect((self.digital_chunks_to_symbols_xx_2, 0), (self.blocks_throttle2_2, 0))
        self.connect((self.digital_chunks_to_symbols_xx_3, 0), (self.blocks_throttle2_3, 0))


    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.blocks_throttle2_0.set_sample_rate(self.samp_rate)
        self.blocks_throttle2_1.set_sample_rate(self.samp_rate)
        self.blocks_throttle2_2.set_sample_rate(self.samp_rate)
        self.blocks_throttle2_3.set_sample_rate(self.samp_rate)

    def get_noise_amp(self):
        return self.noise_amp

    def set_noise_amp(self, noise_amp):
        self.noise_amp = noise_amp
        self.analog_noise_source_x_0.set_amplitude(self.noise_amp)




def main(top_block_cls=all_constellations_demo, options=None):
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
