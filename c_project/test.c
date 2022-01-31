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
	packet_data_length_with_fec = fec_get_enc_msg_length(FEC_TYPE, PACKET_DATA_LENGTH_NO_FEC); // need to make this include different LDPCs

	// for now
	packet_data_length_with_fec = (int) CODEWRD_L / 8;
}

//Current full-data-pipeline test
bool fullSendTest(void) {
	
	printf("test\n");
	
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
		printf("%d,", packet_data.data[i]);
	}
	printf("\n\n");

	getCRC(&packet_data);
	printf("Tx CRC: %d \n", packet_data.crc);

	//Commented out functions are not yet implemented, so cannot be tested
	applyLDPC(packet_data.data);
	//applyFEC(packet_data.data);
	printf("Encoded Data:\n");
	for (unsigned int i = 0; i < packet_data_length_with_fec; i++) {
		printf("%d,", packet_data.data[i]);
	}
	printf("\n\n");

	assemblePacket(&packet_data, &packet_vector, &packet_length);
	applyInterleaving(packet_vector, packet_length);
	
	assembleFrame(&frame_vector, &frame_length, packet_vector, packet_length);
	unsigned int preamble_length = frame_length - packet_length;

	// printf("Orignal frame:\n");
	// for (unsigned int i = 0; i < frame_length; i++) {
	// 	printf("%d", frame_vector[i]);
	// }
	// printf("\n\n");

	applyScrambling(&frame_vector, frame_length, preamble_length);

	// printf("Post-scramble:\n");
	// for (unsigned int i = 0; i < frame_length; i++) {
	// 	printf("%d", frame_vector[i]);
	// }
	// printf("\n\n");

	//Comment or un-comment, depending on the test you are trying to run
	//TODO consider turning into macro functionality in future
	applyChannel(frame_vector, frame_length);
	//applyBitFlips(frame_vector, frame_length); // fucks up the CRC unless CRC is also encoded and decoded

	printf("New frame (after going through channel):\n");
	for (unsigned int i = 0; i < frame_length; i++) {
		printf("%d", frame_vector[i]);
	}
	printf("\n\n");

	//Init "rx" stuff
	packet_t rxpacket_data;//malloc this?
	rxpacket_data.data = (uint8_t*)malloc(packet_data_length_with_fec);
	uint8_t* rxpacket_vector = NULL;
	unsigned int rxpacket_length = 0;

	removeScrambling(&frame_vector, frame_length, preamble_length);

	disassembleFrame(frame_vector, &rxpacket_vector, frame_length);
	removeInterleaving(rxpacket_vector, packet_length);

	disassemblePacket(&rxpacket_data, rxpacket_vector, packet_length);
	// printf("Pre decoding:\n");
	// for (unsigned int i = 0; i < packet_data_length_with_fec; i++) {
	// 	printf("%d,", rxpacket_data.data[i]);
	// }
	// printf("\n\n");
	//removeFEC(rxpacket_data.data);
	printf("Data to be decoded:\n");
	for (unsigned int i = 0; i < PACKET_DATA_LENGTH_NO_FEC; i++) {
		printf("%d,", (rxpacket_data.data[i]));// - packet_data.data[i]));
	}
	printf("\n\n");

	
	decodeLDPC(rxpacket_data.data);



	printf("Difference between Received Corrected Data and Original Data:\n");
	for (unsigned int i = 0; i < PACKET_DATA_LENGTH_NO_FEC; i++) {
		//printf("%d,", rxpacket_data.data[i]);
		printf("%d,", (rxpacket_data.data[i] - packet_data.data[i]));
	}
	printf("\n\n");

	
	if (checkCRC(&rxpacket_data)) {
		printf("CRC Doesn't Match! (need to fix)\n");
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



