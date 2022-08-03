#include "packet_frame.h"
#include "channel.h"
#include "samples_to_bits.h"
#include "ldpc_implementation.h"
#include "dev_utils.h"
#include "config.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

extern int mls_total_preamble_length_bits;
extern int number_of_mls_repititions;
#define PI 3.142857

#if VERBOSE_ENABLED == 1
#define dev_printf(...) printf(__VA_ARGS__)
#else
#define dev_printf(...) stub()
#endif

//Returns pointer to a randomized uint8_t array of length packet_data_length_with_fec_bytes
uint8_t * generateRandPacket(void) {
	uint8_t* data = (uint8_t*)malloc(packet_data_length_with_fec_bytes);
	for (int i = 0; i < packet_data_length_without_fec_bytes; i++) {
		data[i] = rand() & 0xff;
	}
	return data;
}

void printBitsfromBytes(uint8_t * data, unsigned int lengthbytestoprint){
	for(int i = 0; i<lengthbytestoprint; i++){
		for(int j = 0; j<8; j++){
			dev_printf("%d ", (data[i] >> (7-j)) & 0b1);
		}
	}
}


void getFECDataLengths(void) {

	// create arrays
	//packet_data_length_with_fec_bytes = fec_get_enc_msg_length(FEC_TYPE, packet_data_length_without_fec_bytes);
	packet_data_length_with_fec_bytes = (CODEWRD_L / 8)*getNumBlocksPerPacket();
}

