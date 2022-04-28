/* -*- c++ -*- */
/*
 * Copyright 2022 gr-experimental author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_EXPERIMENTAL_VITERBI_H
#define INCLUDED_EXPERIMENTAL_VITERBI_H

#include <gnuradio/experimental/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace experimental {

    /*!
     * \brief <+description of block+>
     * \ingroup experimental
     *
     */
    class EXPERIMENTAL_API viterbi : virtual public gr::block
    {
     public:
      typedef std::shared_ptr<viterbi> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of experimental::viterbi.
       *
       * To avoid accidental use of raw pointers, experimental::viterbi's
       * constructor is in a private implementation
       * class. experimental::viterbi::make is the public interface for
       * creating new instances.
       */
      static sptr make(float ber_threshold, size_t outsync_after, bool fengyun_mode);
    };

  } // namespace experimental
} // namespace gr

#endif /* INCLUDED_EXPERIMENTAL_VITERBI_H */
