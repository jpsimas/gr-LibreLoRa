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
#include "decode_impl.h"

#include <LibreLoRa/getPairityMatrix.h>

namespace gr {
  namespace LibreLoRa {

    decode::sptr
    decode::make(size_t SF)
    {
      return gnuradio::get_initial_sptr
        (new decode_impl(SF));
    }


    /*
     * The private constructor
     */
    decode_impl::decode_impl(size_t SF)
      : SF(SF),
	gr::block("decode",
		  gr::io_signature::make(1, 1, SF*sizeof(uint8_t)),
		  gr::io_signature::make(1, 1, SF*sizeof(uint8_t))) {
      setCR(4);
    }

    /*
     * Our virtual destructor.
     */
    decode_impl::~decode_impl()
    {
    }

    void
    decode_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    }

    int
    decode_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const uint8_t *in = (const uint8_t *) input_items[0];
      uint8_t *out = (uint8_t *) output_items[0];

      // Do <+signal processing+>
      for(size_t i = 0; i < noutput_items; i++)
	for(size_t j = 0; j < SF; j++)
	  out[SF*i + j] = in[SF*i + j];

      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (noutput_items);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

    void decode_impl::setCR(size_t CRnew) {
      CR = CRnew;
      auto pairityMatrix = getPairityMatrix(CR);
      for(int i = 0; i < 4; i++)
	checkMatrix[i] = calcPairity(1 << i);
    }

    void decode_impl::calculateCosetLeaders(){
      std::vector<bool, (1 << (CR + 4))> found;
      for(auto& f : found)
	f = false;
      for(auto& c : checkMatrix)
	found[c] = true;

      for(size_t i = 0; i + 1 < (CR + 4); i++)
	for(size_t j = 0; j < (CR + 4) j++) {
	  uint8_t syndrome = checkMatrix[i]^checkMatrix[j];
	  if(!found(syndrome))
	    cosetLeaders(syndrome, std::make_pair(i, j)) = true;//CHECK THAT SHIT
	  found(syndrome) = true;
	}
    }
    
  } /* namespace LibreLoRa */
} /* namespace gr */

