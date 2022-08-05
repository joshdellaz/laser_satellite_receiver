#include "channel.h"
#include "config.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define PI 3.1415926536

#define BURST_VALUE 1 // = sample during a burst... should be something significantly higher than normal readings
#define FADE_VALUE 0

//Default channel characteristices
//TODO make this a struct instead... mayble?
int bit_rate_mbps = 10;
int snr_db = 20; // for AWGN noise
int fade_freq_hz = 100; // Estimation based on KT06-04 results around 25 degree elevation = 3 or 4 //770 used originally
int fade_len_usec = 1; // 700 originally TODO make this dependent on bitrate!
int burst_len_usec = 50; //typical based on Y. Yamashita et al. "n Efficient LDGM Coding Scheme for Optical Satellite-to-Ground Link Based on a New Channel Model"


void _createBursts(bool *, unsigned);
//void _createFades(bool *, unsigned);
void _applyBursts(bool *, uint8_t *, unsigned);
void _applyFades(uint8_t *, unsigned);
void _applyBurstsToSamples(bool *, float *, unsigned);
void _applyFadesToSamples(float *, unsigned);

float __AWGN_generator(void);
float __randF(void); // rand float between 0 and 1
int __randNum(int, int); // rand number between lower and upper inclusive

void configChannel(int snr, int f_freq, int f_len, int b_len){
	snr_db = snr;
	fade_freq_hz = f_freq;
	fade_len_usec = f_len;
	burst_len_usec = b_len;
}

int getFadeLenUsec(){
	return fade_len_usec;
}

int getFadeFreqHz(){
	return fade_freq_hz;
}

int getBurstLenUsec(){
	return burst_len_usec;
}

void setBitRateMbps(int bitrate){
	bit_rate_mbps = bitrate;
}

int getBitRateMbps(){
	return bit_rate_mbps;
}

//based on F. Moll, “Experimental analysis of channel coherence time and fading behavior in the LEO-ground link
void setFadeParamsBasedOnElevation(float elevation_angle){
	if(elevation_angle < 7.5){//outside of studied dataset; assume constant outside of range
		fade_freq_hz = 20;
		fade_len_usec = 1300;
	} else if(elevation_angle > 27){//outside of studied dataset; assume constant outside of range
		fade_freq_hz = 20;
		fade_len_usec = 1300;
	} else {//roughly estimated curve fits
		fade_freq_hz = pow(10,(2.75-elevation_angle/13.0));
		fade_len_usec = 1.7 - 0.05*elevation_angle;
	}
}

bool applyChannelToSamples(float *samples, unsigned smpls_len) //, uint16_t curr_packet_num)
{
	bool *bursts = (bool *)calloc(smpls_len / SAMP_PER_BIT, sizeof(bool));
	_createBursts(bursts, smpls_len / (8*SAMP_PER_BIT));
	_applyBurstsToSamples(bursts, samples, smpls_len);

	_applyFadesToSamples(samples, smpls_len);

	//float current_packet_snr = _calcSNR(curr_packet_num);

	// add the AWGN
	float signal_power = 0.5; // equal dist of 1s and 0s
	float noise_power = signal_power / (float) pow((double)10.0, (double)(snr_db/10.0)) ;
	float std_dev = (float) sqrtf(noise_power);
	for (unsigned i = 0; i < smpls_len; i++){
		samples[i] = samples[i] + (std_dev * __AWGN_generator());
	}

	free(bursts);
	return true;
}


void _createBursts(bool *Bursts, unsigned input_data_length)
{
	unsigned int bits_per_cyc = bit_rate_mbps * CHNL_CYC; // non-zero integer
	chnl_state chnl_st;								 // current state of the channel
	float init_st = __randF();

	if (init_st < 0.250){
		chnl_st = GOOD_S;
		Bursts[0] = false; // false represents absence of burst erasure
	}
	else if (init_st < 0.50){
		chnl_st = BAD_S;
		Bursts[0] = true; // true represents existence of burst erasure
	}
	else if (init_st < 0.750){
		chnl_st = BAD_UNS;
		Bursts[0] = true;
	}
	else{
		chnl_st = GOOD_UNS;
		Bursts[0] = false;
	}
	//chnl_st = GOOD_S;
	for (unsigned int i = 1; i < 8 * input_data_length; i++)
	{
		if (i % bits_per_cyc == 0)
		{
			// switch case statement to change the channel state
			switch (chnl_st) {
			case GOOD_S:
				if (__randF() < P_a2) {chnl_st = BAD_UNS;}
				else {chnl_st = GOOD_S;}
				break;
			case BAD_S:
				if (__randF() < P_b2) {chnl_st = GOOD_UNS;}
				else {chnl_st = BAD_S;}
				break;
			case BAD_UNS:
				if (__randF() < P_b1) {chnl_st = GOOD_UNS;}
				else {chnl_st = BAD_S;}
				break;
			case GOOD_UNS:
				if (__randF() < P_a1) {chnl_st = BAD_UNS;} 
				else {chnl_st = GOOD_S;}
				break;
			}
		}
		if ((chnl_st == BAD_S) || (chnl_st == BAD_UNS))
		{
			// insert 1 into array of bursts if the current state is bad
			Bursts[i] = true;
		}
		else
		{
			Bursts[i] = false;
		}
	}
}


void _applyFadesToSamples(float *samples, unsigned smpls_len)
{
	float fade_certain_period = 1e6 / fade_freq_hz; // [us] period of time in which there should be a fade
	int total_fades = (int) floor(smpls_len / (fade_certain_period*bit_rate_mbps*SAMP_PER_BIT)); 
	int N = smpls_len / total_fades;
	int M = fade_len_usec * bit_rate_mbps * SAMP_PER_BIT; // in every N samples there's a fade of length M
	for (unsigned i = 0; i < total_fades; i++){
		int fade_start = (i*N) + __randNum(0,N-M-1);
		for (int j = fade_start; j < fade_start + M; j++){
			samples[j] = (float) FADE_VALUE;
		}
	}
}


