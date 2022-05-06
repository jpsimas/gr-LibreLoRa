# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: LoRa Rx
# GNU Radio version: 3.9.3.0

from LibreLoRa import ChirpDetector
from gnuradio import blocks
from gnuradio import gr
from gnuradio.filter import firdes
from gnuradio.fft import window
import sys
import signal
import LibreLoRa
import numpy







class LoRaRx(gr.hier_block2):
    def __init__(self, BW=125e3, SF=7, decimation=1, detectionDecim=1, detectorThreshold=100, ignoreLowDataRateRequired=False, lowDataRate=False, samp_rate=1e6, syncwordNumber=0x00):
        gr.hier_block2.__init__(
            self, "LoRa Rx",
                gr.io_signature(1, 1, gr.sizeof_gr_complex*1),
                gr.io_signature(1, 1, gr.sizeof_char*1),
        )
        self.message_port_register_hier_out("payloadLengthOut")
        self.message_port_register_hier_out("reset")
        self.message_port_register_hier_out("detectionOut")
        self.message_port_register_hier_out("crcOut")

        ##################################################
        # Parameters
        ##################################################
        self.BW = BW
        self.SF = SF
        self.decimation = decimation
        self.detectionDecim = detectionDecim
        self.detectorThreshold = detectorThreshold
        self.ignoreLowDataRateRequired = ignoreLowDataRateRequired
        self.lowDataRate = lowDataRate
        self.samp_rate = samp_rate
        self.syncwordNumber = syncwordNumber

        ##################################################
        # Variables
        ##################################################
        self.OSF = OSF = round(samp_rate/(BW))
        self.symbolSize = symbolSize = int(((2**SF)*OSF)/decimation)
        self.windowSize = windowSize = int(symbolSize*decimation/4)
        self.nUpchirps = nUpchirps = 1
        self.DFTSize = DFTSize = int(symbolSize*decimation)
        self.upchirp = upchirp = numpy.asarray(LibreLoRa.getSymbol(0, SF, symbolSize))
        self.preambleNormalized = preambleNormalized = LibreLoRa.getNormalizedPreambleFrequency(SF, symbolSize, nUpchirps, syncwordNumber)
        self.preambleComplex = preambleComplex = numpy.exp(1j*2*numpy.pi*OSF*decimation*LibreLoRa.getPreambleFrequency(SF, symbolSize, nUpchirps, syncwordNumber))
        self.maxFrameTime = maxFrameTime = (2**(SF)/BW)*(5 + 8 + 257*2*8/SF)
        self.lowDataRateRequired = lowDataRateRequired = ((BW*(2**(-SF))) < 62.5) and (not ignoreLowDataRateRequired)
        self.chirpWindow = chirpWindow = numpy.real(LibreLoRa.getChirpWindow(DFTSize, windowSize, SF, symbolSize*decimation))

        ##################################################
        # Blocks
        ##################################################
        self.blocks_multiply_const_vxx_2 = blocks.multiply_const_ff(decimation)
        self.blocks_int_to_float_0 = blocks.int_to_float(1, DFTSize)
        self.LibreLoRa_symbolDemodNew_0 = LibreLoRa.symbolDemodNew(SF, symbolSize, int(symbolSize/2))
        self.LibreLoRa_streamToHistoryVector_0 = LibreLoRa.streamToHistoryVector_cc(DFTSize, decimation)
        self.LibreLoRa_receiverController_0 = LibreLoRa.receiverController(SF, lowDataRate or lowDataRateRequired)
        self.LibreLoRa_randomize_0 = LibreLoRa.randomize(0, 0)
        self.LibreLoRa_grayEncode_0 = LibreLoRa.grayEncode(SF)
        self.LibreLoRa_deinterleave_0 = LibreLoRa.deinterleave(7, 4)
        self.LibreLoRa_decode_0 = LibreLoRa.decode(4)
        self.LibreLoRa_correlationSync_0 = LibreLoRa.correlationSync_ff(0.8, 0.6, symbolSize, preambleNormalized.size, SF, syncwordNumber)
        self.LibreLoRa_Remainder_0_0 = LibreLoRa.Remainder(1.0)
        self.LibreLoRa_NibblesToBytes_0 = LibreLoRa.NibblesToBytes()
        self.LibreLoRa_FFTMax_0 = LibreLoRa.FFTMax(DFTSize, chirpWindow)
        self.LibreLoRa_Correlation_0 = LibreLoRa.Correlation_ff(preambleNormalized)
        self.LibreLoRa_ChirpDetector_0 = ChirpDetector(
            samp_rate=samp_rate,
            BW=BW,
            SF=SF,
            DFTSize=symbolSize*decimation,
            threshold=detectorThreshold,
            timeout=maxFrameTime,
            DFTDecim=detectionDecim,
        )
        self.LibreLoRa_CRC16_0 = LibreLoRa.CRC16(0x1021, 0x0000, 0)



        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.LibreLoRa_CRC16_0, 'crcOut'), (self, 'crcOut'))
        self.msg_connect((self.LibreLoRa_ChirpDetector_0, 'detectOut'), (self.LibreLoRa_Correlation_0, 'enable'))
        self.msg_connect((self.LibreLoRa_ChirpDetector_0, 'detectOut'), (self.LibreLoRa_FFTMax_0, 'enable'))
        self.msg_connect((self.LibreLoRa_ChirpDetector_0, 'detectOut'), (self.LibreLoRa_correlationSync_0, 'reset'))
        self.msg_connect((self.LibreLoRa_ChirpDetector_0, 'detectOut'), (self, 'detectionOut'))
        self.msg_connect((self.LibreLoRa_correlationSync_0, 'sync'), (self.LibreLoRa_Correlation_0, 'reset'))
        self.msg_connect((self.LibreLoRa_correlationSync_0, 'sync'), (self.LibreLoRa_receiverController_0, 'sync'))
        self.msg_connect((self.LibreLoRa_receiverController_0, 'crc'), (self.LibreLoRa_CRC16_0, 'setXorOut'))
        self.msg_connect((self.LibreLoRa_receiverController_0, 'payloadLength'), (self.LibreLoRa_CRC16_0, 'setPayloadSize'))
        self.msg_connect((self.LibreLoRa_receiverController_0, 'synchronizerReset'), (self.LibreLoRa_ChirpDetector_0, 'reset'))
        self.msg_connect((self.LibreLoRa_receiverController_0, 'synchronizerReset'), (self.LibreLoRa_FFTMax_0, 'reset'))
        self.msg_connect((self.LibreLoRa_receiverController_0, 'synchronizerSetN'), (self.LibreLoRa_correlationSync_0, 'setNOutputItemsToProduce'))
        self.msg_connect((self.LibreLoRa_receiverController_0, 'synchronizerReset'), (self.LibreLoRa_correlationSync_0, 'reset'))
        self.msg_connect((self.LibreLoRa_receiverController_0, 'setCRout'), (self.LibreLoRa_decode_0, 'setCR'))
        self.msg_connect((self.LibreLoRa_receiverController_0, 'setCRout'), (self.LibreLoRa_deinterleave_0, 'setCR'))
        self.msg_connect((self.LibreLoRa_receiverController_0, 'setSFout'), (self.LibreLoRa_deinterleave_0, 'setSF'))
        self.msg_connect((self.LibreLoRa_receiverController_0, 'setSFout'), (self.LibreLoRa_grayEncode_0, 'setSF'))
        self.msg_connect((self.LibreLoRa_receiverController_0, 'lfsrStateOut'), (self.LibreLoRa_randomize_0, 'setLfsrState'))
        self.msg_connect((self.LibreLoRa_receiverController_0, 'setSFout'), (self.LibreLoRa_symbolDemodNew_0, 'setSF'))
        self.msg_connect((self.LibreLoRa_receiverController_0, 'payloadLength'), (self, 'payloadLengthOut'))
        self.msg_connect((self.LibreLoRa_receiverController_0, 'synchronizerReset'), (self, 'reset'))
        self.connect((self.LibreLoRa_ChirpDetector_0, 0), (self.LibreLoRa_streamToHistoryVector_0, 0))
        self.connect((self.LibreLoRa_Correlation_0, 1), (self.LibreLoRa_correlationSync_0, 0))
        self.connect((self.LibreLoRa_Correlation_0, 0), (self.LibreLoRa_correlationSync_0, 1))
        self.connect((self.LibreLoRa_FFTMax_0, 0), (self.blocks_int_to_float_0, 0))
        self.connect((self.LibreLoRa_NibblesToBytes_0, 0), (self.LibreLoRa_randomize_0, 0))
        self.connect((self.LibreLoRa_Remainder_0_0, 0), (self.blocks_multiply_const_vxx_2, 0))
        self.connect((self.LibreLoRa_correlationSync_0, 0), (self.LibreLoRa_symbolDemodNew_0, 0))
        self.connect((self.LibreLoRa_decode_0, 0), (self.LibreLoRa_receiverController_0, 0))
        self.connect((self.LibreLoRa_deinterleave_0, 0), (self.LibreLoRa_decode_0, 0))
        self.connect((self.LibreLoRa_grayEncode_0, 0), (self.LibreLoRa_deinterleave_0, 0))
        self.connect((self.LibreLoRa_randomize_0, 0), (self.LibreLoRa_CRC16_0, 0))
        self.connect((self.LibreLoRa_randomize_0, 0), (self, 0))
        self.connect((self.LibreLoRa_receiverController_0, 0), (self.LibreLoRa_NibblesToBytes_0, 0))
        self.connect((self.LibreLoRa_streamToHistoryVector_0, 0), (self.LibreLoRa_FFTMax_0, 0))
        self.connect((self.LibreLoRa_symbolDemodNew_0, 0), (self.LibreLoRa_grayEncode_0, 0))
        self.connect((self.blocks_int_to_float_0, 0), (self.LibreLoRa_Remainder_0_0, 0))
        self.connect((self.blocks_multiply_const_vxx_2, 0), (self.LibreLoRa_Correlation_0, 0))
        self.connect((self, 0), (self.LibreLoRa_ChirpDetector_0, 0))


    def get_BW(self):
        return self.BW

    def set_BW(self, BW):
        self.BW = BW
        self.set_OSF(round(self.samp_rate/(self.BW)))
        self.set_lowDataRateRequired(((self.BW*(2**(-self.SF))) < 62.5) and (not self.ignoreLowDataRateRequired))
        self.set_maxFrameTime((2**(self.SF)/self.BW)*(5 + 8 + 257*2*8/self.SF))
        self.LibreLoRa_ChirpDetector_0.set_BW(self.BW)

    def get_SF(self):
        return self.SF

    def set_SF(self, SF):
        self.SF = SF
        self.set_chirpWindow(numpy.real(LibreLoRa.getChirpWindow(self.DFTSize, self.windowSize, self.SF, self.symbolSize*self.decimation)))
        self.set_lowDataRateRequired(((self.BW*(2**(-self.SF))) < 62.5) and (not self.ignoreLowDataRateRequired))
        self.set_maxFrameTime((2**(self.SF)/self.BW)*(5 + 8 + 257*2*8/self.SF))
        self.set_preambleComplex(numpy.exp(1j*2*numpy.pi*self.OSF*self.decimation*LibreLoRa.getPreambleFrequency(self.SF, self.symbolSize, self.nUpchirps, self.syncwordNumber)))
        self.set_preambleNormalized(LibreLoRa.getNormalizedPreambleFrequency(self.SF, self.symbolSize, self.nUpchirps, self.syncwordNumber))
        self.set_symbolSize(int(((2**self.SF)*self.OSF)/self.decimation))
        self.set_upchirp(numpy.asarray(LibreLoRa.getSymbol(0, self.SF, self.symbolSize)))
        self.LibreLoRa_ChirpDetector_0.set_SF(self.SF)
        self.LibreLoRa_grayEncode_0.setSF(self.SF)
        self.LibreLoRa_symbolDemodNew_0.setSF(self.SF)

    def get_decimation(self):
        return self.decimation

    def set_decimation(self, decimation):
        self.decimation = decimation
        self.set_DFTSize(int(self.symbolSize*self.decimation))
        self.set_chirpWindow(numpy.real(LibreLoRa.getChirpWindow(self.DFTSize, self.windowSize, self.SF, self.symbolSize*self.decimation)))
        self.set_preambleComplex(numpy.exp(1j*2*numpy.pi*self.OSF*self.decimation*LibreLoRa.getPreambleFrequency(self.SF, self.symbolSize, self.nUpchirps, self.syncwordNumber)))
        self.set_symbolSize(int(((2**self.SF)*self.OSF)/self.decimation))
        self.set_windowSize(int(self.symbolSize*self.decimation/4))
        self.LibreLoRa_ChirpDetector_0.set_DFTSize(self.symbolSize*self.decimation)
        self.blocks_multiply_const_vxx_2.set_k(self.decimation)

    def get_detectionDecim(self):
        return self.detectionDecim

    def set_detectionDecim(self, detectionDecim):
        self.detectionDecim = detectionDecim

    def get_detectorThreshold(self):
        return self.detectorThreshold

    def set_detectorThreshold(self, detectorThreshold):
        self.detectorThreshold = detectorThreshold
        self.LibreLoRa_ChirpDetector_0.set_threshold(self.detectorThreshold)

    def get_ignoreLowDataRateRequired(self):
        return self.ignoreLowDataRateRequired

    def set_ignoreLowDataRateRequired(self, ignoreLowDataRateRequired):
        self.ignoreLowDataRateRequired = ignoreLowDataRateRequired
        self.set_lowDataRateRequired(((self.BW*(2**(-self.SF))) < 62.5) and (not self.ignoreLowDataRateRequired))

    def get_lowDataRate(self):
        return self.lowDataRate

    def set_lowDataRate(self, lowDataRate):
        self.lowDataRate = lowDataRate

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.set_OSF(round(self.samp_rate/(self.BW)))

    def get_syncwordNumber(self):
        return self.syncwordNumber

    def set_syncwordNumber(self, syncwordNumber):
        self.syncwordNumber = syncwordNumber
        self.set_preambleComplex(numpy.exp(1j*2*numpy.pi*self.OSF*self.decimation*LibreLoRa.getPreambleFrequency(self.SF, self.symbolSize, self.nUpchirps, self.syncwordNumber)))
        self.set_preambleNormalized(LibreLoRa.getNormalizedPreambleFrequency(self.SF, self.symbolSize, self.nUpchirps, self.syncwordNumber))

    def get_OSF(self):
        return self.OSF

    def set_OSF(self, OSF):
        self.OSF = OSF
        self.set_preambleComplex(numpy.exp(1j*2*numpy.pi*self.OSF*self.decimation*LibreLoRa.getPreambleFrequency(self.SF, self.symbolSize, self.nUpchirps, self.syncwordNumber)))
        self.set_symbolSize(int(((2**self.SF)*self.OSF)/self.decimation))

    def get_symbolSize(self):
        return self.symbolSize

    def set_symbolSize(self, symbolSize):
        self.symbolSize = symbolSize
        self.set_DFTSize(int(self.symbolSize*self.decimation))
        self.set_chirpWindow(numpy.real(LibreLoRa.getChirpWindow(self.DFTSize, self.windowSize, self.SF, self.symbolSize*self.decimation)))
        self.set_preambleComplex(numpy.exp(1j*2*numpy.pi*self.OSF*self.decimation*LibreLoRa.getPreambleFrequency(self.SF, self.symbolSize, self.nUpchirps, self.syncwordNumber)))
        self.set_preambleNormalized(LibreLoRa.getNormalizedPreambleFrequency(self.SF, self.symbolSize, self.nUpchirps, self.syncwordNumber))
        self.set_upchirp(numpy.asarray(LibreLoRa.getSymbol(0, self.SF, self.symbolSize)))
        self.set_windowSize(int(self.symbolSize*self.decimation/4))
        self.LibreLoRa_ChirpDetector_0.set_DFTSize(self.symbolSize*self.decimation)

    def get_windowSize(self):
        return self.windowSize

    def set_windowSize(self, windowSize):
        self.windowSize = windowSize
        self.set_chirpWindow(numpy.real(LibreLoRa.getChirpWindow(self.DFTSize, self.windowSize, self.SF, self.symbolSize*self.decimation)))

    def get_nUpchirps(self):
        return self.nUpchirps

    def set_nUpchirps(self, nUpchirps):
        self.nUpchirps = nUpchirps
        self.set_preambleComplex(numpy.exp(1j*2*numpy.pi*self.OSF*self.decimation*LibreLoRa.getPreambleFrequency(self.SF, self.symbolSize, self.nUpchirps, self.syncwordNumber)))
        self.set_preambleNormalized(LibreLoRa.getNormalizedPreambleFrequency(self.SF, self.symbolSize, self.nUpchirps, self.syncwordNumber))

    def get_DFTSize(self):
        return self.DFTSize

    def set_DFTSize(self, DFTSize):
        self.DFTSize = DFTSize
        self.set_chirpWindow(numpy.real(LibreLoRa.getChirpWindow(self.DFTSize, self.windowSize, self.SF, self.symbolSize*self.decimation)))
        self.blocks_int_to_float_0.set_scale(self.DFTSize)

    def get_upchirp(self):
        return self.upchirp

    def set_upchirp(self, upchirp):
        self.upchirp = upchirp

    def get_preambleNormalized(self):
        return self.preambleNormalized

    def set_preambleNormalized(self, preambleNormalized):
        self.preambleNormalized = preambleNormalized

    def get_preambleComplex(self):
        return self.preambleComplex

    def set_preambleComplex(self, preambleComplex):
        self.preambleComplex = preambleComplex

    def get_maxFrameTime(self):
        return self.maxFrameTime

    def set_maxFrameTime(self, maxFrameTime):
        self.maxFrameTime = maxFrameTime
        self.LibreLoRa_ChirpDetector_0.set_timeout(self.maxFrameTime)

    def get_lowDataRateRequired(self):
        return self.lowDataRateRequired

    def set_lowDataRateRequired(self, lowDataRateRequired):
        self.lowDataRateRequired = lowDataRateRequired

    def get_chirpWindow(self):
        return self.chirpWindow

    def set_chirpWindow(self, chirpWindow):
        self.chirpWindow = chirpWindow

