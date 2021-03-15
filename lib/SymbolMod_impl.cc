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
#include "SymbolMod_impl.h"
#include <LibreLoRa/getSymbol.h>

namespace gr {
  namespace LibreLoRa {

    SymbolMod::sptr
    SymbolMod::make(size_t SF, size_t symbolSize)
    {
      return gnuradio::make_block_sptr<SymbolMod_impl>(
        SF, symbolSize);
    }


    /*
     * The private constructor
     */
    SymbolMod_impl::SymbolMod_impl(size_t SF, size_t symbolSize)
      : gr::sync_interpolator("SymbolMod",
			      gr::io_signature::make(1, 1, sizeof(uint16_t)),
			      gr::io_signature::make(1, 1, sizeof(float)), symbolSize),
	SF(SF),
	SFCurrent(SF),
	symbolSize(symbolSize)
    {
#ifndef NDEBUG
      std::cout << "SymbolMod: constructed" << std::endl;
#endif
      set_tag_propagation_policy(TPP_CUSTOM);
      
      this->message_port_register_in(pmt::mp("setSF"));
      this->set_msg_handler(pmt::mp("setSF"), [this](pmt::pmt_t msg) {setSF(size_t(pmt::to_long(msg)));});
    }

    /*
     * Our virtual destructor.
     */
    SymbolMod_impl::~SymbolMod_impl()
    {
    }

    int
    SymbolMod_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const uint16_t *in = (const uint16_t *) input_items[0];
      float *out = (float *) output_items[0];

#ifndef NDEBUG
      std::cout << "SymbolMod: modulating " << noutput_items/symbolSize <<  " symbols, SF = " << SF << std::endl;
#endif
      
      for(auto i = 0; i < noutput_items/symbolSize; i++){
	//propagate endOfFrame Tag
	std::vector<gr::tag_t> tags;
	auto nr =  nitems_read(0);
	get_tags_in_range(tags, 0, nr + i, nr + i + 1, pmt::intern("loraEndOfFrame"));
	for(auto tag : tags)
	  add_item_tag(0, nitems_written(0) + i*symbolSize + symbolSize - 1, tag.key, tag.value);	 
	
	static const pmt::pmt_t tagKey = pmt::intern("loraParams");
	get_tags_in_range(tags, 0, nr + i, nr + i + 1, tagKey);
	if(tags.size() != 0) {
	  pmt::pmt_t message = tags[0].value;
	  size_t SFNew = pmt::to_long(pmt::tuple_ref(message, 0));
	  const bool isBeginning = pmt::to_bool(pmt::tuple_ref(message, 2));
	  if(isBeginning) {
	     setSF(SFNew + 2);
	  }
	  setSFCurrent(SFNew);

	  size_t symbolSizeNew = pmt::to_long(pmt::tuple_ref(message, 3));
	  setSymbolSize(symbolSize);
	  
	  add_item_tag(0, nitems_written(0) + i*symbolSize, tagKey, message);
	}
	
	const std::vector<float> symi = getSymbol<float>(in[i]*(1 << (SF - SFCurrent)), SF, symbolSize);
	memcpy(out + i*symbolSize, symi.data(), symbolSize*sizeof(float));

#ifndef NDEBUG
	std::cout << "SymbolMod: modulated symbol: " << std::dec << in[i] << ", SF = " << SFCurrent << std::endl;
#endif
      }

      // Tell runtime system how many output items we produced.
      return (noutput_items/symbolSize)*symbolSize;
    }

    void SymbolMod_impl::setSF(size_t SFNew) {
      SF = SFNew;
    }
    
    void SymbolMod_impl::setSFCurrent(size_t SFNew) {
       SFCurrent = SFNew;
    }

    void SymbolMod_impl::setSymbolSize(size_t symbolSizeNew) {
      symbolSize = symbolSizeNew;
    }
  } /* namespace LibreLoRa */
} /* namespace gr */

