#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <liquid/liquid.h>

#include "sample_handling.h"
#include "dev_utils.h"
#include "config.h"

#include "packet_frame.h"

#if VERBOSE_ENABLED == 1
#define dev_printf(...) printf(__VA_ARGS__)
#else
#define dev_printf(...) stub()
#endif

#define PI 3.142857
#define IS_SIMULATION

int mls_total_preamble_length_bits;
int mls_raw_preamble_length_bits;
int N = 4;
extern int packet_data_length_with_fec_bytes;
float * MLS_array = NULL;

float findAutocorrelation(float * samples);
void chopFront(float ** data, int num_samples_to_chop_off, int length_samples);
float calcSignalPower(float * signal, int len);
uint16_t get_LIQUID_MSEQUENCE_GENPOLY(int order);
//populate static MLS
//consider integrating with preamble get function in packet_frame.c to avoid duplicate code
void initMLS(void){

    setMLSOrderBasedOnChannel();
	//options
	//TODO: Pick a good value for m
	unsigned int m = getMLSOrder();   // shift register length, n=2^m - 1
	mls_raw_preamble_length_bits = (pow(2,m) - 1); // preamble length
    mls_total_preamble_length_bits = mls_raw_preamble_length_bits + 1;

	// create and initialize m-sequence
	msequence ms = msequence_create_genpoly(get_LIQUID_MSEQUENCE_GENPOLY(m));//Fix these struct name definitions... Liquid maybe borked?
	//msequence_print(ms);
	unsigned int n = msequence_get_length(ms);

	// create and initialize first binary sequence on m-sequence
	bsequence mls = bsequence_create(n);
	bsequence_init_msequence(mls, ms);

    MLS_array = (float *)malloc(mls_raw_preamble_length_bits*sizeof(float));
    //non-flipped
    for (unsigned int i = 0; i < n; i++) {
        MLS_array[i] = (float)bsequence_index(mls, i);
    }
    //FLIP IT
    // for (unsigned int i = 0; i < n; i++) {
    //     MLS_array[n - 1 - i] = (float)bsequence_index(mls, i);
    // }

	// clean up memory
	bsequence_destroy(mls);
	msequence_destroy(ms);
}

uint16_t get_LIQUID_MSEQUENCE_GENPOLY(int order){
    switch(order){
        case (1 || 2):
            printf("invalid MLS order (too low)\n");
            exit(1);
        case 3:
            return LIQUID_MSEQUENCE_GENPOLY_M3;
        case 4:
            return LIQUID_MSEQUENCE_GENPOLY_M4;
        case 5:
            return LIQUID_MSEQUENCE_GENPOLY_M5;
        case 6:
            return LIQUID_MSEQUENCE_GENPOLY_M6;
        case 7:
            return LIQUID_MSEQUENCE_GENPOLY_M7;
        case 8:
            return LIQUID_MSEQUENCE_GENPOLY_M8;
        case 9:
            return LIQUID_MSEQUENCE_GENPOLY_M9;
        case 10:
            return LIQUID_MSEQUENCE_GENPOLY_M10;
        case 11:
            return LIQUID_MSEQUENCE_GENPOLY_M11;
        case 12:
            return LIQUID_MSEQUENCE_GENPOLY_M12;
        case 13:
            return LIQUID_MSEQUENCE_GENPOLY_M13;
        case 14:
            return LIQUID_MSEQUENCE_GENPOLY_M14;
        case 15:
            return LIQUID_MSEQUENCE_GENPOLY_M15;
        default:
            printf("invalid MLS order (too high)\n");
            exit(1);
    }
}

//syncFrame() changes "samples" array to start with the first sample after the MLS preamble
//NOTE Please run upsample before passing samples into here
//Consider divying up into smaller functions. Lots of functionality here
//Frees samples pointer input
//Outputs byte array of one frame of user payload data
uint8_t * syncFrame(float * samples, int length_samples_in, int * length_bytes_out, int frame_start_index_guess){

    *length_bytes_out = packet_data_length_with_fec_bytes;

    float * buffer = (float *)malloc((length_samples_in/(N*UPSAMPLE_RATE))*sizeof(float)); 
    float new_autocorr = 0;
    float prev_autocorr = 0;
    float max_autocorr = 0;
    int best_bank = 0;
    int best_shift_bits = 0;
    int max_shiftleft_bits = mls_total_preamble_length_bits/4;
    int max_shiftright_bits = mls_total_preamble_length_bits/2;
    int downhill_counter = 0;
    //printf("\nautocorrelation vals:\n");

    for(int i = 0; i<UPSAMPLE_RATE*N; i++){
        prev_autocorr = 0;
        for (int j = -max_shiftleft_bits; j<max_shiftright_bits; j++){

            for(int k = 0; k < mls_total_preamble_length_bits - 1; k++){// -2 because variable is meant to be multiple of 8
                buffer[k] = samples[frame_start_index_guess + i + (j + k)*N*UPSAMPLE_RATE];
            }

            new_autocorr = findAutocorrelation(buffer);

            if(new_autocorr > max_autocorr){
                if(new_autocorr < mls_raw_preamble_length_bits){//crude fix for ~inf autocor values. find root cause and improve fix
                    max_autocorr = new_autocorr;
                    best_bank = i;//best_sample_in_bit?
                    best_shift_bits = j;
                }
            }

            if(new_autocorr < prev_autocorr){
                downhill_counter++;
                if(downhill_counter > 7*8){//assumes 8 bytes of buffer between MLS and user data
                    break;
                }
            } else {
                downhill_counter = 0;
            }
            prev_autocorr = new_autocorr;
        }
    }

    dev_printf("Max autocorrelation = %f\n", max_autocorr);

    //Pick samples from selected bank and delay
    for (int i = 0; i < (*length_bytes_out*8 + mls_total_preamble_length_bits); i++){
        //Assign each element of buffer to be the selected sample for each bit (including preamble)
        buffer[i] = samples[frame_start_index_guess + best_bank + (best_shift_bits + i)*UPSAMPLE_RATE*N];
    }

    dev_printf("\nBest user data samples\n");
    for(int i = 0; i<100; i++){
        dev_printf("%.1f ", buffer[mls_total_preamble_length_bits + i]);
    }
    dev_printf("\n");

    chopFront(&buffer, mls_total_preamble_length_bits, length_samples_in/(N*UPSAMPLE_RATE));
    //length of samples should now be = length_bits_out

    shiftDownAndNormalizeSamples(&buffer, (*length_bytes_out)*8);

    uint8_t * output = samplesToBytes(buffer, (*length_bytes_out)*8);

    free(samples);
    free(buffer);

    return output;
}

