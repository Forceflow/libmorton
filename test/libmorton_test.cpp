// Libmorton Tests
// This is a program designed to test and benchmark the functionality offered by the libmorton library
//
// Jeroen Baert 2015

// Utility headers
#include "util.h"
#include "libmorton_test.h"
// Standard headers
#include <iostream>
#include <iomanip>
#include <inttypes.h>

// Configuration
// MAX x MAX x MAX coordinates will be tested / generated
#define MAX 256 
static const size_t total = MAX*MAX*MAX; // We need this a lot. 

using namespace std;

static void checkDecodeCorrectness(){
	printf("++ Checking correctness of decoding methods ... ");
	int failures = 0;
	for (size_t i = 0; i < 4096; i++){
		unsigned int correct_x = control_coords[i][0], correct_y = control_coords[i][1], correct_z = control_coords[i][2];
		unsigned int x_result_magicbits, y_result_magicbits, z_result_magicbits;
		unsigned int x_result_for, y_result_for, z_result_for;
		mortonDecode_magicbits(i, x_result_magicbits, y_result_magicbits, z_result_magicbits);
		mortonDecode_for(i, x_result_for, y_result_for, z_result_for);
		if (x_result_magicbits != correct_x || y_result_magicbits != correct_y || z_result_magicbits != correct_z)
		{printf(" Problem with correctness of for loop-based decoding: %u, %u, %u does not match %u,%u,%u",
			x_result_magicbits, y_result_magicbits, z_result_magicbits, correct_x, correct_y, correct_z);}
		if ( x_result_for != correct_x || y_result_for != correct_y || z_result_for != correct_z)
		{printf(" Problem with correctness of for loop-based decoding: %u, %u, %u does not match %u,%u,%u", 
				x_result_for, y_result_for, z_result_for, correct_x, correct_y, correct_z);}
	}
	if (failures != 0){ printf("Correctness test failed \n"); } else { printf("Passed. \n"); }
}

static void checkEncodeCorrectness(){
	printf("++ Checking correctness of encoding methods ... ");
	int failures = 0;
	for (size_t i = 0; i < 16; i++){
		for (size_t j = 0; j < 16; j++){
			for (size_t k = 0; k < 16; k++){
				// correct code
				uint64_t correct_code = control_morton[k + (j * 16) + (i * 16 * 16)];
				// result all our encoding methods give
				uint64_t lut_result = mortonEncode_LUT(i, j, k);
				uint64_t magicbits_result = mortonEncode_magicbits(i, j, k);
				uint64_t for_result = mortonEncode_for(i, j, k);
				// error messages if any code does not match correct result.
				if (lut_result != correct_code){printf(" Problem with correctness of LUT based encoding: %zu does not match %zu \n", lut_result, correct_code); failures++;}
				if (magicbits_result != correct_code){printf(" Problem with correctness of Magicbits based encoding: %zu does not match %zu \n", magicbits_result, correct_code); failures++;}
				if (for_result != correct_code){printf(" Problem with correctness of Magicbits based encoding: %zu does not match %zu \n", for_result, correct_code); failures++;}
			}
		}
	}
	if (failures != 0){printf("Correctness test failed \n");} else {printf("Passed. \n");}
}

// Test performance of encoding methods for a linear stream of coordinates
#pragma optimize( "", off ) // don't optimize this, we're measuring performance here
static void encodePerformanceTestLinear(){
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

// Test performance of encoding methods for a random stream of coordinates
#pragma optimize( "", off ) // don't optimize this, we're measuring performance here
static void encodePerformanceTestRandom(){
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

	// Free all allocated memory
	for (int i = 0; i < total; i++){free(arr[i]);}
	free(arr);
}

// Test performance of decoding a linear set of morton codes
#pragma optimize( "", off ) // don't optimize this, we're measuring performance here
static void decodePerformanceTestLinear(){
	cout << "++ Decoding " << MAX << "^3 morton codes in LINEAR order (" << total << " in total)" << endl;

	// Init timers
	Timer morton_decode_magicbits, morton_decode_for;
	
	// Test magicbits method
	morton_decode_magicbits.reset(); morton_decode_magicbits.start();
	for (size_t i = 0; i < total; i++){
		mortonDecode_magicbits_X(i);
		mortonDecode_magicbits_Y(i);
		mortonDecode_magicbits_Z(i);
	}
	morton_decode_magicbits.stop();
	cout << " Magicbits method: " << morton_decode_magicbits.getTotalTimeMs() << " ms" << endl; 

	// Test For loop method
	morton_decode_for.reset(); morton_decode_for.start();
	for (size_t i = 0; i < total; i++){
		unsigned int x, y, z = 0;
		mortonDecode_for(i, x, y, z);
	}
	morton_decode_for.stop();
	cout << " For-loop method: " << morton_decode_for.getTotalTimeMs() << " ms" << endl;
}

// Test performance of decoding a random set of morton codes
#pragma optimize( "", off ) // don't optimize this, we're measuring performance here
static void decodePerformanceTestRandom(){
	cout << "++ Decoding " << MAX << "^3 morton codes in RANDOM order (" << total << " in total)" << endl;

	// generate random coordinates in array
	cout << " Generating random morton codes ... ";
	size_t* arr = (size_t *) malloc(total * sizeof(size_t));
	for (size_t i = 0; i < total; i++){arr[i] = rand() % total;}
	cout << " done." << endl;

	// init timers
	Timer morton_decode_magicbits, morton_decode_for;

	// Test magicbits method
	morton_decode_magicbits.reset(); morton_decode_magicbits.start();
	for (size_t i = 0; i < total; i++){
		size_t current = arr[i];
		mortonDecode_magicbits_X(current);
		mortonDecode_magicbits_Y(current);
		mortonDecode_magicbits_Z(current);
	}
	morton_decode_magicbits.stop();
	cout << " Magicbits method: " << morton_decode_magicbits.getTotalTimeMs() << " ms" << endl;

	// Test for loop method
	morton_decode_for.reset(); morton_decode_for.start();
	for (size_t i = 0; i < total; i++){
		unsigned int x, y, z = 0;
		mortonDecode_for(i, x, y, z);
	}
	morton_decode_for.stop();
	cout << " For-loop method: " << morton_decode_for.getTotalTimeMs() << " ms" << endl;

	// Free memory
	free(arr);
}

int main(int argc, char *argv[]) {
	checkEncodeCorrectness();
	encodePerformanceTestLinear();
	encodePerformanceTestRandom();
	checkDecodeCorrectness();
	decodePerformanceTestLinear();
	decodePerformanceTestRandom();
}