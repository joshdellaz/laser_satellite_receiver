#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

void plotByteStream(uint8_t * data, int len){
    uint8_t * buf = (uint8_t *)malloc(len*8*sizeof(uint8_t));
	for(int i = 0; i<len; i++){
		for(int j = 0; j<8; j++){
			buf[i*8 + j] = ((data[i] >> (7-j)) & 0b1);
		}
	}
    //save to file
    FILE * file;
    file = fopen("output.csv", "wb");
    for(int i = 0; i < len*8; i++){
        fprintf(file,"%d, ", buf[i]);
    }
    fclose(file);
    free(buf);
    system("python3 plot.py");
    system("rm output.csv");
}


void plotFloatStream(float * data, int len){

    //save to file
    FILE * file;
    file = fopen("output.csv", "wb");
    for(int i = 0; i < len; i++){
        fprintf(file,"%f, ", data[i]);
    }
    fclose(file);
    system("python3 plot.py");
    system("rm output.csv");
}