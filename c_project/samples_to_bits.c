#include "samples_to_bits.h"

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

// bool samplesToBytes(float* samples, int* num_bytes_to_convert, uint8_t* data, float phase_offset) {


//     // /* use averaging? */
//     // idx = (int)((float)rt_roundd_snf((double)idx / 4.0) * 4.0F +
//     //     (float)fmod(idx, 4.0));
//     // /* assuming 1 symbol = pi phase */
//     // /* do the sampling */
//     // for (Lg = 0; Lg < 120; Lg++) {
//     //     if (y[((int)rt_roundd_snf((double)Lg * 4.2 * 4.0) + idx) + 8] >= 0.0) {
//     //         /* rounding issues due to N? */
//     //         output[Lg] = 1;
//     //         /* for later infographic */
//     //     }
//     //     else {
//     //         output[Lg] = 0;
//     //         /* for later infographic */
//     //     }
//     // }
// }


// copying most recent Josh's code!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


//Samples out (returned by pointer) are straight up 1s and zeroes
//Must be un-malloc'd
//Frees input data
//Big endian: high bit transfered first
//phaseshift must be float type with value between 0 and 2*pi. Only phase shifts to the right (cuts off MSB)
float * bytestreamToSamplestream(uint8_t* data, int length_bytes, float phaseshift){
    int samples_per_bit = 4;
    int length_samples = length_bytes*8*samples_per_bit;
    float *samples = (float*)malloc((length_samples) * sizeof(float));

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
    // for (unsigned int i = 0; i < output_length; i++) {
    //     printf("%.0f", samples[i]);
    // }
    // printf("\n\n");

    int num_repititons = 10;//Phase shift resolution = 2*pi/(N*num_repititions)
    float *temp = (float*)malloc((length_samples)*num_repititons * sizeof(float));

    //repeat elements
    for (int i = 0; i < length_samples; i++){
        for(int j = 0; j<num_repititons; j++){
            temp[i*num_repititons + j] = samples[i];
        }
    }

    //shift and downsample
    int phaseshift_number = round(phaseshift/(PI*(float)2)*(float)(num_repititons*samples_per_bit));//Not certain about this
    for (int i = 0; i < length_samples; i++){
        samples[i] = temp[i*num_repititons + phaseshift_number];
    }

    // printf("Samples after phase shift:\n");
    // for (unsigned int i = 0; i < length_samples; i++) {
    //     printf("%.0f", samples[i]);
    // }
    // printf("\n\n");

    //data);
    free(temp);
    return samples;
}