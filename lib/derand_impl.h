/* -*- c++ -*- */
/*
 * Copyright 2022 gr-experimental author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_EXPERIMENTAL_DERAND_IMPL_H
#define INCLUDED_EXPERIMENTAL_DERAND_IMPL_H

#include <gnuradio/experimental/derand.h>

namespace gr {
namespace experimental {

class derand_impl : public derand
{
private:
	uint8_t randomTable[1024];
	void generateRandomTable();

public:
	derand_impl();
	~derand_impl();

	// Where all the action really happens
	void forecast (int noutput_items, gr_vector_int &ninput_items_required);

	int general_work(int noutput_items,
			gr_vector_int &ninput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items);

};

} // namespace experimental
} // namespace gr

#endif /* INCLUDED_EXPERIMENTAL_DERAND_IMPL_H */
