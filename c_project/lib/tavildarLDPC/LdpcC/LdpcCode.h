//
// Created by Saurabh on 11/4/16.
//

#ifndef LDPCC_LDPCCODE_H
#define LDPCC_LDPCCODE_H

#include "WiFiLDPC.h"
#include <vector>
#include <cstdlib>      // std::rand, std::srand
#include <iostream>
#include <cmath>

class LdpcCode {

public:
    
    std::vector<std::vector<uint8_t > > _H_mat;
    unsigned _N;
    unsigned _K;
    unsigned _M;
    unsigned _Z;
    std::vector<std::vector<unsigned>> _column_mat;
    std::vector<std::vector<unsigned>> _row_mat;

    void generate_compact_rep() {
        _column_mat.resize(_N);
        _row_mat.resize(_M);

        for (unsigned i_col = 0; i_col < _N; ++i_col)
            _column_mat.at(i_col).resize(0);

        for (unsigned i_row = 0; i_row < _M; ++ i_row)
            _row_mat.at(i_row).resize(0);

        for (unsigned i_col = 0; i_col < _N; ++i_col) {
            for (unsigned i_row = 0; i_row < _M; ++ i_row) {
                if (_H_mat.at(i_col).at(i_row) == 1 ) {
                    _column_mat.at(i_col).push_back(i_row);
                    _row_mat.at(i_row).push_back(i_col);
                }
            }
        }
    };

    void lifted_ldpc(std::vector<std::vector<int>> baseH) {

        _H_mat.resize(_N);

        for (unsigned j = 0; j < _N ; ++j ) {
            _H_mat.at(j).resize(_M);
        }

        for (unsigned i_row = 0; i_row < _M; ++i_row ) {
            for (unsigned i_col = 0; i_col < _N; ++i_col)
                _H_mat.at(i_col).at(i_row) = 0;
        }

        for (unsigned i_base_row = 0; i_base_row < baseH.at(0).size(); ++i_base_row ) {
            for (unsigned i_base_col = 0; i_base_col < baseH.size(); ++i_base_col ){
                if ( baseH.at(i_base_col).at(i_base_row) >= 0 ) {
                    for (unsigned i_lift = 0; i_lift < _Z ; ++i_lift ) {
                        _H_mat.at(_Z  * i_base_col + ( (i_lift + baseH.at(i_base_col).at(i_base_row)) % _Z ) ).at(_Z  * i_base_row + i_lift) = 1;
                    }
                }
            }
        }

        generate_compact_rep();
    };

    bool check_codeword(std::vector<uint8_t> decoded_cw) {
        bool check = true;
        for ( unsigned i_check = 0; i_check < _M; ++i_check ) {
            uint8_t  c = 0;
            for (unsigned i_col_index = 0; i_col_index < _row_mat.at(i_check).size(); ++i_col_index ){
                unsigned i_col = _row_mat.at(i_check).at(i_col_index);
                c  = c + decoded_cw.at(i_col);
            }
            if ( (c % 2) == 1 ) {
                check = false;
                break;
            }
        }

        return check;
    };

    void generate_gallagher_ldpc();

