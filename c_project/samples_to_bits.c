#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#define PI 3.142857

//Samples out (returned by pointer) are straight up 1s and zeroes
//Must be un-malloc'd
//Big endian: high bit transfered first
float * bytestreamToSamplestream(uint8_t* data, int length_bytes, int *length_samples){
    int samples_per_bit = 4;
    float phase_offset_fraction_of_symbol = 0;
    float *samples = (float*)malloc((8*length_bytes*samples_per_bit) * sizeof(float));
    *length_samples = length_bytes*8*samples_per_bit;

    //repeat each input element
    for(int i = 0; i<length_bytes; i++){
        for (int j = 0; j <8; j++){
            for(int k = 0; k<samples_per_bit; k++){
                if((data[i] & (1 << (7-j))) == 0){
                    samples[i*8*samples_per_bit + j*samples_per_bit + k] = 0;
                } else {
                    samples[i*8*samples_per_bit + j*samples_per_bit + k] = 1;
                }
            }
        }
    }

    //TODO: add shift by phase (upsample to higher rate, add shift, then downsample)

    return samples;
}

//Need to do this for proper filtering and decoding
//Converts input stream from 0 to 1 range (OOK) to a -1 to 1 range, centered at y = zero
bool shiftDownAndNormalizeSamples(float ** samples, int length_samples){
    for(int i = 0; i < length_samples; i++){
        (*samples)[i] -= 0.5;
        (*samples)[i] *= 2;
    }
}

//Big endian: high bit transfered first
uint8_t * samplesToBytes(float* samples, int length_samples, float phase_offset){
    int num_banks = 4;
    int samples_per_bit = 4;
    float phase_offset_fraction_of_symbol = 0;
    int init_offset = num_banks*samples_per_bit;
    int length_bytes = length_samples/(8*samples_per_bit*num_banks);
    uint8_t *bytes = (uint8_t*)malloc(length_bytes * sizeof(uint8_t));//Does this need to be set to {0}?
    int n_offset = round(((float)(samples_per_bit*num_banks))*(phase_offset/PI + 1/2));//Derived via formula manipulation
    //int n_offset = 0;

    for(int i = 0; i<length_bytes; i++){
        bytes[i] = 0;
        for (int j = 0; j<8; j++){
            int testvar_index = (i*8 + j)*samples_per_bit*num_banks + n_offset + init_offset;
            //printf("%.2f ", samples[testvar_index]);
            if(samples[testvar_index] >= 0){//init_offset is to avoid negative indices
                bytes[i] = bytes[i] | (1 << (7-j));//this bit is a 1
                //printf("1 ");
            } else {
                bytes[i] = bytes[i];//this bit is a 0
                //printf("0 ");
            }
        }
    }
    return bytes;
}