#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

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
	uint8_t* msg_enc = (uint8_t*)malloc(packet_data_length_with_fec_bytes);

	// encode message
	fec_encode(q, n, input, msg_enc);//fix lengths

	for (int i = 0; i < packet_data_length_with_fec_bytes; i++) {
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
bool applyScrambling(uint8_t ** input, unsigned int input_length) {
	//not necessary
	//*frame = (uint8_t*)malloc((*frame_length) * sizeof(uint8_t));

    scramble_data(*input, input_length);

	return 0;
}

//Removes LiquidDSP's scrambling to input buffer of input_length
bool removeScrambling(uint8_t ** input, unsigned int input_length) {

    unscramble_data(*input, input_length);

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
	unsigned int m = 9;   // shift register length, n=2^m - 1
	unsigned int repititions = 1;	//Number of MLS repititions in preamble
	unsigned int mls_preamble_length_bits = (pow(2,m) - 1)*repititions; // preamble length

	// create and initialize m-sequence
	msequence ms = msequence_create_genpoly(LIQUID_MSEQUENCE_GENPOLY_M9);//Fix these struct name definitions... Liquid maybe borked?
	//msequence_print(ms);
	unsigned int n = msequence_get_length(ms);

	// create and initialize first binary sequence on m-sequence
	bsequence mls = bsequence_create(n);
	bsequence_init_msequence(mls, ms);

	//Write bits to array. Note bsequence indexing starts on "right"
	uint8_t * bitbuffer = (uint8_t*)malloc(sizeof(uint8_t)*(mls_preamble_length_bits));
	for (int i = 0; i < repititions; i++){
		for (unsigned int j = 0; j < n; j++) {
			bitbuffer[n*i + j] = bsequence_index(mls, j);
		}
	}
	// printf("Generated MLS bits\n");
	// for(int i = 0; i < 50; i++){
	// 	printf("%d ", bitbuffer[i]);
	// }

	//Write bit array to byte array
	//TODO move this to a utils file for other use
	*mls_preamble_length = mls_preamble_length_bits/8 + 1;
	*mls_preamble = (uint8_t*)malloc(sizeof(uint8_t)*(*mls_preamble_length));
	uint8_t temp = 0;
	for (int i = 0; i < (*mls_preamble_length); i++){
		for (unsigned int j = 0; j < 8; j++) {
			temp = (temp << 1) | bitbuffer[i*8 + j];
		}
		(*mls_preamble)[i] = temp;
	}

	// clean up memory
	free(bitbuffer);
	bsequence_destroy(mls);
	msequence_destroy(ms);

	return 0;
}

////Assembles data from "packet" input buffer into output buffer "frame" of length "frame_length"
//NOTE: Ensure *frame and *packet are freed after use!
bool assembleFrame(uint8_t ** frame, unsigned int * frame_length, uint8_t * packet, unsigned int packet_length) {//Basically just adds preamble

	//Add MLS preamble 
	unsigned int mls_preamble_length = 0;
	uint8_t* mls_preamble = NULL;

	printf("get MLS preamble \n");
	getMaximumLengthSequencePreamble(&mls_preamble, &mls_preamble_length);
	printf("MLS preamble received \n");

	int stuffing_length = 8;//stuffing needed for sync to work properly
	*frame_length = mls_preamble_length + packet_length + stuffing_length;
	*frame = (uint8_t*)malloc((*frame_length) * sizeof(uint8_t));

	//Add rest of preamble to start of frame
	//TODO change to MLS preamble
	for (unsigned int i = 0; i < mls_preamble_length; i++) {
		(*frame)[i] = mls_preamble[i];
	}
	free(mls_preamble);
	//Add stuffing to frame
	for (unsigned int i = 0; i < stuffing_length; i++) {
		(*frame)[mls_preamble_length + i] = 0;
	}

	//Add packet data to frame
	for (unsigned int i = 0; i < packet_length; i++) {
		(*frame)[mls_preamble_length + i] = packet[i];
	}

	return 0;
}

//Takes byte buffer containing entire frame of length frame_length and extracts the packet
//data field to the "packet" argument
//TODO: add MLS autocorrelation check (and phase sync?) to determine the start of packet
//TODO: consider how this function may need to be restructured once phase and frame sync 
//is added, since input will be in samples instead of bits
bool disassembleFrame(uint8_t* frame, uint8_t** packet, unsigned int frame_length) {

	unsigned int packet_length = 1 + CRC_DATA_LENGTH_BYTES + packet_data_length_with_fec_bytes + (2 * NUM_PACKETS_LENGTH_BYTES);

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

	*packet_length = packet_data_length_with_fec_bytes;
	*packet = (uint8_t*)malloc((*packet_length)* sizeof(uint8_t));

	//(*packet)[0] = packet_data->selected_fec_scheme;//commented out for demo

	for (int i = 0; i < NUM_PACKETS_LENGTH_BYTES; i++) {
		(*packet)[i] = 0xFF & (packet_data->total_num_packets >> 8*(NUM_PACKETS_LENGTH_BYTES - 1 - i));
	}
	
	for (int i = 0; i < NUM_PACKETS_LENGTH_BYTES; i++) {
		(*packet)[NUM_PACKETS_LENGTH_BYTES + i] = 0xFF & (packet_data->current_packet_num >> 8 * (NUM_PACKETS_LENGTH_BYTES - 1 - i));
	}

	for (int i = 0; i < PACKET_DATA_LENGTH_NO_FEC; i++) {
		(*packet)[2 * NUM_PACKETS_LENGTH_BYTES + i] = (packet_data->data)[i];
	}

	for (int i = 0; i < CRC_DATA_LENGTH_BYTES; i++) {
		(*packet)[2 * NUM_PACKETS_LENGTH_BYTES + PACKET_DATA_LENGTH_NO_FEC + i] = 0xFF & (packet_data->crc >> 8 * (CRC_DATA_LENGTH_BYTES -1 - i));
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
		temp_32 = temp_32 | (mask32 & ((uint32_t)packet[2*NUM_PACKETS_LENGTH_BYTES + PACKET_DATA_LENGTH_NO_FEC + CRC_DATA_LENGTH_BYTES - 1 - i] << 8*i));
	}
	packet_data->crc = temp_32;

	for (int i = 0; i < NUM_PACKETS_LENGTH_BYTES; i++) {

		mask16 = 0;
		mask16 = 0xFF << 8 * (NUM_PACKETS_LENGTH_BYTES -1 -i);
		temp_16 = temp_16 | (mask16 & ((uint16_t)packet[i] << 8 * (NUM_PACKETS_LENGTH_BYTES -1 -i)));
	}
	packet_data->total_num_packets = temp_16;
	temp_16 = 0;

	for (int i = 0; i < NUM_PACKETS_LENGTH_BYTES; i++) {

		mask16 = 0;
		mask16 = 0xFF << 8 * (NUM_PACKETS_LENGTH_BYTES -1 - i);
		temp_16 = temp_16 | (mask16 & ((uint16_t)packet[NUM_PACKETS_LENGTH_BYTES + i  - 1] << 8 * (NUM_PACKETS_LENGTH_BYTES -1 -i)));
	}
	packet_data->current_packet_num = temp_16;

	for (int i = 0; i < PACKET_DATA_LENGTH_NO_FEC; i++) {
		(packet_data->data)[i] = packet[2*NUM_PACKETS_LENGTH_BYTES + i];
	}

	return 0;
}


//TODO
//Takes "input" data buffer of length "input_length", fragments it into packets, turns those packets into frames, 
//and assembles frames into single "output" buffer of "output_length" that can be easily transmitted
bool fragmentDataBufferIntoFrames(uint8_t * input, unsigned int input_length, uint8_t * output, unsigned int output_length) {
	return 0;
}

//TODO
//Inverse of fragmentDataBufferIntoFrames.
//Takes "input" data buffer of length "input_length", strips its frame and packet encapsulation, 
//and assembles data fields of frames into a single "output" buffer of length "output_length" that can be easily saved
bool assembleFramesIntoDataBuffer(uint8_t* input, unsigned int input_length, uint8_t* output, unsigned int output_length) {
	

	return 0;
}


//TODO
//Takes "input" data buffer of length "input_length", fragments it into packets, turns those packets into frames, 
//and assembles frames into single "output" buffer of "output_length" that can be easily transmitted
// bool fragmentDataBufferIntoFrames(uint8_t * input, unsigned int input_length, uint8_t * output, unsigned int * output_length) {

// 	int numpackets = input_length/packet_data_length_no_fec + 1;
// 	*output_length = numpackets*frame_length_bytes;
// 	uint8_t * output = (uint8_t *)malloc(*output_length*sizeof(uint8_t));
// 	packet_t temp_packet;
// 	temp_packet.data = (uint8_t*)malloc(packet_data_length_no_fec);
// 	int current_index = 0;

// 	//for each frame:
// 	for (int i = 0; i < numpackets; i++){
// 		//populate all struct fields
// 		temp_packet.total_num_packets = numpackets;
// 		temp_packet.current_packet_num = i+1;

// 		for(int j = 0; j < packet_data_length_no_fec; j++){
// 			if(current_index > input_length){
// 				temp_packet.data[j] = 0;//stuffing rear-end with zeroes
// 			}
// 			temp_packet.data[j] = input[current_index];
// 			current_index++;
// 		}

// 		getCRC(&temp_packet);

// 		//TODO: call all the tx stuff in fullsend testing order and put data into *output
// 		//NOT COMPLETING UNTIL FULLSEND TEST HAS UNDERGONE REFACTOR

// 	}

// 	return 0;
// }

// //TODO
// //Inverse of fragmentDataBufferIntoFrames.
// //Takes "input" data buffer of length "input_length", strips its frame and packet encapsulation, 
// //and assembles data fields of frames into a single "output" buffer of length "output_length" that can be easily saved
// bool assembleFramesIntoDataBuffer(uint8_t* input, unsigned int input_length, uint8_t* output, unsigned int * output_length) {
	
// 	//do reverse of above function

// 	int numpackets = input_length/frame_length_bytes;
// 	*output_length = numpackets*packet_data_length_no_fec;
// 	uint8_t * output = (uint8_t *)malloc(*output_length*sizeof(uint8_t));
// 	packet_t temp_packet;
// 	temp_packet.data = (uint8_t*)malloc(packet_data_length_no_fec);
// 	int current_index = 0;
// 	int prev_frameno = 0;
// 	int cur_frameno = 0;

// 	//for each frame:
// 	for (int i = 0; i < numpackets; i++){

// 		//TODO: call all the rx stuff in fullsend testing order
// 		//The todo code's output will be 
// 		//NOT COMPLETING UNTIL FULLSEND TEST HAS UNDERGONE REFACTOR

// 		prev_frameno = cur_frameno;
// 		cur_frameno = temp_packet.current_packet_num;

// 		if(cur_frameno > numpackets){
// 			printf("Frame number indicator %d larger than numpackets in transmission\n", cur_frameno);
// 			while(1);//TODO integrate better error handling
// 		}
// 		if(cur_frameno <= prev_frameno){
// 			printf("Frame sequence error at frame %d\n", cur_frameno);
// 			while(1);//TODO integrate better error handling
// 		}
// 		if(current_index > output_length){
// 			printf("Output buffer overflow");
// 			while(1);//TODO integrate better error handling
// 		}
// 		for(int j = 0; j < packet_data_length_no_fec; j++){
// 			output[current_index] = temp_packet.data[j];
// 			current_index++;
// 		}
// 	}

// 	return 0;
// }