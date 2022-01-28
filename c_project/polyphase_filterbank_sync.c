#include <stdbool.h>
#include <math.h>
#include <complex.h>
#include <liquid/liquid.h>
#include <stdio.h>
#include <stdlib.h>
#include "samples_to_bits.h"

#define PI 3.142857

int num_banks = 4;
int mls_total_preamble_length_bits;
int number_of_mls_repititions;
int N = 4;
extern int packet_data_length_with_fec;

//ENSURE shiftDownAndNormalizeSamples IS RUN ON SAMPLES BEFORE THIS FUNCTION
//TODO eliminate extra buffer?
float * resampleInput(float* samplesin, int length_samples_in, int * length_samples_out) {

    float        r = 4;   // resampling rate (output/input) [TODO eliminate magic number]
    float        bw = 0.45f;  // resampling filter bandwidth (HMM)
    unsigned int npfb = 64;     // number of filters in bank (timing resolution)
    float slsl = 60;          // resampling filter sidelobe suppression level
    unsigned int h_len = 16;  // filter semi-length (filter delay)
    int filter_delay = 122;

    // create resampler
    resamp_crcf q = resamp_crcf_create(r, h_len, bw, slsl, npfb);

    float complex* complexbuffer = (float complex*)malloc(length_samples_in*r*sizeof(float complex)); 

    //unsigned int num_written = 0;   // number of values written to buffer this iteration
    unsigned int num_written_total = 0;

    resamp_crcf_execute_block(q, samplesin, length_samples_in, complexbuffer, &num_written_total);

    *length_samples_out = length_samples_in*r;
    if (num_written_total != *length_samples_out) {
        printf("Filter not executed properly");
    }

    // clean up allocated objects
    resamp_crcf_destroy(q);
    return (float *)complexbuffer;
}

//TODO: design such that it can be done on 101010... stream OR w/ MLS sync
//ASSUME INPUT SAMPLES ARRAY IS OF LENGTH 4 samples or more
//INPUT SAMPLES ARRAY MUST HAVE UNDERGONE UPSAMPLING
float determinePhaseOffset(float* samples)
{
    /* parameter definitions */
    
    int initial_n_offset = num_banks*round(N) + 1;

    /* num of symbols of filter */
    /* num of samples in symbol period */
   
    /* Create and apply polyphase filterbank */
    //Values generated via matlab
    unsigned int h_len = 49;  // filter order
    float fircoefs[49] = {
       -1.154575593983648E-17, -0.012730960239875706, -0.025712250513688212,
       -0.038094166414430156,  -0.048988416693311589, -0.057515241064219715,
       -0.062852167922348892,  -0.064281740012951435, -0.061235520866639484,
       -0.053331832980202207,  -0.040404965092938588, -0.022524006578241607,
       1.154575593983648E-17,  0.026619280501558328,  0.056566951130114,
       0.08888638830033703,    0.12247104173327902,   0.15611279717431065,
       0.18855650376704669,    0.21855791604403488,   0.244942083466558,
       0.266659164901011,      0.28283475565057009,   0.29281208551714133,
       0.29618386351056508,    0.29281208551714133,   0.28283475565057009,
       0.266659164901011,      0.244942083466558,     0.21855791604403488,
       0.18855650376704669,    0.15611279717431065,   0.12247104173327902,
       0.08888638830033703,    0.056566951130114,     0.026619280501558328,
       1.154575593983648E-17,  -0.022524006578241607, -0.040404965092938588,
       -0.053331832980202207,  -0.061235520866639484, -0.064281740012951435,
       -0.062852167922348892,  -0.057515241064219715, -0.048988416693311589,
       -0.038094166414430156,  -0.025712250513688212, -0.012730960239875706,
       -1.154575593983648E-17 };

    double buffer = 0;    // filter output buffer
    double banksum[36] = {0};

    //execute filter over 2 symbols?
    //TODO: somehow execute over larger area and do averaging?
    //TODO eliminate magic numbers
    for (int i = 0; i< 2*N; i++){
        for(int j = 0; j<num_banks; j++){
            for(int k = 0; k<49; k++){
                buffer = samples[i*num_banks + j + initial_n_offset + k]*fircoefs[k];
                banksum[i*num_banks + j] += buffer;
            }
        }
    }
    
    /* find phase offset */
    int maxval_location = 0;
    for (int i = 0; i < 36; i++){
        if (banksum[0] < banksum[maxval_location])
            maxval_location = i;
    }
    int best_sample = (int)round((float)maxval_location/(float)num_banks);
    int best_bank = maxval_location % num_banks;
    int n_offset = best_sample*N + best_bank;
    return ((float)n_offset/(float)(N*num_banks))*PI - PI/2;//assuming 1 symbol = pi phase
}


//CODE ABOVE HERE IS FOR STRICTLY POLYPHASE FILTERBANK IMPLEMENTATION
//CODE BELOW HERE IS FOR AUTOCORRELATOR IMPLEMENTATION

void chopFront(float ** data, int num_samples_to_chop_off, int length_samples){

    int length_samples_new = length_samples - num_samples_to_chop_off;
    float * buffer = (float *)malloc(length_samples_new*sizeof(float)); 
    for (int i = 0; i<length_samples_new; i++){
        buffer[i] = *data[i+num_samples_to_chop_off];
    }
    free(*data);

    *data = buffer;
}

