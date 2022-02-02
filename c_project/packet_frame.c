
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "packet_frame.h"
//#include <complex>
#include <liquid/liquid.h>


//Move these into separate file? Considering having "utils" file containing smaller functions, and keeping the higher-level stuff here

//Unless otherwise indicated, functions return 0 upon success and 1 upon failure

//TODO
//Applies FEC to input buffer. Type of FEC depends on type selected.
bool applyFEC(uint8_t* input) {
	
	// options
	unsigned int n = PACKET_DATA_LENGTH_NO_FEC;
	fec_scheme fs = FEC_TYPE;   // error-correcting scheme

	// create object
	fec q = fec_create(fs, NULL);
	uint8_t* msg_enc = (uint8_t*)malloc(packet_data_length_with_fec);

	// encode message
	fec_encode(q, n, input, msg_enc);//fix lengths

	for (int i = 0; i < packet_data_length_with_fec; i++) {
		input[i] = msg_enc[i];
	}

	free(msg_enc);

	fec_destroy(q);

	return 0;
}

//TODO
//Removes FEC from the input buffer. Type of FEC depends on type selected.
bool removeFEC(uint8_t * input) {

	unsigned int n = PACKET_DATA_LENGTH_NO_FEC;
	fec_scheme fs = FEC_TYPE;   // error-correcting scheme

	// create object
	fec q = fec_create(fs, NULL);
	
	unsigned char msg_enc[PACKET_DATA_LENGTH_NO_FEC];

	// decode message
	fec_decode(q, n, input, msg_enc);//fix lengths

	for (int i = 0; i < PACKET_DATA_LENGTH_NO_FEC; i++) {
		input[i] = msg_enc[i];
	}

	fec_destroy(q);

	return 0;
}

//Calculates the crc of the "data" field of the input packet and compares it against the "crc" field
//Returns 1 if crcs not equal, 0 if equal
bool checkCRC(packet_t* received_packet) {

	crc_scheme   check = LIQUID_CRC_32; // error-detection scheme
	uint32_t calcd_crc = (uint32_t)crc_generate_key(check, received_packet->data, PACKET_DATA_LENGTH_NO_FEC);

	if (received_packet->crc != calcd_crc) {
		return 1;
	}
	else {
		return 0;
	}
	
}

//Calculates the crc of the "data" field of the input packet and assigns it to the "crc" field
bool getCRC(packet_t * packet) {//Note that deviating from crc32 size would require code to be changed across project. Not adaptable currently

	crc_scheme   check = LIQUID_CRC_32; // error-detection scheme
	packet->crc = (uint32_t)crc_generate_key(check, packet->data, PACKET_DATA_LENGTH_NO_FEC);

	return 0;
}

//Applies LiquidDSP's scrambling to input buffer of input_length
bool applyScrambling(uint8_t ** input, unsigned int input_length, unsigned int preamble_length) {
	//not necessary
	//*frame = (uint8_t*)malloc((*frame_length) * sizeof(uint8_t));

    scramble_data((*input) + preamble_length, (input_length-preamble_length));

	return 0;
}

//Removes LiquidDSP's scrambling to input buffer of input_length
bool removeScrambling(uint8_t ** input, unsigned int input_length, unsigned int preamble_length) {

    unscramble_data((*input) + preamble_length, (input_length-preamble_length));

	return 0;
}

//TODO
//Applies LiquidDSP's interleaving to input buffer of input_length
bool applyInterleaving(uint8_t* input, unsigned int input_length) {

	// create the interleaver
	interleaver q = interleaver_create(input_length);
	interleaver_set_depth(q, 4);//This should be fine, right?

	// interleave the data
	interleaver_encode(q, input, input);

	// destroy the interleaver object
	interleaver_destroy(q);//not sure if a new interleaver object can be created on the tx and rx end or not... Probably fine!

	return 0;
}

