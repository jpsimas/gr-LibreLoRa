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
#include "AppendPrefix_impl.h"
#include <LibreLoRa/getSymbol.h>

namespace gr {
  namespace LibreLoRa {

    typename AppendPrefix::sptr
    AppendPrefix::make(size_t SF, size_t symbolSize, size_t nUpchirps, uint16_t syncWordNumber)
    {
      return gnuradio::make_block_sptr<AppendPrefix_impl>(
        SF, symbolSize, nUpchirps, syncWordNumber);
    }


    /*
     * The private constructor
     */
    AppendPrefix_impl::AppendPrefix_impl(size_t SF, size_t symbolSize, size_t nUpchirps, uint16_t syncWordNumber) 
      : gr::block("AppendPrefix",
		  gr::io_signature::make(1, 1, sizeof(float)),
		  gr::io_signature::make(1, 1, sizeof(float))),
	SF(SF),
	symbolSize(symbolSize),
	nUpchirps(nUpchirps),
	syncWordNumber(syncWordNumber){

	calculatePrefix();
      this->set_max_noutput_items(prefix.size());
      this->set_min_output_buffer(this->max_noutput_items() + prefix.size());
      this->set_tag_propagation_policy(gr::block::TPP_CUSTOM);
    }

    /*
     * Our virtual destructor.
     */
    AppendPrefix_impl::~AppendPrefix_impl()
    {
    }

    void
    AppendPrefix_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      // const size_t nFrames = (noutput_items + payloadSize + prefix.size() - 1)/(payloadSize + prefix.size());
      
      // ninput_items_required[0] = nFrames*payloadSize;
      ninput_items_required[0] = noutput_items;
    }

    int
    AppendPrefix_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const float *in = (const float *) input_items[0];
      float *out = (float *) output_items[0];
      
      size_t nFrames = 0;
      size_t produced = 0;

      size_t i;
      for(i = 0; i < noutput_items; i++) {

	//propagate tags
	auto nr =  this->nitems_read(0);
	this->get_tags_in_range(tags, 0, nr + i, nr + i + 1);
	for(auto tag : tags) {
	  this->add_item_tag(0, this->nitems_written(0) + i + nFrames*prefix.size(), tag.key, tag.value);
	}
	
	tags.clear();
	static const pmt::pmt_t tagKey = pmt::intern("loraParams");
	this->get_tags_in_range(tags, 0, nr + i, nr + i + 1, tagKey);
	
	if(tags.size() != 0) {
#ifndef NDEBUG
	  // std::cout << "AppendPrefix: got tag. tags.size() = " << tags.size() << std::endl;
#endif

	  const bool isBeginning = pmt::to_bool(pmt::tuple_ref(tags[0].value, 2));
	  if(isBeginning) {
	    const size_t SFNew = pmt::to_long(pmt::tuple_ref(tags[0].value, 0)) + 2;
	    if(SFNew != SF) {
	      SF = SFNew;
	      calculatePrefix();
	      this->set_max_noutput_items(prefix.size());
	      this->set_min_output_buffer(this->max_noutput_items() + prefix.size());
	    }
	    
	    memcpy(out + i + nFrames*prefix.size(), prefix.data(), prefix.size()*sizeof(float));

	    // this->add_item_tag(0, this->nitems_written(0) + i + nFrames*prefix.size(), tagKey, tags[0].value);
	    
	    nFrames++;
	    produced += prefix.size();
#ifndef NDEBUG
	    std::cout << "AppendPrefix: prefix inserted." << std::endl;
#endif
	  }
	}
	
	out[i + nFrames*prefix.size()] = in[i];
	produced++;

	if(produced >= noutput_items) {
#ifndef NDEBUG
	  // std::cout << "AppendPrefix: sent everything. i = " << i << std::endl;
#endif
	  break;
	}
      }

#ifndef NDEBUG
      // std::cout << "AppendPrefix: work ended. i = " << i << ", noutput_items = " << noutput_items << ", produced = " << produced <<  std::endl;
#endif
      // Do <+signal processing+>
      // Tell runtime system how many input items we consumed on
      // each input stream.
      // gr::block::consume_each (noutput_items);
      gr::block::consume_each(i + 1);
      
      // Tell runtime system how many output items we produced.
      // return nFrames*prefix.size() + noutput_items;
      return produced;
    }

    void AppendPrefix_impl::calculatePrefix() {
      prefix.clear();

      auto upchirp = getSymbol<float>(0, SF, symbolSize);
      //append upchirps
      for(auto i = 0; i < nUpchirps; i++)
	prefix.insert(prefix.end(), upchirp.begin(), upchirp.end());

      //appendSyncWord
      const auto syncWord1 = getSymbol<float>(((syncWordNumber >> 4) << 3), SF, symbolSize);
      prefix.insert(prefix.end(), syncWord1.begin(), syncWord1.end());

      const auto syncWord2 = getSymbol<float>(((syncWordNumber & 0xf) << 3), SF, symbolSize);
      prefix.insert(prefix.end(), syncWord1.begin(), syncWord1.end());

      //append downchirps
      for(auto& x : upchirp)
	x = -x;
      
      for(auto i = 0; i < 2; i++)
	prefix.insert(prefix.end(), upchirp.begin(), upchirp.end());

      //append section of downchirp
      size_t OSF = (symbolSize >> SF);

      prefix.insert(prefix.end(),
			       upchirp.begin(),
			       upchirp.begin() + ((upchirp.size()/4) - OSF));

    }
    
  } /* namespace LibreLoRa */
} /* namespace gr */

