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

    template<typename T>
    typename Correlation<T>::sptr
    Correlation<T>::make(const std::vector<T>& symbol)
    {
      return gnuradio::get_initial_sptr
        (new Correlation_impl<T>(symbol));
    }


    /*
     * The private constructor
     */
    template<typename T>
    Correlation_impl<T>::Correlation_impl(const std::vector<T>& symbol)
      : gr::sync_block("Correlation",
		       gr::io_signature::make(1, 1, sizeof(T)),
		       gr::io_signature::make(2, 2, sizeof(T))),
	symbol(symbol) {
      this->set_history(symbol.size());
#ifndef NDEBUG
      std::cout << "Correlation: symbol size:" << symbol.size() << std::endl;
#endif
    }
    
    /*
     * Our virtual destructor.
     */
    template<typename T>
    Correlation_impl<T>::~Correlation_impl()
    {
    }

    template<>
    int
    Correlation_impl<float>::work(int noutput_items,
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
      data_out[0] = samples[0];

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

    template<>
    int
    Correlation_impl<gr_complex>::work(int noutput_items,
				       gr_vector_const_void_star &input_items,
				       gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *corr_out = (gr_complex *) output_items[0];
      gr_complex *data_out = (gr_complex *) output_items[1];

      // Do <+signal processing+>

      gr_complex corr;
      float sumSq;
      gr_complex sum = gr_complex(0 ,0);

      const gr_complex* samples = in;
      volk_32fc_x2_conjugate_dot_prod_32fc(&corr, samples, symbol.data(), symbol.size());
      gr_complex sumSqCompl;
      volk_32fc_x2_conjugate_dot_prod_32fc(&sumSqCompl, samples, samples, symbol.size());
      sumSq = sumSqCompl.real();
      //volk_32fc_accumulator_s32fc(&sum, samples, symbol.size());
      for(auto k = 0; k < symbol.size(); k++)
	sum += samples[k];

      corr_out[0] = corr/sqrt(sumSq - std::norm(sum)/symbol.size());
      data_out[0] = samples[0];

      for(size_t k = 1; k < noutput_items; k++) {
      	sumSq -= std::norm(samples[0]);
      	sum -= samples[0];
	  
      	samples++;

      	sumSq += std::norm(samples[symbol.size() - 1]);
      	sum += samples[symbol.size() - 1];

      	volk_32fc_x2_conjugate_dot_prod_32fc(&corr, samples, symbol.data(), symbol.size());

      	corr_out[k] = corr/sqrt(sumSq - std::norm(sum)/symbol.size());
	data_out[k] = samples[0];
      }

      
      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

    template class Correlation<float>;
    template class Correlation<gr_complex>;
  } /* namespace LibreLoRa */
} /* namespace gr */
