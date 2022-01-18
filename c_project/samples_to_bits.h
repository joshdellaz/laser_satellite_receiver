#include <stdint.h>
#include <stdbool.h>


//perform in chunks?
bool bytestreamToSamplestream(float* samples);
bool resampleInput(float* samples);
float determinePhaseOffset(float* samples);//design such that it can be done on 101010... stream OR w/ MLS sync
bool samplesToBytes(float * samples, int * num_bytes_to_convert, uint8_t * data, float phase_offset);