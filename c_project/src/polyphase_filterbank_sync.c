#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <liquid/liquid.h>

#include "samples_to_bits.h"
#include "dev_utils.h"
#include "config.h"

#include "packet_frame.h"

#if VERBOSE_ENABLED == 1
#define dev_printf(...) printf(__VA_ARGS__)
#else
#define dev_printf(...) stub()
#endif

#ifdef  __cplusplus
   using cfloat = std::complex<float>;
#else
   typedef float complex cfloat;
#endif


#define PI 3.142857
#define IS_SIMULATION

int num_banks = 4;
int mls_total_preamble_length_bits;
int mls_raw_preamble_length_bits;
int N = 4;
extern int packet_data_length_with_fec_bytes;

float * MLS_array = NULL;

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


	// printf("Generated MLS bits\n");
	// for(int i = 0; i < 50; i++){
	// 	printf("%d ", bitbuffer[i]);
	// }

	// clean up memory
	bsequence_destroy(mls);
	msequence_destroy(ms);
}

//ENSURE shiftDownAndNormalizeSamples IS RUN ON SAMPLES BEFORE THIS FUNCTION - not sure about this Jan 29
//Frees input pointer
float * resampleInput(float* samplesin, int length_samples_in, int * length_samples_out) {

    float        r = num_banks;   // resampling rate (output/input) [TODO eliminate magic number]
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
        buffer[i] = (*data)[i+num_samples_to_chop_off];
    }
    free(*data);

    *data = buffer;
}

//http://www.kempacoustics.com/thesis/node84.html
float findAutocorrelation(float * samples){

	//TODO make variables use file globals
    //unsigned int n = 4095;        // Window length
    //unsigned int delay = n/2;    // Overlap delay
	float rxx_peak = 0;

	// printf("sequence:\n");
	// for(int i = 0; i <n; i++){
	// 	printf("%f ", samples[i]);
	// }
    // compute auto-correlation
    rxx_peak = 0;
    //josh (not liquid) style:
    for (int i = 0; i < mls_raw_preamble_length_bits; i++){
        rxx_peak += samples[i]*MLS_array[i];
    }

    //printf("%f ", rxx_peak);
    
    return (float)rxx_peak;
}

//syncFrame() changes "samples" array to start with the first sample after the MLS preamble
//NOTE Please run upsample before passing samples into here
//Consider divying up into smaller functions. Lots of functionality here
//Frees samples pointer input
//Outputs byte array of one frame of user payload data
uint8_t * syncFrame(float * samples, int length_samples_in, int * length_bytes_out, int frame_start_index_guess){

    *length_bytes_out = packet_data_length_with_fec_bytes;


    float * buffer = (float *)malloc((length_samples_in/(N*num_banks))*sizeof(float)); 
    float new_autocorr = 0;
    float prev_autocorr = 0;
    float max_autocorr = 0;
    int best_bank = 0;
    int best_shift_bits = 0;
    int max_shiftleft_bits = 100;
    int max_shiftright_bits = 200;
    int downhill_counter = 0;
    //printf("\nautocorrelation vals:\n");

    for(int i = 0; i<num_banks*N; i++){
        prev_autocorr = 0;
        for (int j = -max_shiftleft_bits; j<max_shiftright_bits; j++){

            for(int k = 0; k < mls_total_preamble_length_bits - 1; k++){// -2 because variable is meant to be multiple of 8
                buffer[k] = samples[frame_start_index_guess + i + (j + k)*N*num_banks];
            }

            new_autocorr = findAutocorrelation(buffer);

            //for testing
            // if(j> -10 && j < 10){
            //     if(i == 4){
            //         printf("%.2f ", new_autocorr);
            //     }
            // }

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
        buffer[i] = samples[frame_start_index_guess + best_bank + (best_shift_bits + i)*num_banks*N];
    }

    dev_printf("\nBest user data samples\n");
    for(int i = 0; i<100; i++){
        dev_printf("%.1f ", buffer[mls_total_preamble_length_bits + i]);
    }
    dev_printf("\n");

    chopFront(&buffer, mls_total_preamble_length_bits, length_samples_in/(N*num_banks));
    //length of samples should now be = length_bits_out

    // printf("Samples before conversion to bits:\n");
    // for (unsigned int i = 0; i < 50; i++) {
    //     printf("%.2f  ", buffer[i]);
    // }
    // printf("\n\n");

    shiftDownAndNormalizeSamples(&buffer, (*length_bytes_out)*8);

    uint8_t * output = samplesToBytes(buffer, (*length_bytes_out)*8);

    free(samples);
    free(buffer);

    return output;
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




// float getNextSample(float * input){
//     return input[nextsample_counter];
//     nextsample_counter++;
// }
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
