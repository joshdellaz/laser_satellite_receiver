#include "func_test.h"
#include "sample_handling.h"
#include <stdlib.h>
#include "ldpc_implementation.h"
#include "stress_test.h"
#include <time.h>
#include "plot.h"
#include "channel.h"
#include "packet_frame.h"
#include "sync.h"

int packet_data_length_with_fec_bytes = 0;
int packet_data_length_without_fec_bytes = 0;
int elevation_angle_deg = 0;

extern float * MLS_array;
extern int snr_db;

void printAllStatistics();


//clarify ambiguites in init order!
int main(void) {
	srand(time(NULL));
	initLDPC();
	//snr_db = 0;
	elevation_angle_deg = 75;
	setBitRateMbps(5);
	setFadeParamsBasedOnElevation(elevation_angle_deg);//must be called before next function. Indicate this more clearly somehow...
	setFrameLengthBasedOnElevation(elevation_angle_deg);
	setPacketDataLengthInclFEC();
	initMLS();
	initChannelState();

	printAllStatistics();

	int repititions = 0;
	for(int i = 0; i < repititions; i++){
		sendAndReceiveRandomlyGeneratedPacket();
	}
	//testSyncEdgeCases();
	//testSNR();
	//testFades();
	//testLDPC();
	//testSpecialCases();
	imageSendTest("../testdata/engphys.ppm");


	//softwareDACandADC();
	//simulatedAutocorSyncTest();

	//testMLSAutoCorrelation();

	return 0;
}

//Doesn't print values in config.h as those are fixed before compile!
void printAllStatistics(){
	printf("----------CURRENT SYSTEM PARAMETERS----------\n");
	printf("Elevation angle = %d degrees\n", elevation_angle_deg);
	float ppm = getFreqStabilityInPPMUsingElevation(elevation_angle_deg);
	printf("Frequency instability due to doppler = %.2f ppm\n", ppm);
	printf("Number of bits untill loss of sync = %d bits\n", getMaxFrameLengthInBits(ppm));
	printf("Block size = %d bits \n", getBlockSizeBits());
	printf("Number of blocks per packet = %d\n", getNumBlocksPerPacket());
	printf("Frame user data length excl. FEC = %d bits\n", 8*packet_data_length_without_fec_bytes);
	printf("Frame user data length incl. FEC = %d bits\n", 8*packet_data_length_with_fec_bytes);
	printf("MLS order = %d (%d bits)\n", getMLSOrder(), (int)(pow(2,getMLSOrder()) - 1));
	printf("Pass length =  %d seconds\n", getPassLengthInSeconds(elevation_angle_deg));
	printf("Bit rate = %d Mbps\n", getBitRateMbps());
	printf("Fade length = %d us, = %d bits\n", getFadeLenUsec(), getFadeLengthBits());
	printf("Fade freq = %d hz\n", getFadeFreqHz());
	printf("Burst length = %d us, = %d bits\n", getBurstLenUsec(), getBurstLengthBits());
	printf("-------------------------------------------\n\n");
}