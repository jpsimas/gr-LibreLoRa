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
#include "correlationSync_impl.h"

#include <gnuradio/block_detail.h>
#include <gnuradio/runtime_types.h>

#ifndef NDEBUG
#include <iostream>
#endif

#include <volk/volk.h>

#include <LibreLoRa/getSymbol.h>

namespace gr {
  namespace LibreLoRa {

    template<typename T>
    typename correlationSync<T>::sptr
    correlationSync<T>::make(float corrMin, float corrStop, size_t symbolSize, size_t preambleSize, size_t SF, uint16_t syncWordNumber)
    {
      return gnuradio::get_initial_sptr
        (new correlationSync_impl<T>(corrMin, corrStop, symbolSize, preambleSize, SF, syncWordNumber));
    }


    /*
     * The private constructor
     */
    template<typename T>
    correlationSync_impl<T>::correlationSync_impl(float corrMin, float corrStop, size_t symbolSize, size_t preambleSize, size_t SF, uint16_t syncWordNumber)
      : gr::block("correlationSync",
		  gr::io_signature::make(2, 2, sizeof(T)),
		  // gr::io_signature::make2(2, 2, symbolSize*sizeof(float), sizeof(bool))
		  gr::io_signature::make(1, 1, symbolSize*sizeof(T))
		  ),
	corrMin(corrMin),
	corrStop(corrStop),
	symbolSize(symbolSize),
	preambleSize(preambleSize),
	SF(SF),
	syncWordNum1((syncWordNumber >> 4) << 3),
	syncWordNum2((syncWordNumber & 0xf) << 3),
	syncd(false),
	fixedMode(true),
	nOutputItemsToProduce(0),
	deSyncAfterDone(false),
	preambleConsumed(false),
	preambleSamplesToConsume(preambleSize),
	detectionCount((1 << SF)) {

      this->set_min_output_buffer(8);
      // this->set_fixed_rate(true);
      
      syncPort = pmt::string_to_symbol("sync");
      this->message_port_register_out(syncPort);

      samplesToProducePort = pmt::string_to_symbol("samplesToProduce");
      this->message_port_register_out(samplesToProducePort);
      
      this->message_port_register_in(pmt::mp("setNOutputItemsToProduce"));
      this->set_msg_handler(pmt::mp("setNOutputItemsToProduce"),
		      [this](pmt::pmt_t msg) {
#ifndef NDEBUG
			std::cout << "correlationSync: setting n to " << int(pmt::to_long(msg)) << std::endl;
#endif

			auto nItems = pmt::to_long(msg);
			
			this->message_port_pub(samplesToProducePort, pmt::from_long(this->symbolSize*nItems));
			
			setNOutputItemsToProduce(int(nItems));
		      });
      this->message_port_register_in(pmt::mp("reset"));
      this->set_msg_handler(pmt::mp("reset"),
		      [this](pmt::pmt_t msg) {
#ifndef NDEBUG
			std::cout << "correlationSync: reset" << std::endl;
#endif
			reset();
		      });

      //generate expected sync word
      syncWordExpected = getSymbol<T>(syncWordNum1, SF, symbolSize);
      auto syncSym2 = getSymbol<T>(syncWordNum2, SF, symbolSize);
      syncWordExpected.insert(syncWordExpected.end(), syncSym2.begin(), syncSym2.end());
#ifndef NDEBUG
      std::cout << "correlationSync: syncWord size: " << syncWordExpected.size() << std::endl;
#endif
    }

    /*
     * Our virtual destructor.
     */
    template<typename T>
    correlationSync_impl<T>::~correlationSync_impl()
    {
    }

    template<typename T>
    void
    correlationSync_impl<T>::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      const size_t n = ((fixedModeEnabled() && (nOutputItemsToProduce < noutput_items) && (nOutputItemsToProduce != 0))? nOutputItemsToProduce : noutput_items);
      // ninput_items_required[0] = (syncd? (preambleConsumed? (symbolSize*n) : std::min((size_t(1 << 14) - 1), preambleSamplesToConsume)) : 2*symbolSize);
      ninput_items_required[0] = (syncd? (preambleConsumed? (symbolSize*n) : preambleSize) : 2*symbolSize);
      // ninput_items_required[0] = fixed_rate_noutput_to_ninput(noutput_items);
      
