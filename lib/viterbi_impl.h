/* -*- c++ -*- */
/*
 * Copyright 2022 gr-experimental author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_EXPERIMENTAL_VITERBI_IMPL_H
#define INCLUDED_EXPERIMENTAL_VITERBI_IMPL_H

#include <gnuradio/experimental/viterbi.h>

extern "C" {
#include <correct-sse.h>
}

namespace gr {
namespace experimental {

enum ViterbiState {
	Syncing,
	Synced
};

const size_t BUFFER_SIZE = 2048;

class viterbi_impl : public viterbi
{
private:
	float d_ber_threshold;
	size_t d_outsync_after;
	bool d_fengyun_mode;

	// Probability look up table
	uint8_t prob_table[256];
	void init_prob_table();
	uint8_t prob_calc(int8_t in);

	float ber_calc(std::complex<uint8_t> *insymbols, size_t len);

	void rotate_phase(bool rotate, size_t len, std::complex<uint8_t> *in, std::complex<uint8_t> *out);
	void depuncture(std::complex<uint8_t> *in, uint8_t *out, size_t len);

	void enter_state(ViterbiState state);

	correct_convolutional_sse *d_conv;
	correct_convolutional_sse *d_conv_ber_test;

	size_t d_shift;
	size_t d_phase;

	size_t d_invalid_buffers;
	ViterbiState d_state;

	std::vector<std::complex<uint8_t>> input_symbols;
	std::vector<std::complex<uint8_t>> input_symbols_rotated;

	std::vector<uint8_t> depunctured_symbols;
	std::vector<uint8_t> decoded_data;
	std::vector<uint8_t> encoded_data;

	std::vector<std::complex<int8_t>> symbols;

public:
	viterbi_impl(float ber_threshold, size_t outsync_after, bool fengyun_mode);
	~viterbi_impl();

	// Where all the action really happens
	void forecast (int noutput_items, gr_vector_int &ninput_items_required);

	int general_work(int noutput_items,
			gr_vector_int &ninput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items);

};

} // namespace experimental
} // namespace gr

#endif /* INCLUDED_EXPERIMENTAL_VITERBI_IMPL_H */
