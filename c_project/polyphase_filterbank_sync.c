#include <stdbool.h>
#include <math.h>
#include <liquid/liquid.h>
#include <stdio.h>

#define PI 3.142857


//For now, only works on fixed-length chunks (128 B * 4 = 512 samples in, cuz stack)
//Samples out is 512*4 in length
//ENSURE shiftDownAndNormalizeSamples IS RUN ON SAMPLES BEFORE THIS FUNCTION
bool resampleInput(float* samplesin, float ** samplesout) {

    float        r = 4;   // resampling rate (output/input) [TODO eliminate magic number]
    float        bw = 0.45f;  // resampling filter bandwidth (HMM)
    unsigned int npfb = 64;     // number of filters in bank (timing resolution)
    float slsl = -60.0f;          // resampling filter sidelobe suppression level
    unsigned int n = 512;    // number of input samples
    unsigned int h_len = 16;  // filter semi-length (filter delay)

    // create resampler
    resamp_crcf q = resamp_crcf_create(r, h_len, bw, slsl, npfb);

    //float y[512*4];         // output buffer
    unsigned int num_written;   // number of values written to buffer

    // ... initialize input ...

    // execute resampler, storing result in output buffer
    resamp_crcf_execute(q, *samplesin, *samplesout, &num_written);//NOTE POINTERS MIGHT BE WRONG
    if (num_written != 512 * 4) {
        printf("Filter not executed properly");
    }

    // clean up allocated objects
    resamp_crcf_destroy(q);
}

//TODO: design such that it can be done on 101010... stream OR w/ MLS sync
//ASSUME INPUT SAMPLES ARRAY IS OF LENGTH 4 samples or more
//INPUT SAMPLES ARRAY MUST HAVE UNDERGONE UPSAMPLING
float determinePhaseOffset(float* samples)
{
    /* parameter definitions */
    int num_banks = 4;
    int N = 4;
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

    // create filter object
    // firfilt_crcf q = firfilt_crcf_create(fircoefs,h_len);

    double buffer = 0;    // filter output buffer
    double banksum[36] = {0};

    //execute filter over 2 symbols?
    //TODO: somehow execute over larger area and do averaging?
    //TODO eliminate magic numbers
    for (int i = 0; i< 2*N; i++){
        for(int j = 0; j<num_banks; j++){
            for(int k = 0; k<49; k++){
                // firfilt_crcf_push(q, samples[i*num_banks + j + initial_n_offset + k]); // push input sample
                // firfilt_crcf_execute(q,&buffer); // compute output
                buffer = samples[i*num_banks + j + initial_n_offset + k]*fircoefs[k];
                banksum[i*num_banks + j] += buffer;
            }
        }
    }


    // destroy filter object
    // firfilt_crcf_destroy(q);

    
    /* find phase offset */
    int maxval_location = 0;
    for (int i = 0; i < 36; i++){
        if (banksum[0] < banksum[maxval_location])
            maxval_location = i;
    }
    int best_sample = (int)round((float)maxval_location/(float)num_banks);
    int best_bank = mod(maxval_location, num_banks);
    int n_offset = best_sample*N + best_bank;
    return ((float)n_offset/(float)(N*num_banks))*PI - PI/2;//assuming 1 symbol = pi phase
}


