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

Wavegen wavegen;
Scope scope;
HDWF hdwf;
char szError[512] = {0};
int bufoutlen = (8000+1);//yuck


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
    int datalength = 8000;
    int delaylength = 8025;

    int length = datalength + delaylength;
    int channel = 1;
    FUNC function = funcCustom;
    double offset = 0;
    double txfreq_hz = 1000000.0;
    double txfrequency = (txfreq_hz)/((double)length + 1.0);//+1 for trigger bit
    double amplitude = 5.0;
    double symmetry = 0;
    double wait = 0;
    double run_time = 0;
    int repeat = 0;
    vector<double> vect;
    
    scope_data rxdata;
    double rxfrequency = 1000000.0;
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
    for(int i = 0; i < (datalength+1); i++){//+1 to account for imperfect offset
        rxdata_trimmed.push_back(rxdata.buffer[i]);
    }

    //should print one frame, including trigger and two extra bits (should show second frame trigger too)
    printf("Rx buffer = \n\n");
    for (auto i: rxdata.buffer){
        std::cout << i << ' ';
    }
    printf("\n");
        
    printf("Trimmed Rx buffer = \n\n");
    for (auto i: rxdata_trimmed){
        std::cout << i << ' ';
    }
}


//Input length must be 8000 because of 8192 buffer size!!!
float * loopbackOneBuffer(float * input, int * outputlen){


    int delaylength = 8000;
    int inputlen = 8000;

    int length = inputlen + delaylength;
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
    double rxfrequency = 1000000.0;
    int rxbuffersize = (int)(((float)length))*(rxfrequency/txfreq_hz);
    double amplitude_range = 6.0;

    //add start condition then delay
    vect.push_back(1);
    for(int i = 0; i<delaylength; i++){
        vect.push_back(0);
    }

    //populate data
    for(int i = 0; i < inputlen; i++){
        vect.push_back((double)(input[i])*3.3/5.0);
    }

    float triggerlevel = 4.5;
    bool edge_rising = true;
    double timeout = 0.0;


    // printf("DAC data = \n\n");
    // for (int i = 0; i < inputlen; i++){
    //     printf("%.2f ", input[i]);
    // }
    // printf("\n");

    // printf("Tx buffer = \n\n");
    // for (auto i: vect){
    //     std::cout << i << ' ';
    // }
    // printf("\n");
    printf("\n\noutput right before dac\n");
    for(int j = 0; j < 100; j++){
        printf("%.2f ", vect[delaylength+1+j]);
    }

    

    //consider moving these calls to an init
    scope.open(hdwf, rxfrequency, rxbuffersize, offset, amplitude_range);
    scope.trigger(hdwf, true, scope.trigger_source.analog, channel, timeout, edge_rising, triggerlevel);

    wavegen.generate(hdwf,channel, function, offset, txfrequency, amplitude, symmetry, wait, run_time, repeat, vect);

    rxdata = scope.record(hdwf, channel, rxfrequency, rxbuffersize);

    scope.close(hdwf);

    // printf("Rx buffer = \n\n");
    // for (auto i: rxdata.buffer){
    //     std::cout << i << ' ';
    // }
    // printf("\n");
    printf("\n\noutput right before after adc\n");
    for(int j = 0; j < 100; j++){
        printf("%.2f ", rxdata.buffer[j]);
    }
    

    *outputlen = (inputlen+1);

    float * rxdata_trimmed = (float *)malloc(sizeof(float)*(*outputlen));
    for(int i = 0; i < *outputlen; i++){//+1 to account for imperfect offset
        rxdata_trimmed[i] = (float)(rxdata.buffer[i]);
    }

    printf("\n\noutput after variable changen");
    for(int j = 0; j < 100; j++){
        printf("%.2f ", rxdata_trimmed[j]);
    }

    return rxdata_trimmed;

}

float * sendAnalogLoopback(float * input, int inputlen, int * outputlen){

    // printf("DAC data = \n\n");
    // for (int i = 0; i < inputlen; i++){
    //     printf("%.2f ", input[i]);
    // }
    // printf("\n");

    int bufinlen = 8000;
    int repititions = inputlen/bufinlen + 1;
    *outputlen = bufoutlen*repititions;
    float * output = (float *)malloc(sizeof(float)*(*outputlen));

    for(int i = 0; i < repititions; i++){
        if(i == 0){
            printf("input\n");
            for(int j = 0; j < 100; j++){
                printf("%.2f ", input[j]);
            }
        }
        float * bufout = NULL;
        bufout = loopbackOneBuffer(input + bufinlen*i, &bufoutlen);
        for(int j = 0; j < bufoutlen; j++){
            output[i*bufoutlen + j] = bufout[j];
        }
        if(i == 0){
            printf("\n\noutput\n");
            for(int j = 0; j < 100; j++){
                printf("%.2f ", output[j]);
            }
        }
        free(bufout);
    }

    //normalize
    for(int i = 0; i < *outputlen; i++){
        output[i] /= 3.3;
    }

    // printf("DAC data = \n\n");
    // for (int i = 0; i < inputlen; i++){
    //     printf("%.2f ", input[i]);
    // }
    // printf("\n");

    return output;
}

void initAD2(void){
    // connect to the device
    printf("Open automatically the first available device\n");
    if(!FDwfDeviceOpen(-1, &hdwf)) {
        FDwfGetLastErrorMsg(szError);
        printf("Device open failed\n\t%s", szError);
    }
    
}