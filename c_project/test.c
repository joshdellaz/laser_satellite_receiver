#include "packet_frame.h"
#include "channel.h"
#include "samples_to_bits.h"
#include "laser_comms.h"
#include "ad2_io.hpp"

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

extern int mls_total_preamble_length_bits;
extern int number_of_mls_repititions;
#define PI 3.142857
#define AD2_DEMO
#define LDPC_ENABLED
#define CHANNEL_ENABLED
#define INTRLV_SCRMBL_ENABLED

//Returns pointer to a randomized uint8_t array of length packet_data_length_with_fec_bytes
uint8_t * generateRandPacket(void) {
	uint8_t* data = (uint8_t*)malloc(packet_data_length_with_fec_bytes);
	for (int i = 0; i < PACKET_DATA_LENGTH_NO_FEC; i++) {
		data[i] = rand() & 0xff;
	}
	return data;
}

void printBitsfromBytes(uint8_t * data, unsigned int lengthbytestoprint){
	for(int i = 0; i<lengthbytestoprint; i++){
		for(int j = 0; j<8; j++){
			printf("%d ", (data[i] >> (7-j)) & 0b1);
		}
	}
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
	//packet_data_length_with_fec_bytes = fec_get_enc_msg_length(FEC_TYPE, PACKET_DATA_LENGTH_NO_FEC);
	packet_data_length_with_fec_bytes = CODEWRD_L / 8;
}