//TODO
//Removes LiquidDSP's interleaving to input buffer of input_length
bool removeInterleaving(uint8_t* input, unsigned int input_length) {

	// create the interleaver
	interleaver q = interleaver_create(input_length);
	interleaver_set_depth(q, 4);//This should be fine, right?

	// de-interleave the data
	interleaver_decode(q, input, input);

	// destroy the interleaver object
	interleaver_destroy(q);//not sure if a new interleaver object can be created on the tx and rx end or not... Probably fine!

	return 0;
}


//Generates MLS preamble based on parameters within function and assigns it, along with its length to the arguments
bool getMaximumLengthSequencePreamble(uint8_t ** mls_preamble, unsigned int *mls_preamble_length) {

	//options
	//TODO: Pick a good value for m
	unsigned int m = 10;   // shift register length, n=2^m - 1
	unsigned int repititions = 5;	//Number of MLS repititions in preamble

	// create and initialize m-sequence
	msequence ms = msequence_create_default(m);//Fix these struct name definitions... Liquid maybe borked?
	//msequence_print(ms);
	unsigned int n = msequence_get_length(ms);

	// create and initialize first binary sequence on m-sequence
	bsequence mls = bsequence_create(n);
	bsequence_init_msequence(mls, ms);


	//Repeat specified number of times and move to new array
	// *mls_preamble = (uint8_t*)malloc(((mls->s_len))* repititions);
	// for (unsigned int i = 0; i < repititions; i++) {
	// 	for (unsigned int j = 0; j < (mls->s_len); j++) {
	// 		(*mls_preamble)[i*(mls->s_len) + j] = (uint8_t)((mls->s)[j]);
	// 	}
	// }
	// *mls_preamble_length = repititions*(mls->s_len);

	// clean up memory
	bsequence_destroy(mls);

	return 0;
}

//TODO
//Checks the autocorrelation of frame input buffer and determines the indices of the first bit of the packet field.
//Note: byte_ and bit_index are indexed from zero (0).
bool syncFrameUsingMLSPreamble(uint8_t *input, unsigned int * byte_index, unsigned int * bit_index) {

	return 0;
}


////Assembles data from "packet" input buffer into output buffer "frame" of length "frame_length"
//NOTE: Ensure *frame and *packet are freed after use!
bool assembleFrame(uint8_t ** frame, unsigned int * frame_length, uint8_t * packet, unsigned int packet_length) {//Basically just adds preamble

	//Add MLS preamble 
	unsigned int alternating_preamble_length = 2;
	unsigned int mls_preamble_length = 0;
	uint8_t* mls_preamble = NULL;

	getMaximumLengthSequencePreamble(&mls_preamble, &mls_preamble_length);

	*frame_length = alternating_preamble_length + mls_preamble_length + packet_length;
	*frame = (uint8_t*)malloc((*frame_length) * sizeof(uint8_t));

	//Add two 10101010 bytes to the start of frame
	//TODO remove?
	for (unsigned int i = 0; i < alternating_preamble_length; i++) {
		(*frame)[i] = 0b10101010;
	}

	//Add rest of preamble to start of frame
	//TODO change to MLS preamble
	for (unsigned int i = 0; i < mls_preamble_length; i++) {
		(*frame)[alternating_preamble_length + i] = mls_preamble[i];
	}
	free(mls_preamble);

	//Add packet data to frame
	for (unsigned int i = 0; i < packet_length; i++) {
		(*frame)[alternating_preamble_length + mls_preamble_length + i] = packet[i];
	}

	return 0;
}

//Takes byte buffer containing entire frame of length frame_length and extracts the packet
//data field to the "packet" argument
//TODO: add MLS autocorrelation check (and phase sync?) to determine the start of packet
//TODO: consider how this function may need to be restructured once phase and frame sync 
//is added, since input will be in samples instead of bits
bool disassembleFrame(uint8_t* frame, uint8_t** packet, unsigned int frame_length) {

	unsigned int packet_length = 1 + CRC_DATA_LENGTH_BYTES + packet_data_length_with_fec + (2 * NUM_PACKETS_LENGTH_BYTES);

	*packet = (uint8_t*)malloc((sizeof(uint8_t))*packet_length);

	//Placeholder to strip preamble(REPLACE WITH PACKET SYNC CODE EVENTUALLY):
	for (int i = 0; i < packet_length; i++) {
		(*packet)[i] = frame[frame_length - packet_length + i];
	}

	return 0;
}

