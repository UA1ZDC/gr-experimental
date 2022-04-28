/* -*- c++ -*- */
/*
 * Copyright 2022 gr-experimental author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_EXPERIMENTAL_REEDSOLOMON_H
#define INCLUDED_EXPERIMENTAL_REEDSOLOMON_H

#include <gnuradio/experimental/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace experimental {

    /*!
     * \brief <+description of block+>
     * \ingroup experimental
     *
     */
    class EXPERIMENTAL_API reedsolomon : virtual public gr::block
    {
     public:
      typedef std::shared_ptr<reedsolomon> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of experimental::reedsolomon.
       *
       * To avoid accidental use of raw pointers, experimental::reedsolomon's
       * constructor is in a private implementation
       * class. experimental::reedsolomon::make is the public interface for
       * creating new instances.
       */
      static sptr make(int n);
    };

  } // namespace experimental
} // namespace gr

#endif /* INCLUDED_EXPERIMENTAL_REEDSOLOMON_H */
