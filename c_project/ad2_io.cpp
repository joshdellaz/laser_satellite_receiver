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


//Because output repeats uncontrollably, a trigger condition is inserted at the start of each frame.
//The scope receives data starting at this trigger of specified length
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

    //change this to test
    int datalength = 50;
    int delaylength = 51;

    int length = datalength + delaylength;
    int channel = 1;
    FUNC function = funcCustom;
    double offset = 0;
    double txfreq_hz = 1000000.0;
    double txfrequency = (txfreq_hz)/((double)length + 1.0);//+1 for trigger bit
    double amplitude = 5;
    double symmetry = 0;
    double wait = 0;
    double run_time = 0;
    int repeat = 1;
    vector<double> vect;
    
    scope_data rxdata;
    double rxfrequency = 4000000.0;
    int rxbuffersize = (int)(((float)length))*(rxfrequency/txfreq_hz);
    double amplitude_range = 6.0;

    //add start condition then delay
    vect.push_back(1);
    for(int i = 0; i<delaylength; i++){
        vect.push_back(0);
    }



    //populate data
    for(int i = 0; i < datalength; i++){
        if(i % 2 == 0){
            vect.push_back(3.3/5.0);//scaled to 0-1 range
        } else {
            vect.push_back(0);
        }
    }

    float triggerlevel = 4.5;
    bool edge_rising = true;
    double timeout = 0.0;

    //consider moving these calls to an init
    scope.open(hdwf, rxfrequency, rxbuffersize, offset, amplitude_range);
    scope.trigger(hdwf, true, scope.trigger_source.analog, channel, timeout, edge_rising, triggerlevel);

    wavegen.generate(hdwf,channel, function, offset, txfrequency, amplitude, symmetry, wait, run_time, repeat, vect);

    rxdata = scope.record(hdwf, channel, rxfrequency, rxbuffersize);

    scope.close(hdwf);

    vector<double> rxdata_trimmed;
    for(int i = 0; i < (datalength+1)*4; i++){//+1 to account for imperfect offset
        rxdata_trimmed.push_back(rxdata.buffer[i]);
    }

    //should print one frame, including trigger and two extra bits (should show second frame trigger too)
    printf("Rx buffer = \n\n");
    for (auto i: rxdata.buffer){
        std::cout << i << ' ';
    }
        
    printf("Trimmed Rx buffer = \n\n");
    for (auto i: rxdata_trimmed){
        std::cout << i << ' ';
    }
}

float * sendAnalogLoopback(float * input, int inputlen, int * outputlen){
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

    int channel = 1;
    FUNC function = funcCustom;
    double offset = 0;
    double txfreq_hz = 1000000.0;
    double txfrequency = (txfreq_hz)/((float)inputlen);//note this may be frequency of the repeated entire pattern
    double amplitude = 3.3;
    double symmetry = 0;
    double wait = 0;
    double run_time = 0;
    int repeat = 0;
    vector<double> vect;

    scope_data rxdata;
    double rxfrequency = 4000000.0;
    int rxbuffersize = 8192;
    double amplitude_range = 4;

    scope.open(hdwf, rxfrequency, rxbuffersize, offset, amplitude_range);

    wavegen.generate(hdwf,channel, function, offset, txfrequency, amplitude, symmetry, wait, run_time, repeat, vect);

    rxdata = scope.record(hdwf, channel, rxfrequency, rxbuffersize);
    scope.close(hdwf);

    for (auto i: rxdata.buffer)
        std::cout << i << ' ';
}