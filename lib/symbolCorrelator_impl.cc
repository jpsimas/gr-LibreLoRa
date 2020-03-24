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
#include "symbolCorrelator_impl.h"
#include <cmath>

namespace gr {
  namespace LibreLoRa {

    symbolCorrelator::sptr
    symbolCorrelator::make(std::vector<float> symbol)
    {
      return gnuradio::get_initial_sptr
        (new symbolCorrelator_impl(symbol));
    }


    /*
     * The private constructor
     */
    symbolCorrelator_impl::symbolCorrelator_impl(std::vector<float> symbol)
      : gr::block("symbolCorrelator",
		  gr::io_signature::make(2, 2, sizeof(float)),
		  gr::io_signature::make(2, 2, sizeof(float))),
	symbol(symbol) {
    }

    /*
     * Our virtual destructor.
     */
    symbolCorrelator_impl::~symbolCorrelator_impl()
    {
    }

    void
    symbolCorrelator_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
      ninput_items_required[0] = 2*noutput_items + symbol.size();
      ninput_items_required[1] = 2*noutput_items + symbol.size();
    }

    int
    symbolCorrelator_impl::general_work (int noutput_items,
					 gr_vector_int &ninput_items,
					 gr_vector_const_void_star &input_items,
					 gr_vector_void_star &output_items)
    {
      const float *in = (const float *) input_items[0];
      const float *corr_in = (const float *) input_items[1];
      float *data_out = (float *) output_items[0];
      float *corr_out = (float *) output_items[1];

      for(size_t k = 0; k < noutput_items; k++)
	data_out[k] = in[k];


      float corr = 0;
      float sumSq = 0;
      float sum = 0;
      
      constexpr float corrMin = 0;//disable for test
      //THIS IS NOT WORKING!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      size_t kFirst;
      for(kFirst = 0; kFirst < noutput_items; kFirst++)
      	if(corr_in[kFirst] > corrMin) {
      	  break;
      	} else
      	  corr_out[kFirst] = 0;

      const float* samples = in + noutput_items + kFirst;
      if(kFirst < noutput_items) {
      	for(size_t j = 0; j < symbol.size(); j++) {
      	  corr += symbol[j]*samples[j];
      	  sumSq += samples[j]*samples[j];
      	  sum += samples[j];
      	}

      	float mean = sum/symbol.size();
	
      	corr_out[kFirst] = corr/sqrt(sumSq - sum*mean);
      
	for(size_t k = kFirst + 1; k < noutput_items; k++) {
	  sumSq -= samples[0]*samples[0];
	  sum -= samples[0];
	  
	  samples++;

	  sumSq += samples[symbol.size() - 1]*samples[symbol.size() - 1];
	  sum += samples[symbol.size() - 1];

	  float mean = sum/symbol.size();
	  
	  corr = 0;
	  for(size_t j = 0; j < symbol.size(); j++)
	    corr += symbol[j]*samples[j];
	
	  corr -= mean*sum;

	  corr_out[k] = corr/sqrt(sumSq - mean*sum);
	}
      }
      // Do <+signal processing+>
      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (noutput_items);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace LibreLoRa */
} /* namespace gr */

