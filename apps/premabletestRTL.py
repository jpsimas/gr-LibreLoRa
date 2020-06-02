#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: Not titled yet
# Author: jp
# GNU Radio version: 3.8.0.0

from distutils.version import StrictVersion

if __name__ == '__main__':
    import ctypes
    import sys
    if sys.platform.startswith('linux'):
        try:
            x11 = ctypes.cdll.LoadLibrary('libX11.so')
            x11.XInitThreads()
        except:
            print("Warning: failed to XInitThreads()")

from PyQt5 import Qt
from gnuradio import qtgui
from gnuradio.filter import firdes
import sip
from gnuradio import blocks
import pmt
from gnuradio import gr
import sys
import signal
from argparse import ArgumentParser
from gnuradio.eng_arg import eng_float, intx
from gnuradio import eng_notation
import LibreLoRa
import numpy
from gnuradio import qtgui

class premabletestRTL(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "Not titled yet")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("Not titled yet")
        qtgui.util.check_set_qss()
        try:
            self.setWindowIcon(Qt.QIcon.fromTheme('gnuradio-grc'))
        except:
            pass
        self.top_scroll_layout = Qt.QVBoxLayout()
        self.setLayout(self.top_scroll_layout)
        self.top_scroll = Qt.QScrollArea()
        self.top_scroll.setFrameStyle(Qt.QFrame.NoFrame)
        self.top_scroll_layout.addWidget(self.top_scroll)
        self.top_scroll.setWidgetResizable(True)
        self.top_widget = Qt.QWidget()
        self.top_scroll.setWidget(self.top_widget)
        self.top_layout = Qt.QVBoxLayout(self.top_widget)
        self.top_grid_layout = Qt.QGridLayout()
        self.top_layout.addLayout(self.top_grid_layout)

        self.settings = Qt.QSettings("GNU Radio", "premabletestRTL")

        try:
            if StrictVersion(Qt.qVersion()) < StrictVersion("5.0.0"):
                self.restoreGeometry(self.settings.value("geometry").toByteArray())
            else:
                self.restoreGeometry(self.settings.value("geometry"))
        except:
            pass

        ##################################################
        # Variables
        ##################################################
        self.BW = BW = 125e3
        self.samp_rate = samp_rate = 2*BW
        self.decimation = decimation = 2
        self.SF = SF = 7
        self.OSF = OSF = round(samp_rate/(BW*decimation))
        self.symbolSize = symbolSize = (2**SF)*OSF
        self.windowSize = windowSize = int(symbolSize/4)
        self.syncwordNumber = syncwordNumber = 0x00
        self.nUpchirps = nUpchirps = 0
        self.DFTSize = DFTSize = 2*symbolSize
        self.upchirp = upchirp = numpy.asarray(LibreLoRa.getSymbol(0, SF, OSF))
        self.preambleNormalized = preambleNormalized = LibreLoRa.getPreambleFrequency(SF, symbolSize, nUpchirps, syncwordNumber)
        self.chirpWindow = chirpWindow = numpy.real(LibreLoRa.getChirpWindow(DFTSize, windowSize, SF, symbolSize))

        ##################################################
        # Blocks
        ##################################################
        self.qtgui_waterfall_sink_x_0_0 = qtgui.waterfall_sink_c(
            2048, #size
            firdes.WIN_BLACKMAN_hARRIS, #wintype
            0, #fc
            samp_rate, #bw
            "", #name
            1 #number of inputs
        )
        self.qtgui_waterfall_sink_x_0_0.set_update_time(0.01)
        self.qtgui_waterfall_sink_x_0_0.enable_grid(False)
        self.qtgui_waterfall_sink_x_0_0.enable_axis_labels(True)



        labels = ['', '', '', '', '',
                  '', '', '', '', '']
        colors = [0, 0, 0, 0, 0,
                  0, 0, 0, 0, 0]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
                  1.0, 1.0, 1.0, 1.0, 1.0]

        for i in range(1):
            if len(labels[i]) == 0:
                self.qtgui_waterfall_sink_x_0_0.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_waterfall_sink_x_0_0.set_line_label(i, labels[i])
            self.qtgui_waterfall_sink_x_0_0.set_color_map(i, colors[i])
            self.qtgui_waterfall_sink_x_0_0.set_line_alpha(i, alphas[i])

        self.qtgui_waterfall_sink_x_0_0.set_intensity_range(-140, 10)

        self._qtgui_waterfall_sink_x_0_0_win = sip.wrapinstance(self.qtgui_waterfall_sink_x_0_0.pyqwidget(), Qt.QWidget)
        self.top_grid_layout.addWidget(self._qtgui_waterfall_sink_x_0_0_win)
        self.qtgui_time_sink_x_0_0_0 = qtgui.time_sink_f(
            int((4 + 1/4 + 2**(-SF)  + 5)*symbolSize), #size
            samp_rate, #samp_rate
            "frequencyTracker output", #name
            2 #number of inputs
        )
        self.qtgui_time_sink_x_0_0_0.set_update_time(0.10)
        self.qtgui_time_sink_x_0_0_0.set_y_axis(-1, 1)

        self.qtgui_time_sink_x_0_0_0.set_y_label('Amplitude', "")

        self.qtgui_time_sink_x_0_0_0.enable_tags(True)
        self.qtgui_time_sink_x_0_0_0.set_trigger_mode(qtgui.TRIG_MODE_AUTO, qtgui.TRIG_SLOPE_POS, 0.9, 0, 0, "")
        self.qtgui_time_sink_x_0_0_0.enable_autoscale(False)
        self.qtgui_time_sink_x_0_0_0.enable_grid(False)
        self.qtgui_time_sink_x_0_0_0.enable_axis_labels(True)
        self.qtgui_time_sink_x_0_0_0.enable_control_panel(True)
        self.qtgui_time_sink_x_0_0_0.enable_stem_plot(False)


        labels = ['Signal 1', 'Signal 2', 'Signal 3', 'Signal 4', 'Signal 5',
            'Signal 6', 'Signal 7', 'Signal 8', 'Signal 9', 'Signal 10']
        widths = [1, 1, 1, 1, 1,
            1, 1, 1, 1, 1]
        colors = ['blue', 'red', 'green', 'black', 'cyan',
            'magenta', 'yellow', 'dark red', 'dark green', 'dark blue']
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
            1.0, 1.0, 1.0, 1.0, 1.0]
        styles = [1, 1, 1, 1, 1,
            1, 1, 1, 1, 1]
        markers = [-1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1]


        for i in range(2):
            if len(labels[i]) == 0:
                self.qtgui_time_sink_x_0_0_0.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_time_sink_x_0_0_0.set_line_label(i, labels[i])
            self.qtgui_time_sink_x_0_0_0.set_line_width(i, widths[i])
            self.qtgui_time_sink_x_0_0_0.set_line_color(i, colors[i])
            self.qtgui_time_sink_x_0_0_0.set_line_style(i, styles[i])
            self.qtgui_time_sink_x_0_0_0.set_line_marker(i, markers[i])
            self.qtgui_time_sink_x_0_0_0.set_line_alpha(i, alphas[i])

        self._qtgui_time_sink_x_0_0_0_win = sip.wrapinstance(self.qtgui_time_sink_x_0_0_0.pyqwidget(), Qt.QWidget)
        self.top_grid_layout.addWidget(self._qtgui_time_sink_x_0_0_0_win)
        self.blocks_throttle_0 = blocks.throttle(gr.sizeof_gr_complex*1, samp_rate/decimation,True)
        self.blocks_tcp_server_sink_0 = blocks.tcp_server_sink(gr.sizeof_char*1, '127.0.0.1', 1202, True)
        self.blocks_null_sink_0 = blocks.null_sink(gr.sizeof_short*1)
        self.blocks_file_source_0 = blocks.file_source(gr.sizeof_gr_complex*1, '/home/jp/gnuradio/out_sdr_OSF_1_255as_SF_7_crcon_CR_1.raw', False, 0, 0)
        self.blocks_file_source_0.set_begin_tag(pmt.PMT_NIL)
        self.LibreLoRa_receiverController_0 = LibreLoRa.receiverController(SF)
        self.LibreLoRa_randomize_0 = LibreLoRa.randomize()
        self.LibreLoRa_grayEncode_0 = LibreLoRa.grayEncode(SF)
        self.LibreLoRa_frequencyTrackerN_0 = LibreLoRa.frequencyTrackerN(1/(2*OSF), SF, OSF, chirpWindow)
        self.LibreLoRa_deinterleave_0 = LibreLoRa.deinterleave(7, 4)
        self.LibreLoRa_decode_0 = LibreLoRa.decode(4)
        self.LibreLoRa_correlationSyncDemod_0 = LibreLoRa.correlationSyncDemod(0.9, 0.7, symbolSize, preambleNormalized.size, SF, syncwordNumber)
        self.LibreLoRa_NibblesToBytes_0 = LibreLoRa.NibblesToBytes()
        self.LibreLoRa_Correlation_0 = LibreLoRa.Correlation(preambleNormalized)
        self.LibreLoRa_CRC16_0 = LibreLoRa.CRC16(0x1021, 0x0000)



        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.LibreLoRa_correlationSyncDemod_0, 'sync'), (self.LibreLoRa_receiverController_0, 'sync'))
        self.msg_connect((self.LibreLoRa_receiverController_0, 'crc'), (self.LibreLoRa_CRC16_0, 'setXorOut'))
        self.msg_connect((self.LibreLoRa_receiverController_0, 'payloadLength'), (self.LibreLoRa_CRC16_0, 'setPayloadSize'))
        self.msg_connect((self.LibreLoRa_receiverController_0, 'synchronizerReset'), (self.LibreLoRa_correlationSyncDemod_0, 'reset'))
        self.msg_connect((self.LibreLoRa_receiverController_0, 'synchronizerSetN'), (self.LibreLoRa_correlationSyncDemod_0, 'setNOutputItemsToProduce'))
        self.msg_connect((self.LibreLoRa_receiverController_0, 'setSFout'), (self.LibreLoRa_correlationSyncDemod_0, 'setSF'))
        self.msg_connect((self.LibreLoRa_receiverController_0, 'setCRout'), (self.LibreLoRa_decode_0, 'setCR'))
        self.msg_connect((self.LibreLoRa_receiverController_0, 'setCRout'), (self.LibreLoRa_deinterleave_0, 'setCR'))
        self.msg_connect((self.LibreLoRa_receiverController_0, 'setSFout'), (self.LibreLoRa_deinterleave_0, 'setSF'))
        self.msg_connect((self.LibreLoRa_receiverController_0, 'setSFout'), (self.LibreLoRa_grayEncode_0, 'setSF'))
        self.msg_connect((self.LibreLoRa_receiverController_0, 'lfsrStateOut'), (self.LibreLoRa_randomize_0, 'setLfsrState'))
        self.connect((self.LibreLoRa_CRC16_0, 0), (self.blocks_null_sink_0, 0))
        self.connect((self.LibreLoRa_Correlation_0, 0), (self.LibreLoRa_correlationSyncDemod_0, 1))
        self.connect((self.LibreLoRa_Correlation_0, 1), (self.LibreLoRa_correlationSyncDemod_0, 0))
        self.connect((self.LibreLoRa_Correlation_0, 0), (self.qtgui_time_sink_x_0_0_0, 0))
        self.connect((self.LibreLoRa_Correlation_0, 1), (self.qtgui_time_sink_x_0_0_0, 1))
        self.connect((self.LibreLoRa_NibblesToBytes_0, 0), (self.LibreLoRa_randomize_0, 0))
        self.connect((self.LibreLoRa_correlationSyncDemod_0, 0), (self.LibreLoRa_grayEncode_0, 0))
        self.connect((self.LibreLoRa_decode_0, 0), (self.LibreLoRa_receiverController_0, 0))
        self.connect((self.LibreLoRa_deinterleave_0, 0), (self.LibreLoRa_decode_0, 0))
        self.connect((self.LibreLoRa_frequencyTrackerN_0, 0), (self.LibreLoRa_Correlation_0, 0))
        self.connect((self.LibreLoRa_grayEncode_0, 0), (self.LibreLoRa_deinterleave_0, 0))
        self.connect((self.LibreLoRa_randomize_0, 0), (self.LibreLoRa_CRC16_0, 0))
        self.connect((self.LibreLoRa_randomize_0, 0), (self.blocks_tcp_server_sink_0, 0))
        self.connect((self.LibreLoRa_receiverController_0, 0), (self.LibreLoRa_NibblesToBytes_0, 0))
        self.connect((self.blocks_file_source_0, 0), (self.blocks_throttle_0, 0))
        self.connect((self.blocks_throttle_0, 0), (self.LibreLoRa_frequencyTrackerN_0, 0))
        self.connect((self.blocks_throttle_0, 0), (self.qtgui_waterfall_sink_x_0_0, 0))

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "premabletestRTL")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()

    def get_BW(self):
        return self.BW

    def set_BW(self, BW):
        self.BW = BW
        self.set_OSF(round(self.samp_rate/(self.BW*self.decimation)))
        self.set_samp_rate(2*self.BW)

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.set_OSF(round(self.samp_rate/(self.BW*self.decimation)))
        self.blocks_throttle_0.set_sample_rate(self.samp_rate/self.decimation)
        self.qtgui_time_sink_x_0_0_0.set_samp_rate(self.samp_rate)
        self.qtgui_waterfall_sink_x_0_0.set_frequency_range(0, self.samp_rate)

    def get_decimation(self):
        return self.decimation

    def set_decimation(self, decimation):
        self.decimation = decimation
        self.set_OSF(round(self.samp_rate/(self.BW*self.decimation)))
        self.blocks_throttle_0.set_sample_rate(self.samp_rate/self.decimation)

    def get_SF(self):
        return self.SF

    def set_SF(self, SF):
        self.SF = SF
        self.set_chirpWindow(numpy.real(LibreLoRa.getChirpWindow(self.DFTSize, self.windowSize, self.SF, self.symbolSize)))
        self.set_preambleNormalized(LibreLoRa.getPreambleFrequency(self.SF, self.symbolSize, self.nUpchirps, self.syncwordNumber))
        self.set_symbolSize((2**self.SF)*self.OSF)
        self.set_upchirp(numpy.asarray(LibreLoRa.getSymbol(0, self.SF, self.OSF)))
        self.LibreLoRa_grayEncode_0.setSF(self.SF)

    def get_OSF(self):
        return self.OSF

    def set_OSF(self, OSF):
        self.OSF = OSF
        self.set_symbolSize((2**self.SF)*self.OSF)
        self.set_upchirp(numpy.asarray(LibreLoRa.getSymbol(0, self.SF, self.OSF)))

    def get_symbolSize(self):
        return self.symbolSize

    def set_symbolSize(self, symbolSize):
        self.symbolSize = symbolSize
        self.set_DFTSize(2*self.symbolSize)
        self.set_chirpWindow(numpy.real(LibreLoRa.getChirpWindow(self.DFTSize, self.windowSize, self.SF, self.symbolSize)))
        self.set_preambleNormalized(LibreLoRa.getPreambleFrequency(self.SF, self.symbolSize, self.nUpchirps, self.syncwordNumber))
        self.set_windowSize(int(self.symbolSize/4))

    def get_windowSize(self):
        return self.windowSize

    def set_windowSize(self, windowSize):
        self.windowSize = windowSize
        self.set_chirpWindow(numpy.real(LibreLoRa.getChirpWindow(self.DFTSize, self.windowSize, self.SF, self.symbolSize)))

    def get_syncwordNumber(self):
        return self.syncwordNumber

    def set_syncwordNumber(self, syncwordNumber):
        self.syncwordNumber = syncwordNumber
        self.set_preambleNormalized(LibreLoRa.getPreambleFrequency(self.SF, self.symbolSize, self.nUpchirps, self.syncwordNumber))

    def get_nUpchirps(self):
        return self.nUpchirps

    def set_nUpchirps(self, nUpchirps):
        self.nUpchirps = nUpchirps
        self.set_preambleNormalized(LibreLoRa.getPreambleFrequency(self.SF, self.symbolSize, self.nUpchirps, self.syncwordNumber))

    def get_DFTSize(self):
        return self.DFTSize

    def set_DFTSize(self, DFTSize):
        self.DFTSize = DFTSize
        self.set_chirpWindow(numpy.real(LibreLoRa.getChirpWindow(self.DFTSize, self.windowSize, self.SF, self.symbolSize)))

    def get_upchirp(self):
        return self.upchirp

    def set_upchirp(self, upchirp):
        self.upchirp = upchirp

    def get_preambleNormalized(self):
        return self.preambleNormalized

    def set_preambleNormalized(self, preambleNormalized):
        self.preambleNormalized = preambleNormalized

    def get_chirpWindow(self):
        return self.chirpWindow

    def set_chirpWindow(self, chirpWindow):
        self.chirpWindow = chirpWindow



def main(top_block_cls=premabletestRTL, options=None):

    if StrictVersion("4.5.0") <= StrictVersion(Qt.qVersion()) < StrictVersion("5.0.0"):
        style = gr.prefs().get_string('qtgui', 'style', 'raster')
        Qt.QApplication.setGraphicsSystem(style)
    qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls()
    tb.start()
    tb.show()

    def sig_handler(sig=None, frame=None):
        Qt.QApplication.quit()

    signal.signal(signal.SIGINT, sig_handler)
    signal.signal(signal.SIGTERM, sig_handler)

    timer = Qt.QTimer()
    timer.start(500)
    timer.timeout.connect(lambda: None)

    def quitting():
        tb.stop()
        tb.wait()
    qapp.aboutToQuit.connect(quitting)
    qapp.exec_()


if __name__ == '__main__':
    main()
