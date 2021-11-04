#include "packet_frame.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>


//Returns pointer to a randomized uint8_t array of length PACKET_DATA_LENGTH_BYTES
uint8_t * generateRandPacket(void) {
	uint8_t* data = (uint8_t*)malloc(PACKET_DATA_LENGTH_BYTES);
	for (int i = 0; i < PACKET_DATA_LENGTH_BYTES; i++) {
		data[i] = rand() & 0xff;
	}
	return data;
}


//For now, just randomly corrupts input_data array of specified length
applyChannel(uint8_t * input_data, unsigned int input_data_length) {
	for (unsigned int i = 0; i < input_data_length; i++) {
		if (i % 8 == 0) {
			input_data[i] = (rand() & 0xff) & input_data[i];
		}
	}
}

//Current full-data-pipeline test
bool fullSendTest(void) {

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
	for (unsigned int i = 0; i < PACKET_DATA_LENGTH_BYTES; i++) {
		printf("%d", packet_data.data[i]);
	}
	printf("\n\n");

	getCRC(&packet_data);


	//Commented out functions are not yet implemented, so cannot be tested
	//applyFEC(packet)
	assemblePacket(&packet_data, &packet_vector, &packet_length);
	applyInterleaving(packet_vector, packet_length);
	//applyScrambling()

	assembleFrame(&frame_vector, &frame_length, packet_vector, packet_length);

	printf("Orignal frame:\n");
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
	rxpacket_data.data = (uint8_t*)malloc(PACKET_DATA_LENGTH_BYTES);
	uint8_t* rxpacket_vector = NULL;
	unsigned int rxpacket_length = 0;

	disassembleFrame(frame_vector, &rxpacket_vector, frame_length);
	removeInterleaving(rxpacket_vector, packet_length);

	disassemblePacket(&rxpacket_data, rxpacket_vector, packet_length);

	printf("Received Data:\n");
	for (unsigned int i = 0; i < PACKET_DATA_LENGTH_BYTES; i++) {
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



