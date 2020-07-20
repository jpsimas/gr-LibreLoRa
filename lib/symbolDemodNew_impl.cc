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
#include "symbolDemodNew_impl.h"

#include <LibreLoRa/getSymbol.h>

#ifndef NDEBUG
#include <iostream>
#endif

namespace gr {
  namespace LibreLoRa {

    symbolDemodNew::sptr
    symbolDemodNew::make(size_t SF, size_t symbolSize, size_t OSF)
    {
      return gnuradio::get_initial_sptr
        (new symbolDemodNew_impl(SF, symbolSize, OSF));
    }


    /*
     * The private constructor
     */
    symbolDemodNew_impl::symbolDemodNew_impl(size_t SF, size_t symbolSize, size_t OSF)
      :	SF(SF),
	OSF(OSF),
	symbolSize(symbolSize),
	count((1 << SF)),
	estimate(symbolSize),
	gr::sync_block("symbolDemodNew",
		  gr::io_signature::make(1, 1, symbolSize*sizeof(float)), 
		  gr::io_signature::make(1, 1, sizeof(uint16_t))) {
      twoUpchirps = getSymbol<float>(0, SF, symbolSize);
      
      twoUpchirps.insert(twoUpchirps.end(), twoUpchirps.begin(), twoUpchirps.end());

#ifndef NDEBUG
      std::cout << "symbolDemodNew: contructed" << std::endl;
#endif

      message_port_register_in(pmt::mp("setSF"));
      set_msg_handler(pmt::mp("setSF"), [this](pmt::pmt_t msg) {setSF(size_t(pmt::to_long(msg)));});
    }

    /*
     * Our virtual destructor.
     */
    symbolDemodNew_impl::~symbolDemodNew_impl()
    {
    }

    int
    symbolDemodNew_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const float *dataIn = (const float *) input_items[0];
      uint16_t *dataOut = (uint16_t *) output_items[0];

      // Do <+signal processing+>
      for(size_t i = 0; i < noutput_items; i++) {

	// size_t minErr = (1 << SF);
	// size_t minSym = 0;
	// for(size_t sym = 0; sym < (1 << SF); sym++) {
	  //gr_complex mean = 0;

	  for(auto& x : count)
	    x = 0;
	  
	  for(size_t j = 0; j < symbolSize; j++){
	    // mean += std::polar<float>(1.0, 2*M_PI*OSF*(dataIn[i*symbolSize + j] - twoUpchirps[j]));;
	     auto decision = int16_t(std::round((1 << SF)*OSF*(dataIn[i*symbolSize + j] - twoUpchirps[j] + 2.0f)))%int16_t(1 << SF);
	    // int16_t decision = 2*(1 << SF) - int16_t(std::round(symbolSize*(std::abs(dataIn[i*symbolSize + j] - twoUpchirps[j + sym*OSF]))))%int16_t(2*(1 << SF));
	    count[decision]++;
	  }
	  

	  // dataOut[i] = uint16_t(round((1 << SF)*std::arg(mean)/(2.0*M_PI)))%uint16_t(1 << SF);
	  size_t maxK = 0;
	  size_t maxCount = 0;
	  for(auto k = 0; k < count.size(); k++)
	    if(count[k] > maxCount){
	      maxK = k;
	      maxCount = count[k];
	    }

	  dataOut[i] = maxK;
// #ifndef NDEBUG
// 	  std::cout << "err: " << maxK << std::endl;
// #endif
	//   if(maxK < minErr){
	//     minSym = sym;
	//     minErr = maxK;
	//   }
	// }

	// dataOut[i] = minSym;
#ifndef NDEBUG
	std::cout << "demodulated symbol: " << std::dec << dataOut[i] << ", SF = " << SF << std::endl;
#endif
      }
      

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace LibreLoRa */
} /* namespace gr */

