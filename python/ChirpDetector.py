# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: ChirpDetector
# Author: jp
# GNU Radio version: 3.8.1.0

from gnuradio import blocks
from gnuradio import fft
from gnuradio.fft import window
from gnuradio import gr
from gnuradio.filter import firdes
import sys
import signal
import LibreLoRa
import numpy
import pmt




class ChirpDetector(gr.hier_block2):
    def __init__(self, samp_rate, BW=125e3, SF=7, DFTSize=None, threshold=0.2, timeout=5):
        gr.hier_block2.__init__(
            self, "ChirpDetector",
                gr.io_signature(1, 1, gr.sizeof_gr_complex*1),
                gr.io_signature(1, 1, gr.sizeof_gr_complex*1),
        )
        if (DFTSize == None):
            self.DFTSize = DFTSize = int((1 << SF)*(samp_rate/BW))
            
        self.message_port_register_hier_out("detectOut")
        
        ##################################################
        # Parameters
        ##################################################
        self.samp_rate = samp_rate
        self.BW = BW
        self.DFTSize = DFTSize
        self.SF = SF
        self.threshold = threshold
        self.timeout = timeout

        ##################################################
        # Variables
        ##################################################
        self.windowSize = windowSize = DFTSize
        self.chirpRate = chirpRate = ((BW/samp_rate)**2)*(2**(-SF ))
        self.chirpWindow = chirpWindow = numpy.real(LibreLoRa.getChirpWindow(DFTSize, windowSize,0, numpy.sqrt(1/chirpRate)))*numpy.sqrt(DFTSize/windowSize)

        ##################################################
        # Blocks
        ##################################################
        self.fft_vxx_0 = fft.fft_vcc(DFTSize, True, numpy.ones(DFTSize), False, 1)
        self.blocks_stream_to_vector_0 = blocks.stream_to_vector(gr.sizeof_gr_complex*1, DFTSize)
        self.blocks_multiply_const_vxx_0 = blocks.multiply_const_vcc(chirpWindow)
        self.blocks_complex_to_mag_squared_0 = blocks.complex_to_mag_squared(DFTSize)
        self.blocks_complex_to_mag_squared_0.set_min_output_buffer(int(timeout*samp_rate/DFTSize))
        self.LibreLoRa_ToneDetector_0 = LibreLoRa.ToneDetector(DFTSize)
        self.LibreLoRa_PowerDetector_0 = LibreLoRa.PowerDetector(samp_rate, threshold, timeout, DFTSize, pmt.to_pmt((SF, BW, samp_rate)))
        self.LibreLoRa_PowerDetector_0.set_min_output_buffer(int(numpy.ceil(timeout*samp_rate)))



        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.LibreLoRa_PowerDetector_0, 'detectOut'), (self, 'detectOut'))
        self.connect((self.LibreLoRa_PowerDetector_0, 0), (self, 0))
        self.connect((self.LibreLoRa_ToneDetector_0, 0), (self.LibreLoRa_PowerDetector_0, 1))
        self.connect((self.blocks_complex_to_mag_squared_0, 0), (self.LibreLoRa_ToneDetector_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0, 0), (self.fft_vxx_0, 0))
        self.connect((self.blocks_stream_to_vector_0, 0), (self.blocks_multiply_const_vxx_0, 0))
        self.connect((self.fft_vxx_0, 0), (self.blocks_complex_to_mag_squared_0, 0))
        self.connect((self, 0), (self.LibreLoRa_PowerDetector_0, 0))
        self.connect((self, 0), (self.blocks_stream_to_vector_0, 0))

    def get_BW(self):
        return self.BW

    def set_BW(self, BW):
        self.BW = BW
        self.set_chirpRate(((self.BW/self.samp_rate)**2)*(2**(-self.SF )))

    def get_DFTSize(self):
        return self.DFTSize

    def set_DFTSize(self, DFTSize):
        self.DFTSize = DFTSize
        self.set_chirpWindow(numpy.real(LibreLoRa.getChirpWindow(self.DFTSize, self.windowSize,0, numpy.sqrt(1/self.chirpRate)))*numpy.sqrt(self.DFTSize/self.windowSize))
        self.set_windowSize(self.DFTSize)

    def get_SF(self):
        return self.SF

    def set_SF(self, SF):
        self.SF = SF
        self.set_chirpRate(((self.BW/self.samp_rate)**2)*(2**(-self.SF )))

    def get_threshold(self):
        return self.threshold

    def set_threshold(self, threshold):
        self.threshold = threshold

    def get_timeout(self):
        return self.timeout

    def set_timeout(self, timeout):
        self.timeout = timeout

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.set_chirpRate(((self.BW/self.samp_rate)**2)*(2**(-self.SF )))

    def get_windowSize(self):
        return self.windowSize

    def set_windowSize(self, windowSize):
        self.windowSize = windowSize
        self.set_chirpWindow(numpy.real(LibreLoRa.getChirpWindow(self.DFTSize, self.windowSize,0, numpy.sqrt(1/self.chirpRate)))*numpy.sqrt(self.DFTSize/self.windowSize))

    def get_chirpRate(self):
        return self.chirpRate

    def set_chirpRate(self, chirpRate):
        self.chirpRate = chirpRate
        self.set_chirpWindow(numpy.real(LibreLoRa.getChirpWindow(self.DFTSize, self.windowSize,0, numpy.sqrt(1/self.chirpRate)))*numpy.sqrt(self.DFTSize/self.windowSize))

    def get_chirpWindow(self):
        return self.chirpWindow

    def set_chirpWindow(self, chirpWindow):
        self.chirpWindow = chirpWindow
        self.blocks_multiply_const_vxx_0.set_k(self.chirpWindow)


