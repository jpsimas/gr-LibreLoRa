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
#include "getSymbol.cc"
#include <numeric>
#include <iostream>

namespace gr {
  namespace LibreLoRa {

    preambleDetector::sptr
    preambleDetector::make(size_t SF, size_t OSF, size_t nUpchirps, uint8_t syncwordNumber, float minCorr, float corrStop) {
      return gnuradio::make_block_sptr<preambleDetector_impl>(
        SF, OSF, nUpchirps, syncwordNumber, minCorr, corrStop);
    }


    /*
     * The private constructor
     */
    preambleDetector_impl::preambleDetector_impl(size_t SF, size_t OSF, size_t nUpchirps, uint8_t syncwordNumber, float minCorr, float corrStop)
      : gr::block("preambleDetector",
		  gr::io_signature::make(1, 1, sizeof(float)),
		  gr::io_signature::make(1, 1, sizeof(float))),
	symbolSize((1 << SF)*OSF),
	minCorr(minCorr),
	corrStop(corrStop),
	delay(0),
	delayEst(0),
	found(false),
	syncd(false),
	corrMax(0) {
      
      //generate preamble

      //upchirps
      for(size_t i = 0; i < nUpchirps; i++)
	preamble.push_back(getSymbol(0, SF, OSF));

      //calculate sync word symbols from sync word number
      preamble.push_back(getSymbol((1 << (SF - 5))*(syncwordNumber >> 4), SF, OSF));

      preamble.push_back(getSymbol((1 << (SF - 5))*(syncwordNumber & 0xf), SF, OSF));

      auto downchirp = getSymbol(0, SF, OSF);
      for(auto& x : downchirp)
	x = -x;

      preamble.push_back(downchirp);
      preamble.push_back(downchirp);

      std::vector<float> fractionOfDownchirp;
      fractionOfDownchirp.insert(fractionOfDownchirp.begin(), downchirp.begin(), downchirp.begin() + (((1 << (SF - 2)) - 1)*OSF - 1));
      preamble.push_back(fractionOfDownchirp);
      
      //normalize and make preamble zero-mean
      preambleSize = 0;
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

      corrVect.reserve(preamble.size());
      sumSqVect.reserve(preamble.size());
      sumVect.reserve(preamble.size());
      for(size_t i = 0; i < preamble.size(); i++) {
	corrVect.push_back(0);
	sumSqVect.push_back(0);	
	sumVect.push_back(0);
      }
    }

    /*
     * Our virtual destructor.
     */
    preambleDetector_impl::~preambleDetector_impl() {
    }

    void
    preambleDetector_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required) {
      ninput_items_required[0] = symbolSize + noutput_items;
    }

    int
    preambleDetector_impl::general_work (int noutput_items,
					 gr_vector_int &ninput_items,
					 gr_vector_const_void_star &input_items,
					 gr_vector_void_star &output_items) {
      const float *in = (const float *) input_items[0];
      float *out = (float *) output_items[0];
      
      //      std::vector<float> samples(in, in + symbolSize);//CONSTRUCTOR NOT WORKING!!
	
      //      auto mean = std::accumulate(samples.begin(), samples.end(), 0)/samples.size();

      //      std::cout << "mean = " << mean << std::endl;


      for(size_t k = 0; k < noutput_items; k++) {
	const float* samples = in + k;
	 for(size_t i = 1; i < preamble.size(); i++) {
	   corrVect[i] = corrVect[i - 1];
	   sumSqVect[i] = sumSqVect[i - 1];
	   sumVect[i] = sumVect[i - 1];
	 }

	corrVect[0] = 0;
	sumSqVect[0] = 0;
	sumVect[0] = 0;
      
	for(size_t i = 0; i < preamble.size(); i++) {
	  for(size_t j = 0; j < preamble[i].size(); j++) {
	    corrVect[i] += preamble[i][j]*samples[j];
	    sumSqVect[i] += samples[j]*samples[j];
	    sumVect[i] += samples[j];
	  }
	}

	float corr = corrVect.back()/sqrt(sumSqVect.back() - sumVect.back()*sumVect.back()/preambleSize);

	//std::cout << "corr = " << corr << std::endl;
      
	if(!found) {
	  syncd = false;
	  if(corr >= minCorr)
	    found = true;
	} else if(corr < corrStop) {
	  delay = delayEst;
	  corrMax = 0;
	  delayEst = 0;
	  found = false;
	  syncd = true; 
	  std::cout << "Preamble Detected" << std::endl;
	  std::cout << "corr = " << corr << std::endl;
	}
      
	if(corr >= corrMax) {
	  corrMax = corr;
	  //std::cout << "corr = " << corr << std::endl;
	  delayEst = 0;
	} else
	  delayEst++;
      }
      
      for(size_t i = 0; i < noutput_items; i++)
	out[i] = in[symbolSize - delay + i];

      
      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (noutput_items);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace LibreLoRa */
} /* namespace gr */

