#include "laser_comms.h"
#include "packet_frame.h"
#include <iostream>
#include "LdpcCode.h"
#include "Constellation.h"
#include <random>
#include <iomanip> 
#include <math.h>
#include <vector>
#include "dev_utils.h"


using namespace std;

#ifdef VERBOSE_ENABLED
#define dev_printf(...) printf(__VA_ARGS__)
#else
#define dev_printf(...) stub()
#endif

LdpcCode ldpc_code(0,0);

void initLDPC(void) {
    unsigned rate_index = 0; 
    if (CODEWRD_R == 0.5) { rate_index = 0; }
    else if (CODEWRD_R == 0.66) { rate_index = 1; }
    else if (CODEWRD_R == 0.75) { rate_index = 2; }
    else if (CODEWRD_R == 0.8) { rate_index = 3; }
    else if (CODEWRD_R == 0.33) { rate_index = 4; }
    else if (CODEWRD_R == 0.2) { rate_index = 5; }
    else {
        cout << "The chosen code rate (" << CODEWRD_R << ") is not supported." << endl;
        return;
    }
    //LdpcCode ldpc_code(0, 0);
    ldpc_code.load_wifi_ldpc((unsigned int) CODEWRD_L, rate_index);
}

void applyLDPC(uint8_t* input) {
    
    unsigned block_length = CODEWRD_L; // parametarize this
    double actual_rate = CODEWRD_R;

    unsigned rate_index;  // parametarize this
    if (CODEWRD_R == 0.5) { rate_index = 0; }
    else if (CODEWRD_R == 0.66) { rate_index = 1; }
    else if (CODEWRD_R == 0.75) { rate_index = 2; }
    else if (CODEWRD_R == 0.8) { rate_index = 3; }
    else if (CODEWRD_R == 0.33) { rate_index = 4; }
    else if (CODEWRD_R == 0.2) { 
        rate_index = 5; 
        actual_rate = 1.0 - 42.0/52.0; // gotta figure out a cleaner way
        }
    else {
        cout << "The chosen code rate (" << CODEWRD_R << ") is not supported." << endl;
        return;
    }
    dev_printf("The chosen code rate is %d\n", CODEWRD_R);
    dev_printf("The code rate index is %d\n", rate_index);
    
    unsigned info_len = ldpc_code.get_info_length();
    if (info_len/8 != (PACKET_DATA_LENGTH_NO_FEC + CRC_DATA_LENGTH_BYTES + 2*NUM_PACKETS_LENGTH_BYTES)) {
        dev_printf("The chosen data length is not compatible with the picked LDPC scheme ...\n");
        dev_printf("Length of data to be encoded (with CRC): %d\n", PACKET_DATA_LENGTH_NO_FEC + CRC_DATA_LENGTH_BYTES + 2*NUM_PACKETS_LENGTH_BYTES);
        dev_printf("LDPC input length: %d\n", (CODEWRD_L * CODEWRD_R)/8);// TODO needs correction 
        //return;
    }
    // cout << "Checking info length \nLDPC info length: " << info_len/8 << endl;
    // cout << "Macro defined length: " << PACKET_DATA_LENGTH_NO_FEC << endl;
    for (int i_block = 0; i_block < NUM_BLOCKS_PCKT; i_block++){
        std::vector<uint8_t> info_bits(info_len, 0); // converting the packet_data.data array to usable format for enbcoding
        uint8_t input_bit; // used for extracting each bit from the packet data array
        uint16_t input_preamb;
        printf("Printing info bits bit by bit: \n");
        for (unsigned i_bit = 0; i_bit < info_len; ++i_bit) { // bit extraction
            input_bit = input[(int) (i_block*CODEWRD_L*CODEWRD_R/8) + (i_bit/8)];
            input_bit >>= (7 - (i_bit % 8));
            info_bits.at(i_bit) = (uint8_t) (input_bit & 0X01);
            printf("%i,", info_bits.at(i_bit));
        }

        std::vector<uint8_t> coded_bits = ldpc_code.encode(info_bits);

        printf("\nPrinting codeword: \n");
        printf("Starting from index %i \n", (unsigned) CODEWRD_L*actual_rate);
        uint8_t output_byte = 0;
        for (unsigned i_bit = CODEWRD_L*actual_rate; i_bit < CODEWRD_L; ++i_bit) { // converting bit stream to uint8_t array
            printf("%i,", coded_bits.at(i_bit));
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
                input[int(NUM_BLOCKS_PCKT*CODEWRD_R*CODEWRD_L/8) + int(i_block*CODEWRD_L*(1-CODEWRD_R)/8) + int((i_bit-CODEWRD_L*CODEWRD_R)/8)] = output_byte;
                output_byte = (uint8_t) 0;
            }
        }
    }
}