    void load_wifi_ldpc(unsigned block_length, unsigned rate_index) {
        int * h_pointer;

        _N = block_length;

        // std::cout << "This is inside load_wifi_ldpc: " << _N << std::endl;
        // std::cout << rate_index << std::endl;

        switch (rate_index) {
            case 0: // rate 1/2
                _K = _N / 2;
                switch(block_length) {
                    case 648:
                        h_pointer = & WiFiLDPC::H_648_1_2[0][0];
                        break;
                    case 1296:
                        h_pointer = & WiFiLDPC::H_1296_1_2[0][0];
                        break;
                    case 1944:
                        h_pointer = & WiFiLDPC::H_1944_1_2[0][0];
                        break;
                    default:
                        h_pointer = & WiFiLDPC::H_648_1_2[0][0];
                        break;
                }
                break;
            case 1: // rate 2/3
                _K = _N * 2 / 3;
                switch(block_length) {
                    case 648:
                        h_pointer = & WiFiLDPC::H_648_2_3[0][0];
                        break;
                    case 1296:
                        h_pointer = & WiFiLDPC::H_1296_2_3[0][0];
                        break;
                    case 1944:
                        h_pointer = & WiFiLDPC::H_1944_2_3[0][0];
                        break;
                    default:
                        h_pointer = & WiFiLDPC::H_648_2_3[0][0];
                        break;
                }
                break;
            case 2: // rate 3/4
                _K = _N * 3 / 4;
                switch(block_length) {
                    case 648:
                        h_pointer = & WiFiLDPC::H_648_3_4[0][0];
                        break;
                    case 1296:
                        h_pointer = & WiFiLDPC::H_1296_3_4[0][0];
                        break;
                    case 1944:
                        h_pointer = & WiFiLDPC::H_1944_3_4[0][0];
                        break;
                    default:
                        h_pointer = & WiFiLDPC::H_648_3_4[0][0];
                        break;
                }
                break;
            case 3: // rate 5/6
                _K = _N * 5 / 6;
                switch(block_length) {
                    case 648:
                        h_pointer = & WiFiLDPC::H_648_5_6[0][0];
                        break;
                    case 1296:
                        h_pointer = & WiFiLDPC::H_1296_5_6[0][0];
                        break;
                    case 1944:
                        h_pointer = & WiFiLDPC::H_1944_5_6[0][0];
                        break;
                    default:
                        h_pointer = & WiFiLDPC::H_648_5_6[0][0];
                        break;
                }
                break;
            case 4: // rate 1/3: BG1 of 5G protographs
                _K = _N * 1 / 3;
                switch(block_length) {
                    case 7072:
                        h_pointer = & WiFiLDPC::H_1296_5_6[0][0];
                        break;
                    case 7616:
                        h_pointer = & WiFiLDPC::H_1296_5_6[0][0];
                        break;
                    case 8160:
                        h_pointer = & WiFiLDPC::H_1944_5_6[0][0];
                        break;
                    case 9792:
                        h_pointer = & WiFiLDPC::H_1944_5_6[0][0];
                        break;
                    default:
                        h_pointer = & WiFiLDPC::H_648_1_2[0][0];
                        break;
                }
                break;
            case 5: // rate 1/5: BG2 of 5G protographs
                _K = (unsigned int) ((float)_N * (1.0 - 42.0/52.0));
                switch(block_length) {
                    case 6656:
                        h_pointer = & WiFiLDPC::H_6656_1_5[0][0];
                        _Z = (unsigned int) (block_length / 52);
                        std::cout << _Z << std::endl;
                        break;
                    // case 1296:
                    //     h_pointer = & WiFiLDPC::H_1296_5_6[0][0];
                    //     break;
                    // case 1944:
                    //     h_pointer = & WiFiLDPC::H_1944_5_6[0][0];
                    //     break;
                    // default:
                    //     h_pointer = & WiFiLDPC::H_648_1_2[0][0];
                    //     break;
                }
                break;
            default: // Not supported
                std::cout << "Rate index value not supported" << std::endl;
                return;
        }

        _M = _N - _K;

        switch(block_length) {
            case 648: _Z = 27;
                break;
            case 1296: _Z = 54;
                break;
            case 1944: _Z = 81;
                break;
            case 6656: _Z = 128;
                break;
            default: std::cout << "Block length value not supported for WiFi LDPC" << std::endl;
        }
        // std::cout << _N << std::endl;
        // std::cout << _Z << std::endl;

        std::vector<std::vector<int>> baseH(_N/_Z);

        for (unsigned i_col = 0; i_col < _N/_Z ; ++i_col )
            baseH.at(i_col).resize(_M/_Z );

        for (unsigned i_col = 0; i_col < _N/_Z ; ++i_col ) {
            for (unsigned i_row = 0; i_row < _M/_Z ; ++i_row ) {
                baseH.at(i_col).at(i_row) = * ( h_pointer + i_col  + i_row * _N/_Z );
            }
        }

        lifted_ldpc(baseH);
    };

    unsigned get_info_length() {return _K;};

