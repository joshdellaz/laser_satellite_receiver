#include "laser_comms.h"
#include "packet_frame.h"
#include "LowRateLDPC.hpp"
#include <iostream>
#include <random>
#include <iomanip>
#include <math.h>

using namespace std;


int* mul_sh(int*, int, int);

extern "C" void encode_LDPC(uint8_t * input) 
{
    // load the H matrix
    int * h_pointer;
    h_pointer = & LR_LDPC::H_6656_1_5[0][0];
    int m = 42;
    int n = 52;
    int z = CODEWRD_L / n;

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            printf("%i,", h_pointer[(i*n) + j]);
        }
        printf("\n");
    }
    
    cout << "H matrix is loaded; encoding in progress..." << endl;

    int temp[z] = { 0 };

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < n-m; j++) {
            // need mul_sh
        }
    }

}


extern "C" void decode_LDPC(uint8_t * rxinput) {
    
}

int * mul_sh(int* x, int len_x, int k) {
    int * y[len_x] = { 0 };
    if (k != -1) {
        // use vectors, should be easier
    }
}