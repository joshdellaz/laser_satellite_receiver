#include "test.h"
#include "samples_to_bits.h"
#include <stdlib.h>
#include "ad2_io.hpp"

int packet_data_length_with_fec_bytes = 0;
extern float * MLS_array;

int main(void) {
	//sendInfWaveform();

	initAD2();
	fullSendTest();

	//getFECDataLengths();
	//initMLS();
	//softwareDACandADC();
	//simulatedAutocorSyncTest();

	//testMLSAutoCorrelation();
	//printf("starting the fullsend test shit\n");
	//fullSendTest();
	//Aah!
	//free(MLS_array);

	return 0;
}