    LdpcCode(unsigned block_length, unsigned info_length): _N(block_length), _K(info_length), _Z(0) {
        _M = _N - _K;
        _H_mat.resize(_N);
        for (unsigned j = 0; j < _N ; ++j ) {
            _H_mat.at(j).resize(_M);
        }
    };

    // Encoding defintion
    std::vector<uint8_t> encode(std::vector<uint8_t> info_bits) {
        // Does encoding by back substitution
        // Assumes a very specific structure on the partiy check matrix
        std::vector<uint8_t > codeword(_N, 0);
        std::copy(info_bits.begin(), info_bits.end(), codeword.begin());

        std::vector<uint8_t > parity(_M, 0);

        for(unsigned i_row = 0; i_row < _M; ++i_row) {
            for(unsigned i_col = 0; i_col < _row_mat.at(i_row).size(); ++i_col) {
                if (_row_mat.at(i_row).at(i_col) < _K)
                    parity.at(i_row) += codeword.at(_row_mat.at(i_row).at(i_col));
            }
            parity.at(i_row) = (uint8_t) (parity.at(i_row) % 2);
        }

        for (unsigned i_col = 0; i_col < _Z; ++i_col) {
            for (unsigned i_row = i_col; i_row < _M; i_row = i_row + _Z) {
                codeword.at(_K + i_col) += parity.at(i_row);
            }
            codeword.at(_K + i_col) = (uint8_t ) (codeword.at(_K + i_col) % 2);
        }

        for(unsigned i_row = 0; i_row < _M; ++i_row) {
            for(unsigned i_col = 0; i_col < _row_mat.at(i_row).size(); ++i_col) {
                if ((_row_mat.at(i_row).at(i_col) >= _K) && (_row_mat.at(i_row).at(i_col) < _K + _Z))
                    parity.at(i_row) += codeword.at(_row_mat.at(i_row).at(i_col));
            }
            parity.at(i_row) = (uint8_t) (parity.at(i_row) % 2);
        }

        for (unsigned i_col = _K + _Z; i_col < _N; i_col = i_col + _Z  ) {
            for (unsigned i_row = 0; i_row < _Z; ++i_row) {
                codeword.at(i_col + i_row) = parity.at(i_col + i_row - _K - _Z);
                parity.at(i_col + i_row - _K ) = (uint8_t) (( parity.at(i_col + i_row - _K) + parity.at(i_col + i_row - _K - _Z)) %2);
            }
        }

        printf("Codeword:\n");
        for (unsigned i = 0; i < _N; i++){
            printf("%i,", codeword.at(i));
        }

         return codeword;
    };


    // Encoding defintion for 5G matrices
    std::vector<uint8_t> encode_modern(std::vector<uint8_t> info_bits) {
        // Does encoding by back substitution
        // Assumes a very specific structure on the partiy check matrix
        std::vector<uint8_t > codeword(_N, 0);
        std::copy(info_bits.begin(), info_bits.end(), codeword.begin());

        std::vector<uint8_t > parity(_M, 0);

        for(unsigned i_row = 0; i_row < 4*_Z; ++i_row) {
            for(unsigned i_col = 0; i_col < _row_mat.at(i_row).size(); ++i_col) {
                if (_row_mat.at(i_row).at(i_col) < _K)
                    parity.at(i_row) += codeword.at(_row_mat.at(i_row).at(i_col));
            }
            parity.at(i_row) = (uint8_t) (parity.at(i_row) % 2);
        }

        for (unsigned i_col = 0; i_col < _Z; ++i_col) {
            for (unsigned i_row = i_col; i_row < 4*_Z; i_row = i_row + _Z) {
                codeword.at(_K + i_col + 1) += parity.at(i_row);
            }
            codeword.at(_K + i_col + 1) = (uint8_t ) (codeword.at(_K + i_col + 1) % 2);
        }

        for(unsigned i_row = 0; i_row < 4*_Z; ++i_row) {
            for(unsigned i_col = 0; i_col < _row_mat.at(i_row).size(); ++i_col) {
                if ((_row_mat.at(i_row).at(i_col) >= _K) && (_row_mat.at(i_row).at(i_col) < _K + _Z))
                    parity.at(i_row) += codeword.at(_row_mat.at(i_row).at(i_col));
            }
            parity.at(i_row) = (uint8_t) (parity.at(i_row) % 2);
        }

        for (unsigned i_col = _K + _Z; i_col < _K + 4*_Z; i_col = i_col + _Z  ) {
            for (unsigned i_row = 0; i_row < _Z; ++i_row) {
                codeword.at(i_col + i_row) = parity.at(i_col + i_row - _K - _Z);
                parity.at(i_col + i_row - _K ) = (uint8_t) (( parity.at(i_col + i_row - _K) + parity.at(i_col + i_row - _K - _Z)) %2);
            }
        }

        printf("Modern codeword:\n");
        for (unsigned i = 0; i < _N; i++){
            printf("%i,", codeword.at(i));
        }

        printf("\n\n%i\n", check_codeword(codeword));

        return codeword;
    };


