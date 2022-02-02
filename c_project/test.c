#include "packet_frame.h"
#include "channel.h"
#include "laser_comms.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>


//Returns pointer to a randomized uint8_t array of length packet_data_length_with_fec
uint8_t * generateRandPacket(void) {
	uint8_t* data = (uint8_t*)malloc(packet_data_length_with_fec);
	for (int i = 0; i < PACKET_DATA_LENGTH_NO_FEC; i++) {
		data[i] = rand() & 0xff;
	}
	return data;
}


//1 (maybe) lsb bit error every 8 bytes
// void applyChannel(uint8_t * input_data, unsigned int input_data_length) {
// 	for (unsigned int i = 0; i < input_data_length; i++) {
// 		if (i % 8 == 0) {
// 			input_data[i] = (input_data[i] >> 1) << 1;//only flips bit if lsb was a 1
// 		}
// 	}
// }

//
void getFECDataLengths(void) {

	// create arrays
	packet_data_length_with_fec = fec_get_enc_msg_length(FEC_TYPE, PACKET_DATA_LENGTH_NO_FEC);
}

//Current full-data-pipeline test
bool fullSendTest(void) {
	
	printf("test");
	say_hello();
	//Init all the things. 
	//Array pointers are init'd to NULL as they are malloc'd and re-assigned within the packetizing functions
	packet_t packet_data;//malloc this?
	packet_data.selected_fec_scheme = LDPC;
	uint8_t* packet_vector = NULL;
	unsigned int packet_length;
	unsigned int frame_length;
	uint8_t* frame_vector = NULL;//malloc? yes

	packet_data.data = generateRandPacket();

	printf("Original Data:\n");
	for (unsigned int i = 0; i < PACKET_DATA_LENGTH_NO_FEC; i++) {
		printf("%d", packet_data.data[i]);
	}
	printf("\n\n");

	getCRC(&packet_data);

	//Commented out functions are not yet implemented, so cannot be tested
	applyFEC(packet_data.data);

	assemblePacket(&packet_data, &packet_vector, &packet_length);
	applyInterleaving(packet_vector, packet_length);
	
	assembleFrame(&frame_vector, &frame_length, packet_vector, packet_length);
	unsigned int preamble_length = frame_length - packet_length;

	printf("Orignal frame:\n");
	for (unsigned int i = 0; i < frame_length; i++) {
		printf("%d", frame_vector[i]);
	}
	printf("\n\n");

	applyScrambling(&frame_vector, frame_length, preamble_length);

	printf("Post-scramble:\n");
	for (unsigned int i = 0; i < frame_length; i++) {
		printf("%d", frame_vector[i]);
	}
	printf("\n\n");

	//Comment or un-comment, depending on the test you are trying to run
	//TODO consider turning into macro functionality in future
	//applyChannel(frame_vector, frame_length);

	//printf("New frame (after going through channel):\n");
	//for (unsigned int i = 0; i < frame_length; i++) {
	//	printf("%d", frame_vector[i]);
	//}
	//printf("\n\n");

	//Init "rx" stuff
	packet_t rxpacket_data;//malloc this?
	rxpacket_data.data = (uint8_t*)malloc(packet_data_length_with_fec);
	uint8_t* rxpacket_vector = NULL;
	unsigned int rxpacket_length = 0;

	removeScrambling(&frame_vector, frame_length, preamble_length);

	disassembleFrame(frame_vector, &rxpacket_vector, frame_length);
	removeInterleaving(rxpacket_vector, packet_length);

	disassemblePacket(&rxpacket_data, rxpacket_vector, packet_length);
	removeFEC(rxpacket_data.data);

	printf("Received Data:\n");
	for (unsigned int i = 0; i < PACKET_DATA_LENGTH_NO_FEC; i++) {
		printf("%d", rxpacket_data.data[i]);
	}
	printf("\n\n");

	if (checkCRC(&rxpacket_data)) {
		printf("CRC Doesn't Match!\n\n");
	}
	else {
		printf("CRC Matches!\n\n");
	}


	//Must free everything malloc'd
	free(packet_data.data);
	free(rxpacket_data.data);
	free(rxpacket_vector);
	free(packet_vector);
	free(frame_vector);
	return 0;
}


//Reads from file. Should write file with identical contents if everything goes well!
//Consider adding code to check that file contents match...
bool fullSendTest_FileIO(void){

	int input_file_len = 0;
	uint8_t * inputdata;
	readFiletoArray("test_input.txt", inputdata, &input_file_len);
	int tx_frames_len = 0;
	uint8_t * tx_frames;
	fragmentDataBufferIntoFrames(inputdata, input_file_len, tx_frames, &tx_frames_len);

	//Consider adding channel and/or all analog stuff here...

	int output_file_len = 0;
	uint8_t * rxdata;
	assembleFramesIntoDataBuffer(tx_frames, tx_frames_len, rxdata, &output_file_len);
	writeArraytoFile("test_output.txt", rxdata, output_file_len);
}