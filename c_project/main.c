#include "test.h"
#include "samples_to_bits.h"
#include <stdlib.h>

int packet_data_length_with_fec_bytes = 0;
extern float * MLS_array;

int main(void) {
	getFECDataLengths();
	initMLS();
	//softwareDACandADC();
	//simulatedAutocorSyncTest();
	fullSendTest();
	//testMLSAutoCorrelation();
	//printf("starting the fullsend test shit\n");
	//fullSendTest();
	//Aah!
	free(MLS_array);

	return 0;
}