void decodeLDPC(uint8_t* rxinput) {
    // ignoring burst positions in LDPC for now

    //LdpcCode ldpc_code(0, 0);

    unsigned block_length = CODEWRD_L; // parametarize this

    unsigned rate_index;  // parametarize this
    if (CODEWRD_R == 0.5) { rate_index = 0; }
    else if (CODEWRD_R == 0.66) { rate_index = 1; }
    else if (CODEWRD_R == 0.75) { rate_index = 2; }
    else if (CODEWRD_R == 0.8) { rate_index = 3; }
    else if (CODEWRD_R == 0.33) { rate_index = 4; }
    else if (CODEWRD_R == 0.2) { rate_index = 5; }
    else {
        cout << "The chosen code rate (" << CODEWRD_R << ") is not supported." << endl;
        return;
    }
    
    //ldpc_code.load_wifi_ldpc((unsigned int) CODEWRD_L, rate_index);

    unsigned info_len = ldpc_code.get_info_length();
    // cout << "Checking info length \nLDPC info length: " << info_len/8 << endl;
    // cout << "Macro defined length: " << PACKET_DATA_LENGTH_NO_FEC << endl;
    
    //cout << "\n The llr vector: " << endl;
    for (int i_block = 0; i_block < NUM_BLOCKS_PCKT; i_block++){
        std::vector<double> llr(CODEWRD_L, 0);
        uint8_t out_bit; // used for extracting each bit from the packet data array
        for (unsigned i_bit = 0; i_bit < CODEWRD_L; ++i_bit) { // bit extraction
            if (i_bit < CODEWRD_L*CODEWRD_R)  {
                out_bit = rxinput[(int) (i_block*CODEWRD_L*CODEWRD_R/8) + (i_bit/8)];
            } else {
                out_bit = rxinput[int(NUM_BLOCKS_PCKT*CODEWRD_R*CODEWRD_L/8) + int(i_block*CODEWRD_L*(1-CODEWRD_R)/8) + int((i_bit-CODEWRD_L*CODEWRD_R)/8)];
            }
            
            out_bit >>= (7 - (i_bit % 8));
            if (out_bit & 0X01) {
                llr.at(i_bit) = -log(9); // log(1/9) log(p0/p1)
            }
            else {
                llr.at(i_bit) = log(9);
            }
            //printf("%f,", llr.at(i_bit));
        }
        
        std::vector<uint8_t> decoded_cw = ldpc_code.decode(llr, MAX_DECODE_ITERS, MIN_SUM);

        // cout << "\n The received coded bits: " << endl;
        // for (unsigned i_bit = 0; i_bit < CODEWRD_L; ++i_bit) {
        //     printf("%i", decoded_cw.at(i_bit));
        // }
        // printf("\n");

        uint8_t out_byte = 0;
        for (unsigned i_bit = 0; i_bit < CODEWRD_L*CODEWRD_R; ++i_bit) { // converting bit stream to uint8_t array
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
                rxinput[(int) (i_block*CODEWRD_L*CODEWRD_R/8) + (i_bit/8)] = out_byte;
                out_byte = (uint8_t) 0;
            }
        }
    }
}