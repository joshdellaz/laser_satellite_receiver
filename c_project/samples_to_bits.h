#include <stdint.h>
#include <stdbool.h>


//perform in chunks?
float * bytestreamToSamplestream(uint8_t* data, int length_bytes, int *length_samples);
bool shiftDownAndNormalizeSamples(float ** samples, int length_samples);
uint8_t * samplesToBytes(float* samples, int length_samples, float phase_offset);
float * resampleInput(float* samplesin, int length_samples_in, int * length_samples_out);
float determinePhaseOffset(float* samples);