//http://www.kempacoustics.com/thesis/node84.html
//Unclear exactly how liquid code works. Needs testing and analysis
float findAutocorrelation(float * samples){

    unsigned int n = mls_total_preamble_length_bits;        // autocorr window length
    unsigned int delay = n/2;    // autocorr overlap delay

    // create autocorrelator object
    autocorr_cccf q = autocorr_cccf_create(n,delay);//Does this need to be done every time?

    float complex rxx[n];          // output auto-correlation

    // compute auto-correlation
    for (int i=0; i<n; i++) {
        autocorr_cccf_push(q,samples[i]);
        autocorr_cccf_execute(q,&rxx[i]);

        // normalize by energy (not sure if necessary)
        rxx[i] /= autocorr_cccf_get_energy(q);
    }

    // find peak
    float complex rxx_peak = 0;
    for (int i=0; i<n; i++) {
        if (i==0 || cabsf(rxx[i]) > cabsf(rxx_peak))
            rxx_peak = rxx[i];
    }

    // destroy autocorrelator object
    autocorr_cccf_destroy(q);
    return (float)rxx_peak;
}

//syncFrame() changes "samples" array to start with the first sample after the MLS preamble
//NOTE Please run upsample before passing samples into here
//Consider divying up into smaller functions. Lots of functionality here
//Frees samples pointer input
uint8_t * syncFrame(float * samples, int length_samples_in, int * length_bytes_out, int frame_start_index_guess){

    *length_bytes_out = length_samples_in/(8*N*num_banks) - mls_total_preamble_length_bits/8;//requires mls total length to be multiple of 8 bits


    float * buffer = (float *)malloc((length_samples_in/(N*num_banks))*sizeof(float)); 
    float new_autocorr = 0;
    float max_autocorr = 0;
    int best_bank = 0;
    int best_shift_bits = 0;
    int max_shiftleft_bits = 1305;
    int max_shiftright_bits = 10;

    for(int i = 0; i<num_banks*N; i++){
        for (int j = -max_shiftleft_bits; j<max_shiftright_bits; j++){

            for(int k = 0; k < mls_total_preamble_length_bits; k++){
                buffer[k] = samples[frame_start_index_guess + i + (j + k)*N*num_banks];
            }

            new_autocorr = findAutocorrelation(buffer);

            if(new_autocorr > max_autocorr){
                max_autocorr = new_autocorr;
                best_bank = i;
                best_shift_bits = j;
            }
        }
    }

    //Pick samples from selected bank and delay
    for (int i = 0; i<*length_bytes_out*8; i++){
        //Assign each element of buffer to be the selected sample for each bit (including preamble)
        buffer[i] = samples[frame_start_index_guess + best_bank + (best_shift_bits + i)*num_banks*N];
    }

    chopFront(&samples, mls_total_preamble_length_bits, length_samples_in/(N*num_banks));
    //length of samples should now be = length_bits_out

    uint8_t * output = samplesToBytes(samples, (*length_bytes_out)*8, 0);

    free(samples);
    free(buffer);

    return output;
}

//Returns signal power value
float calcSignalPower(float * signal, int len){
    //Find formula and put here
    float totalpower;
    for(int i = 0; i<len; i++){
        totalpower += signal[i]/(float)len;
    }
    return totalpower;
}


//Constantly running function waiting for incoming data (from ADC) to pass signal threshold
//This will have to be called repeatedly so that successive frames can be received
//Return value is pointer to float array with length (packet_data_length_with_fec + mls_total_preamble_length_bits)*N + buffersize + stuffing_len
//ENSURE frame_start_index_guess is scaled according to upsample rate in future function calls
float * getIncomingSignalData(int * frame_start_index_guess){
    
    float power_calcd = 0;
    float power_threshold = 0.1;//asuming input signal normalized to 1, avg of buffer signal values must be >power_threshold
    int buffersize = 10*N;//10 bits worth
    float * buffer = (float *)malloc(buffersize*sizeof(float)); 
    int stuffing_len = 1300*N;

    int data_array_size = (packet_data_length_with_fec + mls_total_preamble_length_bits)*N + buffersize + stuffing_len;//TODO
    float * data = (float *)malloc(data_array_size*sizeof(float));

    while(1){
        //succ data from ADC Interface to fill whole buffer here:
        for (int i = 0; i < buffersize; i++){
            data[i] = succDataSample();
        }
        //(here)
        if(calcSignalPower(buffer, buffersize) > power_threshold){
            //write new data first so samples aren't missed
            for (int i = 0; i < data_array_size; i++){
                data[buffersize + stuffing_len+ i] = succDataSample();//fill in with actual function for ADC data succ
            }
            //then copy buffer data to front
            for (int i = 0; i < buffersize; i++){
                data[stuffing_len + i] = buffer[i];
            }
            //then prepend a bunch of 0s in case we had a fade erasure at the start
            for (int i = 0; i < stuffing_len; i++){
                data[i] = 0;//should these be zeroes for best autocorrelation isolation?
            }

            *frame_start_index_guess = stuffing_len + buffersize/2;
            break;
        }
    }
    free(buffer);
    return data;
}