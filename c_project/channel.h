#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#define PI 3.1415926536

/// Channel stuff
#define BIT_RATE 1 // in Mbps
#define SAMP_PER_BIT 4 
#define BURST_VALUE 69.420E20
#define SNR // (for AWGN noise) 
#define BIT_FLIP_FREQ 5 // increase to reduce probability of the bit flippage

// Parameters which are based on the paper
#define P_a1 0.270
#define P_a2 0.060
#define P_b1 0.240
#define P_b2 0.050
#define CHNL_CYC 50 // in us (inverse of bit rate's order which is 10^6)

typedef enum chnl_state {
	GOOD_UNS, // no erasure and unstable
	BAD_UNS, // erasure and unstable
	BAD_S, // erasure and stable
	GOOD_S // no erasure and stable
} chnl_state;

bool applyChannelToSamples(float* samples, unsigned smpls_len);
bool applyChannel(uint8_t* input, unsigned int input_length);
bool applyBitFlips(uint8_t* input, unsigned int input_length);