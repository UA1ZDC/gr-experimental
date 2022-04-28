/* -*- c++ -*- */
/*
 * Copyright 2022 gr-experimental author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_EXPERIMENTAL_DEFRAMER_IMPL_H
#define INCLUDED_EXPERIMENTAL_DEFRAMER_IMPL_H

#include <gnuradio/experimental/deframer.h>

namespace gr {
namespace experimental {

const size_t BUFFER_SIZE = 2048;

enum SyncMachineState { State0, State1, State2, State3 };

using asm_t = uint32_t;

class deframer_impl : public deframer
{
private:
	asm_t shifter;
	bool asmCompare(asm_t a, asm_t b);

	uint8_t *frameBuffer;
	uint8_t byteBuffer;
	unsigned int bufferPosition = 0, bufferBitPosition = 0;
	void pushByte(uint8_t byte);
	void pushBit(bool bit);

	SyncMachineState state;

	unsigned int bitsWritten;
	bool writingData;
	bool invert;
	unsigned int badFrames;
	unsigned int goodFrames;

	void startWriting();
	void enterState(SyncMachineState newState);
	int skip;

public:
	deframer_impl();
	~deframer_impl();

	// Where all the action really happens
	void forecast (int noutput_items, gr_vector_int &ninput_items_required);

	int general_work(int noutput_items,
			gr_vector_int &ninput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items);

};

} // namespace experimental
} // namespace gr

#endif /* INCLUDED_EXPERIMENTAL_DEFRAMER_IMPL_H */
