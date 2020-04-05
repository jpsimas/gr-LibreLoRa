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

#include <gnuradio/filter/fir_filter_blk.h>
#include <gnuradio/blocks/add_blk.h>
#include <gnuradio/blocks/delay.h>
#include <gnuradio/blocks/multiply.h>
#include <gnuradio/blocks/multiply_const.h>
#include <gnuradio/blocks/divide.h>

#include <LibreLoRa/normalizeCorrelation.h>

// #include <gnuradio/blocks/vector_source.h>
// #include <LibreLoRa/symbolCorrelator.h>

#include <LibreLoRa/getSymbol.h>
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
			gr::io_signature::make(2, 2, sizeof(float)))//replace by vector of symbols
			// gr::io_signature::make2(2, 2, sizeof(float), ((1 << SF)*OSF)*sizeof(float)))
	
    {

      std::cout << "BENIS3" << std::endl;
      std::vector<std::vector<float>> preamble;
      
      auto upchirp = getSymbol(0, SF, OSF);
      for(size_t i = 0; i < nUpchirps; i++)
	preamble.push_back(upchirp);

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
      size_t preambleSize = 0;
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


      //invert order of all symbols because of the way fir_filter block works
      // for(auto& sym : preamble) {
      // 	std::vector<float> symTmp = sym;
      //  	for(size_t j = 0; j < sym.size(); j++)
      // 	  sym.at(j) = symTmp.at(symTmp.size() - 1 - j);
      // }

      // std::vector<boost::shared_ptr<symbolCorrelator>> blocks;
      // for(auto& sym : preamble)
      //  	blocks.push_back(symbolCorrelator::make(sym));

      // auto oneblock = gr::blocks::vector_source<float>::make(std::vector<float>({1}), true);
      
      // connect(self(), 0, blocks[0], 0);
      // connect(oneblock, 0, blocks[0], 1);
      
      // for(size_t i = 1; i < blocks.size(); i++) {
      // 	connect(blocks[i - 1], 0, blocks[i], 0);
      // 	connect(blocks[i - 1], 1, blocks[i], 1);
      // }
      
      // connect(blocks.back(), 0, self(), 0);
      // connect(blocks.back(), 1, self(), 1);
      
      std::vector<gr::filter::fir_filter_fff::sptr> blocksCorr;
      std::vector<gr::blocks::delay::sptr> blocksDelay;
      std::vector<gr::blocks::delay::sptr> blocksDelaySq;
      std::vector<gr::filter::fir_filter_fff::sptr> blocksSum;
      std::vector<gr::filter::fir_filter_fff::sptr> blocksSumSq;
      
      for(auto& sym : preamble) {
      	blocksCorr.push_back(gr::filter::fir_filter_fff::make(1, sym));
      	blocksDelay.push_back(gr::blocks::delay::make(sizeof(float), sym.size()));
	
	std::vector<float> onesVect(sym.size(), 1.0);
      	blocksSum.push_back(gr::filter::fir_filter_fff::make(1, onesVect));
      	blocksSumSq.push_back(gr::filter::fir_filter_fff::make(1, onesVect));
      }

      for(size_t i = 0; i + 1 < preamble.size(); i++)
	blocksDelaySq.push_back(gr::blocks::delay::make(sizeof(float), preamble[i].size()));
      	
      auto adder = gr::blocks::add_ff::make();
      auto adderSum = gr::blocks::add_ff::make();
      auto adderSumSq = gr::blocks::add_ff::make();
      auto multiplier = gr::blocks::multiply_ff::make();
      
      connect(self(), 0, blocksCorr.at(0), 0);
      connect(self(), 0, blocksDelay.at(0), 0);

      connect(self(), 0, blocksSum.at(0), 0);
      
      connect(self(), 0, multiplier, 0);
      connect(self(), 0, multiplier, 1);
      connect(multiplier, 0, blocksSumSq.at(0), 0);
      if(blocksDelaySq.size() > 0)
	connect(multiplier, 0, blocksDelaySq.at(0), 0);
      
      for(size_t i = 0; i < blocksCorr.size(); i++){
       	connect(blocksCorr.at(i), 0, adder, i);
	connect(blocksSum.at(i), 0, adderSum, i);
	connect(blocksSumSq.at(i), 0, adderSumSq, i);
      }
      
      for(size_t i = 0; i + 1 < blocksDelay.size(); i++)
      	connect(blocksDelay.at(i), 0, blocksDelay.at(i + 1), 0);
      
      for(size_t i = 0; i + 1 < blocksDelaySq.size(); i++)
	connect(blocksDelaySq.at(i), 0, blocksDelaySq.at(i + 1), 0);
      
      for(size_t i = 0; i + 1 < blocksDelay.size(); i++) {	
      	connect(blocksDelay.at(i), 0, blocksCorr.at(i + 1), 0);
	connect(blocksDelay.at(i), 0, blocksSum.at(i + 1), 0);
	connect(blocksDelaySq.at(i), 0, blocksSumSq.at(i + 1), 0);
      }
      
      connect(blocksDelay.back(), 0, self(), 0);
      
      auto normalizer = normalizeCorrelation::make(preambleSize);
      connect(adder, 0, normalizer, 0);
      connect(adderSum, 0, normalizer, 1);
      connect(adderSumSq, 0, normalizer, 2);
      connect(normalizer, 0, self(), 1);
      
      // auto synchronizer = correlationSync::make(minCorr, corrStop, preambleSize());
      // connect(blocksDelay.back(), 0, syncronizer, 0);
      // connect(normalizer, 0, synchronizer, 1);
      // connect(synchronizer, 0, self(), 0);
      // connect(synchronizer, 1, self(), 1);
    }

    /*
     * Our virtual destructor.
     */
    preambleDetector_impl::~preambleDetector_impl()
    {
    }


  } /* namespace LibreLoRa */
} /* namespace gr */

