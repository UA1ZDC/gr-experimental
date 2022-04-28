/* -*- c++ -*- */
/*
 * Copyright 2022 gr-experimental author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_EXPERIMENTAL_DEFRAMER_H
#define INCLUDED_EXPERIMENTAL_DEFRAMER_H

#include <gnuradio/experimental/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace experimental {

    /*!
     * \brief <+description of block+>
     * \ingroup experimental
     *
     */
    class EXPERIMENTAL_API deframer : virtual public gr::block
    {
     public:
      typedef std::shared_ptr<deframer> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of experimental::deframer.
       *
       * To avoid accidental use of raw pointers, experimental::deframer's
       * constructor is in a private implementation
       * class. experimental::deframer::make is the public interface for
       * creating new instances.
       */
      static sptr make();
    };

  } // namespace experimental
} // namespace gr

#endif /* INCLUDED_EXPERIMENTAL_DEFRAMER_H */
