#include "test.h"
#include "samples_to_bits.h"
#include <stdlib.h>
#include "ad2_io.hpp"
#include "laser_comms.h"

int packet_data_length_with_fec_bytes = 0;
extern float * MLS_array;

int main(void) {

	initAD2();
	//sendInfWaveform();



	getFECDataLengths();
	initMLS();
	initLDPC();
	int repititions = 0;
	for(int i = 0; i < repititions; i++){
		fullSendTest();
	}
	
	
	imageSendTest("sample.ppm");


	//softwareDACandADC();
	//simulatedAutocorSyncTest();

	//testMLSAutoCorrelation();
	//printf("starting the fullsend test shit\n");
	//fullSendTest();
	//Aah!
	//free(MLS_array);

	return 0;
}