void testSyncEdgeCases(void){
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
			case 1://Next tests: same as above, but with phase offsets
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
bool sendAndReceiveRandomlyGeneratedPacket(void) {
	
	//Array pointers are init'd to NULL as they are malloc'd and re-assigned within the packetizing functions
	packet_t packet_data;//malloc this?
	packet_data.selected_fec_scheme = LDPC;
	uint8_t* packet_vector = NULL;
	unsigned int packet_length;
	unsigned int frame_length;
	uint8_t* frame_vector = NULL;

	packet_data.data = generateRandPacket();

	dev_printf("Original Data:\n");
	for (unsigned int i = 0; i < packet_data_length_without_fec_bytes; i++) {
		dev_printf("%d,", packet_data.data[i]);
	}
	dev_printf("\n\n");
	printBitsfromBytes(packet_data.data, 20);
	dev_printf("\n\n");

	dev_printf("getting CRC\n");
	getCRC(&packet_data);

	dev_printf("assembling packet \n");
	assemblePacket(&packet_data, &packet_vector, &packet_length);

	dev_printf("$$$$$ Before encoding:\n");
	for (unsigned int i = 0; i < packet_length; i++) {
		dev_printf("%d,", packet_vector[i]);
	}
	dev_printf("\n\n");

#if LDPC_ENABLED == 1
	applyLDPC(packet_vector);
#endif

	dev_printf("\n$$$$$ After encoding:\n");
	for (unsigned int i = 0; i < packet_length; i++) {
		dev_printf("%d,", packet_vector[i]);
	}
	dev_printf("\n\n");

#if INTRLV_SCRMBL_ENABLED == 1
	printf("applying interleaving \n");
	applyInterleaving(packet_vector, packet_length);
	printf("scrambling eggs \n");
	applyScrambling(&packet_vector, packet_length);
#endif
	
	dev_printf("assembling frame \n");
	assembleFrame(&frame_vector, &frame_length, packet_vector, packet_length);
	unsigned int preamble_length = frame_length - packet_length;
	dev_printf("\nMLS bits\n");
	printBitsfromBytes(frame_vector, 20);
	dev_printf("\n\n");

	dev_printf("Orignal frame:\n");
	for (unsigned int i = 0; i < frame_length; i++) {
		dev_printf("%d", frame_vector[i]);
	}
	dev_printf("\n\n");

	dev_printf("Post-scramble:\n");
	for (unsigned int i = 0; i < frame_length; i++) {
		dev_printf("%d", frame_vector[i]);
	}
	dev_printf("\n\n");

	dev_printf("New frame (after going through channel):\n");
	for (unsigned int i = 0; i < frame_length; i++) {
		dev_printf("%d", frame_vector[i]);
	}
	dev_printf("\n\n");

	// Turn to samples
	int numsamples = 0;
	float phase = 0;
	float *samples = bytestreamToSamplestream(frame_vector, frame_length, &numsamples, phase);

#if CHANNEL_APPLIED_TO_SAMPLES == 1
	applyChannelToSamples(samples, numsamples);
#endif	//need to soften the burst erasures for demo (by changing the transition probabilities defined in channel.h)

	//receive samples via power detection
	int frame_start_index_guess = 0;//start of MLS preamble guess
	int samples_recv_length = 0;
	float * samples_recv= getIncomingSignalData(samples, &frame_start_index_guess, &samples_recv_length);

	//resample
	int numsamples_upsampled = 0;
	float * samples_upsampled = resampleInput(samples_recv, samples_recv_length, &numsamples_upsampled);
	frame_start_index_guess *= 4;

	dev_printf("Post-upsample:\n");
	for (unsigned int i = 0; i < 400; i++) {
		dev_printf("%.2f ", samples_upsampled[mls_total_preamble_length_bits*4*4 + i]);
	}
	dev_printf("\n\n");

	//Init "rx" stuff
	packet_t rxpacket_data;//malloc this?
	rxpacket_data.data = (uint8_t*)malloc(packet_data_length_with_fec_bytes);
	uint8_t* rxpacket_vector = NULL;
	int rxpacket_length = packet_length;
	rxpacket_vector = packet_vector;

#if CHANNEL_APPLIED_TO_BITS == 1
	applyBitFlips(rxpacket_vector, packet_length);
#endif

	//sync & demodulate
	rxpacket_vector = syncFrame(samples_upsampled, numsamples_upsampled, &rxpacket_length, frame_start_index_guess);
	dev_printf("\nData after syncFrame\n");
	printBitsfromBytes(rxpacket_vector, 20);
	dev_printf("\n\n");


	dev_printf("disassembling frame \n");
	disassembleFrame(frame_vector, &rxpacket_vector, frame_length);
#if INTRLV_SCRMBL_ENABLED == 1
	dev_printf("removing eggs\n");
	removeScrambling(&rxpacket_vector, packet_length);

	dev_printf("removing interleaving \n");
	removeInterleaving(rxpacket_vector, packet_length);
#endif

	dev_printf("Received Data:\n");
	for (unsigned int i = 0; i < packet_data_length_without_fec_bytes; i++) {
		dev_printf("%d,", rxpacket_vector[i]);
	}
	dev_printf("\n\n");

#if LDPC_ENABLED == 1
	decodeLDPC(rxpacket_vector);
#endif

	dev_printf("disassembling packet \n");
	disassemblePacket(&rxpacket_data, rxpacket_vector, packet_length);

	dev_printf("Decoded Data:\n");
	for (unsigned int i = 0; i < packet_data_length_without_fec_bytes; i++) {
		dev_printf("%d,", rxpacket_data.data[i]);
	}
	dev_printf("\n\n");
	printBitsfromBytes(rxpacket_data.data, 40);
	dev_printf("\n\n");

	if (checkCRC(&rxpacket_data)) {
		printf("CRC Doesn't Match!\n\n");
	}
	else {
		printf("CRC Matches!\n\n");
	}

	dev_printf("freeing the children \n");
	free(packet_data.data);
	free(rxpacket_data.data);
	free(packet_vector);
	free(frame_vector);
	free(rxpacket_vector);
	return 0;
}

//Returns % of successful (crc matching) packets
float imageSendTest(char * filename) {
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
	fp_damaged = fopen("../testdata/damaged.ppm", "wb");
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
	fp_corrected = fopen("../testdata/corrected.ppm", "wb");
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
	packet_data.total_num_packets = (uint16_t) 3*x*y/packet_data_length_without_fec_bytes;
	packet_data.total_num_packets = packet_data.total_num_packets + ((3*x*y) % packet_data_length_without_fec_bytes ? 1 : 0) - 1;
	unsigned int last_packet_data_length = (3*x*y) % packet_data_length_without_fec_bytes;
	packet_data.data = (uint8_t*)malloc((packet_data_length_without_fec_bytes)* sizeof(uint8_t)); //malloced
	packet_data.selected_fec_scheme = LDPC;

	uint8_t* packet_vector = NULL; //malloced in assemblePacket
	unsigned int packet_length;
	unsigned int frame_length;
	uint8_t* frame_vector = NULL;//malloced in assembleFrame

	packet_t rxpacket_data;
	rxpacket_data.data = (uint8_t*)malloc((packet_data_length_without_fec_bytes)* sizeof(uint8_t)); //malloced
	uint8_t* rxpacket_vector = NULL; //malloced in samplesToBytes
	int rxpacket_length = 0;
	double total_processing_time = 0;
	double pckt_processing_time = 0;
	float incorrect_crcs = 0;

	for (int i = 0; i < packet_data.total_num_packets; i++){ // each iteration is a Tx and Rx of a packet (goes up to packet_data.total_num_packets)
		
		if (fread(packet_data.data, packet_data_length_without_fec_bytes, 1, fp_origin) != 1){
			fprintf(stderr, "Error loading image '%s'\n", filename);
         	exit(1);
		}

		packet_data.current_packet_num = (uint16_t) i;
		getCRC(&packet_data);
		printf(">>>> Packet %d being transmitted\n", i);

		assemblePacket(&packet_data, &packet_vector, &packet_length);
#if LDPC_ENABLED == 1
		applyLDPC(packet_vector);
#endif
#if INTRLV_SCRMBL_ENABLED == 1
		dev_printf("applying interleaving \n");
		applyInterleaving(packet_vector, packet_length);
		dev_printf("scrambling eggs \n");
		applyScrambling(&packet_vector, packet_length);
#endif
		assembleFrame(&frame_vector, &frame_length, packet_vector, packet_length);

		int numsamples = 0;
		float phase = 0;
		float *samples = bytestreamToSamplestream(frame_vector, frame_length, &numsamples, phase);


#if CHANNEL_APPLIED_TO_SAMPLES == 1
		applyChannelToSamples(samples, numsamples);
#endif

#if AD2_DEMO == 1

		float *samples_recv = NULL;
		int frame_start_index_guess = 0;//consider stting this in loopback function if not working well
		int samples_recv_length = 0;
		samples_recv = sendAnalogLoopback(samples, numsamples, &samples_recv_length);

		//Prepend a buncha zero samples (randomly generated amount between 4 and 1000)
		int stuffing_len = 100*4;
		//int stuffing_len = (rand() % (1000 - 4 + 1)) + 4;
		int j = samples_recv_length;
		float buffer;
		samples_recv_length += stuffing_len;
		samples_recv = (float *)realloc(samples_recv, samples_recv_length*sizeof(float));//Assuming this appends extra size allocated
		for(j; j >= 0; j--){
			buffer = samples_recv[j];
			samples_recv[stuffing_len + j] = buffer;
		}
		for(j = 0; j < stuffing_len; j++){
			samples_recv[j] = 0;
		}

#else
		//receive samples via power detection
		pckt_processing_time = (double)clock()/CLOCKS_PER_SEC;
		int frame_start_index_guess = 0;//start of MLS preamble guess
		int samples_recv_length = 0;
		float * samples_recv= getIncomingSignalData(samples, &frame_start_index_guess, &samples_recv_length);

#endif

		dev_printf("Before Upsample:\n");
		for (unsigned int i = 0; i < 400; i++) {
			dev_printf("%.2f  ", samples_recv[i]);
		}
		dev_printf("\n\n");
		
		int numsamples_upsampled = 0;
		float * samples_upsampled = resampleInput(samples_recv, samples_recv_length, &numsamples_upsampled);

		frame_start_index_guess *= 4;
		dev_printf("Upsampled:\n");
		for (unsigned int i = 0; i < 400; i++) {
			dev_printf("%.2f  ", samples_upsampled[i]);
		}
		dev_printf("\n\n");

		rxpacket_vector = syncFrame(samples_upsampled, numsamples_upsampled, &rxpacket_length, frame_start_index_guess);

#if INTRLV_SCRMBL_ENABLED == 1
		dev_printf("removing eggs\n");
		removeScrambling(&rxpacket_vector, packet_length);

		dev_printf("removing interleaving \n");
		removeInterleaving(rxpacket_vector, packet_length);
#endif
		fwrite(&rxpacket_vector[2 * NUM_PACKETS_LENGTH_BYTES], packet_data_length_without_fec_bytes, 1, fp_damaged); // write to damaged file
#if LDPC_ENABLED == 1
		decodeLDPC(rxpacket_vector);
#endif
		disassemblePacket(&rxpacket_data, rxpacket_vector, packet_length);
		pckt_processing_time = (double)clock()/CLOCKS_PER_SEC - pckt_processing_time;
		dev_printf("Packet processing time = %f ms\n", pckt_processing_time*1000);
		total_processing_time += pckt_processing_time;
		
		dev_printf("> Packet %d received\n", rxpacket_data.current_packet_num);
		dev_printf("tx'd user data:\n");
		printBitsfromBytes(packet_data.data, 40);
		dev_printf("\n\n");
		dev_printf("received user data:\n");
		printBitsfromBytes(rxpacket_data.data, 40);
		dev_printf("\n\n");

		if (checkCRC(&rxpacket_data)) {
			printf("CRC Doesn't Match!\n\n");
			incorrect_crcs++;
		}
		else {
			printf("CRC Matches!\n\n");
		}
		fwrite(rxpacket_data.data, packet_data_length_without_fec_bytes, 1, fp_corrected); // write to corrected file
		
		dev_printf("freeing the children \n");
		free(packet_vector);
		free(frame_vector);
		free(rxpacket_vector);
	}

	dev_printf("Total processing time = %f ms\n", total_processing_time*1000);

	fclose(fp_origin);
	fclose(fp_damaged);
	fclose(fp_corrected);
	free(packet_data.data);
	free(rxpacket_data.data);

	return incorrect_crcs/((float)(packet_data.total_num_packets));
}


int findNumOfDifferentBits(uint8_t a, uint8_t b){
	int count = 0;
    for (int i = 0; i < 8; i++) {

        // right shift both the numbers by 'i' and
        // check if the bit at the 0th position is different
        if (((a >> i) & 1) != ((b >> i) & 1)) {
            count++;
        }
    }
	return count;
}
//returns % as decimal of incorrect bits
//Checks it on the classic engpyhs image
float checkEfficacy(void){
	FILE * fp_origin = fopen("../testdata/engphys.ppm", "rb");
	FILE * fp_endpoint = fopen("../testdata/corrected.ppm", "rb");
	char buff[16];
	int x,y, rgb_comp_color;

	//read image format
	fgets(buff, sizeof(buff), fp_origin);
	fgets(buff, sizeof(buff), fp_endpoint);
	//read image size information
	fscanf(fp_endpoint, "%d %d", &x, &y);
    fscanf(fp_origin, "%d %d", &x, &y);
	//read rgb component
	fscanf(fp_endpoint, "%d", &rgb_comp_color);
    fscanf(fp_origin, "%d", &rgb_comp_color);

	int total_num_packets = (int)3*x*y/packet_data_length_without_fec_bytes - 1;//-1 prevents overread
	int total_len_bytes = packet_data_length_without_fec_bytes*total_num_packets;
	uint8_t * sourcedata = (uint8_t*)malloc((total_len_bytes)* sizeof(uint8_t)); //malloced
	uint8_t * rxdata = (uint8_t*)malloc((total_len_bytes)* sizeof(uint8_t)); //malloced

	fread(sourcedata, total_len_bytes, 1, fp_origin);
	fread(rxdata, total_len_bytes, 1, fp_endpoint);

	float num_biterrors = 0;
	for(int i = 0; i < total_len_bytes; i++){
		num_biterrors += findNumOfDifferentBits(sourcedata[i], rxdata[i]);
	}

	return num_biterrors/((float)(8*total_len_bytes));
}