#include "packet_frame.h"

#include <stdint.h>

uint8_t * generateRandPacket(void) {
	uint8_t* data = (uint8_t*)malloc(DATA_LENGTH_BYTES);
	for (int i = 0; i < DATA_LENGTH_BYTES; i++) {
		data[i] = rand() & 0xff;
	}
}

bool fullSend(void) {

	//Is the following order correct?

	packet_t packet;
	packet.fec_scheme = LDPC;

	//generateRandPacket()
	//getCRC()
	//applyFEC(packet)//does this act on the entire packet? Assuming no.
	//applyInterleaving()
	//assemblePacket()//not sure if this should be before or after interleaving
	//applyScrambling()
	//assembleFrame()
	//applyChannel()
}



free(data);