#include <algorithm>
#include "laser_comms.h"
#include "packet_frame.h"
#include "LowRateLDPC.hpp"
#include <iostream>
#include <random>
#include <iomanip>
#include <math.h>
#include <vector>
#include <iterator>
#include <numeric>


using namespace std;
//namespace ranges = std::ranges;

vector<uint8_t> mul_sh(vector<uint8_t>, int);

extern "C" void encode_LDPC(uint8_t * input) 
{
    // load the H matrix
    int * h_pointer;
    h_pointer = & LR_LDPC::H_6656_1_5[0][0];
    int m = 42;
    int n = 52;
    int z = CODEWRD_L / n;
    vector<uint8_t> bits(CODEWRD_L, (uint8_t)0); // stores the bit array that will be encoded

    cout << "Message as a bit array:" << endl;
    uint8_t input_bit; // copying message into the bit array
    for (unsigned i_bit = 0; i_bit < CODEWRD_L; i_bit++) { // bit extraction
        input_bit = input[(int)i_bit / 8];
        input_bit >>= (7 - (i_bit % 8));
        bits.at(i_bit) = (uint8_t)(input_bit & 0X01);
        printf("%i", bits.at(i_bit));
    }
    printf("\n");

    // for (int i = 1; i <= m; i++) {
    //     for (int j = 1; j <= n; j++) {
    //         printf("%i,", h_pointer[(i-1)*n + j-1]); // confirmed working
    //     }
    //     printf("\n");
    // }
    
    cout << "H matrix is loaded; encoding in progress..." << endl;

    vector<uint8_t> temp(z, 0);

    cout << "Checking temp:" << endl;

    for (int i = 1; i <= 4; i++) {
        for (int j = 1; j <= n-m; j++) {
            vector<uint8_t> part_msg(bits.begin() + (j-1)*z, bits.begin() + j*z); // copy of the needed part of msg (input) 

            //printf("part_msg size: %i \n", part_msg.size());
            vector<uint8_t> addition = mul_sh(part_msg, h_pointer[(i-1)*n + j-1]);
            //printf("addition size: %i \n", addition.size());
            for (int p = 0; p < z; p++) {
                temp.at(p) = (temp.at(p) + addition.at(p)) % 2;
                //printf("%i,", temp.at(p)); // compare this with MATLAB result
            }
            //printf("\n");
        }
    }

    int p1_sh = h_pointer[(2-1)*n + n-m+1-1]; // -1 is not simplified for ease of comparison with matlab
    if (p1_sh == -1) {
        p1_sh = h_pointer[(3-1)*n + n-m+1-1];
    }

    vector<uint8_t> p1 = mul_sh(temp, z-p1_sh);
    for (int i = 0; i < z; i++) {
        bits.at((n-m-1)*z+1-1+i) = p1.at(i);
    }
}


extern "C" void decode_LDPC(uint8_t * rxinput) {
    
}


vector<uint8_t> mul_sh(vector<uint8_t> x, int k) { // confirmed working
    int l = x.size();
    vector<uint8_t> y(l, 0);
    if (k != -1) {
        for (int i = k; i < k + l; i++) {
            y.at(i - k) = x.at(i < l ? i : i - l);
        }
    }
    return y;
}