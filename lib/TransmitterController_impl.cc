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
#include "TransmitterController_impl.h"

namespace gr {
  namespace LibreLoRa {

    TransmitterController::sptr
    TransmitterController::make(size_t SF, size_t CR, size_t payloadSize, bool CRCPresent, bool lowDataRate)
    {
      return gnuradio::get_initial_sptr
        (new TransmitterController_impl(SF, CR, payloadSize, CRCPresent, lowDataRate));
    }


    /*
     * The private constructor
     */
    TransmitterController_impl::TransmitterController_impl(size_t SF, size_t CR, size_t payloadSize, bool CRCPresent, bool lowDataRate)
      : gr::block("TransmitterController",
		  gr::io_signature::make(1, 1, sizeof(uint8_t)),
		  gr::io_signature::make(1, 1, sizeof(uint8_t))),
	SF(SF),
	SFCurrent(SF),
	CR(CR),
	CRCurrent(CR),
	payloadSize(payloadSize),
	CRCPresent(CRCPresent),
	lowDataRate(lowDataRate),
	currentState(sendingHeader)
    {

      size_t SFPayload = lowDataRate? (SF - 2) : SF;
      size_t nNibbles = 2*(payloadSize + (CRCPresent? CRCSize : 0));
      size_t nExtraNibbles = 0;
      if(nNibbles > (SF - 7))
	nExtraNibbles = (SFPayload - (nNibbles - (SF - 7))%SFPayload)%SFPayload;
      nNibblesTotal = nNibbles + nExtraNibbles;

#ifndef NDEBUG
      std::cout << "TransmitterController: constructed. Low Data Rate? " << (lowDataRate? "YES" : "NO") << ", payloadSize = " << payloadSize << ", nNibblesTotal = " << nNibblesTotal << std::endl;
#endif
      
      setSFPort = pmt::string_to_symbol("setSFout");
      setCRPort = pmt::string_to_symbol("setCRout");      
      nSamplesPort = pmt::string_to_symbol("nSamples");
      message_port_register_out(setSFPort);
      message_port_register_out(setCRPort);
      message_port_register_out(nSamplesPort);
      
      calculateHeader();
    }

    /*
     * Our virtual destructor.
     */
    TransmitterController_impl::~TransmitterController_impl()
    {
    }

    void
    TransmitterController_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      switch(currentState) {
      case sendingHeader:
	ninput_items_required[0] = (SF - 7);
	break;
      case sendingPayload:
	ninput_items_required[0] = 2*payloadSize - (SF - 7);
	break;	
      }
    }

    int
    TransmitterController_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const uint8_t *in = (const uint8_t *) input_items[0];
      uint8_t *out = (uint8_t *) output_items[0];
      
      switch(currentState) {
      case sendingHeader:
#ifndef NDEBUG
	std::cout << "TransmitterController: sending header." << std::endl;
#endif

	setSFCurrent(SF-2);
	setCRCurrent(4);

	sendParamsTag(true);
	
	//output header
	memcpy(out, headerNibbles, 5*sizeof(uint8_t));
	memcpy(out + 5, in, (SF - 7)*sizeof(uint8_t));
	currentState = sendingPayload;
	consume_each(SF - 7);

#ifndef NDEBUG
	// std::cout << "TransmitterController: produced nibbles: " << std::endl;
	// for(auto i = 0; i < (SF - 2); i++)
	//   std::cout << std::hex << unsigned(out[i]) << " ";
	// std::cout << std::endl;
#endif
	
	return (SF - 2);
      case sendingPayload:

#ifndef NDEBUG
	std::cout << "TransmitterController: sending payload." << std::endl;
#endif

	setSFCurrent(lowDataRate? (SF - 2) : SF);
	setCRCurrent(CR);

	sendParamsTag();

	//output payload
	memcpy(out, in, (2*payloadSize - (SF - 7))*sizeof(uint8_t));

	if(CRCPresent) {
	   //caculate crc
	  uint16_t crc = calculateCRCFromNibbles(in);
	  uint32_t crcNibbles = bytes2nibbles<uint32_t, uint16_t>(crc);

	  memcpy(out + (2*payloadSize - (SF - 7)), &crcNibbles, 2*CRCSize*sizeof(uint8_t));
	}

	memset(out + (2*payloadSize - (SF - 7)) + 2*(CRCPresent ? CRCSize : 0), 0,
	       (nNibblesTotal - ((2*payloadSize - (SF - 7)) + 2*(CRCPresent ? CRCSize : 0)))*sizeof(uint8_t)); 

#ifndef NDEBUG
	// std::cout << "TransmitterController: produced nibbles: " << std::endl;
	// for(auto i = 0; i < nNibblesTotal - (SF - 7); i++)
	//   std::cout << std::hex <<  unsigned(out[i]) << " ";
	// std::cout << std::endl;
#endif
	
	currentState = sendingHeader;
	
	// Tell runtime system how many input items we consumed on
	// each input stream.
	consume_each (2*payloadSize - (SF - 7));
	
	// Tell runtime system how many output items we produced.
	return nNibblesTotal - (SF - 7);
      }
      
      return 0;
    }

    void TransmitterController_impl::setSFCurrent(size_t SFnew) {
      SFCurrent = SFnew;
      // message_port_pub(setSFPort, pmt::from_long(SFCurrent));
    }

    void TransmitterController_impl::setCRCurrent(size_t CRnew) {
      CRCurrent = CRnew;
      // message_port_pub(setCRPort, pmt::from_long(CRCurrent));
    }

    void TransmitterController_impl::sendParamsTag(bool isStartOfFrame) {
      static const pmt::pmt_t tagKey = pmt::intern("loraParams");
      add_item_tag(0, nitems_written(0), tagKey, pmt::make_tuple(pmt::from_long(SFCurrent), pmt::from_long(CRCurrent), pmt::from_bool(isStartOfFrame)));
    }
    
  } /* namespace LibreLoRa */
} /* namespace gr */

