#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: CyberEther Line Plot - Live Demo
# Author: Youssef Mahmoud
# Description: Cosine plus a little Gaussian noise into the CyberEther line plot sink, so the trace clearly moves frame to frame.
# GNU Radio version: v3.11.0.0git-1100-gf06564b3

from gnuradio import analog
from gnuradio import blocks
from gnuradio import cyberether
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




class lineplot_live_demo(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self, "CyberEther Line Plot - Live Demo", catch_exceptions=True)

        ##################################################
        # Variables
        ##################################################
        self.samp_rate = samp_rate = 48000

        ##################################################
        # Blocks
        ##################################################

        self.cyberether_cyber_lineplot_sink_1_1_0 = cyberether.cyber_lineplot_sink_c(2048, 'four')
        self.cyberether_cyber_lineplot_sink_1_1 = cyberether.cyber_lineplot_sink_c(2048, 'three')
        self.cyberether_cyber_lineplot_sink_0_0 = cyberether.cyber_lineplot_sink_c(1024, "two")
        self.cyberether_cyber_lineplot_sink_0 = cyberether.cyber_lineplot_sink_c(1024, "one")
        self.blocks_throttle2_1_1_0 = blocks.throttle( gr.sizeof_gr_complex*1, samp_rate, True, 0 if "auto" == "auto" else max( int(float(0.1) * samp_rate) if "auto" == "time" else int(0.1), 1) )
        self.blocks_throttle2_1_1 = blocks.throttle( gr.sizeof_gr_complex*1, samp_rate, True, 0 if "auto" == "auto" else max( int(float(0.1) * samp_rate) if "auto" == "time" else int(0.1), 1) )
        self.blocks_throttle2_0_0 = blocks.throttle( gr.sizeof_gr_complex*1, samp_rate, True, 0 if "auto" == "auto" else max( int(float(0.1) * samp_rate) if "auto" == "time" else int(0.1), 1) )
        self.blocks_throttle2_0 = blocks.throttle( gr.sizeof_gr_complex*1, samp_rate, True, 0 if "auto" == "auto" else max( int(float(0.1) * samp_rate) if "auto" == "time" else int(0.1), 1) )
        self.blocks_add_xx_0_0 = blocks.add_vcc(1)
        self.blocks_add_xx_0 = blocks.add_vcc(1)
        self.analog_sig_source_x_1_1_0 = analog.sig_source_c(samp_rate, analog.GR_SAW_WAVE, 1200, 0.6, 0, 0)
        self.analog_sig_source_x_1_1 = analog.sig_source_c(samp_rate, analog.GR_SAW_WAVE, 1200, 0.6, 0, 0)
        self.analog_sig_source_x_0_0 = analog.sig_source_c(samp_rate, analog.GR_COS_WAVE, 500, 0.8, 0, 0)
        self.analog_sig_source_x_0 = analog.sig_source_c(samp_rate, analog.GR_COS_WAVE, 500, 0.8, 0, 0)
        self.analog_noise_source_x_0_0 = analog.noise_source_c(analog.GR_GAUSSIAN, 0.1, 0)
        self.analog_noise_source_x_0 = analog.noise_source_c(analog.GR_GAUSSIAN, 0.1, 0)


        ##################################################
        # Connections
        ##################################################
        self.connect((self.analog_noise_source_x_0, 0), (self.blocks_add_xx_0, 1))
        self.connect((self.analog_noise_source_x_0_0, 0), (self.blocks_add_xx_0_0, 1))
        self.connect((self.analog_sig_source_x_0, 0), (self.blocks_add_xx_0, 0))
        self.connect((self.analog_sig_source_x_0_0, 0), (self.blocks_add_xx_0_0, 0))
        self.connect((self.analog_sig_source_x_1_1, 0), (self.blocks_throttle2_1_1, 0))
        self.connect((self.analog_sig_source_x_1_1_0, 0), (self.blocks_throttle2_1_1_0, 0))
        self.connect((self.blocks_add_xx_0, 0), (self.blocks_throttle2_0, 0))
        self.connect((self.blocks_add_xx_0_0, 0), (self.blocks_throttle2_0_0, 0))
        self.connect((self.blocks_throttle2_0, 0), (self.cyberether_cyber_lineplot_sink_0, 0))
        self.connect((self.blocks_throttle2_0_0, 0), (self.cyberether_cyber_lineplot_sink_0_0, 0))
        self.connect((self.blocks_throttle2_1_1, 0), (self.cyberether_cyber_lineplot_sink_1_1, 0))
        self.connect((self.blocks_throttle2_1_1_0, 0), (self.cyberether_cyber_lineplot_sink_1_1_0, 0))


    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.analog_sig_source_x_0.set_sampling_freq(self.samp_rate)
        self.analog_sig_source_x_0_0.set_sampling_freq(self.samp_rate)
        self.analog_sig_source_x_1_1.set_sampling_freq(self.samp_rate)
        self.analog_sig_source_x_1_1_0.set_sampling_freq(self.samp_rate)
        self.blocks_throttle2_0.set_sample_rate(self.samp_rate)
        self.blocks_throttle2_0_0.set_sample_rate(self.samp_rate)
        self.blocks_throttle2_1_1.set_sample_rate(self.samp_rate)
        self.blocks_throttle2_1_1_0.set_sample_rate(self.samp_rate)




def main(top_block_cls=lineplot_live_demo, options=None):
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

    cyberether.present(tb, device=cyberether.DeviceType.Metal)


if __name__ == '__main__':
    main()
