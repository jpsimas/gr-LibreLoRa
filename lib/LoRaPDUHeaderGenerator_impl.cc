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
#include "LoRaPDUHeaderGenerator_impl.h"

namespace gr {
  namespace LibreLoRa {

    LoRaPDUHeaderGenerator::sptr
    LoRaPDUHeaderGenerator::make(const std::string& lengthTagName)
    {
      return gnuradio::get_initial_sptr
        (new LoRaPDUHeaderGenerator_impl(lengthTagName));
    }


    /*
     * The private constructor
     */
    LoRaPDUHeaderGenerator_impl::LoRaPDUHeaderGenerator_impl(const std::string& lengthTagName)
      : gr::tagged_stream_block("LoRaPDUHeaderGenerator",
				gr::io_signature::make(1, 1, sizeof(uint8_t)),
				gr::io_signature::make(1, 1, sizeof(uint8_t)), lengthTagName),
	BW(125e3),
	fOffset(0)
    {
      set_tag_propagation_policy(TPP_DONT);
      
      message_port_register_in(pmt::mp("detectionIn"));
      set_msg_handler(pmt::mp("detectionIn"), [this](pmt::pmt_t msg) {processDetection(msg);});
    }

    /*
     * Our virtual destructor.
     */
    LoRaPDUHeaderGenerator_impl::~LoRaPDUHeaderGenerator_impl()
    {
    }

    int
    LoRaPDUHeaderGenerator_impl::calculate_output_stream_length(const gr_vector_int &ninput_items)
    {
      int noutput_items = ninput_items[0] + headerLength;
      return noutput_items ;
    }

    int
    LoRaPDUHeaderGenerator_impl::work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const uint8_t *in = (const uint8_t *) input_items[0];
      uint8_t *out = (uint8_t *) output_items[0];

      size_t SF, CR;
      bool CRCPresent, lowDataRate;
      size_t nHeaderBytes = 0;
      //read loraFrameParams tag
      std::vector<gr::tag_t> tags;
      get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0) + 1, pmt::intern("loraFrameParams"));
      if(tags.size() != 0) {
	nHeaderBytes += headerLength;  
	
	pmt::pmt_t message = tags[0].value;
	SF = pmt::to_long(pmt::tuple_ref(message, 0));
	CR = pmt::to_long(pmt::tuple_ref(message, 1));
	CRCPresent = pmt::to_bool(pmt::tuple_ref(message, 2));
	lowDataRate = pmt::to_bool(pmt::tuple_ref(message, 3));

	const loraPDUHeader header = {
	  0x01,
	  uint8_t(SF),
	  uint8_t(CR),
	  CRCPresent,
	  lowDataRate,
	  BW,
	  0x00,//syncWord (not implemented)
	  fOffset
	};
	
	memcpy(out, (uint8_t*)&header, headerLength);
      }
      
      memcpy(out + nHeaderBytes, in, ninput_items[0]);

      const size_t nOut = ninput_items[0] + nHeaderBytes;
      //add new lengthTag
      add_item_tag(0, nitems_written(0), pmt::intern(d_length_tag_key_str), pmt::from_long(nOut));
      
      // Tell runtime system how many output items we produced.
      return nOut;
    }

    void
    LoRaPDUHeaderGenerator_impl::processDetection(pmt::pmt_t msg){
      pmt::pmt_t params = pmt::tuple_ref(msg, 0);

      BW = pmt::to_float(pmt::tuple_ref(params, 1));
      const size_t sampRate =  pmt::to_float(pmt::tuple_ref(params, 2));
      
      const float fOffsetNorm = pmt::to_float(pmt::tuple_ref(msg, 1));
      fOffset = fOffsetNorm*sampRate;
    }
    
  } /* namespace LibreLoRa */
} /* namespace gr */

