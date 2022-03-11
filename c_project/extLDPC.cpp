#include "laser_comms.h"
#include "packet_frame.h"
#include "LowRateLDPC.hpp"
#include <iostream>
#include <random>
#include <iomanip>
#include <math.h>
#include <vector>


using namespace std;


vector<int> mul_sh(vector<int>, int);

extern "C" void encode_LDPC(uint8_t * input) 
{
    // load the H matrix
    int * h_pointer;
    h_pointer = & LR_LDPC::H_6656_1_5[0][0];
    int m = 42;
    int n = 52;
    int z = CODEWRD_L / n;

    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            printf("%i,", h_pointer[(i-1)*n + j-1]);
        }
        printf("\n");
    }
    
    cout << "H matrix is loaded; encoding in progress..." << endl;

    vector<int> temp(z, 0);

    cout << "Checking temp:" << endl;

    for (int i = 1; i <= 4; i++) {
        for (int j = 1; j <= n-m; j++) {
            // need mul_sh
            vector<int> part_msg(input + (j-1)*z, input + j*z); // copy of the needed part of msg (input) NEEDS FIXING
            //printf("part_msg size: %i \n", part_msg.size());
            vector<int> addition = mul_sh(part_msg, h_pointer[(i-1)*n + j-1]);
            //printf("addition size: %i \n", addition.size());
            for (int p = 0; p < z; p++) {
                printf("%i,", part_msg.at(p));
                temp.at(p) = (temp.at(p) + addition.at(p)) % 2;
                //printf("%i,", temp.at(p)); // compare this with MATLAB result
            }
            printf("\n");
        }
    }

}


extern "C" void decode_LDPC(uint8_t * rxinput) {
    
}


vector<int> mul_sh(vector<int> x, int k) {
    int l = x.size();
    vector<int> y(l, 0);
    if (k != -1) {
        for (int i = k; i < k + l; i++) {
            y.at(i - k) = x.at(i < l ? i : i - l);
        }
    }
    return y;
}