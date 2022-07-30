#define BIT_RATE 1 // in Mbps (should be 25)
#define SAMP_PER_BIT 4

//LDPC params
#define CODEWRD_L 6656
#define NUM_BLOCKS_PCKT 2
#define CODEWRD_R 0.2 // for 1/3 write 0.33
#define MAX_DECODE_ITERS 25
#define BLOCK_SIZE 160 // length of encodable in Bytes

//Sizes of fields in packet structure
#define NUM_PACKETS_LENGTH_BYTES 2
#define CRC_DATA_LENGTH_BYTES 4
#define FRAME_LENGTH_BYTES //TODO determine proper value
#define FEC_TYPE LIQUID_FEC_HAMMING74 //Other options = 
#define PACKET_DATA_LENGTH_NO_FEC (BLOCK_SIZE*NUM_BLOCKS_PCKT -  CRC_DATA_LENGTH_BYTES - 2*NUM_PACKETS_LENGTH_BYTES)
//^ 316 = 4 blocks 235 = 3 blocks, 154 = 2 blocks, 73 = 1 block

//Test params (set to 0 to disable)
#define LDPC_ENABLED 1
#define INTRLV_SCRMBL_ENABLED 1
#define CHANNEL_APPLIED_TO_SAMPLES 1
#define CHANNEL_APPLIED_TO_BITS 0

#define VERBOSE_ENABLED 0    //prints literally everything

//Sanity Checks
#if (CHANNEL_APPLIED_TO_SAMPLES == 1 && CHANNEL_APPLIED_TO_BITS == 1)
#error "Channel cannot be applied to samples AND bits! Please pick one"
#endif