//Constantly running function waiting for incoming data (from ADC) to pass signal threshold
//This will have to be called repeatedly so that successive frames can be received
//ENSURE frame_start_index_guess is scaled according to upsample rate in future function calls
//Todo get current_index to reset and not go infinite
float * getIncomingSignalData(float * ADC_output_float, int * frame_start_index_guess, int * output_length){

    int current_index = 0;
    float power_threshold = 0.1;//asuming input signal normalized to 1, avg of buffer signal values must be >power_threshold
    int buffersize = 10*N;//10 bits worth
    float * buffer = (float *)malloc(buffersize*sizeof(float)); 
    int stuffing_len = 1300*N;//Make dependent variable based on bitrate and fade erasure length, eventually

    *output_length = (packet_data_length_with_fec_bytes*8 + mls_total_preamble_length_bits)*N + buffersize + stuffing_len;//This is maybe throwing seg fault
    float * data = (float *)malloc((* output_length)*sizeof(float));

    while(1){

        if(current_index > *output_length){//might need tweak in future to be right length
            current_index = 0;
        }
        for (int i = 0; i < buffersize; i++){
            buffer[i] = ADC_output_float[current_index];//succ data from ADC Interface to fill whole buffer here:
            current_index++;
        }

        if(calcSignalPower(buffer, buffersize) > power_threshold){

            //write new data first so samples aren't missed
            for (int i = 0; i < (*output_length - buffersize - stuffing_len -68); i++){//figure out where -68 number comes from... but its needed to fix things
                if((ADC_output_float[current_index] < 2.0) && (ADC_output_float[current_index] > -2.0)){//TODO: fix this from accessing memory beyond array
                    data[buffersize + stuffing_len + i] = ADC_output_float[current_index];//fill in with actual function for ADC data succ
                    current_index++;
                } else {
                    data[buffersize + stuffing_len + i] = 0;
                    current_index++;
                }
            }
            //then copy buffer data to front
            for (int i = 0; i < buffersize; i++){
                data[stuffing_len + i] = buffer[i];
            }
            //then prepend a bunch of 0s in case we had a fade erasure at the start
            for (int i = 0; i < stuffing_len; i++){
                data[i] = 0;//should these be zeroes for best autocorrelation isolation?
            }

            *frame_start_index_guess = stuffing_len + buffersize/2;//11 bits due to zeroes at start of current mls
            break;
        }
    }

    free(buffer);
    free(ADC_output_float);

    dev_printf("Samples after power detector :\n");
    for (unsigned int i = 0; i < 50*3; i++) {
        dev_printf("%.0f", data[stuffing_len + i]);
    }
    dev_printf("\n");

    return data;
}

void chopFront(float ** data, int num_samples_to_chop_off, int length_samples){

    int length_samples_new = length_samples - num_samples_to_chop_off;
    float * buffer = (float *)malloc(length_samples_new*sizeof(float)); 
    for (int i = 0; i<length_samples_new; i++){
        buffer[i] = (*data)[i+num_samples_to_chop_off];
    }
    free(*data);

    *data = buffer;
}

//http://www.kempacoustics.com/thesis/node84.html
float findAutocorrelation(float * samples){

	float rxx_peak = 0;
    rxx_peak = 0;

    for (int i = 0; i < mls_raw_preamble_length_bits; i++){
        rxx_peak += samples[i]*MLS_array[i];
    }
    
    return (float)rxx_peak;
}


//Returns signal power value
float calcSignalPower(float * signal, int len){
    //Find formula and put here
    float totalpower = 0;
    for(int i = 0; i<len; i++){
        totalpower += signal[i]/(float)len;
    }
    return totalpower;
}