void _applyBurstsToSamples(bool *Bursts, float *samples, unsigned smpls_len)
{
	// maybe TODO: make the bursts sharp normal distrbutions instead of constant values, maybe relate the height to the busrt length
	unsigned burst_len = 0;
	for (unsigned int i = 0; i < smpls_len / SAMP_PER_BIT; i++)
	{
		if (Bursts[i])
		{
			for (unsigned j = SAMP_PER_BIT * i; j < SAMP_PER_BIT * (i + 1); j++)
			{
				burst_len++;
				if (burst_len == 1){ // random offset at the beginning of the burst
					float chance = __randF();
					if (chance < 0.33){
						j++;
					} else if (chance < 0.67){
						j += 2;
					} else {j += 3;}
				}
				samples[j] = (float)BURST_VALUE;
			}
		} else {burst_len = 0;} // zero the burst length when burst ends
	}
}


float __randF(void)
{
	return (float)rand() / (float)RAND_MAX;
}

int __randNum(int lower, int upper)
{
	return (rand() % (upper - lower + 1)) + lower;
}

float __AWGN_generator(void)
{ // feels like overkill but screw it
	// from https://www.embeddedrelated.com/showcode/311.php
	/* Generates additive white Gaussian Noise samples with zero mean and a standard deviation of 1. */
	double temp1;
	double temp2;
	float result;
	int p;
	p = 1;
	while (p > 0)
	{
		temp2 = (rand() / ((double)RAND_MAX)); /* rand() function generates an integer between 0 and  RAND_MAX, which is defined in stdlib.h */
		if (temp2 == 0)
		{ // temp2 is >= (RAND_MAX / 2)
			p = 1;
		}
		else
		{ // temp2 is < (RAND_MAX / 2)
			p = -1;
		}
	} // end while()
	temp1 = cos((2.0 * (double)PI) * rand() / ((double)RAND_MAX));
	result = (float)sqrt(-2.0 * log(temp2)) * temp1;
	return result; // return the generated random sample to the caller
}


bool applyChannel(uint8_t *input_data, unsigned int input_data_length) // works with bit stream (not samples)
{ // change description to return array of bools
	// initialize array of bursts
	bool *bursts = (bool *)calloc(8 * input_data_length, sizeof(bool));
	_createBursts(bursts, input_data_length);
	_applyBursts(bursts, input_data, input_data_length);
	_applyFades(input_data, input_data_length);
	
	free(bursts);
	return true;
}

void _applyBursts(bool *Bursts, uint8_t *input_data, unsigned int input_data_length) // works with bit stream (not samples)
{
	uint8_t bytePar; // bursts array parsed to byte size to be or'ed with input data
	for (unsigned int i = 0; i < input_data_length; i++)
	{
		bytePar = (uint8_t)0;
		for (int bit = 0; bit < 8; bit++)
		{
			//printf("%d", Bursts[(8 * i) + bit]);
			if (Bursts[(8 * i) + bit])
			{
				if (bit == 0)
				{
					bytePar = bytePar | 0x80;
				}
				else if (bit == 1)
				{
					bytePar = bytePar | 0x40;
				}
				else if (bit == 2)
				{
					bytePar = bytePar | 0x20;
				}
				else if (bit == 3)
				{
					bytePar = bytePar | 0x10;
				}
				else if (bit == 4)
				{
					bytePar = bytePar | 0x08;
				}
				else if (bit == 5)
				{
					bytePar = bytePar | 0x04;
				}
				else if (bit == 6)
				{
					bytePar = bytePar | 0x02;
				}
				else if (bit == 7)
				{
					bytePar = bytePar | 0x01;
				}
			}
			if (bit == 7)
			{
				input_data[i] = input_data[i] | bytePar;
			}
		}
	}
}

void _applyFades(uint8_t *input_data, unsigned int input_data_length)
{
	float fade_certain_period = 1e6 / fade_freq_hz; // [us] period of time in which there should be a fade
	int total_fades = (int) floor(8*input_data_length / (fade_certain_period*bit_rate_mbps)); 
	int N = (int) floor(8*input_data_length / total_fades); // in bits
	int M = fade_len_usec * bit_rate_mbps; // in bits
	for (unsigned i = 0; i < total_fades; i++){
		int fade_start = (i*N) + __randNum(0,N-M-1);
		int leadingZeros = (fade_start % 8) ? 8 - (fade_start % 8) : 0;
		int trailingZeros = (fade_start + M) % 8;
		for (int j = fade_start/8; j < (fade_start + M)/8; j++){
			input_data[j] = input_data[j] & 0x00;
		}
		printf("\n\n");
		input_data[fade_start/8] &= (uint8_t) (255 - pow(2,(leadingZeros-1)));
		input_data[(fade_start + M - 1) / 8] &= (uint8_t) (pow(2,(8-trailingZeros)) - 1);
	}
}


bool applyBitFlips(uint8_t *input_data, unsigned int input_data_length) // works with bit stream (not samples)
{
	for (unsigned int i = 0; i < input_data_length; i++)
	{
		if (i % BIT_FLIP_FREQ == 0)
		{
			input_data[i] = (rand() & 0xff) & input_data[i];
			input_data[i] = (input_data[i] >> 1) << 1; //only flips bit if lsb was a 1
		}
	}
	return true;
}

int getBurstLengthBits(){
	return burst_len_usec * bit_rate_mbps;
}
int getFadeLengthBits(){
	return fade_len_usec * bit_rate_mbps;
}