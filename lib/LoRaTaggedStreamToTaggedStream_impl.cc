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
#include "LoRaTaggedStreamToTaggedStream_impl.h"

namespace gr {
  namespace LibreLoRa {

    LoRaTaggedStreamToTaggedStream::sptr
    LoRaTaggedStreamToTaggedStream::make(const std::string& lengthTagName)
    {
      return gnuradio::get_initial_sptr
        (new LoRaTaggedStreamToTaggedStream_impl(lengthTagName));
    }


    /*
     * The private constructor
     */
    LoRaTaggedStreamToTaggedStream_impl::LoRaTaggedStreamToTaggedStream_impl(const std::string& lengthTagName)
      : gr::tagged_stream_block("LoRaTaggedStreamToTaggedStream",
				gr::io_signature::make(1, 1, sizeof(uint8_t)),
				gr::io_signature::make(1, 1, sizeof(uint8_t)), lengthTagName)
    {
      set_tag_propagation_policy(TPP_DONT);
    }

    /*
     * Our virtual destructor.
     */
    LoRaTaggedStreamToTaggedStream_impl::~LoRaTaggedStreamToTaggedStream_impl()
    {
    }

    int
    LoRaTaggedStreamToTaggedStream_impl::calculate_output_stream_length(const gr_vector_int &ninput_items)
    {
      //maximum number of output items that can be produced
      int noutput_items = ninput_items[0];
      return noutput_items;
    }

    int
    LoRaTaggedStreamToTaggedStream_impl::work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const uint8_t *in = (const uint8_t *) input_items[0];
      uint8_t *out = (uint8_t *) output_items[0];

      size_t nHeaderBytes = 0;
      
      if(ninput_items[0] >= headerLength) {
	// read header
	const loraPDUHeader* header = (loraPDUHeader*) (in);
	if(header->hasHeader == 0x01) {
	  nHeaderBytes += headerLength;
	  
	  // send header info as a loraFrameParams tag
	  pmt::pmt_t message = pmt::make_tuple(pmt::from_long(header->SF),
					       pmt::from_long(header->CR),
					       pmt::from_bool(header->payloadCRCPresent),
					       pmt::from_bool(header->lowDataRate),
					       pmt::from_float(header->BW),
					       pmt::from_long(header->syncWordNum),
					       pmt::from_float(header->fCenter));
      
	  add_item_tag(0, nitems_written(0), pmt::intern("loraFrameParams"), message);
	}
      }

      const size_t nOutput = ninput_items[0] - nHeaderBytes;
      
      add_item_tag(0, nitems_written(0), pmt::intern(d_length_tag_key_str), pmt::from_long(nOutput));
      
      memcpy(out, in + nHeaderBytes, nOutput);
      // Tell runtime system how many output items we produced.
      return nOutput;
    }

  } /* namespace LibreLoRa */
} /* namespace gr */

