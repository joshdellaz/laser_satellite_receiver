#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <complex.h>
#include <liquid/liquid.h>
#include "config.h"
#define PI 3.1415926536 // fixed

#ifdef  __cplusplus
   using cfloat = std::complex<float>;
#else
   typedef float complex cfloat;
#endif

//Samples out (returned by pointer) are straight up 1s and zeroes
//Must be un-malloc'd
//des not free input data
//Big endian: high bit transfered first
//phaseshift must be float type with value between 0 and 2*pi. Only phase shifts to the right (cuts off MSB)
float * bytestreamToSamplestream(uint8_t* data, int length_bytes, int *length_samples, float phaseshift){
    int samples_per_bit = 4;
    *length_samples = length_bytes*8*samples_per_bit;
    float *samples = (float*)malloc((*length_samples) * sizeof(float));


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

    // printf("Samples before phase shift:\n");
    // for (unsigned int i = 0; i < 100; i++) {
    //     printf("%.0f", samples[i]);
    // }
    // printf("\n\n");

    int num_repititons = 10;//Phase shift resolution = 2*pi/(N*num_repititions)
    float *temp = (float*)malloc((*length_samples)*num_repititons * sizeof(float));

    //repeat elements
    for (int i = 0; i < *length_samples; i++){
        for(int j = 0; j<num_repititons; j++){
            temp[i*num_repititons + j] = samples[i];
        }
    }

    //shift and downsample
    int phaseshift_number = round(phaseshift/(PI*(float)2)*(float)(num_repititons*samples_per_bit));//Not certain about this
    for (int i = 0; i < *length_samples; i++){
        samples[i] = temp[i*num_repititons + phaseshift_number];
    }

    free(temp);
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
uint8_t * samplesToBytes(float* samples, int length_samples){

    int length_bytes = length_samples/8;
    uint8_t *bytes = (uint8_t*)malloc(length_bytes * sizeof(uint8_t));

    for(int i = 0; i<length_bytes; i++){
        bytes[i] = 0;
        for (int j = 0; j<8; j++){
            if(samples[i*8 + j] >= 0){
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

//ENSURE shiftDownAndNormalizeSamples IS RUN ON SAMPLES BEFORE THIS FUNCTION - not sure about this Jan 29
//Frees input pointer
float * resampleInput(float* samplesin, int length_samples_in, int * length_samples_out) {

    float        r = UPSAMPLE_RATE;   // resampling rate (output/input)
    float        bw = 0.45f;  // resampling filter bandwidth (HMM)
    unsigned int npfb = 64;     // number of filters in bank (timing resolution)
    float slsl = 60;          // resampling filter sidelobe suppression level
    unsigned int h_len = 16;  // filter semi-length (filter delay)
    int filter_delay = 122;

    cfloat * complexbuffer = (cfloat*)malloc(length_samples_in*r*sizeof(cfloat)); 


    // create resampler
    resamp_crcf q = resamp_crcf_create(r, h_len, bw, slsl, npfb);


    //unsigned int num_written = 0;   // number of values written to buffer this iteration
    unsigned int num_written_total = 0;
    int repititions = 1;
    unsigned int temp = 0;
    for(int i = 0; i < repititions; i++){
        resamp_crcf_execute_block(q, (cfloat *)samplesin, length_samples_in/repititions, &(complexbuffer[i*length_samples_in/repititions]), &temp);
        num_written_total += temp;
    }


    *length_samples_out = length_samples_in*r;
    if (num_written_total != *length_samples_out) {
        printf("Filter not executed properly");
    }

    // clean up allocated objects
    resamp_crcf_destroy(q);
    free(samplesin);
    return (float *)complexbuffer;
}