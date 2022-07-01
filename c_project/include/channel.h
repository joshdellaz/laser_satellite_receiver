#include <stdint.h>
#include <stdbool.h>
#include <math.h>

/// Channel stuff
#define BIT_RATE 1 // in Mbps (should be 25)
#define SAMP_PER_BIT 4
#define BURST_VALUE 1 // = sample during a burst... should be something significantly higher than normal readings
#define FADE_VALUE 0
#define SNR_DB	10 // for AWGN noise
#define FADE_FREQ 770 // per second, estimation based on KT06-04 results around 25 degree elevation = 3 or 4
#define FADE_LEN 200 // in us

#define BIT_FLIP_FREQ 5 // increase to reduce probability of the bit flippage

// Parameters which are based on the paper
#define P_a1 0.270 
#define P_a2 0.060
#define P_b1 0.240
#define P_b2 0.050
#define CHNL_CYC 10 // in us (e-6s) should be 50 based on the paper

typedef enum chnl_state
{
	GOOD_UNS, // no erasure and unstable
	BAD_UNS,  // erasure and unstable
	BAD_S,	  // erasure and stable
	GOOD_S	  // no erasure and stable
} chnl_state;

bool applyChannelToSamples(float *samples, unsigned smpls_len); //, uint16_t curr_packet_num);
bool applyChannel(uint8_t *input, unsigned int input_length);
bool applyBitFlips(uint8_t *input, unsigned int input_length);