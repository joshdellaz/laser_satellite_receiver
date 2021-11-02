#include <stdint.h>
#include <stdbool.h>

#define PACKET_DATA_LENGTH_BYTES 256
#define NUM_PACKETS_LENGTH_BYTES 2
#define CRC_DATA_LENGTH_BYTES 4

typedef enum fec_scheme_type {

	LDPC = 1,
	R_S = 2

}fec_scheme_type;

typedef struct packet_t {//add #IF's for num_packet size

	fec_scheme_type selected_fec_scheme;
	uint16_t total_num_packets;//These necessary?
	uint16_t current_packet_num;
	uint8_t *data; //Should we indidcate data length here?
	uint32_t crc;

} packet_t;//TODO: add field for FEC additional stuff?


bool applyFEC(void);
bool removeFEC(void);
bool checkCRC(packet_t* received_packet);
bool getCRC(packet_t* packet);
bool applyScrambling(void);
bool removeScrambling(void);
bool applyInterleaving(void);
bool removeInterleaving(void);
bool getMaximumLengthSequencePreamble(uint8_t* mls_preamble, unsigned int* mls_preamble_length);
bool syncFrameUsingMLSPreamble(void);
bool assembleFrame(uint8_t** frame, unsigned int* frame_length, uint8_t* packet, unsigned int packet_length);
bool disassembleFrame(uint8_t* frame, uint8_t** packet, unsigned int frame_length);
bool assemblePacket(packet_t* packet_data, uint8_t** packet, unsigned int* packet_length);
bool disassemblePacket(packet_t* packet_data, uint8_t* packet, unsigned int packet_length);
bool fragmentDataBufferIntoFrames(uint8_t* input, uint8_t* output);
bool assembleFramesIntoDataBuffer(uint8_t* input, uint8_t* output);