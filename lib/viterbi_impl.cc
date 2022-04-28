/* -*- c++ -*- */
/*
 * Copyright 2022 gr-experimental author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "viterbi_impl.h"

#include "util/math.h"

// Uncomment this if you want debug information
//#define DEBUG

// Number of symbols to test BER against
#define BER_TEST_SYMS 1024

#ifndef M_E
#define M_E 2.7182818284590452354
#endif
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace gr {
namespace experimental {

//#pragma message("set the following appropriately and remove this warning")
using input_type = gr_complex;
//#pragma message("set the following appropriately and remove this warning")
using output_type = unsigned char;
viterbi::sptr
viterbi::make(float ber_threshold, size_t outsync_after, bool fengyun_mode)
{
	return gnuradio::make_block_sptr<viterbi_impl>(
			ber_threshold, outsync_after, fengyun_mode);
}


/*
 * The private constructor
 */
viterbi_impl::viterbi_impl(float ber_threshold, size_t outsync_after, bool fengyun_mode)
: gr::block("viterbi",
		gr::io_signature::make(1 /* min inputs */, 1 /* max inputs */, sizeof(input_type)),
		gr::io_signature::make(1 /* min outputs */, 1 /*max outputs */, sizeof(output_type))),
		d_ber_threshold(ber_threshold),
		d_outsync_after(outsync_after),
		d_fengyun_mode(fengyun_mode) {
	enter_state(Syncing);
	init_prob_table();

	d_conv_ber_test = correct_convolutional_sse_create(2, 7, new uint16_t[2]{0x4F, 0x6D});
	d_conv          = correct_convolutional_sse_create(2, 7, new uint16_t[2]{0x4F, 0x6D});

	d_phase = 0;
	d_shift = 0;
}

/*
 * Our virtual destructor.
 */
viterbi_impl::~viterbi_impl()
{
	correct_convolutional_sse_destroy(d_conv);
	correct_convolutional_sse_destroy(d_conv_ber_test);
}

void viterbi_impl::enter_state(ViterbiState state) {
	switch (state) {
	case Syncing:
		d_state = Syncing;
		break;
	case Synced:
		d_state = Synced;
		d_invalid_buffers = 0;
		break;
	default:
		throw std::runtime_error("Viterbi: invalid state");
	}
}

// r=3/4 depuncture
void viterbi_impl::depuncture(std::complex<uint8_t> *in, uint8_t *out, size_t len) {
	size_t bits = 0;
	for (size_t i = 0; i < len; i++) {
		if (i % 2 == 0) {
			out[bits++] = in[i].real();
			out[bits++] = in[i].imag();
		} else {
			out[bits++] = 128;
			if (d_fengyun_mode) {
				out[bits++] = in[i].real();
				out[bits++] = in[i].imag();
			} else {
				out[bits++] = in[i].imag();
				out[bits++] = in[i].real();
			}
			out[bits++] = 128;
		}
	}
}

// Gets a bit...
template <typename T>
inline bool getBit(T data, int bit) {
	return (data >> bit) & 1;
}

// Calculate BER
float viterbi_impl::ber_calc(std::complex<uint8_t> *insymbols, size_t len) {
	depunctured_symbols.reserve(len * 3);
	decoded_data.reserve(len); // Far bigger than it needs to be
	encoded_data.reserve(len * 3);

	// Depuncture and decode
	depuncture(insymbols, depunctured_symbols.data(), len);
	correct_convolutional_sse_decode_soft(d_conv_ber_test, depunctured_symbols.data(), len*3, decoded_data.data());

	// Reencode
	correct_convolutional_sse_encode(d_conv_ber_test, decoded_data.data(), (len*1.5)/8, encoded_data.data());

	// Calculate the number of differences between the input and reencoded data
	size_t errors = 0, bits = 0;
	for (size_t k = 0; k < len*3; k++) {
		// Skip punctured symbols
		if (depunctured_symbols[k] == 128)
			continue;

		bits++;
		if ((depunctured_symbols[k] > 127) != getBit(encoded_data[k / 8], 7 - k%8))
			errors++;
	}

	// Calculate BER
	return (float)errors / (float)bits * 8.0f;
}

// Rotate phase
void
viterbi_impl::rotate_phase(bool rotate, size_t len, std::complex<uint8_t> *in, std::complex<uint8_t> *out) {
	if (rotate) {
		for (size_t i = 0; i < len; i++) {
			out[i].real(in[i].imag());
			out[i].imag(UINT8_MAX - in[i].real());
		}
	} else {
		std::memcpy(out, in, len * sizeof(std::complex<uint8_t>));
	}
}

