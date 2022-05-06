# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: LoRa Tx
# Author: jp
# GNU Radio version: 3.9.3.0

from gnuradio import gr
from gnuradio.filter import firdes
from gnuradio.fft import window
import sys
import signal
import LibreLoRa
import numpy







class LoRaTx(gr.hier_block2):
    def __init__(self, BW=125e3, CR=1, CRCPresent=True, SF=8, lowDataRate=False, nUpchirps=8, payloadSize=10, samp_rate=8e6, syncwordNumber=0):
        gr.hier_block2.__init__(
            self, "LoRa Tx",
                gr.io_signature(1, 1, gr.sizeof_char*1),
                gr.io_signature(1, 1, gr.sizeof_gr_complex*1),
        )

        ##################################################
        # Parameters
        ##################################################
        self.BW = BW
        self.CR = CR
        self.CRCPresent = CRCPresent
        self.SF = SF
        self.lowDataRate = lowDataRate
        self.nUpchirps = nUpchirps
        self.payloadSize = payloadSize
        self.samp_rate = samp_rate
        self.syncwordNumber = syncwordNumber

        ##################################################
        # Variables
        ##################################################
        self.OSF = OSF = round(samp_rate/(BW))
        self.symbolSize = symbolSize = int((2**SF)*OSF)
        self.preambleFrequency = preambleFrequency = LibreLoRa.getPreambleFrequency(SF, symbolSize, nUpchirps, syncwordNumber)

        ##################################################
        # Blocks
        ##################################################
        self.LibreLoRa_randomize_0 = LibreLoRa.randomize(255, payloadSize)
        self.LibreLoRa_TransmitterController_0 = LibreLoRa.TransmitterController(SF, symbolSize, CR, payloadSize, CRCPresent, lowDataRate)
        self.LibreLoRa_SymbolMod_0 = LibreLoRa.SymbolMod(SF, symbolSize)
        self.LibreLoRa_Interleave_0 = LibreLoRa.Interleave(SF, CR)
        self.LibreLoRa_GrayDecode_0 = LibreLoRa.GrayDecode(SF)
        self.LibreLoRa_FrequencyMod_0 = LibreLoRa.FrequencyMod()
        self.LibreLoRa_Encode_0 = LibreLoRa.Encode(CR)
        self.LibreLoRa_CRC16_0 = LibreLoRa.CRC16(0x1021, 0, payloadSize)
        self.LibreLoRa_BytesToNibbles_0 = LibreLoRa.BytesToNibbles()
        self.LibreLoRa_AppendSilence_0 = LibreLoRa.AppendSilence()
        self.LibreLoRa_AppendPrefix_0 = LibreLoRa.AppendPrefix(SF, symbolSize, nUpchirps, syncwordNumber)



        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.LibreLoRa_CRC16_0, 'crcOut'), (self.LibreLoRa_TransmitterController_0, 'setCRC'))
        self.msg_connect((self.LibreLoRa_TransmitterController_0, 'transmissionStart'), (self.LibreLoRa_AppendSilence_0, 'start'))
        self.connect((self.LibreLoRa_AppendPrefix_0, 0), (self.LibreLoRa_FrequencyMod_0, 0))
        self.connect((self.LibreLoRa_AppendSilence_0, 0), (self, 0))
        self.connect((self.LibreLoRa_BytesToNibbles_0, 0), (self.LibreLoRa_TransmitterController_0, 0))
        self.connect((self.LibreLoRa_Encode_0, 0), (self.LibreLoRa_Interleave_0, 0))
        self.connect((self.LibreLoRa_FrequencyMod_0, 0), (self.LibreLoRa_AppendSilence_0, 0))
        self.connect((self.LibreLoRa_GrayDecode_0, 0), (self.LibreLoRa_SymbolMod_0, 0))
        self.connect((self.LibreLoRa_Interleave_0, 0), (self.LibreLoRa_GrayDecode_0, 0))
        self.connect((self.LibreLoRa_SymbolMod_0, 0), (self.LibreLoRa_AppendPrefix_0, 0))
        self.connect((self.LibreLoRa_TransmitterController_0, 0), (self.LibreLoRa_Encode_0, 0))
        self.connect((self.LibreLoRa_randomize_0, 0), (self.LibreLoRa_BytesToNibbles_0, 0))
        self.connect((self, 0), (self.LibreLoRa_CRC16_0, 0))
        self.connect((self, 0), (self.LibreLoRa_randomize_0, 0))


    def get_BW(self):
        return self.BW

    def set_BW(self, BW):
        self.BW = BW
        self.set_OSF(round(self.samp_rate/(self.BW)))

    def get_CR(self):
        return self.CR

    def set_CR(self, CR):
        self.CR = CR

    def get_CRCPresent(self):
        return self.CRCPresent

    def set_CRCPresent(self, CRCPresent):
        self.CRCPresent = CRCPresent

    def get_SF(self):
        return self.SF

    def set_SF(self, SF):
        self.SF = SF
        self.set_preambleFrequency(LibreLoRa.getPreambleFrequency(self.SF, self.symbolSize, self.nUpchirps, self.syncwordNumber))
        self.set_symbolSize(int((2**self.SF)*self.OSF))

    def get_lowDataRate(self):
        return self.lowDataRate

    def set_lowDataRate(self, lowDataRate):
        self.lowDataRate = lowDataRate

    def get_nUpchirps(self):
        return self.nUpchirps

    def set_nUpchirps(self, nUpchirps):
        self.nUpchirps = nUpchirps
        self.set_preambleFrequency(LibreLoRa.getPreambleFrequency(self.SF, self.symbolSize, self.nUpchirps, self.syncwordNumber))

    def get_payloadSize(self):
        return self.payloadSize

    def set_payloadSize(self, payloadSize):
        self.payloadSize = payloadSize

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.set_OSF(round(self.samp_rate/(self.BW)))

    def get_syncwordNumber(self):
        return self.syncwordNumber

    def set_syncwordNumber(self, syncwordNumber):
        self.syncwordNumber = syncwordNumber
        self.set_preambleFrequency(LibreLoRa.getPreambleFrequency(self.SF, self.symbolSize, self.nUpchirps, self.syncwordNumber))

    def get_OSF(self):
        return self.OSF

    def set_OSF(self, OSF):
        self.OSF = OSF
        self.set_symbolSize(int((2**self.SF)*self.OSF))

    def get_symbolSize(self):
        return self.symbolSize

    def set_symbolSize(self, symbolSize):
        self.symbolSize = symbolSize
        self.set_preambleFrequency(LibreLoRa.getPreambleFrequency(self.SF, self.symbolSize, self.nUpchirps, self.syncwordNumber))

    def get_preambleFrequency(self):
        return self.preambleFrequency

    def set_preambleFrequency(self, preambleFrequency):
        self.preambleFrequency = preambleFrequency

