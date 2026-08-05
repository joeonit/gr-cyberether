#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: CyberEther - All Options Demo (4 plots, every knob)
# Author: Youssef Mahmoud
# Description: Kitchen-sink demo. Four sinks in one window, exercising every new knob: complex + float type variants, Time + Frequency display, all four Operations (Real, Imaginary, Amplitude, Phase). Two complex multi-tone and two float multi-tone sources drive the four sinks via separate paths.
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




class all_options_demo(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self, "CyberEther - All Options Demo (4 plots, every knob)", catch_exceptions=True)

        ##################################################
        # Variables
        ##################################################
        self.samp_rate = samp_rate = 48000

        ##################################################
        # Blocks
        ##################################################

        self.sink_c_float_line_freq_amp = cyberether.cyber_lineplot_sink_f(1024, "C_float_line_freq", cyberether.Domain.Frequency, '')
        self.sink_b_complex_waterfall_freq_phase = cyberether.cyber_waterfall_sink_c(1024, "B_complex_waterfall_freq", 512, cyberether.Domain.Frequency, '1,0,1,2')
        self.sink_a_complex_line_time_imag = cyberether.cyber_lineplot_sink_c(1024, "A_complex_line_time", cyberether.Domain.Time, '')
        self.blocks_throttle2_f = blocks.throttle( gr.sizeof_float*1, samp_rate, True, 0 if "auto" == "auto" else max( int(float(0.1) * samp_rate) if "auto" == "time" else int(0.1), 1) )
        self.blocks_throttle2_c = blocks.throttle( gr.sizeof_gr_complex*1, samp_rate, True, 0 if "auto" == "auto" else max( int(float(0.1) * samp_rate) if "auto" == "time" else int(0.1), 1) )
        self.blocks_add_xx_f = blocks.add_vff(1)
        self.blocks_add_xx_c = blocks.add_vcc(1)
        self.analog_sig_source_x_f2 = analog.sig_source_f(samp_rate, analog.GR_SAW_WAVE, 600, 0.4, 0, 0)
        self.analog_sig_source_x_f1 = analog.sig_source_f(samp_rate, analog.GR_COS_WAVE, 1200, 0.6, 0, 0)
        self.analog_sig_source_x_c2 = analog.sig_source_c(samp_rate, analog.GR_COS_WAVE, (-1500), 0.4, 0, 0)
        self.analog_sig_source_x_c1 = analog.sig_source_c(samp_rate, analog.GR_COS_WAVE, 800, 0.7, 0, 0)
        self.analog_noise_source_x_f = analog.noise_source_f(analog.GR_GAUSSIAN, 0.05, 0)
        self.analog_noise_source_x_c = analog.noise_source_c(analog.GR_GAUSSIAN, 0.05, 0)


        ##################################################
        # Connections
        ##################################################
        self.connect((self.analog_noise_source_x_c, 0), (self.blocks_add_xx_c, 2))
        self.connect((self.analog_noise_source_x_f, 0), (self.blocks_add_xx_f, 2))
        self.connect((self.analog_sig_source_x_c1, 0), (self.blocks_add_xx_c, 0))
        self.connect((self.analog_sig_source_x_c2, 0), (self.blocks_add_xx_c, 1))
        self.connect((self.analog_sig_source_x_f1, 0), (self.blocks_add_xx_f, 0))
        self.connect((self.analog_sig_source_x_f2, 0), (self.blocks_add_xx_f, 1))
        self.connect((self.blocks_add_xx_c, 0), (self.blocks_throttle2_c, 0))
        self.connect((self.blocks_add_xx_f, 0), (self.blocks_throttle2_f, 0))
        self.connect((self.blocks_throttle2_c, 0), (self.sink_a_complex_line_time_imag, 0))
        self.connect((self.blocks_throttle2_c, 0), (self.sink_b_complex_waterfall_freq_phase, 0))
        self.connect((self.blocks_throttle2_f, 0), (self.sink_c_float_line_freq_amp, 0))


    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.analog_sig_source_x_c1.set_sampling_freq(self.samp_rate)
        self.analog_sig_source_x_c2.set_sampling_freq(self.samp_rate)
        self.analog_sig_source_x_f1.set_sampling_freq(self.samp_rate)
        self.analog_sig_source_x_f2.set_sampling_freq(self.samp_rate)
        self.blocks_throttle2_c.set_sample_rate(self.samp_rate)
        self.blocks_throttle2_f.set_sample_rate(self.samp_rate)




def main(top_block_cls=all_options_demo, options=None):
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
