#include <stdint.h>

#define DATA_LENGTH_BYTES 256

typedef enum fec_scheme_type {

	LDPC = 1,
	R_S = 2

}fec_scheme_type;

typedef struct packet_t {

	fec_scheme_type fec_scheme;
	uint16_t total_num_packets;//These necessary?
	uint16_t current_packet_num;
	uint8_t* data; //Should we indidcate data length here?
	uint32_t crc;

} packet_t;