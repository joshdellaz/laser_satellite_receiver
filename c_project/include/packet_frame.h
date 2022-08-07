#ifndef PACKET_FRAME_H
#define PACKET_FRAME_H

#include <stdint.h>
#include <stdbool.h>
#include <liquid/liquid.h>
#include "config.h"

extern int packet_data_length_with_fec_bytes;
extern int packet_data_length_without_fec_bytes;

//Enum type to avoid magic numbers
//TODO evaluate if needed for design
typedef enum fec_scheme_type {

	LDPC = 1,
	R_S = 2

}fec_scheme_type;

//Structure of packet for easy handling of separate fields/parameters
//Field lengths = n*8 bits for simplicity
//TODO add #IF's to allow for variable size of non-data fields during compile
//TODO: add field for FEC additional stuff?
typedef struct packet_t {

	fec_scheme_type selected_fec_scheme;
	uint16_t total_num_packets;
	uint16_t current_packet_num;
	uint8_t *data; //Consider indicating data length within packet instead of macro?
	uint32_t crc;

} packet_t;

/*
FRAME DEFINITIONS
-------------------------------------------------
| MLS PREAMBLE | 8 BYTES STUFFING | PACKET DATA |
-------------------------------------------------
*/



bool applyFEC(uint8_t* input);
bool removeFEC(uint8_t* input);
bool checkCRC(packet_t* received_packet);
bool getCRC(packet_t* packet);
bool applyScrambling(uint8_t ** input, unsigned int input_length);
bool removeScrambling(uint8_t ** input, unsigned int input_length);
bool applyInterleaving(uint8_t* input, unsigned int input_length);
bool removeInterleaving(uint8_t* input, unsigned int input_length);
bool getMaximumLengthSequencePreamble(uint8_t** mls_preamble, unsigned int* mls_preamble_length);
bool syncFrameUsingMLSPreamble(uint8_t* input, unsigned int* byte_index, unsigned int* bit_index);
bool assembleFrame(uint8_t** frame, unsigned int* frame_length, uint8_t* packet, unsigned int packet_length);
bool disassembleFrame(uint8_t* frame, uint8_t** packet, unsigned int frame_length);
bool assemblePacket(packet_t* packet_data, uint8_t** packet, unsigned int* packet_length);
bool disassemblePacket(packet_t* packet_data, uint8_t* packet, unsigned int packet_length);
int getNumBlocksPerPacket();
void setFrameLengthBasedOnElevation(int elevation_angle_degrees);
int getMLSOrder();
int getPassLengthInSeconds(unsigned min_elevation_degrees);
float getFreqStabilityInPPMUsingElevation(int elevation_angle_degrees);
int getMaxFrameLengthInBits(float total_freq_stability_ppm);
void setMLSOrderBasedOnChannel();
bool fragmentDataBufferIntoFrames(uint8_t* input, unsigned int input_length, uint8_t* output, unsigned int output_length);
bool assembleFramesIntoDataBuffer(uint8_t* input, unsigned int input_length, uint8_t* output, unsigned int output_length);

#endif