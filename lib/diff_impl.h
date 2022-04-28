/* -*- c++ -*- */
/*
 * Copyright 2022 gr-experimental author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_EXPERIMENTAL_DIFF_IMPL_H
#define INCLUDED_EXPERIMENTAL_DIFF_IMPL_H

#include <gnuradio/experimental/diff.h>

namespace gr {
  namespace experimental {

    class diff_impl : public diff
    {
     private:
      // Nothing to declare in this block.
         bool Xin_1 = 0;
         bool Yin_1 = 0;

     public:
      diff_impl();
      ~diff_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);

    };

  } // namespace experimental
} // namespace gr

#endif /* INCLUDED_EXPERIMENTAL_DIFF_IMPL_H */
