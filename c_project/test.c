#include "packet_frame.h"
#include "channel.h"
#include "samples_to_bits.h"
#include "laser_comms.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

extern int mls_total_preamble_length_bits;
extern int number_of_mls_repititions;
#define PI 3.142857


//Returns pointer to a randomized uint8_t array of length packet_data_length_with_fec_bytes
uint8_t * generateRandPacket(void) {
	uint8_t* data = (uint8_t*)malloc(packet_data_length_with_fec_bytes);
	for (int i = 0; i < PACKET_DATA_LENGTH_NO_FEC; i++) {
		data[i] = rand() & 0xff;
	}
	return data;
}


//1 (maybe) lsb bit error every 8 bytes
// void applyChannel(uint8_t * input_data, unsigned int input_data_length) {
// 	for (unsigned int i = 0; i < input_data_length; i++) {
// 		if (i % 8 == 0) {
// 			input_data[i] = (input_data[i] >> 1) << 1;//only flips bit if lsb was a 1
// 		}
// 	}
// }

//
void getFECDataLengths(void) {

	// create arrays
	packet_data_length_with_fec_bytes = fec_get_enc_msg_length(FEC_TYPE, PACKET_DATA_LENGTH_NO_FEC);
}

void simulatedAutocorSyncTest(void){
    //full MLS & randomly generated payload data.
	int mls_total_preamble_length_bytes = mls_total_preamble_length_bits/8 + 1;

	int originaldatalength = mls_total_preamble_length_bytes + packet_data_length_with_fec_bytes;
	uint8_t* originalframe = (uint8_t*)malloc(originaldatalength);
	
	uint8_t* mls_temp = NULL;
	unsigned int mls_temp_length = 0;
	getMaximumLengthSequencePreamble(&mls_temp, &mls_temp_length);

	for (int i = 0; i < mls_total_preamble_length_bytes; i++) {
		originalframe[i] = mls_temp[i];
	}
	free(mls_temp);

	srand(time(NULL));
	for (int i = 0; i < packet_data_length_with_fec_bytes; i++) {
		originalframe[mls_total_preamble_length_bytes - 1 + i] = rand() & 0xff;
	}

	printf("\nOriginal Data:\n");
	for (unsigned int i = 0; i < 50; i++) {
		printf("%d ", originalframe[i]);
	}
	printf("\n\n");

	

	int numsamples = 0;
	float phase;

	for(int i = 0; i<11; i++){
		printf("TEST ROUND %d:\n\n", i);

		switch (i){
			case 0://Basic test
				phase = 0;
				//done
				break;
			case 1://Next tests: same as above, but with phase offset of pi/4, pi/2, 3pi/4, pi, 5pi/4, 3pi/2, 7pi/4, and 2pi.
				phase = PI/4.0;				
				break;
			case 2:
				phase = PI/2.0;
				break;
			case 3:
				phase = 3.0*PI/4.0;
				break;
			case 4:
				phase = PI;				
				break;
			case 5:
				phase = 5.0*PI/4.0;				
				break;			
			case 6:
				phase = 3.0*PI/2.0;				
				break;
			case 7:
				phase = 7.0*PI/4.0;				
				break;
			case 8:
				phase = 2.0*PI;
				break;
			case 9: //Next test: Original data but with 1250 bit fade erasure at front of preamble. pi/2 phase offset
				phase = PI/2.0;
				for(int i = 0; i<(1250/8); i++){
					originalframe[i] = 0;
				}
				
				break;
			case 10: //Next test: Original data but with 1250 bit burst erasure in middle of one MLS. pi/2 phase offset
				phase = PI/2.0;
				for(int i = 0; i<(1250/8); i++){
					originalframe[2*(mls_total_preamble_length_bytes)/10 + i] = 0;
				}		
				break;
			case 11: //Next test: Original data but with 1250 bit burst erasure in middle of whole preamble. pi/2 phase offset
				phase = PI/2.0;
				for(int i = 0; i<(1250/8); i++){
					originalframe[4*(mls_total_preamble_length_bytes)/10 + i] = 0;
				}		
				break;
		}

		float *samples = bytestreamToSamplestream(originalframe, originaldatalength, &numsamples, phase);

		//Prepend a buncha zero samples (randomly generated amount between 4 and 1000)
		int stuffing_len = (rand() % (1000 - 4 + 1)) + 4;
		int j = numsamples;
		float buffer;
		numsamples += stuffing_len;
		samples = realloc(samples, numsamples*sizeof(float));//Assuming this appends extra size allocated
		for(j; j >= 0; j--){
			buffer = samples[j];
			samples[stuffing_len -1 + j] = buffer;
		}
		for(j = 0; j < stuffing_len; j++){
			samples[j] = 0;
		}
		printf("ADC output Samples, minus prepended zeroes:\n");
		for (unsigned int i = 0; i < 50*4; i++) {
			printf("%.0f", samples[stuffing_len -1 + i]);
		}
		printf("\n\n");


		//Channel would be applied here if implemented

		//Test it!
		int frame_start_index_guess = 0;
		int samples_shifted_length = 0;
		float * samples_shifted = getIncomingSignalData(samples, &frame_start_index_guess, &samples_shifted_length);//Does this still throw a seg fault sometimes?

		int numsamples_upsampled = 0;
		float * samples_upsampled = resampleInput(samples_shifted, samples_shifted_length, &numsamples_upsampled);
		
		printf("Samples before sync :\n");
		for (unsigned int i = 0; i < 60*4*4; i++) {
			printf("%.1f ", samples_upsampled[(1300-1)*4*4+ i]);//offset to account for stuffing in getIncomingSignalData
		}
		printf("\n\n");
		
		int finaldatalength = 0;
		uint8_t * converteddata = syncFrame(samples_upsampled, numsamples_upsampled, &finaldatalength, frame_start_index_guess);


		printf("Original user data:\n");
		for (unsigned int i = 0; i < 50; i++) {
			printf("%d", originalframe[mls_total_preamble_length_bytes - 1 + i]);
		}
		printf("\n\n");
		printf("Converted(demodulated) user data:\n");
		for (unsigned int i = 0; i < 50; i++) {
			printf("%d", converteddata[i]);
		}
		printf("\n\n");

		int counter = 0;
		for(int i = 0; i<finaldatalength; i++){
			if(originalframe[mls_total_preamble_length_bytes + i] == converteddata[i]){
				counter++;
			}
		}
		if(counter == finaldatalength){
			printf("Successful demodulation!\n\n");
		} else {
			printf("Unuccessful demodulation!\n\n");
		}
		free(converteddata);
	}
}

