#include "packet_frame.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

uint8_t * generateRandPacket(void) {
	uint8_t* data = (uint8_t*)malloc(PACKET_DATA_LENGTH_BYTES);
	for (int i = 0; i < PACKET_DATA_LENGTH_BYTES; i++) {
		data[i] = rand() & 0xff;
	}
	return data;
}


//big disruption
applyChannel(uint8_t * input_data, unsigned int input_data_length) {
	for (unsigned int i = 0; i < input_data_length; i++) {
		if (i % 8 == 0) {
			input_data[i] = (rand() & 0xff) & input_data[i];
		}
	}
}

bool fullSendTest(void) {

	//Is the following order correct?

	packet_t packet_data;//malloc this?
	packet_data.selected_fec_scheme = LDPC;
	uint8_t* packet_vector = NULL;
	unsigned int packet_length;
	unsigned int frame_length;
	uint8_t* frame_vector = NULL;//malloc? yes

	packet_data.data = generateRandPacket();
	getCRC(&packet_data);

	printf("Original Data:\n");
	for (unsigned int i = 0; i < PACKET_DATA_LENGTH_BYTES; i++) {
		printf("%d", packet_data.data[i]);
	}
	printf("\n\n");
	//applyFEC(packet)//does this act on the entire packet? Assuming no.
	//applyInterleaving()
	assemblePacket(&packet_data, &packet_vector, &packet_length);//not sure if this should be before or after interleaving
	free(packet_data.data);
	//applyScrambling()

	assembleFrame(&frame_vector, &frame_length, packet_vector, packet_length);
	printf("Orignal frame:\n");
	for (unsigned int i = 0; i < frame_length; i++) {
		printf("%d", frame_vector[i]);
	}
	printf("\n\n");
	
	//applyChannel(frame_vector, frame_length);

	printf("New frame (after going through channel):\n");
	for (unsigned int i = 0; i < frame_length; i++) {
		printf("%d", frame_vector[i]);
	}
	printf("\n\n");

	packet_t rxpacket_data;//malloc this?
	rxpacket_data.data = (uint8_t*)malloc(PACKET_DATA_LENGTH_BYTES);
	uint8_t* rxpacket_vector = NULL;
	unsigned int rxpacket_length = 0;

	disassembleFrame(frame_vector, &rxpacket_vector, frame_length);

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

	free(rxpacket_data.data);
	free(rxpacket_vector);
	free(packet_vector);
	free(frame_vector);
	return 0;
}



