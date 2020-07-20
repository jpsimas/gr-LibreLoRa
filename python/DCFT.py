# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: DCFT
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
import threading





class DCFT(gr.hier_block2):
    def __init__(self, DFTSize=2048, chirpRate=2**(-11), samp_rate=1e6, windowSize=32):
        gr.hier_block2.__init__(
            self, "DCFT",
                gr.io_signature(1, 1, gr.sizeof_gr_complex*1),
                gr.io_signature(1, 1, gr.sizeof_gr_complex*DFTSize),
        )

        self._lock = threading.RLock()

        ##################################################
        # Parameters
        ##################################################
        self.DFTSize = DFTSize
        self.chirpRate = chirpRate
        self.samp_rate = samp_rate
        self.windowSize = windowSize

        ##################################################
        # Variables
        ##################################################
        self.chirpWindow = chirpWindow = numpy.real(LibreLoRa.getChirpWindow(DFTSize, windowSize,0, numpy.sqrt(1/chirpRate)))*numpy.sqrt(DFTSize/windowSize)

        ##################################################
        # Blocks
        ##################################################
        self.fft_vxx_0 = fft.fft_vcc(DFTSize, True, numpy.ones(DFTSize), False, 1)
        self.blocks_stream_to_vector_0 = blocks.stream_to_vector(gr.sizeof_gr_complex*1, DFTSize)
        self.blocks_multiply_const_vxx_0 = blocks.multiply_const_vcc(chirpWindow)



        ##################################################
        # Connections
        ##################################################
        self.connect((self.blocks_multiply_const_vxx_0, 0), (self.fft_vxx_0, 0))
        self.connect((self.blocks_stream_to_vector_0, 0), (self.blocks_multiply_const_vxx_0, 0))
        self.connect((self.fft_vxx_0, 0), (self, 0))
        self.connect((self, 0), (self.blocks_stream_to_vector_0, 0))


    def get_DFTSize(self):
        return self.DFTSize

    def set_DFTSize(self, DFTSize):
        with self._lock:
            self.DFTSize = DFTSize
            self.set_chirpWindow(numpy.real(LibreLoRa.getChirpWindow(self.DFTSize, self.windowSize,0, numpy.sqrt(1/self.chirpRate)))*numpy.sqrt(self.DFTSize/self.windowSize))

    def get_chirpRate(self):
        return self.chirpRate

    def set_chirpRate(self, chirpRate):
        with self._lock:
            self.chirpRate = chirpRate
            self.set_chirpWindow(numpy.real(LibreLoRa.getChirpWindow(self.DFTSize, self.windowSize,0, numpy.sqrt(1/self.chirpRate)))*numpy.sqrt(self.DFTSize/self.windowSize))

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        with self._lock:
            self.samp_rate = samp_rate

    def get_windowSize(self):
        return self.windowSize

    def set_windowSize(self, windowSize):
        with self._lock:
            self.windowSize = windowSize
            self.set_chirpWindow(numpy.real(LibreLoRa.getChirpWindow(self.DFTSize, self.windowSize,0, numpy.sqrt(1/self.chirpRate)))*numpy.sqrt(self.DFTSize/self.windowSize))

    def get_chirpWindow(self):
        return self.chirpWindow

    def set_chirpWindow(self, chirpWindow):
        with self._lock:
            self.chirpWindow = chirpWindow
            self.blocks_multiply_const_vxx_0.set_k(self.chirpWindow)


