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

#ifndef INCLUDED_LIBRELORA_DEINTERLEAVE_H
#define INCLUDED_LIBRELORA_DEINTERLEAVE_H

#include <LibreLoRa/api.h>
#include <gnuradio/sync_decimator.h>

namespace gr {
  namespace LibreLoRa {

    /*!
     * \brief <+description of block+>
     * \ingroup LibreLoRa
     *
     */
    class LIBRELORA_API deinterleave : virtual public gr::sync_decimator
    {
     public:
      typedef boost::shared_ptr<deinterleave> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of LibreLoRa::deinterleave.
       *
       * To avoid accidental use of raw pointers, LibreLoRa::deinterleave's
       * constructor is in a private implementation
       * class. LibreLoRa::deinterleave::make is the public interface for
       * creating new instances.
       */
      static sptr make(size_t SF, size_t CR);
      virtual void setSF(size_t SF) = 0;
      virtual void setCR(size_t CR) = 0;
    };

  } // namespace LibreLoRa
} // namespace gr

#endif /* INCLUDED_LIBRELORA_DEINTERLEAVE_H */

