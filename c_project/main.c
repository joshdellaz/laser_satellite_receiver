#include "test.h"

int packet_data_length_with_fec = 0;

int main(void) {
	getFECDataLengths();
	printf("starting the fullsend test shit\n");
	fullSendTest();
	//Aah!
}