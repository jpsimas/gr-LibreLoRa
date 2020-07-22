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
#include <cmath>
#include "chirpSource_impl.h"

#include <LibreLoRa/getSymbol.h>

namespace gr {
  namespace LibreLoRa {

    chirpSource::sptr
    chirpSource::make(size_t SF, size_t symbolSize)
    {
      return gnuradio::get_initial_sptr
        (new chirpSource_impl(SF, symbolSize));
    }


    /*
     * The private constructor
     */
    chirpSource_impl::chirpSource_impl(size_t SF, size_t symbolSize)
      : gr::sync_block("chirpSource",
              gr::io_signature::make(0, 0, 0),
		       gr::io_signature::make(1, 1, sizeof(gr_complex))),
	symbolSize(symbolSize),
	delta(float(1 << SF)*M_PI/(symbolSize*symbolSize)),
	index(0)
    {
      upchirp = std::vector<gr_complex>(symbolSize);
      for(size_t i = 0; i < upchirp.size(); i++){
	float t = i - (upchirp.size() - 1)/2.0;
	upchirp.at(i) = std::polar<float>(1.0, delta*t*t);
      }
    }
    
    /*
     * Our virtual destructor.
     */
    chirpSource_impl::~chirpSource_impl()
    {
    }

    int
    chirpSource_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      gr_complex *out = (gr_complex *) output_items[0];

      // Do <+signal processing+>
      for(size_t i = 0; i < noutput_items; i++){
	out[i] = upchirp[index];
	index = (index + 1)%upchirp.size();
      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace LibreLoRa */
} /* namespace gr */

