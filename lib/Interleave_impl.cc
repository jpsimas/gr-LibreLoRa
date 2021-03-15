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
#include "Interleave_impl.h"

namespace gr {
  namespace LibreLoRa {

    Interleave::sptr
    Interleave::make(size_t SF, size_t CR)
    {
      return gnuradio::make_block_sptr<Interleave_impl>(
        SF, CR);
    }


    /*
     * The private constructor
     */
    Interleave_impl::Interleave_impl(size_t SF, size_t CR)
      : codeLength(CR + 4),
	SF(SF),
	changedParams(false),
	gr::block("Interleave",
		  gr::io_signature::make(1, 1, sizeof(uint8_t)),
		  gr::io_signature::make(1, 1, sizeof(uint16_t)))
    {
#ifndef NDEBUG
      std::cout << "Interleave: constructed" << std::endl;
#endif

      set_tag_propagation_policy(TPP_CUSTOM);
      set_relative_rate(codeLength, SF);
      
      message_port_register_in(pmt::mp("setSF"));
      set_msg_handler(pmt::mp("setSF"), [this](pmt::pmt_t msg) {setSF(size_t(pmt::to_long(msg)));});

      message_port_register_in(pmt::mp("setCR"));
      set_msg_handler(pmt::mp("setCR"), [this](pmt::pmt_t msg) {setCR(size_t(pmt::to_long(msg)));});
    }

    /*
     * Our virtual destructor.
     */
    Interleave_impl::~Interleave_impl()
    {
    }

    void
    Interleave_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = SF*((noutput_items + codeLength - 1)/codeLength);
    }

    int
    Interleave_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const uint8_t *in = (const uint8_t *) input_items[0];
      uint16_t *out = (uint16_t *) output_items[0];

      set_relative_rate(codeLength, SF);

      size_t codeLengthInitial = codeLength;
      size_t SFInitial = SF;
      
      const size_t blocksToProduce = (noutput_items + codeLength - 1)/codeLength;

#ifndef NDEBUG
      // std::cout << "Interleave: blocksToProduce = " << blocksToProduce << std::endl;
#endif
	
      for(size_t k = 0; k < codeLength; k++)
	out[k] = 0;

      size_t i;
      for(i = 0; i < blocksToProduce; i++) {
	
	if(!changedParams || i != 0) {
	  
	  //read loraParam tag if present
	  std::vector<gr::tag_t> tags;
	  auto nr =  nitems_read(0);
	  static const pmt::pmt_t tagKey = pmt::intern("loraParams");
	  get_tags_in_range(tags, 0, nr + i*SF, nr + i*SF + 1, tagKey);
	  if(tags.size() != 0) {
	    // add_item_tag(0, nitems_written(0) + i*codeLength, tagKey, tags[0].value);
	    
	    pmt::pmt_t message = tags[0].value;
	    size_t SFnew = pmt::to_long(pmt::tuple_ref(message, 0));
	    setSF(SFnew);
	    size_t CRnew = pmt::to_long(pmt::tuple_ref(message, 1));
	    setCR(CRnew);
	    changedParams = true;
	    
	    break;
	  }
	} else {
	    
	  changedParams = false;
	  set_relative_rate(codeLength, SF);
	  codeLengthInitial = codeLength;
	  SFInitial = SF;

#ifndef NDEBUG
	  std::cout << "Interleave: updated parameters." << std::endl;
#endif
	}

#ifndef NDEBUG
	std::cout << "Interleave: propagating Tags. nBlocks: " << blocksToProduce << std::endl;
#endif
	//propagate tags in the beggining of blocks
	std::vector<gr::tag_t> tags; 
	auto nr =  nitems_read(0);
	get_tags_in_range(tags, 0, nr + i*SF, nr + i*SF + 1);
	for(auto tag : tags)
	  add_item_tag(0, nitems_written(0) + i*codeLength, tag.key, tag.value);

	//propagate tags in the end of blocks
	tags.clear();
	get_tags_in_range(tags, 0, nr + i*SF + SF - 1, nr + i*SF + SF - 1 + 1);
	for(auto tag : tags)
	  add_item_tag(0, nitems_written(0) + i*codeLength + codeLength - 1, tag.key, tag.value);
	
#ifndef NDEBUG
 	std::cout << "Interleave: interleaving symbols: ";
	for(size_t j = 0; j < SF; j++)
	  std::cout << std::hex << unsigned(in[i*SF + j]) << " ";
	std::cout << ", SF = " << SF << ", CR = " << codeLength - 4 << ", nBlocks: " << blocksToProduce << std::endl;
#endif
     
	for(size_t j = 0; j < SF; j++)
	  for(size_t k = 0; k < codeLength; k++)
	    out[i*codeLength + k] |= (in[i*SF + (j + k)%SF] >> k & 0x01) << j;

#ifndef NDEBUG
 	// std::cout << "Interleave: interleaved symbols: ";
 	// for(size_t k = 0; k < codeLength; k++)
	//   std::cout << std::hex << unsigned(out[i*codeLength + k]) << " ";
	// std::cout << std::endl;
#endif
      }

      // Tell runtime system how many input items we consumed on
      // each input stream.
      // consume_each (noutput_items);
      consume_each(SFInitial*i);

      // Tell runtime system how many output items we produced.
      // return noutput_items;
      return codeLengthInitial*i;
    }

    void Interleave_impl::setSF(size_t SFNew) {
      SF = SFNew;
    }
    
    void Interleave_impl::setCR(size_t CR) {
      codeLength = CR + 4;
    }
    
  } /* namespace LibreLoRa */
} /* namespace gr */