//MATLAB Generated Code- OBSELETE
//
//
//bool resample(float* samples) {
//
//    float        r = 4;   // resampling rate (output/input) [TODO eliminate magic number]
//    unsigned int m = 13;     // resampling filter semi-length (filter delay)
//    float        As = 60.0f;  // resampling filter stop-band attenuation [dB]
//    float        bw = 0.45f;  // resampling filter bandwidth
//    unsigned int npfb = 64;     // number of filters in bank (timing resolution)
//    unsigned int n = 400;    // number of input samples
//    float        fc = 0.044f; // complex sinusoid frequency
//}
//
//bool determinePhaseOffset(float* samples, float phase_offset)//design such that it can be done on 101010... stream OR w/ MLS sync
//{
//    static const double fircoefs[49] = {
//        -1.154575593983648E-17, -0.012730960239875706, -0.025712250513688212,
//        -0.038094166414430156,  -0.048988416693311589, -0.057515241064219715,
//        -0.062852167922348892,  -0.064281740012951435, -0.061235520866639484,
//        -0.053331832980202207,  -0.040404965092938588, -0.022524006578241607,
//        1.154575593983648E-17,  0.026619280501558328,  0.056566951130114,
//        0.08888638830033703,    0.12247104173327902,   0.15611279717431065,
//        0.18855650376704669,    0.21855791604403488,   0.244942083466558,
//        0.266659164901011,      0.28283475565057009,   0.29281208551714133,
//        0.29618386351056508,    0.29281208551714133,   0.28283475565057009,
//        0.266659164901011,      0.244942083466558,     0.21855791604403488,
//        0.18855650376704669,    0.15611279717431065,   0.12247104173327902,
//        0.08888638830033703,    0.056566951130114,     0.026619280501558328,
//        1.154575593983648E-17,  -0.022524006578241607, -0.040404965092938588,
//        -0.053331832980202207,  -0.061235520866639484, -0.064281740012951435,
//        -0.062852167922348892,  -0.057515241064219715, -0.048988416693311589,
//        -0.038094166414430156,  -0.025712250513688212, -0.012730960239875706,
//        -1.154575593983648E-17 };
//    
//    static const signed char iv[20] = { 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
//                                       0, 1, 0, 1, 0, 1, 0, 1, 0, 1 };
//    double input_samples[5040];
//    double y[2030];
//    double shiftedinput[2016];
//    double a__1[1008];
//    double varargin_1[504];
//    double input_data[120];
//    double b_r[100];
//    double bankout[49];
//    double banksum[36];
//    double accum;
//    double inphi_idx;
//    double inplo_idx;
//    double out_idx;
//    int Lg;
//    int filthi_idx;
//    int idx;
//    int k;
//    int r;
//    int tmp_filtlo_idx;
//    signed char output[120];
//    bool b_p;
//    bool exitg1;
//    bool p;
//    /* parameter definitions */
//    /* clear */
//    /* in rads */
//    /* Can't have more than 1 decimal!!! Number of samples per bit */
//    /* num of symbols of filter */
//    /* num of samples in symbol period */
//    /* generate square wave w/ 20 bit alternation and then random sequence */
//    memset(&input_data[0], 0, 120U * sizeof(double));
//    for (r = 0; r < 20; r++) {
//        input_data[r] = iv[r];
//    }
//    /*b_rand(b_r);*/
//    for (k = 0; k < 100; k++) {
//        inphi_idx = floor(b_r[k] * 2.0);
//        b_r[k] = inphi_idx;
//        input_data[k + 20] = inphi_idx;
//    }
//    /* Apply appropriate sample rate */
//    idx = -1;
//    for (k = 0; k < 120; k++) {
//        for (filthi_idx = 0; filthi_idx < 42; filthi_idx++) {
//            input_samples[(idx + filthi_idx) + 1] = input_data[k];
//        }
//        idx += 42;
//    }
//    /* if shift > half of sample, first sample is shifted by one */
//
//
//   
//    /* Create and apply polyphase filterbank */
//    for (Lg = 0; Lg < 9; Lg++) {
//        for (filthi_idx = 0; filthi_idx < 4; filthi_idx++) {
//            idx = (Lg << 2) + filthi_idx;
//            memcpy(&shiftedinput[0], &y[9], 2016U * sizeof(double));
//            tmp_filtlo_idx = idx + 17;
//            memset(&a__1[0], 0, 1008U * sizeof(double));
//            if (0 <= tmp_filtlo_idx) {
//                memcpy(&a__1[0], &y[9], (tmp_filtlo_idx + 1) * sizeof(double));
//            }
//            r = 1997 - idx;
//            for (k = 0; k <= r; k++) {
//                shiftedinput[k] = shiftedinput[(k + idx) + 18];
//            }
//            for (k = 0; k <= tmp_filtlo_idx; k++) {
//                shiftedinput[(k - idx) + 1998] = a__1[k];
//            }
//            for (k = 0; k < 49; k++) {
//                bankout[k] = fabs(shiftedinput[k] * fircoefs[k]);
//            }
//            inphi_idx = bankout[0];
//            for (k = 0; k < 48; k++) {
//                inphi_idx += bankout[k + 1];
//            }
//            banksum[idx] = inphi_idx;
//        }
//    }
//    /* find phase offset */
//    if (!isnan(banksum[0])) {
//        idx = 1;
//    }
//    else {
//        idx = 0;
//        k = 2;
//        exitg1 = false;
//        while ((!exitg1) && (k < 37)) {
//            if (!isnan(banksum[k - 1])) {
//                idx = k;
//                exitg1 = true;
//            }
//            else {
//                k++;
//            }
//        }
//    }
//    if (idx == 0) {
//        idx = 1;
//    }
//    else {
//        inplo_idx = banksum[idx - 1];
//        r = idx + 1;
//        for (k = r; k < 37; k++) {
//            inphi_idx = banksum[k - 1];
//            if (inplo_idx > inphi_idx) {
//                inplo_idx = inphi_idx;
//                idx = k;
//            }
//        }
//    }
//}