// Calculate the probability that a symbol is a one
// See http://www.dsplog.com/2007/08/05/bit-error-probability-for-bpsk-modulation/
// and https://www.desmos.com/calculator/2dd0myeckw
void
viterbi_impl::init_prob_table() {
	for (int i = -128; i < 128; i++) {
		float E = 0.707f;
		float N = 0.32f;

		float val = i/127.0f;

		float a = 1.0f/std::sqrt((float)M_PI*N) * std::pow((float)M_E, -std::pow(-val + std::sqrt(E), 2.0f)/N);
		float b = 1.0f/std::sqrt((float)M_PI*N) * std::pow((float)M_E, -std::pow( val + std::sqrt(E), 2.0f)/N);

		prob_table[i+128] = (a - b)*127.0f + 127.0f;
	}
}
uint8_t
viterbi_impl::prob_calc(int8_t in) {
	return prob_table[in+128];
}

void
viterbi_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
{
	//#pragma message("implement a forecast that fills in how many items on each input you need to produce noutput_items and remove this warning")
	/* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
	ninput_items_required[0] = BUFFER_SIZE;
}

int
viterbi_impl::general_work (int noutput_items,
		gr_vector_int &ninput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items)
{
	auto in_syms = static_cast<const input_type*>(input_items[0]);
	auto out = static_cast<output_type*>(output_items[0]);

	//#pragma message("Implement the signal processing in your block and remove this warning")
	// Do <+signal processing+>
	auto ninputs = BUFFER_SIZE;

	input_symbols.reserve(ninputs);
	input_symbols_rotated.reserve(ninputs);

    symbols.reserve(ninputs);
    for (size_t i = 0; i < ninputs; i++) {
        float real = clamp(in_syms[i].real()*127.0f, 127.0f);
        float imag = clamp(in_syms[i].imag()*127.0f, 127.0f);
        symbols[i] = std::complex<int8_t>(real, imag);
    }

	for (size_t i = 0; i < ninputs; i++) {
		input_symbols[i] = std::complex<uint8_t>(
				prob_calc(symbols.data()[i].real()),
				prob_calc(symbols.data()[i].imag())
		);

		// 128 is used for a punctured symbol, which these aren't
		if (input_symbols[i].real() == 128) input_symbols[i].real(127);
		if (input_symbols[i].imag() == 128) input_symbols[i].imag(127);
	}

	switch (d_state) {
	case Syncing:{
		float bestber = 1;
		for (int rotation = 0; rotation < (d_fengyun_mode ? 1 : 2); rotation++) {
			rotate_phase(rotation == 1, BER_TEST_SYMS, input_symbols.data(), input_symbols_rotated.data());

			for (int shift = 0; shift < 2; shift++) {
				float ber = ber_calc(input_symbols_rotated.data() + shift, BER_TEST_SYMS);
				if (ber < bestber) {
					d_phase = rotation;
					d_shift = shift;
					bestber = ber;
				}
			}
		}
#ifdef DEBUG
printf("Viterbi: Syncing: %s, %s, BER = %f\n", d_phase?"rotated 90deg":"no rotation  ", d_shift?"shifted ":"no shift", bestber);
#endif
if (bestber < d_ber_threshold) {
#ifdef DEBUG
	printf("Viterbi: switch state to Synced\n");
#endif
	enter_state(Synced);
}
break;
	}
	case Synced: {
		rotate_phase(d_phase == 1, BER_TEST_SYMS, input_symbols.data(), input_symbols_rotated.data());
		float ber = ber_calc(input_symbols_rotated.data() + d_shift, BER_TEST_SYMS);

		if (ber > d_ber_threshold) {
#ifdef DEBUG
			printf("Viterbi: Synced: BER %3f exceeds BER threshold, %zu/%zu\n", ber, d_invalid_buffers, d_outsync_after);
#endif
			d_invalid_buffers++;

			if (d_invalid_buffers > d_outsync_after) {
#ifdef DEBUG
				printf("Viterbi: switch state to Syncing\n");
#endif
				enter_state(Syncing);
			}
		} else {
			d_invalid_buffers = 0;
		}
		break;
	}
	default:
		throw std::runtime_error("Viterbi: bad state\n");
	}

	// Just exit now if we aren't synced yet
	if (d_state == Syncing)
	{
		consume_each (ninputs);
		return 0;
	}

	// Actually decode the data
	depunctured_symbols.reserve(ninputs * 3);

	rotate_phase(d_phase == 1, ninputs, input_symbols.data(), input_symbols_rotated.data());
	depuncture(input_symbols_rotated.data() + d_shift, depunctured_symbols.data(), ninputs - d_shift);
	correct_convolutional_sse_decode_soft(d_conv, depunctured_symbols.data(), ninputs*3, out);

	// Tell runtime system how many input items we consumed on
	// each input stream.
	consume_each (ninputs);

	// Tell runtime system how many output items we produced.
	return (ninputs*1.5)/8;
}

} /* namespace experimental */
} /* namespace gr */
