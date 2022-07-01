#include "channel.h"
#include "func_test.h"
#include <stdlib.h>
#include "plot.h"

extern int snr_db;
extern int fade_freq;
extern int fade_len;

//TODO List
//Should all of these tests have dynamic phase between packets? How to implement that...
//Determine how to do continuous tx/rx instead of packet-by-packet
//How might I simulate clock drift over time...
//Ensure all phases can sync properly in func_test.c
//Probably do all tests @ 2 samp/bit? Any advantage to 4?


//Transfer test image repeatedly, varying SNR by 1dB until no more data received
void testSNR(){
    int max_snr = 20;
    float * successrate = (float *)malloc(max_snr*sizeof(float));
    for(int i = max_snr; i>0; i--){
        snr_db = i;
        successrate[i] = imageSendTest("../testdata/engphys.ppm");
    }
    
    free(successrate);
}

//Plot a matrix of number of missed packets or unrecovered data as a factor of both fade length and frequency
//Compare plot to actual data rate
//Same thing for bursts
void testFades(){

}
void testBursts(){

}

//
void testSpecialCases(){

}

void testAll(){

}