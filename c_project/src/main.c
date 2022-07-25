#include "func_test.h"
#include "samples_to_bits.h"
#include <stdlib.h>
//#include "ad2_io.hpp"
#include "laser_comms.h"
#include "stress_test.h"
#include <time.h>
#include "plot.h"
int packet_data_length_with_fec_bytes = 0;
extern float * MLS_array;
extern int snr_db;

int main(void) {

	//snr_db = 0;

	getFECDataLengths();
	initMLS();
	initLDPC();
	int repititions = 0;
	for(int i = 0; i < repititions; i++){
		fullSendTest();
	}
	
	//testSNR();
	//testFades();
	testLDPC();
	//testSpecialCases();
	//imageSendTest("../testdata/engphys.ppm");


	//softwareDACandADC();
	//simulatedAutocorSyncTest();

	//testMLSAutoCorrelation();

	return 0;
}