#include <stdint.h>

void initMLS(void);
uint8_t * syncFrame(float * samples, int length_samples_in, int * length_bytes_out, int frame_start_index_guess);
float * getIncomingSignalData(float * ADC_output_float, int * frame_start_index_guess, int * output_length);
uint16_t get_LIQUID_MSEQUENCE_GENPOLY(int order);