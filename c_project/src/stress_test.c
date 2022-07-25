#include "channel.h"
#include "func_test.h"
#include <stdlib.h>
#include "plot.h"

extern int snr_db;
extern int fade_freq;
extern int fade_len;
extern int burst_freq;
extern int burst_len;

//TODO List
//Should all of these tests have dynamic phase between packets? How to implement that...
//Determine how to do continuous tx/rx instead of packet-by-packet
//How might I simulate clock drift over time...
//Ensure all phases can sync properly in func_test.c
//Probably do all tests @ 2 samp/bit? Any advantage to 4?


//Transfer test image repeatedly, varying SNR by 1dB until no more data received
void testSNR(){
    int max_snr = 20;
    int min_snr = 2;
    float * dropped_packets_percent = (float *)malloc(max_snr*sizeof(float));
    float * bad_bits_percent = (float *)malloc(max_snr*sizeof(float));
    for(int i = max_snr; i>min_snr; i--){
        snr_db = i;
        dropped_packets_percent[i] = imageSendTest("../testdata/engphys.ppm");
        bad_bits_percent[i] = checkEfficacy();
    }
    plotFloatStream(dropped_packets_percent, max_snr, "Fraction of Dropped Packets vs SNR");
    plotFloatStream(bad_bits_percent, max_snr, "Fraction of Incorrect Bits vs SNR");
    free(dropped_packets_percent);
    free(bad_bits_percent);
}

//Plot a matrix of number of missed packets or unrecovered data as a factor of both fade length and frequency
//Compare plot to actual data rate
void testFades(){
    int min_len = 1;//us
    int max_len = 500;
    int min_freq = 100;
    int max_freq = 600;
    int iterations = 5;
    int index = 0;
    float * dropped_packets_percent = (float *)malloc((max_len/iterations)*(max_freq/iterations)*sizeof(float));//matrix where each row = max_len/iterations long
    float * bad_bits_percent = (float *)malloc((max_len/iterations)*(max_freq/iterations)*sizeof(float));
    for(int i = min_len; i < max_len; i += max_len/iterations){
        for(int j = min_freq; j < max_freq; j += max_freq/iterations){
            fade_len = i;
            fade_freq = j;
            dropped_packets_percent[index] = imageSendTest("../testdata/engphys.ppm");
            bad_bits_percent[index] = checkEfficacy();
            index++;
        }
    }
    plotContour(dropped_packets_percent, iterations, iterations, max_len/iterations, "Fraction of Dropped Packets vs Fade Len and Freq");
    plotContour(bad_bits_percent, iterations, iterations, max_len/iterations, "Fraction of Incorrect Bits vs Fade Len and Freq");
    free(dropped_packets_percent);
    free(bad_bits_percent);
}


//Incomplete... finish once bursts enabled
void testBursts(){
    int min_len = 1;//us
    int max_len = 10000;
    int min_freq = 100;
    int max_freq = 2000;
    int iterations = 20;
    int index = 0;
    float * dropped_packets_percent = (float *)malloc((max_len/iterations)*(max_freq/iterations)*sizeof(float));//matrix where each row = max_len/iterations long
    float * bad_bits_percent = (float *)malloc((max_len/iterations)*(max_freq/iterations)*sizeof(float));
    for(int i = min_len; i < max_len; i + max_len/iterations){
        for(int j = min_freq; j < max_freq; j + max_freq/iterations){
            burst_len = i;
            burst_freq = j;
            dropped_packets_percent[index] = imageSendTest("../testdata/engphys.ppm");
            bad_bits_percent[index] = checkEfficacy();
            index++;
        }
    }
    plotContour(dropped_packets_percent, iterations, iterations, max_len/iterations, "Fraction of Dropped Packets vs Burst Len and Freq");
    plotContour(bad_bits_percent, iterations, iterations, max_len/iterations, "Fraction of Dropped Packets vs Burst Len and Freq");
    free(dropped_packets_percent);
    free(bad_bits_percent);
}

//wrapper
void testSpecialCases(){
    testSyncEdgeCases();
}

void testLDPC(){
    getFECDataLengths();
    fullSendTest();
}