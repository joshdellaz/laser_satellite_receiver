#include <stdint.h>
#include <stdbool.h>
#include <math.h>

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

void configChannel(int snr, int f_freq, int f_len, int b_len);
int getFadeLenUsec();
int getFadeFreqHz();
int getBurstLenUsec();
void setBitRateMbps(int bitrate);
int getBitRateMbps();
void setFadeParamsBasedOnElevation(float elevation_angle);
void initChannelState();
bool applyChannelToSamples(float *samples, unsigned smpls_len); //, uint16_t curr_packet_num);
bool applyChannel(uint8_t *input, unsigned int input_length);
bool applyBitFlips(uint8_t *input, unsigned int input_length);
int getBurstLengthBits();
int getFadeLengthBits();