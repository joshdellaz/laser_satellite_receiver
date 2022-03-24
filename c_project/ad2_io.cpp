extern "C" {
#include <digilent/waveforms/dwf.h>
}
#include "wavegen.h"

#include <iostream>
#include <vector>
using namespace std;

#include <unistd.h>
#include <sys/time.h>
#define Wait(ts) usleep((int)(1000000*ts))

void sendInfWaveform(void){
    Wavegen wavegen;

    HDWF hdwf;
    char szError[512] = {0};

    // connect to the device
    printf("Open automatically the first available device\n");
    if(!FDwfDeviceOpen(-1, &hdwf)) {
        FDwfGetLastErrorMsg(szError);
        printf("Device open failed\n\t%s", szError);
    }


    int length = 1028;

    int channel = 1;
    FUNC function = funcCustom;
    double offset = 0;
    double frequency = 1000000;
    double amplitude = 1;
    double symmetry = 0;
    double wait = 0;
    double run_time = 0;
    int repeat = 0;
    vector<double> vect;


    for(int i = 0; i < length; i++){
        if(i % 2 == 0){
            vect.push_back(0);
        } else {
            vect.push_back(1);
        }
    }


    wavegen.generate(hdwf,channel, function, offset, frequency, amplitude, symmetry, wait, run_time, repeat, vect);
}