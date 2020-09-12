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

#ifndef INCLUDED_LIBRELORA_APPENDPREFIX_H
#define INCLUDED_LIBRELORA_APPENDPREFIX_H

#include <LibreLoRa/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace LibreLoRa {

    /*!
     * \brief <+description of block+>
     * \ingroup LibreLoRa
     *
     */
    template<typename T>
    class LIBRELORA_API AppendPrefix : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<AppendPrefix<T>> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of LibreLoRa::AppendPrefix.
       *
       * To avoid accidental use of raw pointers, LibreLoRa::AppendPrefix's
       * constructor is in a private implementation
       * class. LibreLoRa::AppendPrefix::make is the public interface for
       * creating new instances.
       */
      static sptr make(const size_t payloadSize, const std::vector<T> prefix);
    };

    typedef AppendPrefix<float> AppendPrefix_ff;
    typedef AppendPrefix<gr_complex> AppendPrefix_cc;
    typedef AppendPrefix<uint8_t> AppendPrefix_bb;
    typedef AppendPrefix<uint16_t> AppendPrefix_ss;

  } // namespace LibreLoRa
} // namespace gr

#endif /* INCLUDED_LIBRELORA_APPENDPREFIX_H */
