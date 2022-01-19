#include "test.h"

int packet_data_length_with_fec = 0;

int main(void) {
	getFECDataLengths();
	softwareDACandADC();
	
	//fullSendTest();
	//Aah!
}