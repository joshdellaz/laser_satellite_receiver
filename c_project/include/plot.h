#include <stdint.h>

//Plots data automatically using python's matplotlib
void plotByteStream(uint8_t * data, int len, char * title);
void plotFloatStream(float * data, int len, char * title);
void plotContour(float * data, int x, int y, int scale, char * title);