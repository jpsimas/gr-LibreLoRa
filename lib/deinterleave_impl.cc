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
#include "deinterleave_impl.h"

namespace gr {
  namespace LibreLoRa {

    deinterleave::sptr
    deinterleave::make(size_t SF, size_t CR)
    {
      return gnuradio::get_initial_sptr
        (new deinterleave_impl(SF, CR));
    }


    /*
     * The private constructor
     */
    deinterleave_impl::deinterleave_impl(size_t SF, size_t CR)
      :	codeLength(CR + 4),
	SF(SF),
	gr::block("deinterleave",
			   gr::io_signature::make(1, 1, sizeof(uint16_t)),
			   gr::io_signature::make(1, 1, sizeof(uint8_t)))
    {
      std::cout << "ENCABULATION STABILIZER 1500 enabled!" << std::endl;
    }

    /*
     * Our virtual destructor.
     */
    deinterleave_impl::~deinterleave_impl()
    {
    }

     void
    deinterleave_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
     {
       ninput_items_required[0] = codeLength*(noutput_items/SF);
       // std::cout << "deinterleave: forecast called: nouput_items_required = " << ninput_items_required[0] << ", noutput_items = " << noutput_items << std::endl;
     }

    int
    deinterleave_impl::general_work (int noutput_items,
				     gr_vector_int &ninput_items,
				     gr_vector_const_void_star &input_items,
				     gr_vector_void_star &output_items)
    {
      const uint16_t *in = (const uint16_t *) input_items[0];
      uint8_t *out = (uint8_t *) output_items[0];

      //      std::cout << "deinterleave: work called: noutput_items = " << noutput_items << std::endl;
      
      const size_t blocksToProduce = noutput_items/SF;

      if(blocksToProduce != 0) {
	std::cout << "producing: " << blocksToProduce << " blocks (" << "nouput_items = " << noutput_items << ", SF = " << SF << ")" << std::endl;
      
	// Do <+signal processing+>
      }
      
      for(size_t k = 0; k < SF; k++)
	out[k] = 0;

      for(size_t i = 0; i < blocksToProduce; i++){

	std::cout << "deinterleaving symbols: ";
	for(size_t j = 0; j < codeLength; j++)
	  std::cout << std::hex << in[i*codeLength + j] << " ";
	std::cout << std::endl;
	
	for(size_t j = 0; j < codeLength; j++)
	  for(size_t k = 0; k < SF; k++)
	    out[i*SF + (j + k)%SF] |= (in[i*codeLength + j] >> k & 0x01) << j;

      }
      // Tell runtime system how many input items we consumed on
      // each input stream.
      // consume_each (noutput_items);
      consume_each(codeLength*blocksToProduce);

      // Tell runtime system how many output items we produced.
      // return noutput_items;
      return SF*blocksToProduce;
    }

    void deinterleave_impl::setSF(size_t SFNew) {
      SF = SFNew;
      //set_relative_rate(SF/codeLength);
    }
    void deinterleave_impl::setCR(size_t CR) {
      codeLength = CR + 4;
      //set_history(codeLength);
      //set_relative_rate(float(SF)/codeLength);
      //set_decimation(codeLength);
    }
  } /* namespace LibreLoRa */
} /* namespace gr */

