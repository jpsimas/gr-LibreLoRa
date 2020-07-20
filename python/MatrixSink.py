#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2020 Joao Pedro de O Simas.
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#


import numpy
from gnuradio import gr
from matplotlib.figure import Figure


from PyQt5 import QtWidgets
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas

import matplotlib.animation as animation

class MatrixSink(gr.basic_block, FigureCanvas):
    """
    docstring for block MatrixSink
    """
    def __init__(self, nRows, nCols, minVal, maxVal, Parent):
        self.nRows = nRows
        self.nCols = nCols

        in_type = (numpy.float32,self.nCols)
        in_type_list = []
        for k in range(self.nRows):
            in_type_list.append(in_type)
        
        gr.basic_block.__init__(self,
            name="MatrixSink",
            in_sig=in_type_list,
            out_sig=None)

        self.fig = Figure(figsize=(4, 4), dpi=90)
    
        self.axes = self.fig.add_subplot(111)
        
        FigureCanvas.__init__(self, self.fig)
        self.setParent(Parent)
        
        FigureCanvas.setSizePolicy(self,
                                   QtWidgets.QSizePolicy.Expanding,
                                   QtWidgets.QSizePolicy.Expanding)
        
        FigureCanvas.updateGeometry(self)
           
        self.data = numpy.zeros((self.nRows, self.nCols));
        
        self.img = self.axes.imshow(self.data, vmin = minVal, vmax = maxVal, cmap='viridis', interpolation='none', aspect='auto')
        
        self.draw()
        
        self.ax_bgd = self.copy_from_bbox(self.axes.bbox)

        self.show()
        
    def general_work(self, input_items, output_items):
        self.data = input_items[:][0] #+ self.data
        
        self.img.set_data(self.data)
        self.img.set_clim(0, numpy.max(self.data))
        self.restore_region(self.ax_bgd)
        self.axes.draw_artist(self.img)
        self.blit(self.axes.bbox)

        self.flush_events()

        self.consume_each(1)
        return 0

    def forecast(self, noutput_items, ninput_items_required):
        for i in range(len(ninput_items_required)):
            ninput_items_required[i] = 1

