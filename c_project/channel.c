#include "channel.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

float _randF(void); // rand float between 0 and 1
void _createBursts(bool *, unsigned);
void _applyBursts(bool *, uint8_t *, unsigned);
void _applyBurstsToSamples(bool *, float *, unsigned);
float _AWGN_generator(void);

bool applyBitFlips(uint8_t *input_data, unsigned int input_data_length)
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

bool applyChannelToSamples(float *samples, unsigned smpls_len)
{
	bool *bursts = (bool *)calloc(smpls_len / SAMP_PER_BIT, sizeof(bool));
	_createBursts(bursts, smpls_len / SAMP_PER_BIT);
	_applyBurstsToSamples(bursts, samples, smpls_len);
	return true;
}

bool applyChannel(uint8_t *input_data, unsigned int input_data_length)
{ // change description to return array of bools
	// initialize array of bursts
	bool *bursts = (bool *)calloc(8 * input_data_length, sizeof(bool));
	_createBursts(bursts, input_data_length);
	_applyBursts(bursts, input_data, input_data_length);
	return true;
}

float _randF(void)
{
	return (float)rand() / (float)RAND_MAX;
}

float _AWGN_generator(void)
{ // feels like overkill but fuck it
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

void _createBursts(bool *Bursts, unsigned input_data_length)
{
	unsigned int bits_per_cyc = BIT_RATE * CHNL_CYC; // non-zero integer
	chnl_state chnl_st;								 // current state of the channel
	float init_st = _randF();

	if (init_st < 0.250)
	{
		chnl_st = GOOD_S;
		Bursts[0] = false; // false represents absence of burst erasure
	}
	else if (init_st < 0.50)
	{
		chnl_st = BAD_S;
		Bursts[0] = true; // true represents existence of burst erasure
	}
	else if (init_st < 0.750)
	{
		chnl_st = BAD_UNS;
		Bursts[0] = true;
	}
	else
	{
		chnl_st = GOOD_UNS;
		Bursts[0] = false;
	}
	//chnl_st = GOOD_S; ////////////////////////////////////////////////////////////////////////////////// TEMPORARY
	printf("\n\n initial state: %d \n\n", chnl_st);
	printf("Bursts: \n");
	for (unsigned int i = 1; i < 8 * input_data_length; i++)
	{
		if (i % bits_per_cyc == 0)
		{
			// switch case statement to change the channel state
			switch (chnl_st)
			{
			case GOOD_S:
				if (_randF() < P_a2)
				{
					chnl_st = BAD_UNS;
				}
				else
				{
					chnl_st = GOOD_S;
				}
				break;
			case BAD_S:
				if (_randF() < P_b2)
				{
					chnl_st = GOOD_UNS;
				}
				else
				{
					chnl_st = BAD_S;
				}
				break;
			case BAD_UNS:
				if (_randF() < P_b1)
				{
					chnl_st = GOOD_UNS;
				}
				else
				{
					chnl_st = BAD_S;
				}
				break;
			case GOOD_UNS:
				if (_randF() < P_a1)
				{
					chnl_st = BAD_UNS;
				}
				else
				{
					chnl_st = GOOD_S;
				}
				break;
			}
		}
		if ((chnl_st == BAD_S) || (chnl_st == BAD_UNS))
		{
			// insert 1 into array of bursts if the current state is bad
			Bursts[i] = true;
			printf("1");
		}
		else
		{
			Bursts[i] = false;
			printf("0");
		}
	}
}

void _applyBursts(bool *Bursts, uint8_t *input_data, unsigned int input_data_length)
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

void _applyBurstsToSamples(bool *Bursts, float *samples, unsigned smpls_len)
{
	for (unsigned int i = 0; i < smpls_len / SAMP_PER_BIT; i++)
	{
		if (Bursts[i])
		{
			for (unsigned j = SAMP_PER_BIT * i; j < SAMP_PER_BIT * (i + 1); j++)
			{
				samples[j] = (float)BURST_VALUE;
			}
		}
	}
}