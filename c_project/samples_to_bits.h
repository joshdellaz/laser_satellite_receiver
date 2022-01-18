#include <stdint.h>
#include <stdbool.h>


//perform in chunks?
float * bytestreamToSamplestream(uint8_t* data, int length_bytes, int *length_samples);
bool shiftDownAndNormalizeSamples(float ** samples, int length_samples);
uint8_t * samplesToBytes(float* samples, int length_samples, float phase_offset);
bool resampleInput(float* samplesin, float ** samplesout);
float determinePhaseOffset(float* samples);//design such that it can be done on 101010... stream OR w/ MLS sync