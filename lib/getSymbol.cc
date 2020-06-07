/* -*- c++ -*- */
/*
 * Copyright 2020 Joao Pedro de O Simas.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <LibreLoRa/getSymbol.h>

#include <complex>
#include <cmath>

namespace gr {
  namespace LibreLoRa {

    template<>
    std::vector<float> getSymbol(size_t symbolNum, size_t SF, size_t OSF) {
      const size_t symbolSize = (1 << SF)*OSF;
      std::vector<float> symbol(symbolSize);
      for(size_t i = 0; i < symbolSize; i++) {
	auto k = (symbolNum*OSF + i)%symbolSize;
	symbol[i] = (k - (symbolSize - 1)/2.0)/(OSF*(symbolSize - 1));
      }
      return symbol;
    }

    template<>
    std::vector<std::complex<float>> getSymbol(size_t symbolNum, size_t SF, size_t OSF) {
      const auto symbolFreq = getSymbol<float>(symbolNum, SF, OSF);
      std::vector<std::complex<float>> symbol(symbolFreq.size());
      for(auto i = 0; i < symbol.size(); i++)
	symbol[i] = std::polar<float>(1.0, 2*M_PI*symbolFreq[i]);

      return symbol;
    }
    
  } /* namespace LibreLoRa */
} /* namespace gr */

