#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

void plotByteStream(uint8_t * data, int len, char * title){
    uint8_t * buf = (uint8_t *)malloc(len*8*sizeof(uint8_t));
	for(int i = 0; i<len; i++){
		for(int j = 0; j<8; j++){
			buf[i*8 + j] = ((data[i] >> (7-j)) & 0b1);
		}
	}
    //save data to file
    FILE * file;
    file = fopen("output.csv", "w");
    for(int i = 0; i < len*8; i++){
        fprintf(file,"%d, ", buf[i]);
    }
    fclose(file);
    free(buf);
    //save title to file
    file = fopen("title.csv", "w");
    int results = fputs(title, file);
    fclose(file);
    
    system("python3 plot_scatter.py");
    system("rm output.csv");
}


void plotFloatStream(float * data, int len, char * title){

    //save to file
    FILE * file;
    file = fopen("output.csv", "wb");
    for(int i = 0; i < len; i++){
        fprintf(file,"%f, ", data[i]);
    }
    fclose(file);

    //save title to file
    file = fopen("title.csv", "w");
    int results = fputs(title, file);
    fclose(file);
    system("python3 plot_scatter.py");
    system("rm output.csv");
}

void plotContour(float * data, int x, int y, int scale, char * title){
    //save to file
    FILE * file;
    file = fopen("output.csv", "wb");
    for(int i = 0; i < x*y; i++){
        fprintf(file,"%f, ", data[i]);
    }
    fclose(file);

    //save title to file
    file = fopen("title.csv", "w");
    int results = fputs(title, file);
    fprintf(file,", %d",x);
    fprintf(file,", %d",y);
    fprintf(file,", %d",scale);

    fclose(file);
    system("python3 plot_contour.py");
    system("rm output.csv");
    system("rm title.csv");
}