void simulatedAutocorSyncTest(void){
    //full MLS & randomly generated payload data.
	int mls_total_preamble_length_bytes = mls_total_preamble_length_bits/8;

	int originaldatalength = mls_total_preamble_length_bytes + packet_data_length_with_fec_bytes;
	
	unsigned int erasure_len_bits = 1250;
	int numsamples = 0;
	float phase = 0;
	int testspassed = 0;
	int rnd = 0;
	unsigned int erasure_val = 0;


	for(rnd = 0; rnd<=11; rnd++){
		printf("TEST ROUND %d:\n\n", rnd);

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
			uint8_t temp = rand() & 0xff;
			if(i < 8){
				originalframe[mls_total_preamble_length_bytes + i] = 0;
			} else {
				originalframe[mls_total_preamble_length_bytes + i] = temp;
			}
		}

		// printf("\nOriginal Data:\n");
		// for (unsigned int i = 0; i < 50; i++) {
		// 	printf("%d ", originalframe[i]);
		// }
		// printf("\n\n");



		switch (rnd){
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
				//phase = PI/2.0;
				for(int i = 0; i<(erasure_len_bits/8); i++){
					originalframe[i] = erasure_val;
				}
				
				break;
			case 10: //Next test: Original data but with 1250 bit burst erasure in middle of one MLS. pi/2 phase offset
				phase = PI/2.0;
				for(int i = 0; i<(erasure_len_bits/8); i++){
					originalframe[2*(mls_total_preamble_length_bytes)/10 + i] = erasure_val;
				}		
				break;
			case 11: //Next test: Original data but with 1250 bit burst erasure in middle of whole preamble. pi/2 phase offset
				phase = PI/2.0;
				for(int i = 0; i<(erasure_len_bits/8); i++){
					originalframe[4*(mls_total_preamble_length_bytes)/10 + i] = erasure_val;
				}		
				break;
		}

		float *samples = bytestreamToSamplestream(originalframe, originaldatalength, &numsamples, phase);

		printf("last 16*4 samples after phase shift\n");
		for(int i = 0; i < 16*4; i++){
			printf("%1.0f ", samples[numsamples - 16*4 + i]);
		}
		printf("\n");

		//Prepend a buncha zero samples (randomly generated amount between 4 and 1000)
		int stuffing_len = 8;
		//int stuffing_len = (rand() % (1000 - 4 + 1)) + 4;
		int j = numsamples;
		float buffer;
		numsamples += stuffing_len;
		samples = (float *)realloc(samples, numsamples*sizeof(float));//Assuming this appends extra size allocated
		for(j; j >= 0; j--){
			buffer = samples[j];
			samples[stuffing_len + j] = buffer;
		}
		for(j = 0; j < stuffing_len; j++){
			samples[j] = 0;
		}
		printf("ADC output Samples\n");
		for (unsigned int i = 0; i < 50*3; i++) {
			printf("%.0f", samples[i]);
		}
		printf("\n");
		printf("Last few ADC output samples:\n");
		for (unsigned int i = 0; i < 16*4; i++) {
			printf("%.0f ", samples[numsamples - 16*4 + i]);
		}
		printf("\n");


		//Channel would be applied here if implemented

		//Test it!
		int frame_start_index_guess = 0;//start of MLS preamble guess
		int samples_shifted_length = 0;
		float * samples_shifted = getIncomingSignalData(samples, &frame_start_index_guess, &samples_shifted_length);//Does this still throw a seg fault sometimes?

		printf("Last few samples before resample :\n");
		for (unsigned int i = 0; i < 16*4*4; i++) {
			printf("%.0f ", samples_shifted[samples_shifted_length - 16*4*4 + i]);
		}
		printf("\n");

		// printf("Actual frame start index = %d\n\n", stuffing_len-1);

		// printf("Power detector frame_start_index_guess (before upsampling, minus stuffing) = %d\n\n", frame_start_index_guess - 1300*4);

		int numsamples_upsampled = 0;
		float * samples_upsampled = resampleInput(samples_shifted, samples_shifted_length, &numsamples_upsampled);
		frame_start_index_guess *= 4;
		
		// printf("Samples before sync :\n");
		// for (unsigned int i = 0; i < 60*3; i++) {
		// 	printf("%.1f ", samples_upsampled[(1300-1)*4*4 + 11*4*4 + i]);//offset to account for stuffing in getIncomingSignalData
		// }
		// printf("\n\n");
		// printf("Last few samples before sync :\n");
		// for (unsigned int i = 0; i < 16*4*8 - 16; i++) {
		// 	printf("%.1f ", samples_upsampled[numsamples_upsampled - 16*4*8 + i]);
		// }
		// printf("\n");
		
		int finaldatalength = 0;
		uint8_t * converteddata = syncFrame(samples_upsampled, numsamples_upsampled, &finaldatalength, frame_start_index_guess);


		printf("Original user data:\n");
		printBitsfromBytes(&(originalframe[mls_total_preamble_length_bytes + 20]), 4);
		printf("\n");
		printf("Converted(demodulated) user data:\n");
		printBitsfromBytes(converteddata+20, 4);
		printf("\n");

		printf("Last 5 bytes of user data:\n");
		printBitsfromBytes(&(originalframe[mls_total_preamble_length_bytes + packet_data_length_with_fec_bytes - 6]), 6);
		printf("\n");
		printf("Last 5 bytes of demoded data:\n");
		printBitsfromBytes(&(converteddata[packet_data_length_with_fec_bytes - 6]), 6);
		printf("\n\n");



		int counter = 0;
		for(int i = 0; i<finaldatalength; i++){
			if(originalframe[mls_total_preamble_length_bytes + i] == converteddata[i]){
				counter++;
			}
		}
		if(counter == finaldatalength){
			printf("Successful test %d\n\n", rnd);
			testspassed++;
		} else {
			printf("Unuccessful test %d!\n\n", rnd);
		}
		free(converteddata);
		free(originalframe);
	}
	
	if(testspassed == rnd + 1){
		printf("All tests passed!\n\n");
	} else {
		printf("%d out of %d tests passed\n\n", testspassed, rnd);
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

	uint8_t *converteddata = filterbankSamplesToBytes(samples_shifted, numsamples_shifted, phase);
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
//To be added: parameters for changing signal phase, prepending zeroes, and changing channel traits, 
//current issues: free(rx)
bool fullSendTest(void) {
	
	
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
		printf("%d", packet_data.data[i]);
	}
	printf("\n\n");
	printBitsfromBytes(packet_data.data, 20);
	printf("\n\n");

	printf("getting CRC\n");
	getCRC(&packet_data);

	//Commented out functions are not yet implemented, so cannot be tested
	//printf("applying FEC \n");
	//applyFEC(packet_data.data);

	printf("assembling packet \n");
	assemblePacket(&packet_data, &packet_vector, &packet_length);
#ifdef INTRLV_SCRMBL_ENABLED
	printf("applying interleaving \n");
	applyInterleaving(packet_vector, packet_length);
	printf("scrambling eggs \n");
	applyScrambling(&packet_vector, packet_length);
#endif
	
	printf("assembling frame \n");
	assembleFrame(&frame_vector, &frame_length, packet_vector, packet_length);
	unsigned int preamble_length = frame_length - packet_length;
	printf("\nMLS bits\n");
	printBitsfromBytes(frame_vector, 20);
	printf("\n\n");

	// printf("Orignal frame:\n");
	// for (unsigned int i = 0; i < frame_length; i++) {
	// 	printf("%d", frame_vector[i]);
	// }
	// printf("\n\n");

	// printf("Post-scramble:\n");
	// for (unsigned int i = 0; i < frame_length; i++) {
	// //	printf("%d", frame_vector[i]);
	// }
	// printf("\n\n");

	//Comment or un-comment, depending on the test you are trying to run
	//TODO consider turning into macro functionality in future
	//applyChannel(frame_vector, frame_length);

	//printf("New frame (after going through channel):\n");
	//for (unsigned int i = 0; i < frame_length; i++) {
	//	printf("%d", frame_vector[i]);
	//}
	//printf("\n\n");


	//Turn to samples
	int numsamples = 0;
	float phase = 0;
	float *samples = bytestreamToSamplestream(frame_vector, frame_length, &numsamples, phase);


#ifdef CHANNEL_ENABLED
	applyChannelToSamples(samples, numsamples);
#endif	//need to soften the burst erasures for demo (by changing the transition probabilities defined in channel.h)


//TODO figure out length and naming...
#ifdef AD2_DEMO

	// printf("DAC Out:\n");
	// for (unsigned int i = 0; i < numsamples/4; i++) {
	// 	printf("%.2f ", samples[i]);
	// }
	// printf("\n\n");
	
	float *samples_recv = NULL;
	int frame_start_index_guess = 0;//consider stting this in loopback function if not working well
	int samples_recv_length = 0;
	samples_recv = sendAnalogLoopback(samples, numsamples, &samples_recv_length);

	printf("ADC In:\n");
	for (unsigned int i = 0; i < 100; i++) {
		printf("%.2f ", samples_recv[i]);
	}
	printf("\n\n");

#else
	//receive samples via power detection
	int frame_start_index_guess = 0;//start of MLS preamble guess
	int samples_recv_length = 0;
	float * samples_recv= getIncomingSignalData(samples, &frame_start_index_guess, &samples_recv_length);

#endif

	//resample
	int numsamples_upsampled = 0;
	float * samples_upsampled = resampleInput(samples_recv, samples_recv_length, &numsamples_upsampled);
	frame_start_index_guess *= 4;

	printf("Post-upsample:\n");
	for (unsigned int i = 0; i < 400; i++) {
		printf("%.2f ", samples_upsampled[mls_total_preamble_length_bits*4*4 + i]);
	}
	printf("\n\n");

	//Init "rx" stuff
	packet_t rxpacket_data;//malloc this?
	rxpacket_data.data = (uint8_t*)malloc(packet_data_length_with_fec_bytes);
	uint8_t* rxpacket_vector = NULL;
	int rxpacket_length = 0;

	//sync & demodulate
	rxpacket_vector = syncFrame(samples_upsampled, numsamples_upsampled, &rxpacket_length, frame_start_index_guess);
	// printf("\nData after syncFrame\n");
	// printBitsfromBytes(rxpacket_vector, 20);
	// printf("\n\n");


	// printf("disassembling frame \n");
	// disassembleFrame(frame_vector, &rxpacket_vector, frame_length);
#ifdef INTRLV_SCRMBL_ENABLED
	printf("removing eggs\n");
	removeScrambling(&rxpacket_vector, packet_length);

	printf("removing interleaving \n");
	removeInterleaving(rxpacket_vector, packet_length);
#endif

	printf("disassembling packet \n");
	disassemblePacket(&rxpacket_data, rxpacket_vector, packet_length);
	//printf("removing FEC \n");
	//removeFEC(rxpacket_data.data);

	printf("Received Data:\n");
	for (unsigned int i = 0; i < PACKET_DATA_LENGTH_NO_FEC; i++) {
		printf("%d", rxpacket_data.data[i]);
	}
	printf("\n\n");
	printBitsfromBytes(rxpacket_data.data, 40);
	printf("\n\n");

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
	free(packet_vector);
	free(frame_vector);
	free(rxpacket_vector);
	return 0;
}


