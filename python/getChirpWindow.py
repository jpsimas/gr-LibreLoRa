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

def getChirpWindow(DFTSize, windowSize, SF, symbolSize):
    OSF = (symbolSize/(2**SF));

    ind = (numpy.arange(0, DFTSize) - (DFTSize - 1)/2)
    
    ##alpha = numpy.log(10)/windowSize;
    #window = numpy.exp((-1j*numpy.pi/(OSF*symbolSize))*(ind**2))*numpy.exp(-alpha*numpy.abs(ind));
    ##window = numpy.exp((-1j*numpy.pi/(OSF*symbolSize))*(numpy.arange(0, DFTSize)**2) -alpha*numpy.arange(0, DFTSize));

    #alpha = numpy.log(10)/((windowSize/2)**2);
    #window = numpy.exp((-1j*numpy.pi/(OSF*symbolSize) - alpha)*(ind**2));

    window = numpy.exp((-1j*numpy.pi/(OSF*symbolSize))*(ind**2));
    window[0:int((DFTSize - windowSize)/2)] = 0;
    window[(-int((DFTSize - windowSize)/2) - 1):(-1)] = 0;
    return window;
