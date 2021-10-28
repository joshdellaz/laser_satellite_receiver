#include "liquid_condensed.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>


#include "packet_frame.h"
#include "liquid.internal.h"




//Move these into separate file? Considering having "utils" file containing smaller functions, and keeping the higher-level stuff here

bool applyFEC() {
	//rs & other liquid implementations are easy to implement.. openFEC (LDPC) is more complex
	
	return 0;
}

bool removeFEC() {


	return 0;
}

bool getCRC(packet_t * packet) {

	crc_scheme   check = LIQUID_CRC_32; // error-detection scheme
	packet->crc = (uint32_t)crc_generate_key(check, packet->data, DATA_LENGTH_BYTES);

	return 0;
}

bool applyScrambling() {

	return 0;
}

bool removeScrambling() {

	return 0;
}

bool applyInterleaving() {

	return 0;
}

bool removeInterleaving() {

	return 0;
}


//
bool getMaximumLengthSequencePreamble(uint8_t * mls_preamble, unsigned int *mls_preamble_length) {

	// options
	//TODO: Pick a good value for m
	unsigned int m = 10;   // shift register length, n=2^m - 1
	unsigned int repititions = 5;	//Number of MLS repititions in preamble

	// create and initialize m-sequence
	msequence_s ms = msequence_create_default(m);//Fix these struct name definitions... Liquid maybe borked?
	//msequence_print(ms);
	unsigned int n = msequence_get_length(ms);

	// create and initialize first binary sequence on m-sequence
	bsequence_s mls = bsequence_create(n);
	bsequence_init_msequence(mls, ms);


	//Repeat specified number of times and move to new array
	mls_preamble = (uint8_t*)malloc(repititions * (mls->s_len));
	for (int i = 0; i < repititions; i++) {
		for (int j = 0; j < (mls->s_len); j++) {
			mls_preamble[i*(mls->s_len) + j] = (mls->s)[j];
		}
	}
	mls_preamble_length = repititions*(mls->s_len);

	// clean up memory
	bsequence_destroy(mls);

	return 0;
}


//Output format = byte array of bits to be transmitted where lowest index should be transmitted first
//ENSURE *frame and *packet are freed after use!
bool assembleFrame(uint8_t * frame, uint8_t * packet, unsigned int packet_length) {//Basically just adds preamble
	
	//Add 10101010... to start
	unsigned int alternating_preamble_length = 2;
	for (int i = 0; i < alternating_preamble_length; i++) {
		frame[i] = 0b10101010;
	}

	//Add MLS preamble next
	unsigned int mls_preamble_length = 0;
	uint8_t* mls_preamble;

	getMaximumLengthSequencePreamble(mls_preamble, mls_preamble_length);
	for (int i = 0; i < mls_preamble_length; i++) {
		frame[alternating_preamble_length + i] = mls_preamble[i];
	}
	free(mls_preamble);

	//Add packet data
	for (int i = 0; i < packet_length; i++) {
		frame[alternating_preamble_length + mls_preamble_length + i] = packet[i];
	}

	return 0;
}


bool disassembleFrame() {//Just checks MLS autocorrelation, determines start of packet, and strips preamble
	return 0;
}

bool assemblePacket(packet_t *packet, unsigned int *packet_length) {//much much easier if fields n*8 bits?
	//verify endianness
	uint8_t assembled_packet[PACKET_DATA_LENGTH_BYTES + FEC_SCHEME_FIELD_SIZE] =

	assembled_packet[1]



	return 0;
}

bool disassemblePacket() {
	return 0;
}

bool fragmentDataBufferIntoFrames() {

}