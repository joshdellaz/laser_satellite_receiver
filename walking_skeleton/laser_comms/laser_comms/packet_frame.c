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

bool checkCRC(packet_t* received_packet) {

	crc_scheme   check = LIQUID_CRC_32; // error-detection scheme
	uint32_t calcd_crc = (uint32_t)crc_generate_key(check, received_packet->data, PACKET_DATA_LENGTH_BYTES);

	if (received_packet->crc != calcd_crc) {
		return 1;
	}
	else {
		return 0;
	}
	
}


bool getCRC(packet_t * packet) {//Note that deviating from crc32 size would require code to be changed across project. Not adaptable currently

	crc_scheme   check = LIQUID_CRC_32; // error-detection scheme
	packet->crc = (uint32_t)crc_generate_key(check, packet->data, PACKET_DATA_LENGTH_BYTES);

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

//return = 
bool syncFrameUsingMLSPreamble() {



}


//Output format = byte array of bits to be transmitted where lowest index should be transmitted first
//ENSURE *frame and *packet are freed after use!
bool assembleFrame(uint8_t * frame, , unsigned int * frame_length, uint8_t * packet, unsigned int packet_length) {//Basically just adds preamble
	
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

	frame_length = alternating_preamble_length + mls_preamble_length + packet_length;

	return 0;
}


bool disassembleFrame(uint8_t* frame, , unsigned int* frame_length, uint8_t* packet, unsigned int packet_length) {//ADD MALLOC
	//Just checks MLS autocorrelation, determines start of packet, and strips preamble

	//Placeholder (REPLACE):
	for (int i = 0; i < PACKET_DATA_LENGTH_BYTES; i++) {
		packet[i] = frame[(&frame_length - PACKET_DATA_LENGTH_BYTES) + i];//1 offset due to indexing?
	}

	return 0;
}

//Returns pointer to malloc'd packet that must be free'd later. Also returns packet_length
//Macro usage probably okay because we ddont expect to change any of those values dynamically
bool assemblePacket(packet_t *packet_data, uint8_t *packet, unsigned int *packet_length) {//much much easier if fields n*8 bits?
	

	packet_length = (sizeof(packet_t) - 1) + PACKET_DATA_LENGTH_BYTES;

	packet = (uint8_t*)malloc(packet_length* sizeof(uint8_t));

	packet[0] = packet_data->fec_scheme;

	for (int i = 0; i < NUM_PACKETS_LENGTH_BYTES; i++) {
		packet[1 + i] = 0xFF & (packet_data->total_num_packets >> 8*(NUM_PACKETS_LENGTH_BYTES) - 1 - i));//needs mask? Not sure
	}
	
	for (int i = 0; i < NUM_PACKETS_LENGTH_BYTES; i++) {
		packet[1 + NUM_PACKETS_LENGTH_BYTES + i] = 0xFF & (packet_data->current_packet_num >> 8 * (NUM_PACKETS_LENGTH_BYTES)-1 - i));//needs mask? Not sure
	}

	for (int i = 0; i < PACKET_DATA_LENGTH_BYTES; i++) {
		packet[1 + 2 * NUM_PACKETS_LENGTH_BYTES + i] = (packet_data->data)[i];
	}

	for (int i = 0; i < CRC_DATA_LENGTH_BYTES; i++) {
		packet[1 + 2*NUM_PACKETS_LENGTH_BYTES + PACKET_DATA_LENGTH_BYTES + i] = 0xFF & (packet_data->crc >> 8 * (CRC_DATA_LENGTH_BYTES)-1 - i));//needs mask? Not sure
	}

	return 0;
}

bool disassemblePacket(packet_t* packet_data, uint8_t* packet, unsigned int* packet_length) {//ADD MALLOC

	uint32_t temp_crc = 0;
	uint32_t mask = 0;
	for (int i = 0; i < CRC_DATA_LENGTH_BYTES; i++) {
		mask = 0;
		mask = 0xFF << 8*i;
		temp_crc = temp_crc | (mask & packet[&packet_length-1 -i]);
	}
	packet_data->crc = temp_crc;

	//TODO BELOW

	for (int i = 0; i < NUM_PACKETS_LENGTH_BYTES; i++) {
		packet[1 + i] = 0xFF & (packet_data->total_num_packets >> 8 * (NUM_PACKETS_LENGTH_BYTES)-1 - i));//needs mask? Not sure
	}

	for (int i = 0; i < NUM_PACKETS_LENGTH_BYTES; i++) {
		packet[1 + NUM_PACKETS_LENGTH_BYTES + i] = 0xFF & (packet_data->current_packet_num >> 8 * (NUM_PACKETS_LENGTH_BYTES)-1 - i));//needs mask? Not sure
	}

	for (int i = 0; i < PACKET_DATA_LENGTH_BYTES; i++) {
		packet[1 + 2 * NUM_PACKETS_LENGTH_BYTES + i] = (packet_data->data)[i];
	}

	return 0;
}

bool fragmentDataBufferIntoFrames(uint8_t * input, uint8_t * output) {
	//TODO wait until later
	//This is where the packet_t->data should get malloc'd, I think.
}

bool assembleFramesIntoDataBuffer(uint8_t* input, uint8_t* output) {
	//TODO wait until later
}