void softwareDACandADC(void){
	//TOODOOOOOOO
	int originaldatalength = 10000;
	uint8_t* originaldata = (uint8_t*)malloc(originaldatalength);
	for (int i = 0; i < originaldatalength; i++) {
		originaldata[i] = rand() & 0xff;
	}
	originaldata[0] = 0b01010101;
	originaldata[1] = 0b01010101;
	originaldata[2] = 0b01010101;


	printf("Original Data:\n");
	for (unsigned int i = 0; i < originaldatalength; i++) {
		printf("%d", originaldata[i]);
	}
	printf("\n\n");

	int numsamples = 0;
	float *samples = bytestreamToSamplestream(originaldata, originaldatalength, &numsamples, 0);
	// printf("ADC OUTPUT:\n");
	// for (unsigned int i = 0; i < numsamples; i++) {
	// 	printf("%.2f  ", samples[i]);
	// }
	// printf("\n\n");

	shiftDownAndNormalizeSamples(&samples, numsamples);
	// printf("Processed Samples:\n");
	// for (unsigned int i = 0; i < numsamples; i++) {
	// 	printf("%.2f  ", samples[i]);
	// }
	// printf("\n\n");

	int numsamples_upsampled = 0;
	float * samples_upsampled = resampleInput(samples, numsamples, &numsamples_upsampled);
	// printf("Upsampled Samples:\n");
	// for (unsigned int i = 0; i < numsamples_upsampled; i++) {
	// 	printf("%.2f  ", samples_upsampled[i]);
	// }
	// printf("\n\n");

	//shift samples
	//TODO eliminate this extreme jankiness
	int filterdelay = 121;
	int numsamples_shifted = numsamples_upsampled-filterdelay;
	float *samples_shifted = (float *)malloc(numsamples_shifted*sizeof(float)); 
	for(int i = 0; i<numsamples_shifted; i++){
		samples_shifted[i] = samples_upsampled[filterdelay+i];
	}
	// printf("Shifted Samples:\n");
	// for (unsigned int i = 0; i < numsamples_shifted; i++) {
	// 	printf("%.2f  ", samples_shifted[i]);
	// }
	// printf("\n\n");

	float phase = determinePhaseOffset(samples_shifted);
	printf("Phase Offset(rads):\n");
	printf("%f\n\n", phase);

	uint8_t *converteddata = samplesToBytes(samples_shifted, numsamples_shifted, phase);
	int finaldatalength = originaldatalength - 1;
	printf("Converted data:\n");
	for (unsigned int i = 0; i < finaldatalength; i++) {//-1 to account for filter delay
		printf("%d", converteddata[i]);
	}
	printf("\n\n");

	int counter = 0;
	for(int i = 0; i<finaldatalength; i++){
		if(originaldata[i] == converteddata[i]){
			counter++;
		}
	}
	if(counter == finaldatalength){
		printf("Successful demodulation!\n\n");
	} else {
		printf("Unuccessful demodulation!\n\n");
	}


	free(originaldata);
	free(samples);
	free(samples_upsampled);
	free(converteddata);
}

