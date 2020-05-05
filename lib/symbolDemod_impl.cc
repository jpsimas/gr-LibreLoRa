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
#include <iostream>

namespace gr {
  namespace LibreLoRa {

    symbolDemod::sptr
    symbolDemod::make(size_t SF, size_t symbolSize, bool implicit = false)
    {
      return gnuradio::get_initial_sptr
        (new symbolDemod_impl(SF, symbolSize, implicit));
    }


    /*
     * The private constructor
     */
    symbolDemod_impl::symbolDemod_impl(size_t SF, size_t symbolSize, bool implicit)
      :	SF(SF),
	symbolSize(symbolSize),
	implicit(implicit),
	started(true),
	gr::block("symbolDemod",
		       gr::io_signature::make(1, 1, symbolSize*sizeof(float)), 
		       gr::io_signature::make(1, 1, sizeof(uint16_t))) {
      //set_relative_rate(1.0/symbolSize);
      twoUpchirps = getSymbol(0, SF, (symbolSize >> SF));
      twoUpchirps.insert(twoUpchirps.end(), twoUpchirps.begin(), twoUpchirps.end());
      std::cout << "TURBO ENCABULATOR 1000 activated!" << std::endl;

      message_port_register_in(pmt::mp("setSF"));
      set_msg_handler(pmt::mp("setSF"), [this](pmt::pmt_t msg) {setSF(size_t(pmt::to_long(msg)));});
    }

    /*
     * Our virtual destructor.
     */
    symbolDemod_impl::~symbolDemod_impl()
    {
    }

    void
    symbolDemod_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
      ninput_items_required[0] = noutput_items;//1;
      // std::cout << "symbolDemod: forecast called: nouput_items_required = " << ninput_items_required[0] << std::endl;
    }

    int
    symbolDemod_impl::general_work (int noutput_items,
				    gr_vector_int &ninput_items,
				    gr_vector_const_void_star &input_items,
				    gr_vector_void_star &output_items)
    {
      const float *dataIn = (const float *) input_items[0];
      uint16_t *dataOut = (uint16_t *) output_items[0];

      std::cout << "demodulating " << noutput_items <<  " symbols, SF = " << SF << std::endl;
      if(started) {
	// Do <+signal processing+>
	for(size_t i = 0; i < noutput_items; i++) {
	  //size_t i = 0;
	  float corrMax = 0;
	  size_t jMax = 0;
	  for(size_t j = 0; j < symbolSize; j++) {
	    float corrJ;
	    volk_32f_x2_dot_prod_32f(&corrJ, dataIn + i*symbolSize, twoUpchirps.data() + j, symbolSize);
	    if(corrJ >= corrMax) {
	      corrMax = corrJ;
	      jMax = j;
	    }
	  }
	  dataOut[i] = round(jMax*(1 << SF)/float(symbolSize));
	  std::cout << "demodulated symbol: " << std::dec << dataOut[i] << ", SF = " << SF << std::endl;
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

    void symbolDemod_impl::setSF(size_t SFNew) {
      SF = SFNew;
    }
  } /* namespace LibreLoRa */
} /* namespace gr */

