/* -*- c++ -*- */
/*
 * Copyright 2020 Joao Pedro de O. Simas.
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

#include <gnuradio/io_signature.h>
#include "preambleDetector_impl.h"

#include <gnuradio/blocks/vector_source.h>
#include "getSymbol.cc"
#include <LibreLoRa/symbolCorrelator.h>
#include <vector>

namespace gr {
  namespace LibreLoRa {

    preambleDetector::sptr
    preambleDetector::make(size_t SF, size_t OSF, size_t nUpchirps, uint8_t syncWordNumber, float minCorr, float corrStop)
    {
      return gnuradio::get_initial_sptr
        (new preambleDetector_impl(SF, OSF, nUpchirps, syncWordNumber, minCorr, corrStop));
    }


    /*
     * The private constructor
     */
    preambleDetector_impl::preambleDetector_impl(size_t SF, size_t OSF, size_t nUpchirps, uint8_t syncWordNumber, float minCorr, float corrStop)
      : gr::hier_block2("preambleDetector",
			gr::io_signature::make(1, 1, sizeof(float)),
			gr::io_signature::make(2, 2, sizeof(float)))
    {
      std::vector<std::vector<float>> preamble;
      
      //upchirps
      for(size_t i = 0; i < nUpchirps; i++)
      	preamble.push_back(getSymbol(0, SF, OSF));

      //calculate sync word symbols from sync word number
      preamble.push_back(getSymbol((1 << (SF - 5))*(syncWordNumber >> 4), SF, OSF));

      preamble.push_back(getSymbol((1 << (SF - 5))*(syncWordNumber & 0xf), SF, OSF));

      auto downchirp = getSymbol(0, SF, OSF);
      for(auto& x : downchirp)
      	x = -x;

      preamble.push_back(downchirp);
      preamble.push_back(downchirp);

      std::vector<float> fractionOfDownchirp;
      fractionOfDownchirp.insert(fractionOfDownchirp.begin(), downchirp.begin(), downchirp.begin() + (((1 << (SF - 2)) - 1)*OSF - 1));
      preamble.push_back(fractionOfDownchirp);
      
      //normalize and make preamble zero-mean
      float preambleSize = 0;
      float mean = 0;
  
      //normalize and make preamble zero-mean
      for(auto& sym : preamble) {
      	for(auto& x : sym)
      	  mean += x;
      	preambleSize += sym.size();
      }

      mean /= preambleSize;
      std::cout << "mean " << mean << std::endl;
      std::cout << "preableSize " << preambleSize << std::endl;

      for(auto& sym : preamble)
      	for(auto& x : sym)
      	  x -= mean;
      
      float norm = 0;
      for(auto& sym : preamble)
      	for(auto& x : sym)
      	  norm += x*x;

      norm = sqrt(norm);

      std::cout << "preamble norm = " << norm << std::endl;
      
      for(auto& sym : preamble)
      	for(auto& x : sym)
      	  x /= norm;


      std::vector<boost::shared_ptr<symbolCorrelator>> blocks;
      for(auto& sym : preamble)
      	blocks.push_back(symbolCorrelator::make(sym));

      auto zeroblock = gr::blocks::vector_source<float>::make(std::vector<float>({0}), true);
      
      connect(self(), 0, blocks[0], 0);
      connect(zeroblock, 0, blocks[0], 1);
      
      for(size_t i = 1; i < blocks.size(); i++) {
      	connect(blocks[i - 1], 0, blocks[i], 0);
	connect(blocks[i - 1], 1, blocks[i], 1);
      }
      
      connect(blocks.back(), 0, self(), 0);
      connect(blocks.back(), 1, self(), 1);
    }

    /*
     * Our virtual destructor.
     */
    preambleDetector_impl::~preambleDetector_impl()
    {
    }


  } /* namespace LibreLoRa */
} /* namespace gr */

