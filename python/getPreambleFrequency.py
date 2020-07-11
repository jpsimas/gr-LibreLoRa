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
import LibreLoRa

def getPreambleFrequency(SF, symbolSize, nUpchirps, syncWordNumber):
    ##OSF = int(symbolSize/(2**SF));
    ##upchirp = numpy.asarray(LibreLoRa.getSymbol(0, SF, OSF));
    upchirp = numpy.asarray(LibreLoRa.getSymbol(0, SF, symbolSize));

    #append upchirps
    preamble = numpy.array([]);
    for i in range(0, nUpchirps - 1):
        preamble = numpy.concatenate((preamble, upchirp));

    #append syncword
    # preamble = numpy.concatenate((preamble, numpy.asarray(LibreLoRa.getSymbol(((syncWordNumber >> 4) << (SF - 4)), SF, OSF))));
    # preamble = numpy.concatenate((preamble, numpy.asarray(LibreLoRa.getSymbol(((syncWordNumber & 0xf) << (SF - 4)), SF, OSF))));
    
    preamble = numpy.concatenate((preamble, numpy.asarray(LibreLoRa.getSymbol(((syncWordNumber >> 4) << 3), SF, symbolSize))));
    preamble = numpy.concatenate((preamble, numpy.asarray(LibreLoRa.getSymbol(((syncWordNumber & 0xf) << 3), SF, symbolSize))));

    #append downchirps
    for i in range(0, 2):
        preamble = numpy.concatenate((preamble, -upchirp));        
    #preamble = numpy.concatenate((preamble, -upchirp[0:(int(upchirp.size/4) - OSF)]));
    
    OSF = int(symbolSize/(2**SF))
    preamble = numpy.concatenate((preamble, -upchirp[0:(int(upchirp.size/4) - OSF)]));
    
    return preamble;


