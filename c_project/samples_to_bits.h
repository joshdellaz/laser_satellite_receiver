#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#define PI 3.142857


//perform in chunks?
//bool resample(float* samples);
//bool determinePhaseOffset(float* samples, float phase_offset);//design such that it can be done on 101010... stream OR w/ MLS sync
//bool samplesToBytes(float * samples, int * num_bytes_to_convert, uint8_t * data, float phase_offset);

float * bytestreamToSamplestream(uint8_t* data, int length_bytes, float phaseshift);