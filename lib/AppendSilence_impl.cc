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
#include "AppendSilence_impl.h"

namespace gr {
  namespace LibreLoRa {

    AppendSilence::sptr
    AppendSilence::make()
    {
      return gnuradio::get_initial_sptr
        (new AppendSilence_impl());
    }


    /*
     * The private constructor
     */
    AppendSilence_impl::AppendSilence_impl()
      : gr::block("AppendSilence",
		  gr::io_signature::make(1, 1, sizeof(gr_complex)),
		  gr::io_signature::make(1, 1, sizeof(gr_complex))),
	transmittingFrame(false){
      // set_max_noutput_items((1 << 13));

      message_port_register_in(pmt::mp("start"));
      set_msg_handler(pmt::mp("start"), [this](pmt::pmt_t msg) {transmittingFrame = true;});
    }

    /*
     * Our virtual destructor.
     */
    AppendSilence_impl::~AppendSilence_impl()
    {
    }

    void
    AppendSilence_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = transmittingFrame? noutput_items : 0;
    }

    int
    AppendSilence_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];

      size_t nSamplesUsed = 0;
      for(auto i = 0; i < noutput_items; i++) {
	if(transmittingFrame) {
	  out[i] = in[i];

	  nSamplesUsed++;

	  //check is next sample has endOfFrame tag
	  std::vector<gr::tag_t> tags;
	  auto nr =  nitems_read(0);
	  static const pmt::pmt_t tagKey = pmt::intern("loraEndOfFrame");
	  get_tags_in_range(tags, 0, nr + i, nr + i + 1, tagKey);
	  if(tags.size() != 0) {
#ifndef NDEBUG
	    std::cout << "AppendSilence: got end of frame." << std::endl;
#endif
	    transmittingFrame = false;
	  }
	} else {
	  out[i] = 0;
	}
      }
	
      
      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (nSamplesUsed);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace LibreLoRa */
} /* namespace gr */

