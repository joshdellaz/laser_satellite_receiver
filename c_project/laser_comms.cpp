#include "laser_comms.h"
#include <iostream>
#include "tavildarLDPC/LdpcC/LdpcCode.h"
#include "tavildarLDPC/LdpcC/Constellation.h"
#include <random>
#include <iomanip> 


using namespace std;

extern "C" void applyLDPC(uint8_t* input) {
    std::cout << "Hello, from laser_comms!\n";

    unsigned max_err = 250;
    unsigned max_runs = 10000;

    bool min_sum = false;

    unsigned code_index = 0;

    LdpcCode ldpc_code(0, 0);

    std::vector<unsigned> block_length_vec{648, 1296, 1944};

    std::vector<unsigned> constellations{1, 2, 3};

    std::vector<unsigned> rate_index_vec{0, 1, 2};

    // Used to run EbNo points of interest for the rate used
    std::vector<unsigned>  rate_offset{0, 0, 1};

    // Used to run EbNo points of interest as per the constellation used
    std::vector<double>  constellations_ebno_offset{0.0, 3.0, 7.0};

    cout << "Here are some numbers " << max_runs << " and " << block_length_vec.at(1) << endl;

    unsigned block_length = CODEWRD_L;

    unsigned rate_index;
    if (CODEWRD_R == 0.5) { rate_index = 0; }
    else if (CODEWRD_R == (double) (2/3)) { rate_index = 1; }
    else if (CODEWRD_R == (double) (3/4)) { rate_index = 2; }
    else { rate_index = 3; }
    cout << "The code rate is " << CODEWRD_R << " and its index is " << rate_index << endl;
    
    ldpc_code.load_wifi_ldpc((unsigned int) CODEWRD_L, (unsigned int) CODEWRD_R);

    unsigned info_len = ldpc_code.get_info_length(); //load_wifi_ldpc(block_length, rate_index);
    cout << "Checking info length: " << info_len << endl;
    
    uint8_t input_bit;
    std::vector<uint8_t> info_bits(info_len, 0);
    for (unsigned i_bit = 0; i_bit < info_len; ++i_bit) {
        input_bit = input[(int) i_bit/8];
        input_bit >>= (7 - (i_bit % 8));
        info_bits.at(i_bit) = (uint8_t) (input_bit & 0X01);
        printf("%i", info_bits.at(i_bit));
    }
    printf("\n");

    std::vector<uint8_t> coded_bits = ldpc_code.encode(info_bits);

    cout << "\n Here's the coded bits: " << endl;
    for (unsigned i_bit = 0; i_bit < CODEWRD_L; ++i_bit) {
        printf("%i", coded_bits.at(i_bit));
    }
    printf("\n");
}