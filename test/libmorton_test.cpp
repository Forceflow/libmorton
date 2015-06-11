// Utility libraries
#include "util.h"
#include "libmorton_test.h"
#include <iostream>
#include <iomanip>
#include <inttypes.h>

#define MAX 256 // MAX x MAX x MAX coordinates will be tested / generated
const size_t total = MAX*MAX*MAX;
double LUT_ms = 0; // save LUT timing

using namespace std;

void checkCorrectness(){
	printf("++ Checking correctness of encoding methods ... ");
	int failures = 0;
	for (size_t i = 0; i < 16; i++){
		for (size_t j = 0; j < 16; j++){
			for (size_t k = 0; k < 16; k++){
				uint64_t correct_code = control_morton[k + (j * 16) + (i * 16 * 16)];
				uint64_t lut_result = mortonEncode_LUT(i, j, k);
				uint64_t magicbits_result = mortonEncode_magicbits(i, j, k);
				uint64_t for_result = mortonEncode_for(i, j, k);
				if (lut_result != correct_code){printf(" Problem with correctness of LUT based method: %zu does not match %zu \n", lut_result, correct_code); failures++;}
				if (magicbits_result != correct_code){printf(" Problem with correctness of Magicbits based method: %zu does not match %zu \n", magicbits_result, correct_code); failures++;}
				if (for_result != correct_code){printf(" Problem with correctness of Magicbits based method: %zu does not match %zu \n", for_result, correct_code); failures++;}
			}
		}
	}
	if (failures != 0){printf("Correctness test failed \n");} else {printf("Passed. \n");}
}

#pragma optimize( "", off ) // don't optimize this, we're measuring performance here
void encodePerformanceTestLinear(){
	cout << "++ Encoding " << MAX << "^3 morton codes in LINEAR order (" << total << " in total)" << endl;

	Timer morton_LUT, morton_magicbits, morton_for;
	morton_LUT.reset(); morton_LUT.start();
	for (size_t i = 0; i < MAX; i++){
		for (size_t j = 0; j < MAX; j++){
			for (size_t k = 0; k < MAX; k++){
				mortonEncode_LUT(i, j, k);
			}
		}
	}
	morton_LUT.stop();
	cout << " LUT-based method: " << morton_LUT.getTotalTimeMs() << " ms" << endl;
	LUT_ms = morton_LUT.getTotalTimeMs();

	morton_magicbits.reset(); morton_magicbits.start();
	for (size_t i = 0; i < MAX; i++){
		for (size_t j = 0; j < MAX; j++){
			for (size_t k = 0; k < MAX; k++){
				mortonEncode_magicbits(i, j, k);
			}
		}
	}
	morton_magicbits.stop();
	cout << " Magic bits-based method: " << morton_magicbits.getTotalTimeMs() << " ms" << endl;

	morton_for.reset(); morton_for.start();
	for (size_t i = 0; i < MAX; i++){
		for (size_t j = 0; j < MAX; j++){
			for (size_t k = 0; k < MAX; k++){
				mortonEncode_for(i, j, k);
			}
		}
	}
	morton_for.stop();
	cout << " For-loop method: " << morton_for.getTotalTimeMs() << " ms" << endl;
}

#pragma optimize( "", off ) // don't optimize this, we're measuring performance here
void encodePerformanceTestRandom(){
	cout << "++ Encoding " << MAX << "^3 morton codes in RANDOM order (" << total << " in total)" << endl;

	// generate random coordinates in double array (because we're fancy like that)
	cout << " Generating random coordinates ... ";
	int **arr = (int **)malloc(total * sizeof(int *));
	for (int i = 0; i < total; i++){ arr[i] = (int *)malloc(3 * sizeof(int));}
	for (size_t i = 0; i < total; i++){ arr[i][0] = rand() % MAX; arr[i][1] = rand() % MAX; arr[i][2] = rand() % MAX;}
	cout << " done." << endl;

	// init timers
	Timer morton_LUT, morton_magicbits, morton_for;

	// Start testing performance
	morton_LUT.reset(); morton_LUT.start();
	for (size_t i = 0; i < total; i++){mortonEncode_LUT(arr[i][0], arr[i][1], arr[i][2]);}
	morton_LUT.stop();
	cout << " LUT-based method: " << morton_LUT.getTotalTimeMs() << " ms" << endl;

	morton_magicbits.reset(); morton_magicbits.start();
	for (size_t i = 0; i < total; i++){mortonEncode_magicbits(arr[i][0], arr[i][1], arr[i][2]);}
	morton_magicbits.stop();
	cout << " Magic bits-based method: " << morton_magicbits.getTotalTimeMs() << " ms" << endl;

	morton_for.reset(); morton_for.start();
	for (size_t i = 0; i < total; i++){ mortonEncode_for(arr[i][0], arr[i][1], arr[i][2]);}
	morton_for.stop();
	cout << " For-loop method: " << morton_for.getTotalTimeMs() << " ms" << endl;

	// Get rid of memory
	for (int i = 0; i < total; i++){free(arr[i]);}
	free(arr);
}

#pragma optimize( "", off ) // don't optimize this, we're measuring performance here
void decodePerformanceTestLinear(){
	cout << "++ Decoding " << MAX << "^3 morton codes in LINEAR order (" << total << " in total)" << endl;
	Timer morton_decode_magicbits, morton_decode_for;
	morton_decode_magicbits.reset(); morton_decode_magicbits.start();
	for (size_t i = 0; i < total; i++){
		mortonDecode_magicbits_X(i);
		mortonDecode_magicbits_Y(i);
		mortonDecode_magicbits_Z(i);
	}
	morton_decode_magicbits.stop();
	cout << " Magicbits method: " << morton_decode_magicbits.getTotalTimeMs() << " ms" << endl; 

	morton_decode_for.reset(); morton_decode_for.start();
	for (size_t i = 0; i < total; i++){
		unsigned int x, y, z = 0;
		mortonDecode_for(i, x, y, z);
	}
	morton_decode_for.stop();
	cout << " For-loop method: " << morton_decode_for.getTotalTimeMs() << " ms" << endl;
}

int main(int argc, char *argv[]) {
	checkCorrectness();
	encodePerformanceTestLinear();
	encodePerformanceTestRandom();
	decodePerformanceTestLinear();
}
