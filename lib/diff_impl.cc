/* -*- c++ -*- */
/*
 * Copyright 2022 gr-experimental author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "diff_impl.h"

namespace gr {
  namespace experimental {

    //#pragma message("set the following appropriately and remove this warning")
    using input_type = unsigned char;
    //#pragma message("set the following appropriately and remove this warning")
    using output_type = unsigned char;
    diff::sptr
    diff::make()
    {
      return gnuradio::make_block_sptr<diff_impl>(
        );
    }


    /*
     * The private constructor
     */
    diff_impl::diff_impl()
      : gr::block("diff",
              gr::io_signature::make(2 /* min inputs */, 2 /* max inputs */, sizeof(input_type)),
              gr::io_signature::make(1 /* min outputs */, 1 /*max outputs */, sizeof(output_type)))
    {}

    /*
     * Our virtual destructor.
     */
    diff_impl::~diff_impl()
    {
    }

    void
    diff_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
    //#pragma message("implement a forecast that fills in how many items on each input you need to produce noutput_items and remove this warning")
      /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    	ninput_items_required[0] = noutput_items;
    	ninput_items_required[1] = noutput_items;
    }

    int
    diff_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      auto in_a = static_cast<const input_type*>(input_items[0]);
      auto in_b = static_cast<const input_type*>(input_items[1]);
      auto out = static_cast<output_type*>(output_items[0]);

      //#pragma message("Implement the signal processing in your block and remove this warning")
      // Do <+signal processing+>

      uint8_t diffOut, diffBuffer = 0;
      size_t o = 0;

      for (size_t i = 0; i < noutput_items; i++) {
          for(int j = 7; j >= 0; j--) {
              bool Xin = std::bitset<8>(in_a[i]).test(j);
              bool Yin = std::bitset<8>(in_b[i]).test(j);

              if (Xin_1 == Yin_1) {
                  bool Xout = (Xin_1 ^ Xin);
                  bool Yout = (Yin_1 ^ Yin);
                  diffOut = Xout << 1 | Yout;
              } else {
                  bool Xout = (Yin_1 ^ Yin);
                  bool Yout = (Xin_1 ^ Xin);
                  diffOut = Xout << 1 | Yout;
              }

              Xin_1 = Xin;
              Yin_1 = Yin;

              diffBuffer = diffBuffer << 2 | diffOut;
              if(j == 3 || j == 7) out[o++] = diffBuffer;
          }
      }

      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (noutput_items);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace experimental */
} /* namespace gr */
