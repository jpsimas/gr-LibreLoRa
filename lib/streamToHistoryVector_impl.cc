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
#include "streamToHistoryVector_impl.h"

namespace gr {
  namespace LibreLoRa {

    template<typename T>
    typename streamToHistoryVector<T>::sptr
    streamToHistoryVector<T>::make(size_t nVect)
    {
      return gnuradio::get_initial_sptr
        (new streamToHistoryVector_impl<T>(nVect));
    }


    /*
     * The private constructor
     */
    template<typename T>
    streamToHistoryVector_impl<T>::streamToHistoryVector_impl(size_t nVect)
      : gr::sync_block("streamToHistoryVector",
		       gr::io_signature::make(1, 1, sizeof(T)),
		       gr::io_signature::make(1, 1, nVect*sizeof(T))),
	nVect(nVect) {
      gr::sync_block::set_history(nVect);
    }

    /*
     * Our virtual destructor.
     */
    template<typename T>
    streamToHistoryVector_impl<T>::~streamToHistoryVector_impl()
    {
    }

    // template<typename T>
    // void
    // streamToHistoryVector_impl<T>::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    // {
    //   /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    //   ninput_items_required[0] = noutput_items + nVect - 1;
    // }

    template<typename T>
    int
    streamToHistoryVector_impl<T>::work(int noutput_items,
				     gr_vector_const_void_star &input_items,
				     gr_vector_void_star &output_items)
    {
      const T *in = (const T *) input_items[0];
      T *out = (T *) output_items[0];

      // Do <+signal processing+>
      for(size_t i = 0; i < noutput_items; i++) {
	// for(size_t j = 0; j < nVect; j++)
	//   out[nVect*i + j] = in[i + j];
	memcpy(out + nVect*i, in + i, nVect*sizeof(gr_complex));
      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

    template class streamToHistoryVector<float>;
    template class streamToHistoryVector<gr_complex>;
  } /* namespace LibreLoRa */
} /* namespace gr */

