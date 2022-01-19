#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#define PI 3.142857


//Matlab code:
//%do the sampling
//size = 20+num_of_data_bits;
//output = zeros(1, size);
//selected_points_x = zeros(1, size);
//for i = 0:(size - 1)
//    if input_signal(round((i)*N*num_banks) + n_offset) >= 0 %rounding issues due to N?
//        output(i+1) = 1;
//        selected_points_x(i+1) = round((i)*N*num_banks) + n_offset;%for later infographic
//    else
//        output(i+1) = 0;
//        selected_points_x(i+1) = round((i)*N*num_banks) + n_offset;%for later infographic
//    end
//end

// static double rt_roundd_snf(double u)
// {
//     double y;
//     if (fabs(u) < 4.503599627370496E+15) {
//         if (u >= 0.5) {
//             y = floor(u + 0.5);
//         }
//         else if (u > -0.5) {
//             y = u * 0.0;
//         }
//         else {
//             y = ceil(u - 0.5);
//         }
//     }
//     else {
//         y = u;
//     }
//     return y;
// }

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
        for (int j = 0; j<8; j++){
            int testvar_index = (i*8 + j)*samples_per_bit*num_banks + n_offset + init_offset;
            printf("%.2f ", samples[testvar_index]);
            if(samples[testvar_index] >= 0){//init_offset is to avoid negative indices
                
                bytes[i] = bytes[i] | (1 << (7-j));//this bit is a 1
            } else {
                bytes[i] = bytes[i];//this bit is a 0
            }
        }
    }
    printf("\n");
    return bytes;

    // /* use averaging? */
    // idx = (int)((float)rt_roundd_snf((double)idx / 4.0) * 4.0F +
    //     (float)fmod(idx, 4.0));
    // /* assuming 1 symbol = pi phase */
    // /* do the sampling */
    // for (Lg = 0; Lg < 120; Lg++) {
    //     if (y[((int)rt_roundd_snf((double)Lg * 4.2 * 4.0) + idx) + 8] >= 0.0) {
    //         /* rounding issues due to N? */
    //         output[Lg] = 1;
    //         /* for later infographic */
    //     }
    //     else {
    //         output[Lg] = 0;
    //         /* for later infographic */
    //     }
    // }
}