      ninput_items_required[1] = ninput_items_required[0];
    }

    namespace{
      template<typename T>
	T conj(T x) {
	return x;
      }
      
      template<>
	gr_complex conj(gr_complex x) {
	return std::conj(x);
      }

      template<typename T>
	T correctOffset(T x, T offset) {
	return x - offset;
	// return x;
      }

      template<>
	gr_complex correctOffset(gr_complex x, gr_complex offset) {
	return x*offset;
      }

      constexpr size_t log2(size_t num, size_t count = 0) {
	return (num == 1)? count : log2(num >> 1, count + 1);
      }
    }

    template<typename T>
    void
    correlationSync_impl<T>::estimateOffset(const T *preamble) {
      // const T* downchirps = preamble + (preambleSize - (2*symbolSize + preambleSize%symbolSize));
      // auto twoDownchirps = getSymbol<T>(0, SF, symbolSize);
      // for(auto& x : twoDownchirps)
      // 	x = -x;
      
      // twoDownchirps.insert(twoDownchirps.end(), twoDownchirps.begin(), twoDownchirps.end());

      // static std::vector<size_t> detectionCount((1 << SF));
      // for(auto& x : detectionCount)
      // 	x = 0;
      
      // for(size_t j = 0; j < 2*symbolSize; j++){
      // 	 auto decision = int16_t(std::round(symbolSize*(downchirps[j] - twoDownchirps[j] + 2.0f)))%int16_t((1 << SF));
      // 	detectionCount[decision]++;
      // }
      const T* syncWord = preamble;
      
      for(auto& x : detectionCount)
	x = 0;
      
      for(size_t j = 0; j < 2*symbolSize; j++){
	int16_t decision = (int32_t(std::round(float(symbolSize)*(syncWord[j] - syncWordExpected[j])))%int32_t((1 << SF)) + int32_t((1 << SF)))%int32_t((1 << SF));
	detectionCount.at(decision)++;
      }
      
      int16_t maxK = 0;
      size_t maxCount = 0;
      for(auto k = 0; k < detectionCount.size(); k++)
	if(detectionCount[k] > maxCount){
	  maxK = k;
	  maxCount = detectionCount[k];
	}

      //correction to compensate for non-integer preamble size
      if(symbolSize < (1 << SF))
	maxK++;
      
      maxK = int16_t(maxK + (1 << (SF - 1)))%int16_t((1 << SF)) - int16_t((1 << (SF - 1)));
      
      offset = float(maxK)/(symbolSize);
      
#ifndef NDEBUG
	std::cout << "correlationSync: estimated offset: " << std::dec << offset << " (" << maxK << ")" << std::endl;
      // std::cout << "correlationSync: counts: ";
      // for(auto x : detectionCount)
      // 	std::cout << x << ", ";
      // std::cout << std::endl;
#endif	
    }

    template<>
    void
    correlationSync_impl<gr_complex>::estimateOffset(const gr_complex *preamble){
      //size_t minPreSize = 4*symbolSize + symbolSize/4 - (symbolSize >> (SF + 2));
	
      // auto downchirp = getSymbol<gr_complex>(0, log2(symbolSize), 1);
      auto downchirp = getSymbol<gr_complex>(0, log2(symbolSize), symbolSize);
      for(auto& x : downchirp)
	x = std::conj(x);

      gr_complex corr;
      volk_32fc_x2_conjugate_dot_prod_32fc(&corr, downchirp.data(), preamble + preambleSize - (3*symbolSize/2 + preambleSize%symbolSize), symbolSize);
      
      offset = -corr/(std::abs(corr) + 1e-6f);
      
#ifndef NDEBUG
	  std::cout << "correlationSync: estimated offset: " << offset << std::endl;
#endif	
    }


