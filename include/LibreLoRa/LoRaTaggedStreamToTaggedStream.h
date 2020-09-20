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

#ifndef INCLUDED_LIBRELORA_LORATAGGEDSTREAMTOTAGGEDSTREAM_H
#define INCLUDED_LIBRELORA_LORATAGGEDSTREAMTOTAGGEDSTREAM_H

#include <LibreLoRa/api.h>
#include <gnuradio/tagged_stream_block.h>

namespace gr {
  namespace LibreLoRa {

    /*!
     * \brief <+description of block+>
     * \ingroup LibreLoRa
     *
     */
    class LIBRELORA_API LoRaTaggedStreamToTaggedStream : virtual public gr::tagged_stream_block
    {
     public:
      typedef boost::shared_ptr<LoRaTaggedStreamToTaggedStream> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of LibreLoRa::LoRaTaggedStreamToTaggedStream.
       *
       * To avoid accidental use of raw pointers, LibreLoRa::LoRaTaggedStreamToTaggedStream's
       * constructor is in a private implementation
       * class. LibreLoRa::LoRaTaggedStreamToTaggedStream::make is the public interface for
       * creating new instances.
       */
      static sptr make(const std::string& lengthTagName);
    };

  } // namespace LibreLoRa
} // namespace gr

#endif /* INCLUDED_LIBRELORA_LORATAGGEDSTREAMTOTAGGEDSTREAM_H */