//Assembles data from packet_data into output buffer "packet" of length "packet_length"
//NOTE: Ensure *packet is freed after use!
bool assemblePacket(packet_t *packet_data, uint8_t **packet, unsigned int *packet_length) {
	
	//Macro usage probably okay because we ddont expect to change any of those values dynamically?

	*packet_length = 1 + CRC_DATA_LENGTH_BYTES + 2*NUM_PACKETS_LENGTH_BYTES + packet_data_length_with_fec;
	*packet = (uint8_t*)malloc((*packet_length)* sizeof(uint8_t));

	(*packet)[0] = packet_data->selected_fec_scheme;

	for (int i = 0; i < NUM_PACKETS_LENGTH_BYTES; i++) {
		(*packet)[1 + i] = 0xFF & (packet_data->total_num_packets >> 8*(NUM_PACKETS_LENGTH_BYTES - 1 - i));
	}
	
	for (int i = 0; i < NUM_PACKETS_LENGTH_BYTES; i++) {
		(*packet)[1 + NUM_PACKETS_LENGTH_BYTES + i] = 0xFF & (packet_data->current_packet_num >> 8 * (NUM_PACKETS_LENGTH_BYTES - 1 - i));
	}

	for (int i = 0; i < packet_data_length_with_fec; i++) {
		(*packet)[1 + 2 * NUM_PACKETS_LENGTH_BYTES + i] = (packet_data->data)[i];
	}

	for (int i = 0; i < CRC_DATA_LENGTH_BYTES; i++) {
		(*packet)[1 + 2*NUM_PACKETS_LENGTH_BYTES + packet_data_length_with_fec + i] = 0xFF & (packet_data->crc >> 8 * (CRC_DATA_LENGTH_BYTES -1 - i));
	}

	return 0;
}

//Takes byte buffer containing entire packet of length packet_length and parses it, 
//assigning its fields to the packet_t struct type appropriately
bool disassemblePacket(packet_t* packet_data, uint8_t* packet, unsigned int packet_length) {

	uint32_t temp_32 = 0;
	uint16_t temp_16 = 0;
	uint32_t mask32 = 0;
	uint16_t mask16 = 0;

	//Parse crc bits of input packet, assemble them into single variable, and save to packet_t "crc" field
	//The rest of function does the same thing for other fields
	for (int i = 0; i < CRC_DATA_LENGTH_BYTES; i++) {
		mask32 = 0;
		mask32 = 0xFF << 8*i;
		temp_32 = temp_32 | (mask32 & ((uint32_t)packet[packet_length - 1 - i] << 8*i));
	}
	packet_data->crc = temp_32;

	for (int i = 0; i < NUM_PACKETS_LENGTH_BYTES; i++) {

		mask16 = 0;
		mask16 = 0xFF << 8 * (NUM_PACKETS_LENGTH_BYTES -i);
		temp_16 = temp_16 | (mask16 & ((uint16_t)packet[i - 1] << 8 * i));
	}
	packet_data->total_num_packets = temp_16;

	for (int i = 0; i < NUM_PACKETS_LENGTH_BYTES; i++) {

		mask16 = 0;
		mask16 = 0xFF << 8 * (NUM_PACKETS_LENGTH_BYTES - i);
		temp_16 = temp_16 | (mask16 & ((uint16_t)packet[NUM_PACKETS_LENGTH_BYTES + i  - 1] << 8*i));
	}
	packet_data->current_packet_num = temp_16;

	for (int i = 0; i < packet_data_length_with_fec; i++) {
		(packet_data->data)[i] = packet[2*NUM_PACKETS_LENGTH_BYTES +1 + i];
	}

	return 0;
}