    // Decoding definition
    std::vector<uint8_t> decode(std::vector<double> llr_vec, unsigned max_iter, bool min_sum) {

        std::vector<std::vector<double> > edge_mat( _M, std::vector<double>(0));
        std::vector<std::vector<double> > last_edge_mat( _M, std::vector<double>(0));
        std::vector<double> updated_llr = llr_vec;

        std::vector<uint8_t > decoded_cw(_N);

        for (unsigned i_row = 0; i_row < _M; ++i_row) {
            edge_mat.at(i_row).resize(_row_mat.at(i_row).size(), 0);
            last_edge_mat.at(i_row).resize(_row_mat.at(i_row).size(), 0);
        }

        for (unsigned iter = 0; iter < max_iter; ++iter) {

            for (unsigned i_row = 0; i_row < _M; ++i_row ) {
                for (unsigned i_col_index1 = 0; i_col_index1 < _row_mat.at(i_row).size(); ++i_col_index1 ) {
                    double tmp = 1;
                    if (min_sum)
                        tmp = 100;
                    for (unsigned i_col_index2 = 0; i_col_index2 < _row_mat.at(i_row).size(); ++i_col_index2 ) {
                        if (i_col_index1 == i_col_index2) {
                            continue;
                        }
                        unsigned i_col2 = _row_mat.at(i_row).at(i_col_index2);
                        double l1 = updated_llr.at(i_col2) - last_edge_mat.at(i_row).at(i_col_index2);
                        l1 = std::min(l1, 20.0);
                        l1 = std::max(l1, -20.0);
                        if ( min_sum ) {
                            double sign_tmp = 1.0;
                            if (tmp < 0.0) sign_tmp = -1.0;
                            double sign_l1 = 1.0;
                            if (l1 < 0.0) sign_l1 = -1.0;
                            tmp = sign_tmp * sign_l1 * std::min(std::abs(l1), std::abs(tmp));
                        }
                        else
                            tmp = tmp * tanh(l1/2);
                    }
                    if ( min_sum ) {
                        edge_mat.at(i_row).at(i_col_index1) = tmp;
                    }
                    else {
                        edge_mat.at(i_row).at(i_col_index1) = 2 * atanh(tmp);
                    }
                }
            }

            last_edge_mat = edge_mat;

            updated_llr = llr_vec;

            for (unsigned i_row = 0; i_row < _M; ++i_row) {
                for (unsigned i_col_index = 0; i_col_index < _row_mat.at(i_row).size() ; ++i_col_index ) {
                    unsigned i_col = _row_mat.at(i_row).at(i_col_index);
                    updated_llr.at(i_col) = updated_llr.at(i_col) + last_edge_mat.at(i_row).at(i_col_index);
                }
            }

            for (unsigned i_col = 0; i_col < _N; ++i_col ) {
                if (updated_llr.at(i_col) > 0) {
                    decoded_cw.at(i_col) = 0;
                }
                else {
                    decoded_cw.at(i_col) = 1;
                }
            }

            if (check_codeword(decoded_cw) ) {
                break;
            }

        } // Iteration loop end

        return decoded_cw;
    };

};

#endif //LDPCC_LDPCCODE_H
