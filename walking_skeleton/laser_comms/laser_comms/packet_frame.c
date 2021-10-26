#include "liquid_condensed.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>


#include "packet_frame.h"




//Move these into separate file?

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

bool assembleFrame() {
	return 0;
}


bool disassembleFrame() {
	return 0;
}

bool assemblePacket() {
	return 0;
}

bool disassemblePacket() {
	return 0;
}