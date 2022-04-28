/* -*- c++ -*- */
/*
 * Copyright 2022 gr-experimental author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "reedsolomon_impl.h"

#define FRAME_SIZE 8192
#define FRAME_SIZE_BYTES (FRAME_SIZE/8)

namespace gr {
namespace experimental {

//#pragma message("set the following appropriately and remove this warning")
using input_type = unsigned char;
//#pragma message("set the following appropriately and remove this warning")
using output_type = unsigned char;
reedsolomon::sptr
reedsolomon::make(int n)
{
	return gnuradio::make_block_sptr<reedsolomon_impl>(
			n);
}


/*
 * The private constructor
 */
reedsolomon_impl::reedsolomon_impl(int n)
: gr::block("reedsolomon",
		gr::io_signature::make(1 /* min inputs */, 1 /* max inputs */, sizeof(input_type)),
		gr::io_signature::make(1 /* min outputs */, 1 /*max outputs */, sizeof(output_type))),
		n(n)
{
	rs = correct_reed_solomon_create(correct_rs_primitive_polynomial_ccsds, 112, 11, 32);
}

/*
 * Our virtual destructor.
 */
reedsolomon_impl::~reedsolomon_impl()
{
	correct_reed_solomon_destroy(rs);
}

ssize_t
reedsolomon_impl::decode_ccsds(uint8_t *data) {
	for (size_t i = 0; i < 255; i++) {
		data[i] = FromDualBasis[data[i]];
	}

	ssize_t errors = decode_rs8(data);

	for (size_t i = 0; i < 255; i++) {
		data[i] = ToDualBasis[data[i]];
	}

	return errors;
}

std::vector<ssize_t>
reedsolomon_impl::decode_intreleaved_ccsds(uint8_t *data, size_t n) {
	uint8_t rsWorkBuffer[255];
	std::vector<ssize_t> errors;

	for (size_t i = 0; i < n; i++) {
		deinterleave(&data[4], rsWorkBuffer, i, n);
		errors.push_back(decode_ccsds(rsWorkBuffer));
		interleave(rsWorkBuffer, &data[4], i, n);
	}

	return errors;
}

ssize_t
reedsolomon_impl::decode_rs8(uint8_t *data) {
	uint8_t odata[255];
	ssize_t err = correct_reed_solomon_decode(this->rs, data, 255, odata);

	if (err == -1) {
		return -1;
	} else {
		err = 0;
		// Calculate wrong bytes
		for (size_t i = 0; i < 223; i++) {
			if (data[i] != odata[i]) {
				err++;
			}
		}
	}

	std::memcpy(data, odata, 255);
	return err;
}

void
reedsolomon_impl::deinterleave(const uint8_t *data, uint8_t *output, uint8_t pos, uint8_t I) {
	for (size_t i = 0; i < 255; i++) {
		output[i] = data[i * I + pos];
	}
}

void
reedsolomon_impl::interleave(const uint8_t *data, uint8_t *output, uint8_t pos, uint8_t I) {
	for (size_t i = 0; i < 233; i++) {
		output[i * I + pos] = data[i];
	}
}

void
reedsolomon_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
{
	//#pragma message("implement a forecast that fills in how many items on each input you need to produce noutput_items and remove this warning")
	/* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
	ninput_items_required[0] = FRAME_SIZE_BYTES;
}

int
reedsolomon_impl::general_work (int noutput_items,
		gr_vector_int &ninput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items)
{
	auto in = static_cast<const input_type*>(input_items[0]);
	auto out = static_cast<output_type*>(output_items[0]);

	//#pragma message("Implement the signal processing in your block and remove this warning")
	// Do <+signal processing+>
	auto ninputs = FRAME_SIZE_BYTES;

	//frame.reserve(ninputs);

	std::memcpy(out, in, FRAME_SIZE_BYTES);

	decode_intreleaved_ccsds(out, n);


	//std::memcpy(out, frame.data(), FRAME_SIZE_BYTES);

	// Tell runtime system how many input items we consumed on
	// each input stream.
	consume_each (ninputs);

	// Tell runtime system how many output items we produced.
	return ninputs;
}

} /* namespace experimental */
} /* namespace gr */