// void testMLSAutoCorrelation(void){

// 	getMaximumLengthSequencePreamble(uint8_t ** mls_preamble, unsigned int *mls_preamble_length) {
// 	findAutocorrelation(uint8_t * samples){

// }

// //TODO
// //Reads from file. Should write file with identical contents if everything goes well!
// //Consider adding code to check that file contents match...
// bool fullSendTest_FileIO(void){

// 	int input_file_len = 0;
// 	uint8_t * inputdata;
// 	readFiletoArray("test_input.txt", inputdata, &input_file_len);
// 	int tx_frames_len = 0;
// 	uint8_t * tx_frames;
// 	fragmentDataBufferIntoFrames(inputdata, input_file_len, tx_frames, &tx_frames_len);

// 	//Consider adding channel and/or all analog stuff here...

// 	int output_file_len = 0;
// 	uint8_t * rxdata;
// 	assembleFramesIntoDataBuffer(tx_frames, tx_frames_len, rxdata, &output_file_len);
// 	writeArraytoFile("test_output.txt", rxdata, output_file_len);
// }

bool imageSendTest(char * filename) {
	
	char buff[16];
	FILE *fp_origin, *fp_damaged, *fp_corrected;
	int x,y, rgb_comp_color;

	//open PPM file for reading
	fp_origin = fopen(filename, "rb");
	if (!fp_origin) {
		fprintf(stderr, "Unable to open file '%s'\n", filename);
		exit(1);
	}
	//read image format
	if (!fgets(buff, sizeof(buff), fp_origin)) {
		perror(filename);
		exit(1);
	}
	//check the image format
    if (buff[0] != 'P' || buff[1] != '6') {
         fprintf(stderr, "Invalid image format (must be 'P6')\n");
         exit(1);
    }
	//read image size information
    if (fscanf(fp_origin, "%d %d", &x, &y) != 2) {
         fprintf(stderr, "Invalid image size (error loading '%s')\n", filename);
         exit(1);
    }
	printf("x, y: %d, %d\n",x,y);
	//read rgb component
    if (fscanf(fp_origin, "%d", &rgb_comp_color) != 1) {
         fprintf(stderr, "Invalid rgb component (error loading '%s')\n", filename);
         exit(1);
    }
    //check rgb component depth
    if (rgb_comp_color!= 255) {
         fprintf(stderr, "'%s' does not have 8-bits components\n", filename);
         exit(1);
    }


	//open PPM file for writing
	fp_damaged = fopen("damaged.ppm", "wb");
	if (!fp_damaged) {
		fprintf(stderr, "Unable to open file damaged.ppm\n");
		exit(1);
	}
	//write the header file
    //image format
    fprintf(fp_damaged, "P6\n");
    //image size
    fprintf(fp_damaged, "%d %d\n",x,y);
    // rgb component depth
    fprintf(fp_damaged, "%d",255);//no newline, since it is transmitted from file


	//open PPM file for writing
	fp_corrected = fopen("corrected.ppm", "wb");
	if (!fp_corrected) {
		fprintf(stderr, "Unable to open file corrected.ppm\n");
		exit(1);
	}
	//write the header file
    //image format
    fprintf(fp_corrected, "P6\n");
    //image size
    fprintf(fp_corrected, "%d %d\n",x,y);
    // rgb component depth
    fprintf(fp_corrected, "%d",255);//no newline, since it is transmitted from file

	
	packet_t packet_data;
	packet_data.total_num_packets = (uint16_t) 3*x*y/PACKET_DATA_LENGTH_NO_FEC;
	packet_data.total_num_packets = packet_data.total_num_packets + ((3*x*y) % PACKET_DATA_LENGTH_NO_FEC ? 1 : 0);
	unsigned int last_packet_data_length = (3*x*y) % PACKET_DATA_LENGTH_NO_FEC;
	packet_data.data = (uint8_t*)malloc((PACKET_DATA_LENGTH_NO_FEC)* sizeof(uint8_t)); //malloced
	packet_data.selected_fec_scheme = LDPC;

	uint8_t* packet_vector = NULL; //malloced in assemblePacket
	unsigned int packet_length;
	unsigned int frame_length;
	uint8_t* frame_vector = NULL;//malloced in assembleFrame

	packet_t rxpacket_data;
	rxpacket_data.data = (uint8_t*)malloc((PACKET_DATA_LENGTH_NO_FEC)* sizeof(uint8_t)); //malloced
	uint8_t* rxpacket_vector = NULL; //malloced in samplesToBytes
	int rxpacket_length = 0;

	for (int i = 0; i < 11000; i++){ // each iteration is a Tx and Rx of a packet (goes up to packet_data.total_num_packets)
		
		if (fread(packet_data.data, PACKET_DATA_LENGTH_NO_FEC, 1, fp_origin) != 1){
			fprintf(stderr, "Error loading image '%s'\n", filename);
         	exit(1);
		}

		packet_data.current_packet_num = (uint16_t) i;
		getCRC(&packet_data);
		printf(">>>> Packet %d being transmitted", i);

		assemblePacket(&packet_data, &packet_vector, &packet_length);
#ifdef LDPC_ENABLED
		applyLDPC(packet_vector);
#endif
#ifdef INTRLV_SCRMBL_ENABLED
		printf("applying interleaving \n");
		applyInterleaving(packet_vector, packet_length);
		printf("scrambling eggs \n");
		applyScrambling(&packet_vector, packet_length);
#endif
		assembleFrame(&frame_vector, &frame_length, packet_vector, packet_length);

		int numsamples = 0;
		float phase = 0;
		float *samples = bytestreamToSamplestream(frame_vector, frame_length, &numsamples, phase);

#ifdef CHANNEL_ENABLED
		applyChannelToSamples(samples, numsamples);
#endif

#ifdef AD2_DEMO

		float *samples_recv = NULL;
		int frame_start_index_guess = 0;//consider stting this in loopback function if not working well
		int samples_recv_length = 0;
		samples_recv = sendAnalogLoopback(samples, numsamples, &samples_recv_length);

#else
		//receive samples via power detection
		int frame_start_index_guess = 0;//start of MLS preamble guess
		int samples_recv_length = 0;
		float * samples_recv= getIncomingSignalData(samples, &frame_start_index_guess, &samples_recv_length);

#endif
		//resample
		int numsamples_upsampled = 0;
		float * samples_upsampled = resampleInput(samples_recv, samples_recv_length, &numsamples_upsampled);
		frame_start_index_guess *= 4;

		rxpacket_vector = syncFrame(samples_upsampled, numsamples_upsampled, &rxpacket_length, frame_start_index_guess);


#ifdef INTRLV_SCRMBL_ENABLED
		printf("removing eggs\n");
		removeScrambling(&rxpacket_vector, packet_length);

		printf("removing interleaving \n");
		removeInterleaving(rxpacket_vector, packet_length);
#endif

		fwrite(&rxpacket_vector[2 * NUM_PACKETS_LENGTH_BYTES], PACKET_DATA_LENGTH_NO_FEC, 1, fp_damaged); // write to damaged file
#ifdef LDPC_ENABLED
		decodeLDPC(rxpacket_vector);
#endif
		disassemblePacket(&rxpacket_data, rxpacket_vector, packet_length);
		printf("> Packet %d received\n", rxpacket_data.current_packet_num);

		if (checkCRC(&rxpacket_data)) {
			printf("CRC Doesn't Match!\n\n");
		}
		else {
			printf("CRC Matches!\n\n");
		}
		fwrite(rxpacket_data.data, PACKET_DATA_LENGTH_NO_FEC, 1, fp_corrected); // write to corrected file
		
		printf("freeing the children \n");
		free(packet_vector);
		free(frame_vector);
		free(rxpacket_vector);
	}

	fclose(fp_origin);
	fclose(fp_damaged);
	fclose(fp_corrected);
	free(packet_data.data);
	free(rxpacket_data.data);

	return 0;
}