    template<typename T>
    int
    correlationSync_impl<T>::general_work (int noutput_items,
    					gr_vector_int &ninput_items,
    					gr_vector_const_void_star &input_items,
    					gr_vector_void_star &output_items)
    {
      const T *data_in = (const T *) input_items[0];
      const T *corr = (const T *) input_items[1];
      T* data_out = (T*) output_items[0];
      // bool* syncd_out = (bool*) output_items[1];

#ifndef NDEBUG
      // std::cout << "correlationSync: work called: noutput_items = " << noutput_items << std::endl;
#endif
      
      // *syncd_out = false;
      // Do <+signal processing+>

      if(!syncd) {
    	bool foundFirstPt = false;
	float corrMaxNorm = 0;
	corrMax = 0;
    	size_t maxPos = 0;

    	for(size_t i = 0; i < 2*symbolSize; i++) {
    	  if(foundFirstPt) {
    	    if(norm(corr[i]) <= corrStop) {
	      corrMax = conj<T>(corrMax)/std::abs(corrMax);
    	      foundFirstPt = false;
	      this->consume_each(maxPos);
    	      syncd = true;
	      preambleConsumed = false;
	      preambleSamplesToConsume = preambleSize;
#ifndef NDEBUG
    	      std::cout << "correlationSync: sync'd" << std::endl;
#endif
    	      // *syncd_out = true;
    	      // produce(1, 1);
	      // return WORK_CALLED_PRODUCE;
	      
	      this->message_port_pub(syncPort, pmt::PMT_NIL);
#ifndef NDEBUG
    	      std::cout << "correlationSync: produced syncd signal" << std::endl;
#endif
	      return 0;
    	    } else if(norm(corr[i]) > corrMaxNorm) {
	      if(i < symbolSize) {
    	      // if(i < ninput_items[0] - symbolSize) {
    		corrMaxNorm = norm(corr[i]);
    		maxPos = i;
    	      } else
    		break;
	    }
	  } else if(norm(corr[i]) >= corrMin) {
	    if(i < symbolSize) {
	    // if(i < ninput_items[0] - symbolSize) {
	      foundFirstPt = true;
	      corrMax = corr[i];
	      corrMaxNorm = norm(corr[i]);
	    } else
	      break;
	  }
	}//for

    	this->consume_each(symbolSize);
	// this->consume_each(ninput_items[0] - symbolSize);
	
    	return 0;
      } else {
	if(!preambleConsumed) {
	  estimateOffset(data_in);  
	  this->consume_each(preambleSize);
// 	  size_t nSamples = std::min((size_t(1 << 14) - 1), preambleSamplesToConsume);

// #ifndef NDEBUG
// 	  std::cout << "correlationSync: consuming " << nSamples << " preamble samples out of " << preambleSamplesToConsume<< std::endl;
// #endif
	  
// 	  this->consume_each(nSamples);
// 	  preambleSamplesToConsume -= nSamples;
// 	  if(preambleSamplesToConsume == 0)
	  preambleConsumed = true;
	  return 0;
	}
	
	size_t n = ((fixedModeEnabled() && (nOutputItemsToProduce < noutput_items))? nOutputItemsToProduce : noutput_items);
	
	for(size_t j = 0; j < n; j++)
	  for(size_t i = 0; i < symbolSize; i++){
	    ///data_out[i + symbolSize*j] = correctOffset<T>(data_in[i + symbolSize*j], corrMax);
	    data_out[i + symbolSize*j] = correctOffset<T>(data_in[i + symbolSize*j], offset);
	    //data_out[i + symbolSize*j] = corrMax*data_in[i + symbolSize*j];
	    // data_out[i + symbolSize*j] = data_in[i + symbolSize*j];
	  }

	this->consume_each (symbolSize*n);
	
	if(fixedModeEnabled())
	  nOutputItemsToProduce -= n;
#ifndef NDEBUG
	if(n != 0)
	  std::cout << "correlationSync: produced " << n << " synced symbols" << std::endl;
#endif

	if(nOutputItemsToProduce == 0 && deSyncAfterDone) {
	  deSyncAfterDone = false;
	  syncd = false;
	}

	return n;
      }
    }
    
    template<typename T>    
    void correlationSync_impl<T>::reset() {
      syncd = false;
    }

    template<>
    constexpr float
    correlationSync_impl<float>::norm(float x) {
      return x;
    }
      
    template<>
    float
    correlationSync_impl<gr_complex>::norm(gr_complex x) {
      return std::norm(x);
    }
      
    template class correlationSync<float>;
    template class correlationSync<gr_complex>;
  } /* namespace LibreLoRa */
} /* namespace gr */

