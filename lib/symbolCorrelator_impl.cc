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
#include <volk/volk.h>

namespace gr {
  namespace LibreLoRa {

    symbolCorrelator::sptr
    symbolCorrelator::make(const std::vector<float>& symbol)
    {
      return gnuradio::get_initial_sptr
        (new symbolCorrelator_impl(symbol));
    }


    /*
     * The private constructor
     */
    symbolCorrelator_impl::symbolCorrelator_impl(const std::vector<float>& symbol)
      : gr::sync_block("symbolCorrelator",
		  gr::io_signature::make(4, 4, sizeof(float)),
		  gr::io_signature::make(4, 4, sizeof(float))),
	symbol(symbol) {
      set_history(symbol.size() + 1);
    }

    /*
     * Our virtual destructor.
     */
    symbolCorrelator_impl::~symbolCorrelator_impl()
    {
    }

     // void
     // symbolCorrelator_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
     // {
     //   /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
     //   ninput_items_required[0] = 2*noutput_items + symbol.size();
     //   ninput_items_required[1] = 2*noutput_items + symbol.size();
     //   ninput_items_required[2] = 2*noutput_items + symbol.size();
     //   ninput_items_required[3] = 2*noutput_items + symbol.size();
     // }

    int
    symbolCorrelator_impl::work (int noutput_items,
					 gr_vector_const_void_star &input_items,
					 gr_vector_void_star &output_items)
    {
      const float *in = (const float *) input_items[0];
      const float *corr_in = (const float *) input_items[1];
      const float *sum_in = (const float *) input_items[2];
      const float *sumSq_in = (const float *) input_items[3];
      float *data_out = (float *) output_items[0];
      float *corr_out = (float *) output_items[1];
      float* sum_out = (float *) output_items[2];
      float* sumSq_out = (float *) output_items[3];

      for(size_t k = 0; k < noutput_items; k++)
	data_out[k] = in[k];

      float corr;
      float sumSq;
      float sum;

      const float* samples = in + symbol.size();
      volk_32f_x2_dot_prod_32f(&corr, samples, symbol.data(), symbol.size());
      volk_32f_x2_dot_prod_32f(&sumSq, samples, samples, symbol.size());
      volk_32f_accumulator_s32f(&sum, samples, symbol.size());
	
      corr_out[0] = corr + corr_in[0];
      sum_out[0] = sum + sum_in[0];
      sumSq_out[0] = sumSq + sumSq_in[0];
      
      for(size_t k = 1; k < noutput_items; k++) {
      	sumSq -= samples[0]*samples[0];
      	sum -= samples[0];
	  
      	samples++;

      	sumSq += samples[symbol.size() - 1]*samples[symbol.size() - 1];
      	sum += samples[symbol.size() - 1];

      	volk_32f_x2_dot_prod_32f(&corr, samples, symbol.data(), symbol.size());

      	corr_out[k] = corr + corr_in[k];
      	sum_out[k] = sum + sum_in[k];
      	sumSq_out[k] = sumSq + sumSq_in[k];
      }



      
      // const float* samples = in + noutput_items;
      
      // for(size_t k = 0; k < noutput_items; k++) {
      // 	volk_32f_x2_dot_prod_32f(&corr, samples + k, symbol.data(), symbol.size());
      // 	corr_out[k] = corr + corr_in[k];
      // }

      // Do <+signal processing+>
      // Tell runtime system how many input items we consumed on
      // each input stream.
      //consume_each(noutput_items);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace LibreLoRa */
} /* namespace gr */