//Current full-data-pipeline test
bool fullSendTest(void) {
	
	say_hello();
	//Init all the things. 
	//Array pointers are init'd to NULL as they are malloc'd and re-assigned within the packetizing functions
	packet_t packet_data;//malloc this?
	packet_data.selected_fec_scheme = LDPC;
	uint8_t* packet_vector = NULL;
	unsigned int packet_length;
	unsigned int frame_length;
	uint8_t* frame_vector = NULL;//malloc? yes

	packet_data.data = generateRandPacket();

	printf("Original Data:\n");
	for (unsigned int i = 0; i < PACKET_DATA_LENGTH_NO_FEC; i++) {
	//	printf("%d", packet_data.data[i]);
	}
	// printf("\n\n");

	printf("getting CRC\n");
	getCRC(&packet_data);

	//Commented out functions are not yet implemented, so cannot be tested
	printf("applying FEC \n");
	applyFEC(packet_data.data);

	printf("assembling packet \n");
	assemblePacket(&packet_data, &packet_vector, &packet_length);
	printf("applying interleaving \n");
	applyInterleaving(packet_vector, packet_length);
	
	printf("assembling frame \n");
	assembleFrame(&frame_vector, &frame_length, packet_vector, packet_length);
	unsigned int preamble_length = frame_length - packet_length;

	printf("Orignal frame:\n");
	for (unsigned int i = 0; i < frame_length; i++) {
	//	printf("%d", frame_vector[i]);
	}
	// printf("\n\n");

	printf("scrambling eggs \n");
	applyScrambling(&frame_vector, frame_length, preamble_length);

	printf("Post-scramble:\n");
	for (unsigned int i = 0; i < frame_length; i++) {
	//	printf("%d", frame_vector[i]);
	}
	// printf("\n\n");

	//Comment or un-comment, depending on the test you are trying to run
	//TODO consider turning into macro functionality in future
	//applyChannel(frame_vector, frame_length);

	//printf("New frame (after going through channel):\n");
	//for (unsigned int i = 0; i < frame_length; i++) {
	//	printf("%d", frame_vector[i]);
	//}
	//printf("\n\n");

	//Init "rx" stuff
	packet_t rxpacket_data;//malloc this?
	rxpacket_data.data = (uint8_t*)malloc(packet_data_length_with_fec_bytes);
	uint8_t* rxpacket_vector = NULL;
	unsigned int rxpacket_length = 0;

	printf("removing eggs\n");
	removeScrambling(&frame_vector, frame_length, preamble_length);

	printf("disassembling frame \n");
	disassembleFrame(frame_vector, &rxpacket_vector, frame_length);
	printf("removing interleaving \n");
	removeInterleaving(rxpacket_vector, packet_length);

	printf("disassembling packet \n");
	disassemblePacket(&rxpacket_data, rxpacket_vector, packet_length);
	printf("removing FEC \n");
	removeFEC(rxpacket_data.data);

	printf("Received Data:\n");
	for (unsigned int i = 0; i < PACKET_DATA_LENGTH_NO_FEC; i++) {
	//	printf("%d", rxpacket_data.data[i]);
	}
	// printf("\n\n");

	if (checkCRC(&rxpacket_data)) {
		printf("CRC Doesn't Match!\n\n");
	}
	else {
		printf("CRC Matches!\n\n");
	}


	//Must free everything malloc'd
	printf("freeing the children \n");
	free(packet_data.data);
	free(rxpacket_data.data);
	free(rxpacket_vector);
	free(packet_vector);
	free(frame_vector);
	return 0;
}


// void testMLSAutoCorrelation(void){

// 	getMaximumLengthSequencePreamble(uint8_t ** mls_preamble, unsigned int *mls_preamble_length) {
// 	findAutocorrelation(uint8_t * samples){

// }
