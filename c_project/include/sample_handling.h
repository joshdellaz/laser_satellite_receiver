#include <stdint.h>
#include <stdbool.h>

float * bytestreamToSamplestream(uint8_t* data, int length_bytes, int *length_samples, float phaseshift);
bool shiftDownAndNormalizeSamples(float ** samples, int length_samples);
uint8_t * samplesToBytes(float* samples, int length_samples);
float * resampleInput(float* samplesin, int length_samples_in, int * length_samples_out);