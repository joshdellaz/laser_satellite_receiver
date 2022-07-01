#include <stdint.h>

int readFiletoArray(char * filename, uint8_t * data, int * len_data);
int writeArraytoFile(char * filename, uint8_t * data, int len_data);