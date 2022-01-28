#include "laser_comms.h"
#include "packet_frame.h"
#include <iostream>
#include "tavildarLDPC/LdpcC/LdpcCode.h"
#include "tavildarLDPC/LdpcC/Constellation.h"
#include <random>
#include <iomanip> 
#include <math.h>


using namespace std;

// extern "C" void initLDPC(LdpcCode * ldpc_scheme) {

// }

extern "C" void applyLDPC(uint8_t* input) {
    std::cout << "Starting LDPC encoding...\n";
    
    LdpcCode ldpc_code(0, 0);

    unsigned block_length = CODEWRD_L; // parametarize this

    unsigned rate_index;  // parametarize this
    if (CODEWRD_R == 0.5) { rate_index = 0; }
    else if (CODEWRD_R == (double) (2/3)) { rate_index = 1; }
    else if (CODEWRD_R == (double) (3/4)) { rate_index = 2; }
    else if (CODEWRD_R == (double) (4/5)) { rate_index = 3; }
    else {
        cout << "The chosen code rate (" << CODEWRD_R << ") is not supported." << endl;
        return;
    }
    
    ldpc_code.load_wifi_ldpc((unsigned int) CODEWRD_L, rate_index);

    unsigned info_len = ldpc_code.get_info_length();
    if (info_len/8 != (PACKET_DATA_LENGTH_NO_FEC + CRC_DATA_LENGTH_BYTES)) {
        cout << "The chosen data length is not compatible with the picked LDPC scheme ..." << endl;
        cout << "Length of data to be encoded (with CRC): " << PACKET_DATA_LENGTH_NO_FEC + CRC_DATA_LENGTH_BYTES << endl;
        cout << "LDPC input length: " << (CODEWRD_L * CODEWRD_R)/8 << endl;
        return;
    }
    // cout << "Checking info length \nLDPC info length: " << info_len/8 << endl;
    // cout << "Macro defined length: " << PACKET_DATA_LENGTH_NO_FEC << endl;
    

    std::vector<uint8_t> info_bits(info_len, 0); // converting the packet_data.data array to usable format for enbcoding
    uint8_t input_bit; // used for extracting each bit from the packet data array
    for (unsigned i_bit = 0; i_bit < info_len; ++i_bit) { // bit extraction
        input_bit = input[(int) i_bit/8];
        input_bit >>= (7 - (i_bit % 8));
        info_bits.at(i_bit) = (uint8_t) (input_bit & 0X01);
        //printf("%i", info_bits.at(i_bit));
    }
    //printf("\n");

    std::vector<uint8_t> coded_bits = ldpc_code.encode(info_bits);

    // cout << "\n The coded bits: " << endl;
    // for (unsigned i_bit = 0; i_bit < CODEWRD_L; ++i_bit) {
    //     printf("%i", coded_bits.at(i_bit));
    // }
    // printf("\n");

    uint8_t output_byte = 0;
    for (unsigned i_bit = 0; i_bit < CODEWRD_L; ++i_bit) { // converting bit stream to uint8_t array
        if (coded_bits.at(i_bit)) {
            // could do this more cleanly by converting the sum of 2^(i_bit % 8) over every 8 bits
            if ((i_bit % 8) == 0) {
                output_byte = output_byte | 0x80;
            }
            else if ((i_bit % 8) == 1) {
                output_byte = output_byte | 0x40;
            }
            else if ((i_bit % 8) == 2) {
                output_byte = output_byte | 0x20;
            }
            else if ((i_bit % 8) == 3) {
                output_byte = output_byte | 0x10;
            }
            else if ((i_bit % 8) == 4) {
                output_byte = output_byte | 0x08;
            }
            else if ((i_bit % 8) == 5) {
                output_byte = output_byte | 0x04;
            }
            else if ((i_bit % 8) == 6) {
                output_byte = output_byte | 0x02;
            }
        }
        if ((i_bit % 8) == 7) {
            if (coded_bits.at(i_bit)) {output_byte = output_byte | 0x01;}
            input[(int) i_bit/8] = output_byte;
            output_byte = (uint8_t) 0;
        }
    }
    printf("\n");
}

extern "C" void decodeLDPC(uint8_t* rxinput) {
    std::cout << "Starting LDPC decoding...\n";

    LdpcCode ldpc_code(0, 0);

    unsigned block_length = CODEWRD_L; // parametarize this

    unsigned rate_index;  // parametarize this
    if (CODEWRD_R == 0.5) { rate_index = 0; }
    else if (CODEWRD_R == (double) (2/3)) { rate_index = 1; }
    else if (CODEWRD_R == (double) (3/4)) { rate_index = 2; }
    else if (CODEWRD_R == (double) (4/5)) { rate_index = 3; }
    else {
        cout << "The chosen code rate (" << CODEWRD_R << ") is not supported." << endl;
    }
    
    ldpc_code.load_wifi_ldpc((unsigned int) CODEWRD_L, rate_index);

    unsigned info_len = ldpc_code.get_info_length();
    // cout << "Checking info length \nLDPC info length: " << info_len/8 << endl;
    // cout << "Macro defined length: " << PACKET_DATA_LENGTH_NO_FEC << endl;
    
    //cout << "\n The llr vector: " << endl;
    std::vector<double> llr(CODEWRD_L, 0);
    uint8_t out_bit; // used for extracting each bit from the packet data array
    for (unsigned i_bit = 0; i_bit < CODEWRD_L; ++i_bit) { // bit extraction
        out_bit = rxinput[(int) i_bit/8];
        out_bit >>= (7 - (i_bit % 8));
        if (out_bit & 0X01) {
            llr.at(i_bit) = -log(9); // log(1/9) log(p0/p1)
        }
        else {
            llr.at(i_bit) = log(9);
        }
        //printf("%f,", llr.at(i_bit));
    }
    printf("\n");
    
    std::vector<uint8_t> decoded_cw = ldpc_code.decode(llr, MAX_DECODE_ITERS, MIN_SUM);

    // cout << "\n The received coded bits: " << endl;
    // for (unsigned i_bit = 0; i_bit < CODEWRD_L; ++i_bit) {
    //     printf("%i", decoded_cw.at(i_bit));
    // }
    // printf("\n");

    uint8_t out_byte = 0;
    for (unsigned i_bit = 0; i_bit < CODEWRD_L; ++i_bit) { // converting bit stream to uint8_t array
        if (decoded_cw.at(i_bit)) {
            // could do this more cleanly by converting the sum of 2^(i_bit % 8) over every 8 bits
            if ((i_bit % 8) == 0) {
                out_byte = out_byte | 0x80;
            }
            else if ((i_bit % 8) == 1) {
                out_byte = out_byte | 0x40;
            }
            else if ((i_bit % 8) == 2) {
                out_byte = out_byte | 0x20;
            }
            else if ((i_bit % 8) == 3) {
                out_byte = out_byte | 0x10;
            }
            else if ((i_bit % 8) == 4) {
                out_byte = out_byte | 0x08;
            }
            else if ((i_bit % 8) == 5) {
                out_byte = out_byte | 0x04;
            }
            else if ((i_bit % 8) == 6) {
                out_byte = out_byte | 0x02;
            }
        }
        if ((i_bit % 8) == 7) {
            if (decoded_cw.at(i_bit)) {out_byte = out_byte | 0x01;}
            rxinput[(int) i_bit/8] = out_byte;
            out_byte = (uint8_t) 0;
        }
    }
    //printf("\n");
}