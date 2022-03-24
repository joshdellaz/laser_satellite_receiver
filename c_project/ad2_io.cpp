extern "C" {
#include <digilent/waveforms/dwf.h>
}
#include "wavegen.h"
#include "scope.h"

#include <iostream>
#include <vector>
using namespace std;

#include <unistd.h>
#include <sys/time.h>
#define Wait(ts) usleep((int)(1000000*ts))

void sendInfWaveform(void){
    Wavegen wavegen;
    Scope scope;

    HDWF hdwf;
    char szError[512] = {0};

    // connect to the device
    printf("Open automatically the first available device\n");
    if(!FDwfDeviceOpen(-1, &hdwf)) {
        FDwfGetLastErrorMsg(szError);
        printf("Device open failed\n\t%s", szError);
    }


    int length = 100;

    int channel = 1;
    FUNC function = funcCustom;
    double offset = 0;
    double txfreq_hz = 1000000.0;
    double txfrequency = (txfreq_hz)/((float)length);//note this may be frequency of the repeated entire pattern
    double amplitude = 1;
    double symmetry = 0;
    double wait = 0;
    double run_time = 0;
    int repeat = 0;
    vector<double> vect;

    scope_data rxdata;
    double rxfrequency = 4000000.0;
    int rxbuffersize = 8192;
    double amplitude_range = 2;


    for(int i = 0; i < length; i++){
        if(i % 2 == 0){
            vect.push_back(0);
        } else {
            vect.push_back(1);
        }
    }

    scope.open(hdwf, rxfrequency, rxbuffersize, offset, amplitude_range);

    wavegen.generate(hdwf,channel, function, offset, txfrequency, amplitude, symmetry, wait, run_time, repeat, vect);

    rxdata = scope.record(hdwf, channel, rxfrequency, rxbuffersize);
    scope.close(hdwf);

    for (auto i: rxdata.buffer)
        std::cout << i << ' ';
}