#include "test.h"
#include "samples_to_bits.h"
#include <stdlib.h>

int packet_data_length_with_fec_bytes = 0;
extern float * MLS_array;
int *chnl_tracking = 0;

int main(void) {
	getFECDataLengths();
	initMLS();
	//softwareDACandADC();
	//simulatedAutocorSyncTest();
	//fullSendTest();

	imageSendTest("sample.ppm");
	//testMLSAutoCorrelation();
	
	//Aah!
	free(MLS_array);

	return 0;
}