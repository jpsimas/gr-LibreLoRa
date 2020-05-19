/* -*- c++ -*- */
/*
 * Copyright 2020 Joao Pedro de O Simas.
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
#include "Correlation_impl.h"

namespace gr {
  namespace LibreLoRa {

    Correlation::sptr
    Correlation::make(const std::vector<float>& symbol)
    {
      return gnuradio::get_initial_sptr
        (new Correlation_impl(symbol));
    }


    /*
     * The private constructor
     */
    Correlation_impl::Correlation_impl(const std::vector<float>& symbol)
      : gr::sync_block("Correlation",
		       gr::io_signature::make(1, 1, sizeof(float)),
		       gr::io_signature::make(2, 2, sizeof(float))),
	symbol(symbol) {
      set_history(symbol.size());
      std::cout << "symbol size:" << symbol.size() << std::endl;
    }
    
    /*
     * Our virtual destructor.
     */
    Correlation_impl::~Correlation_impl()
    {
    }

    int
    Correlation_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const float *in = (const float *) input_items[0];
      float *corr_out = (float *) output_items[0];
      float *data_out = (float *) output_items[1];

      // Do <+signal processing+>

      float corr;
      float sumSq;
      float sum;

      const float* samples = in;
      volk_32f_x2_dot_prod_32f(&corr, samples, symbol.data(), symbol.size());
      volk_32f_x2_dot_prod_32f(&sumSq, samples, samples, symbol.size());
      volk_32f_accumulator_s32f(&sum, samples, symbol.size());

      corr_out[0] = corr/sqrt(sumSq - sum*sum/symbol.size());

      for(size_t k = 1; k < noutput_items; k++) {
      	sumSq -= samples[0]*samples[0];
      	sum -= samples[0];
	  
      	samples++;

      	sumSq += samples[symbol.size() - 1]*samples[symbol.size() - 1];
      	sum += samples[symbol.size() - 1];

      	volk_32f_x2_dot_prod_32f(&corr, samples, symbol.data(), symbol.size());

      	corr_out[k] = corr/sqrt(sumSq - sum*sum/symbol.size());
	data_out[k] = samples[0];
      }

      
      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace LibreLoRa */
} /* namespace gr */

