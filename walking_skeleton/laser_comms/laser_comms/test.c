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
	return 0;
}


//big disruption
applyChannel(uint8_t * input_data, unsigned int input_data_length) {
	for (int i = 0; i < input_data_length; i++) {
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
	uint8_t* frame_vector = NULL;

	packet_data.data = generateRandPacket();
	getCRC(&packet_data);
	//applyFEC(packet)//does this act on the entire packet? Assuming no.
	//applyInterleaving()
	assemblePacket(&packet_data, packet_vector, &packet_length);//not sure if this should be before or after interleaving
	free(packet_data.data);
	//applyScrambling()
	assembleFrame(frame_vector, &frame_length, packet_vector, packet_length);
	for (int i = 0; i < frame_length; i++) {
		printf("%d", frame_vector[i]);
	}
	
	applyChannel(frame_vector, frame_length);

	packet_t rxpacket_data;//malloc this?
	uint8_t* rxpacket_vector;
	unsigned int rxpacket_length;

	//disassembleFrame();

	//disassemblePacket();

	if (checkCRC(&rxpacket_data)) {
		printf("CRC Doesn't Match!");
	}
	else {
		printf("CRC Matches!");
	}

	free(packet_vector);
	free(frame_vector);
	return 0;
}



