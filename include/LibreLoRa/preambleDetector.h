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

#ifndef INCLUDED_LIBRELORA_PREAMBLEDETECTOR_H
#define INCLUDED_LIBRELORA_PREAMBLEDETECTOR_H

#include <LibreLoRa/api.h>
#include <gnuradio/hier_block2.h>

namespace gr {
  namespace LibreLoRa {

    /*!
     * \brief <+description of block+>
     * \ingroup LibreLoRa
     *
     */
    class LIBRELORA_API preambleDetector : virtual public gr::hier_block2
    {
     public:
      typedef std::shared_ptr<preambleDetector> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of LibreLoRa::preambleDetector.
       *
       * To avoid accidental use of raw pointers, LibreLoRa::preambleDetector's
       * constructor is in a private implementation
       * class. LibreLoRa::preambleDetector::make is the public interface for
       * creating new instances.
       */
      static sptr make(size_t SF, size_t OSF, size_t nUpchirps, uint8_t syncWordNumber);
    };

  } // namespace LibreLoRa
} // namespace gr

#endif /* INCLUDED_LIBRELORA_PREAMBLEDETECTOR_H */

