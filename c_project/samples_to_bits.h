#include <stdint.h>
#include <stdbool.h>


//perform in chunks?
float * bytestreamToSamplestream(uint8_t* data, int length_bytes, int *length_samples);
bool shiftDownAndNormalizeSamples(float ** samples, int length_samples);
uint8_t * samplesToBytes(float* samples, int length_samples, float phase_offset);
float * resampleInput(float* samplesin, int length_samples_in, int * length_samples_out);
float determinePhaseOffset(float* samples);
void chopFront(float ** data, int num_samples_to_chop_off, int length_samples);
float findAutocorrelation(float * samples);
uint8_t * syncFrame(float * samples, int length_samples_in, int * length_bytes_out, int frame_start_index_guess);
