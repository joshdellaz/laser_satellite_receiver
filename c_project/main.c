#include "test.h"

int packet_data_length_with_fec_bytes = 0;

int main(void) {
	getFECDataLengths();
	//softwareDACandADC();
	simulatedAutocorSyncTest();
	//fullSendTest();
	//Aah!

	return 0;
}