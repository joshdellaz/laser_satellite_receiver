#pragma once
#include <stdbool.h>
#include <stdint.h>

//perform in chunks?
bool resample(float* samples);
bool determinePhaseOffset(float* samples, float phase_offset);//design such that it can be done on 101010... stream OR w/ MLS sync
bool samplesToBytes(float * samples, int * num_bytes_to_convert, uint8_t * data, float phase_offset);