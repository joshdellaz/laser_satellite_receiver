
#define SAMP_PER_BIT 4

//LDPC params
#define CODEWRD_L 6656
#define CODEWRD_R 0.2 // for 1/5 rate
#define MAX_DECODE_ITERS 25

//Synchronization params
#define CLOCK_STABILITY_PPM 1

//pick MLS length based on bitrate and channel
#define FRACTION_OF_MLS_INTACT 0.5 //If MLS hit with erasure, this is how much we want intact for sync purposes (TBC)

//Sizes of fields in packet structure
#define NUM_PACKETS_LENGTH_BYTES 2
#define CRC_DATA_LENGTH_BYTES 4
#define FRAME_LENGTH_BYTES //TODO determine proper value

//Testing params (set to 0 to disable)
#define LDPC_ENABLED 0
#define INTRLV_SCRMBL_ENABLED 0
#define CHANNEL_APPLIED_TO_SAMPLES 0
#define CHANNEL_APPLIED_TO_BITS 0

#define VERBOSE_ENABLED 0    //prints literally everything

//Sanity Checks
#if (CHANNEL_APPLIED_TO_SAMPLES == 1 && CHANNEL_APPLIED_TO_BITS == 1)
#error "Channel cannot be applied to samples AND bits! Please pick one"
#endif