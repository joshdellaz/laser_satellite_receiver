#include <stdbool.h>
#include <math.h>
#include <complex.h>
#include <liquid/liquid.h>
#include <stdio.h>
#include <stdlib.h>

#define PI 3.142857

int num_banks = 4;
int mls_total_preamble_length_bits;
int number_of_mls_repititions;
int N = 4;

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

void chopFront(float ** data, int num_samples_to_chop_off, int length_samples){

    int length_samples_new = length_samples - num_samples_to_chop_off;
    float * buffer = (float *)malloc(length_samples_new*sizeof(float)); 
    for (int i = 0; i<length_samples_new; i++){
        buffer[i] = *data[i+num_samples_to_chop_off];
    }
    free(*data);

    *data = buffer;
}

//Currently assumes fixed-length preamble
//TODO eliminate above assumption
float findAutocorrelation(float * samples, int delay){
    //Move erik code here
}

//syncFrame() changes "samples" array to start with the first sample after the MLS preamble
//NOTE Please run upsample before passing samples into here
//Current design assumes entire preamble is intact
//TODO: eliminate above assumption
//Consider divying up into smaller functions. Lots of functionality here
//Frees samples pointer
uint8_t * syncFrame(float * samples, int length_samples_in, int * length_bytes_out){

    *length_bytes_out = length_samples_in/(8*N*num_banks) - mls_total_preamble_length_bits/8;//requires mls total length to be multiple of 8 bits

    float * buffer = (float *)malloc((length_samples_in/(N*num_banks))*sizeof(float)); 
    float new_autocorr = 0;
    float max_autocorr = 0;
    int best_bank = 0;
    int best_delay = 0;
    int max_delay_bits = 20;//TODO: determine best number, given burst erasure length and mls length

    for(int i = 0; i<num_banks*N; i++){
        for(int k = 0; k < mls_total_preamble_length_bits; k++){
            buffer[k] = samples[i + k*N*num_banks];
        }
        for (int j = -max_delay_bits; j<max_delay_bits; j++){
            new_autocorr = findAutocorrelation(buffer, j);
            if(new_autocorr > max_autocorr){
                max_autocorr = new_autocorr;
                best_bank = i;
                best_delay = j;
            }
        }
    }

    //TODO determine amount to chop based on best_bank and best_delay
    //Assume 2 MLS repititions
    chopFront(samples, mls_total_preamble_length_bits*N*num_banks, length_samples_in);

    //TODO pick only previously selected samples and figure out where to store them before passing to samplesToBytes()

    uint8_t * output = samplesToBytes(samples, (*length_bytes_out)*8*N*num_banks, 0);

    free(samples);
    free(buffer);

    return output;
}
