/* -*- c++ -*- */
/*
 * Copyright 2022 gr-experimental author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "derand_impl.h"

#define FRAME_SIZE 8192
#define FRAME_SIZE_BYTES (FRAME_SIZE/8)

namespace gr {
namespace experimental {

//#pragma message("set the following appropriately and remove this warning")
using input_type = unsigned char;
//#pragma message("set the following appropriately and remove this warning")
using output_type = unsigned char;
derand::sptr
derand::make()
{
	return gnuradio::make_block_sptr<derand_impl>(
	);
}


/*
 * The private constructor
 */
derand_impl::derand_impl()
: gr::block("derand",
		gr::io_signature::make(1 /* min inputs */, 1 /* max inputs */, sizeof(input_type)),
		gr::io_signature::make(1 /* min outputs */, 1 /*max outputs */, sizeof(output_type)))
{
	generateRandomTable();
}

/*
 * Our virtual destructor.
 */
derand_impl::~derand_impl()
{
}

// Polynomial is standard CCSDS, 1 + x3 + x5 + x7 + x8
void derand_impl::generateRandomTable() {
	uint8_t shiftRegisiter = 0xff;
	std::memset(randomTable, 0, 1024);

	// 1024 bytes in a packet, 4 of which are the ASM
	for (int i = 4; i < 1024; i++) {
		for (int j = 0; j < 8; j++) {
			// Take latest bit from `shiftRegisiter` and put it into `randomTable`
			randomTable[i] = randomTable[i] << 1;
			randomTable[i] |= std::bitset<8>(shiftRegisiter).test(7);

			// Next bit in PN sequence is XOR of all polynomials
			bool next = std::bitset<8>(shiftRegisiter).test(7) ^
					std::bitset<8>(shiftRegisiter).test(4) ^
					std::bitset<8>(shiftRegisiter).test(2) ^
					std::bitset<8>(shiftRegisiter).test(0);

			// Shift the shift regisiter
			shiftRegisiter = shiftRegisiter << 1;
			shiftRegisiter |= next;
		}
	}

#if 0
	// Print randomization table
	for (int i = 0; i < 1024; i++) {
		printf("%i: %X\n", i, randomTable[i]);
	}
#endif
}

void
derand_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
{
	//#pragma message("implement a forecast that fills in how many items on each input you need to produce noutput_items and remove this warning")
	/* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
	ninput_items_required[0] = FRAME_SIZE_BYTES;

}

int
derand_impl::general_work (int noutput_items,
		gr_vector_int &ninput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items)
{
	auto in = static_cast<const input_type*>(input_items[0]);
	auto out = static_cast<output_type*>(output_items[0]);

	//#pragma message("Implement the signal processing in your block and remove this warning")
	// Do <+signal processing+>
	auto ninputs = FRAME_SIZE_BYTES;

	for(size_t i = 0; i < ninputs; i++) {
		out[i] = in[i] ^ randomTable[i];
	}


	// Tell runtime system how many input items we consumed on
	// each input stream.
	consume_each (ninputs);

	// Tell runtime system how many output items we produced.
	return ninputs;
}

} /* namespace experimental */
} /* namespace gr */
