#include "channel.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

float randF(void); // rand float between 0 and 1
void applyBursts(bool*, uint8_t*, unsigned int);

bool applyChannel(uint8_t* input_data, unsigned int input_data_length) { // change description to return array of bools
	unsigned int bits_per_cyc = BIT_RATE * CHNL_CYC; // non-zero integer
	chnl_state chnl_st; // current state of the channel
	float init_st = randF();
	// initialize array of bursts
	bool* bursts = (bool*)calloc(8*input_data_length, sizeof(bool));

	if (init_st < 0.250) { 
		chnl_st = GOOD_S;
		bursts[0] = false; // false represents absence of burst erasure
	}
	else if (init_st < 0.50) { 
		chnl_st = BAD_S;
		bursts[0] = true; // true represents existence of burst erasure
	}
	else if (init_st < 0.750) { 
		chnl_st = BAD_UNS; 
		bursts[0] = true; 
	}
	else { 
		chnl_st = GOOD_UNS;
		bursts[0] = false;
	}

	for (unsigned int i = 1; i < 8*input_data_length; i++) {
		if (i % bits_per_cyc == 0) {
			// switch case statement to change the channel state
			switch (chnl_st) {
			case GOOD_S:
				if (randF() < P_a2) { chnl_st = BAD_UNS; }
				else { chnl_st = GOOD_S; }
			case BAD_S:
				if (randF() < P_b2) { chnl_st = GOOD_UNS; }
				else { chnl_st = BAD_S; }
			case BAD_UNS:
				if (randF() < P_b1) { chnl_st = GOOD_UNS; }
				else { chnl_st = BAD_S; }
			case GOOD_UNS:
				if (randF() < P_a1) { chnl_st = BAD_UNS; }
				else { chnl_st = GOOD_S; }
			}
		}
		if ((chnl_st == BAD_S) || (chnl_st == BAD_UNS)) {
			// insert 1 into array of bursts if the current state is bad
			bursts[i] = true;
		} else { bursts[i] = false; }
	}
	applyBursts(bursts, input_data, input_data_length);
	return true;
}

float randF(void) {
	return (float)rand() / (float)RAND_MAX;
}

void applyBursts(bool *Bursts, uint8_t *input_data, unsigned int input_data_length) {
	uint8_t bytePar; // bursts array parsed to byte size to be or'ed with input data
	for (unsigned int i = 0; i < input_data_length; i++) {
		// temporary way of flipping bits, will be done differently (using AWGN) once the channel can output oversampled signal
		if (randF() < ((float)(i) / (float)(BIT_FLIP_CRTL*input_data_length))) { // kinda random, more probable near the end of the frame
			input_data[i] = (rand() & 0xff) & input_data[i];
		} // flipping can be overwritten by a burst if there is one
		bytePar = 0;
		for (int bit = 0; bit < 8; bit++) {
			if (Bursts[(8 * i) + bit]) {
				if (bit == 0) {
					bytePar = bytePar | 0x80;
				}
				else if (bit == 1) {
					bytePar = bytePar | 0x40;
				}
				else if (bit == 2) {
					bytePar = bytePar | 0x20;
				}
				else if (bit == 3) {
					bytePar = bytePar | 0x10;
				}
				else if (bit == 4) {
					bytePar = bytePar | 0x08;
				}
				else if (bit == 5) {
					bytePar = bytePar | 0x04;
				}
				else if (bit == 6) {
					bytePar = bytePar | 0x02;
				}
				else if (bit == 7) {
					bytePar = bytePar | 0x01;
				}
			}
			if (bit == 7) {
				input_data[i] = input_data[i] | bytePar;
				bytePar = 0;
			}
		}
	}
}