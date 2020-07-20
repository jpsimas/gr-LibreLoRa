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
#include "symbolDemod_impl.h"
#include <LibreLoRa/getSymbol.h>
#include <volk/volk.h>

#include <cmath>

#ifndef NDEBUG
#include <iostream>
#endif

namespace gr {
  namespace LibreLoRa {

    template<typename T>
    typename symbolDemod<T>::sptr
    symbolDemod<T>::make(size_t SF, size_t symbolSize, bool implicit, size_t windowSize)
    {
      return gnuradio::get_initial_sptr
        (new symbolDemod_impl<T>(SF, symbolSize, implicit, windowSize));
    }


    /*
     * The private constructor
     */
    template<typename T>
    symbolDemod_impl<T>::symbolDemod_impl(size_t SF, size_t symbolSize, bool implicit, size_t windowSize)
      :	SF(SF),
	symbolSize(symbolSize),
	implicit(implicit),
	windowSize(windowSize),
	startingIndex((symbolSize - windowSize)/2),
	started(true),
	offset(0.0f),
	gr::block("symbolDemod",
		       gr::io_signature::make(1, 1, symbolSize*sizeof(T)), 
		       gr::io_signature::make(1, 1, sizeof(uint16_t))) {
      //set_relative_rate(1.0/symbolSize);
      // upchirps = getSymbol<T>(0, SF, (symbolSize >> SF));
      
      upchirps = getSymbol<T>(0, SF, symbolSize);
      
      upchirps.insert(upchirps.end(), upchirps.begin(), upchirps.end());

#ifndef NDEBUG
      std::cout << "symbolDemod: constructed" << std::endl;
      std::cout << "symbolDemod: window size: " << windowSize << std::endl;
      std::cout << "symbolDemod: starting index: " << startingIndex << std::endl;
#endif

      this->message_port_register_in(pmt::mp("setSF"));
      this->set_msg_handler(pmt::mp("setSF"), [this](pmt::pmt_t msg) {setSF(size_t(pmt::to_long(msg)));});
    }

    /*
     * Our virtual destructor.
     */
    template<typename T>
    symbolDemod_impl<T>::~symbolDemod_impl()
    {
    }

    template<typename T>
    void
    symbolDemod_impl<T>::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
      ninput_items_required[0] = noutput_items;//1;
      // std::cout << "symbolDemod: forecast called: nouput_items_required = " << ninput_items_required[0] << std::endl;
    }

    template<>
    int
    symbolDemod_impl<float>::general_work (int noutput_items,
				    gr_vector_int &ninput_items,
				    gr_vector_const_void_star &input_items,
				    gr_vector_void_star &output_items)
    {
      const float *dataIn = (const float *) input_items[0];
      uint16_t *dataOut = (uint16_t *) output_items[0];

#ifndef NDEBUG
      std::cout << "demodulating " << noutput_items <<  " symbols, SF = " << SF << std::endl;
#endif
      if(started) {
	// Do <+signal processing+>
	for(size_t i = 0; i < noutput_items; i++) {
	  //size_t i = 0;
	  float corrMax = 0;
	  size_t jMax = 0;
	  
	  for(size_t j = 0; j < symbolSize; j++) {
	    float corrJ;
	    volk_32f_x2_dot_prod_32f(&corrJ, dataIn + i*symbolSize, upchirps.data() + j, symbolSize);

	    //volk_32f_x2_dot_prod_32f(&corrJ, dataIn + i*symbolSize, getSymbol<float>(j, SF, symbolSize).data(), symbolSize);
	    
	    if(corrJ >= corrMax) {
	      corrMax = corrJ;
	      jMax = j;
	    }
	  }

	  float err = jMax - (symbolSize >> SF)*std::round(float(jMax)/(symbolSize >> SF));
	  offset += 0.1*err;
	  
	  dataOut[i] = uint16_t(std::round((jMax + std::round(offset))*(1 << SF)/float(symbolSize)))%uint16_t(1 << SF);
	  //dataOut[i] = jMax;
#ifndef NDEBUG
	  std::cout << "demodulated symbol: " << std::dec << dataOut[i] << ", SF = " << SF << std::endl;
#endif
	}
      
	// Tell runtime system how many input items we consumed on
	// each input stream.
	consume_each (noutput_items);

	// Tell runtime system how many output items we produced.
	return noutput_items;
	//consume_each(1);
	//return 1;
      } else {
	//consume_each(1);
	return 0;
      }
    }

    template<>
    int
    symbolDemod_impl<gr_complex>::general_work (int noutput_items,
				    gr_vector_int &ninput_items,
				    gr_vector_const_void_star &input_items,
				    gr_vector_void_star &output_items)
    {
      const gr_complex *dataIn = (const gr_complex *) input_items[0];
      uint16_t *dataOut = (uint16_t *) output_items[0];

#ifndef NDEBUG
      std::cout << "demodulating " << noutput_items <<  " symbols, SF = " << SF << std::endl;
#endif

      for(size_t i = 0; i < noutput_items; i++) {
	gr_complex corr;
	volk_32fc_x2_conjugate_dot_prod_32fc(&corr, dataIn + i*symbolSize + startingIndex, upchirps.data() + startingIndex, windowSize);

	float freq = std::arg(corr)/(2.0*M_PI) + 1.0;
	
	dataOut[i] = uint16_t(round(freq*(1 << SF)))%uint16_t(1 << SF);
#ifndef NDEBUG
	std::cout << "demodulated symbol: " << std::dec << dataOut[i] << ", SF = " << SF << std::endl;
#endif
      }
      
      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (noutput_items);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }
    
    template<typename T>
    void symbolDemod_impl<T>::setSF(size_t SFNew) {
      SF = SFNew;
    }

  template class symbolDemod<float>;
  template class symbolDemod<gr_complex>;
  } /* namespace LibreLoRa */
} /* namespace gr */

