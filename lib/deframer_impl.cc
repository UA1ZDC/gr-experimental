/* -*- c++ -*- */
/*
 * Copyright 2022 gr-experimental author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "deframer_impl.h"

#include <bitset>

// Slightly modified from Oleg's original values
const unsigned int stateThresholds[4] = { 0, 2, 6, 16 };

#define ASM_SIZE 32
#define ASM 0x1ACFFC1D
#define INVERSE_ASM (~ASM)

#define FRAME_SIZE 8192
#define FRAME_SIZE_BYTES (FRAME_SIZE/8)

namespace gr {
namespace experimental {

//#pragma message("set the following appropriately and remove this warning")
using input_type = unsigned char;
//#pragma message("set the following appropriately and remove this warning")
using output_type = unsigned char;
deframer::sptr
deframer::make()
{
	return gnuradio::make_block_sptr<deframer_impl>(
	);
}


/*
 * The private constructor
 */
deframer_impl::deframer_impl()
: gr::block("deframer",
		gr::io_signature::make(1 /* min inputs */, 1 /* max inputs */, sizeof(input_type)),
		gr::io_signature::make(1 /* min outputs */, 1 /*max outputs */, sizeof(output_type))),
		shifter(0),
		frameBuffer(new uint8_t[FRAME_SIZE_BYTES]),
		byteBuffer(0),
		bufferPosition(0),
		bufferBitPosition(0),
		state(State0),
		bitsWritten(0),
		writingData(false),
		invert(false),
		badFrames(0),
		goodFrames(0),
		skip(0) {

}

/*
 * Our virtual destructor.
 */
deframer_impl::~deframer_impl()
{
	delete[] frameBuffer;
}

bool
deframer_impl::asmCompare(asm_t a, asm_t b) {
	return std::bitset<ASM_SIZE>(a ^ b).count() <= stateThresholds[state];
}

// Push a byte into the buffer
void
deframer_impl::pushByte(uint8_t byte) {
	frameBuffer[bufferPosition++] = byte;
}

// Push a bit into the buffer
void
deframer_impl::pushBit(bool bit) {
	byteBuffer = byteBuffer << 1 | bit;
	bufferBitPosition++;

	if(bufferBitPosition == 8){
		pushByte(byteBuffer);
		bufferBitPosition = 0;
	}
}

void
deframer_impl::startWriting() {
	bufferPosition = 0;
	bufferBitPosition = 0;
	bitsWritten = 0;
	writingData = true;
}

void
deframer_impl::enterState(SyncMachineState newState) {
	goodFrames = 0;
	badFrames = 0;
	state = newState;
}

void
deframer_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
{
	//#pragma message("implement a forecast that fills in how many items on each input you need to produce noutput_items and remove this warning")
	/* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
	ninput_items_required[0] = BUFFER_SIZE;
}

int
deframer_impl::general_work (int noutput_items,
		gr_vector_int &ninput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items)
{
	auto in = static_cast<const input_type*>(input_items[0]);
	auto out = static_cast<output_type*>(output_items[0]);

	//#pragma message("Implement the signal processing in your block and remove this warning")
	// Do <+signal processing+>
	auto ninputs = BUFFER_SIZE;

	bool complete_frame = false;

	for(unsigned int i = 0; i < ninputs; i++) {
		for(int j = 7; j >= 0; j--) {
			bool bit = std::bitset<ASM_SIZE>(in[i]).test(j);
			if(invert) bit = !bit;

			shifter = shifter << 1 | bit;

			if(writingData) {
				// Append syncword, backwards
				if(bitsWritten == 0) {
					for(int ASMBit = ASM_SIZE-1; ASMBit >= 0; ASMBit--) {
						pushBit(std::bitset<ASM_SIZE>(ASM).test(ASMBit));
					}
					bitsWritten += ASM_SIZE;
				}

				pushBit(bit);
				bitsWritten++;

				// At the end of a frame, copy the data into the output pointer and reset
				if (bitsWritten == FRAME_SIZE) {
					writingData = false;
					bitsWritten = 0;
					skip = ASM_SIZE;
					complete_frame = true;
					std::memcpy(out, frameBuffer, FRAME_SIZE_BYTES);
				}

				if (state != State1) continue;
			}

			// Skip until next sync marker
			if (skip > 1) {
				skip--;
				continue;
			}

			switch (state) {
			// Checks for a perfect sync marker with no errors, if we find one jump to State2
			case State0:
				if (asmCompare(shifter, ASM)) {
					enterState(State2);
					startWriting();
				} else if (asmCompare(shifter, INVERSE_ASM)) {
					invert = !invert;
					enterState(State2);
					startWriting();
				}
				break;
				// Allow up to 2 bit errors, if we check 5 frames without success go back to State0
				// assuming we have lost all lock
			case State1:
				if (asmCompare(shifter, ASM)) {
					startWriting();
					badFrames = 0;
					enterState(State2);
				} else {
					badFrames++;
					goodFrames = 0;
					if (badFrames == 5) {
						enterState(State0);
					}
				}
				break;
				// Intermediate state between the strict State0 and lenient State3
			case State2:
				if (asmCompare(shifter, ASM)) {
					startWriting();
					goodFrames++;
					badFrames = 0;
					if (goodFrames == 5) {
						enterState(State3);
					}
				} else {
					badFrames++;
					goodFrames = 0;
					if (badFrames == 2) {
						enterState(State1);
					}
				}
				break;
				// Assume fully locked, allow a very high level of errors
			case State3:
				if (asmCompare(shifter, ASM)) {
					startWriting();
				} else {
					enterState(State2);
				}
				break;
			default:
				throw std::runtime_error("Invalid value in enum `SyncMachineState`");
			}
		}
	}


	// Tell runtime system how many input items we consumed on
	// each input stream.
	consume_each (FRAME_SIZE_BYTES);

	// Tell runtime system how many output items we produced.
	return (complete_frame ? FRAME_SIZE_BYTES : 0);
}

} /* namespace experimental */
} /* namespace gr */
