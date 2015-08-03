// Libmorton Tests
// This is a program designed to test and benchmark the functionality offered by the libmorton library
//
// Jeroen Baert 2015

// Utility headers
#include "util.h"
#include "libmorton_test.h"
// Standard headers
#include <iostream>

// Configuration
size_t MAX;
size_t total; 

using namespace std;

static void check3D_DecodeCorrectness(){
	printf("++ Checking correctness of decoding methods ... ");
	size_t failures = 0;
	for (size_t i = 0; i < 4096; i++){
		uint_fast32_t correct_x = control_coords[i][0], correct_y = control_coords[i][1], correct_z = control_coords[i][2];
		uint_fast32_t x_result_lut, y_result_lut, z_result_lut;
		uint_fast32_t x_result_magicbits, y_result_magicbits, z_result_magicbits;
		uint_fast32_t x_result_for, y_result_for, z_result_for;
		morton3D_64_Decode_LUT(i, x_result_lut, y_result_lut, z_result_lut);
		morton3D_64_Decode_magicbits(i, x_result_magicbits, y_result_magicbits, z_result_magicbits);
		morton3D_64_Decode_for(i, x_result_for, y_result_for, z_result_for);
		if (x_result_lut != correct_x || y_result_lut != correct_y || z_result_lut != correct_z)
		{printf("    Problem with correctness of for LUT-table based decoding: %u, %u, %u does not match %u,%u,%u",
		x_result_lut, y_result_lut, z_result_lut, correct_x, correct_y, correct_z); failures++;}
		if (x_result_magicbits != correct_x || y_result_magicbits != correct_y || z_result_magicbits != correct_z)
		{printf("    Problem with correctness of for magicbits-based decoding: %u, %u, %u does not match %u,%u,%u",
		x_result_magicbits, y_result_magicbits, z_result_magicbits, correct_x, correct_y, correct_z); failures++;}
		if ( x_result_for != correct_x || y_result_for != correct_y || z_result_for != correct_z)
		{printf("    Problem with correctness of for loop-based decoding: %u, %u, %u does not match %u,%u,%u",
		x_result_for, y_result_for, z_result_for, correct_x, correct_y, correct_z); failures++;}
	}
	if (failures != 0){ printf("Correctness test failed %llu times \n", failures); } else { printf("Passed. \n"); }
}

static void check3D_EncodeCorrectness(){
	printf("++ Checking correctness of encoding methods ... ");
	int failures = 0;
	for (size_t i = 0; i < 16; i++){
		for (size_t j = 0; j < 16; j++){
			for (size_t k = 0; k < 16; k++){
				// fetch correct code
				uint_fast64_t correct_code = control_morton[k + (j * 16) + (i * 16 * 16)];

				// result all our encoding methods give
				uint_fast64_t lut_shifted_result = morton3D_64_Encode_LUT_shifted(i, j, k);
				uint_fast64_t lut_result = morton3D_64_Encode_LUT(i, j, k);
				uint_fast64_t magicbits_result = morton3D_64_Encode_magicbits(i, j, k);
				uint_fast64_t for_result = morton3D_64_Encode_for(i, j, k);

				// error messages if any code does not match correct result.
				if (lut_shifted_result != correct_code){ printf("    Problem with correctness of LUT based encoding: %zu does not match %zu \n", lut_shifted_result, correct_code); failures++; }
				if (lut_result != correct_code){printf("    Problem with correctness of LUT based encoding: %zu does not match %zu \n", lut_result, correct_code); failures++; }
				if (magicbits_result != correct_code){printf("    Problem with correctness of Magicbits based encoding: %zu does not match %zu \n", magicbits_result, correct_code); failures++;}
				if (for_result != correct_code){printf("    Problem with correctness of Magicbits based encoding: %zu does not match %zu \n", for_result, correct_code); failures++;}
			}
		}
	}
	if (failures != 0){printf("Correctness test failed \n");} else {printf("Passed. \n");}
}

#pragma optimize( "", off ) // don't optimize this, we're measuring performance here
template <typename morton, typename coord>
static double testEncode_3D_Linear_Perf(morton(*function)(coord, coord, coord)){
	Timer t;
	t.reset(); t.start();
	for (size_t i = 0; i < MAX; i++){
		for (size_t j = 0; j < MAX; j++){
			for (size_t k = 0; k < MAX; k++){
				function(i, j, k);
			}
		}
	}
	t.stop();
	return t.getTotalTimeMs();
}

// Test performance of encoding methods for a linear stream of coordinates
#pragma optimize( "", off ) // don't optimize this, we're measuring performance here
static void Encode_3D_LinearPerf(){
	cout << "++ Encoding " << MAX << "^3 morton codes in LINEAR order (" << total << " in total)" << endl;
	cout << "    LUT preshifted: " << testEncode_3D_Linear_Perf<uint_fast64_t, uint_fast32_t>(&morton3D_64_Encode_LUT_shifted) << " ms" << endl;
	cout << "    LUT:            " << testEncode_3D_Linear_Perf<uint_fast64_t, uint_fast32_t>(&morton3D_64_Encode_LUT) << " ms" << endl;
	cout << "    Magicbits:      " << testEncode_3D_Linear_Perf<uint_fast64_t, uint_fast32_t>(&morton3D_64_Encode_magicbits) << " ms" << endl;
	cout << "    For:            " << testEncode_3D_Linear_Perf<uint_fast64_t, uint_fast32_t>(&morton3D_64_Encode_for) << " ms" << endl;
}

// Test performance of encoding methods for a random stream of coordinates
#pragma optimize( "", off ) // don't optimize this, we're measuring performance here
static void Encode_3D_RandomPerf(){
	cout << "++ Encoding " << MAX << "^3 morton codes in RANDOM order (" << total << " in total)" << endl;

	// generate random coordinates in double array (because we're fancy like that)
	cout << "    Generating random coordinates ... ";
	uint_fast32_t* random_x = (uint_fast32_t *) malloc(total * sizeof(uint_fast32_t *));
	uint_fast32_t* random_y = (uint_fast32_t *) malloc(total * sizeof(uint_fast32_t *));
	uint_fast32_t* random_z = (uint_fast32_t *) malloc(total * sizeof(uint_fast32_t *));
	for (size_t i = 0; i < total; i++){ 
		random_x[i] = rand() % MAX; 
		random_y[i] = rand() % MAX; 
		random_z[i] = rand() % MAX; 
	}
	cout << " done." << endl;

	// init timers
	Timer morton_LUT, morton_magicbits, morton_for;

	// Start testing performance
	morton_LUT.reset(); morton_LUT.start();
	for (size_t i = 0; i < total; i++){ 
		morton_LUT.stop();
		uint_fast32_t randx = rand() % MAX;
		uint_fast32_t randy = rand() % MAX;
		uint_fast32_t randz = rand() % MAX;
		morton_LUT.start();
		morton3D_64_Encode_LUT(randx, randy, randz); 
	}
	morton_LUT.stop();
	cout << "    LUT-based method: " << morton_LUT.getTotalTimeMs() << " ms" << endl;

	morton_magicbits.reset(); morton_magicbits.start();
	for (size_t i = 0; i < total; i++){ morton3D_64_Encode_magicbits(rand() % MAX, rand() % MAX, rand() % MAX); }
	morton_magicbits.stop();
	cout << "    Magic bits-based method: " << morton_magicbits.getTotalTimeMs() << " ms" << endl;

#if MAX<=256
	morton_for.reset(); morton_for.start();
	for (size_t i = 0; i < total; i++){ morton3D_64_Encode_for(rand() % MAX, rand() % MAX, rand() % MAX); }
	morton_for.stop();
	cout << "    For-loop method: " << morton_for.getTotalTimeMs() << " ms" << endl;
#else
	cout << "    For-loop method: SKIPPED, TAKES WAY TOO LONG." << endl;
#endif

	// Free all allocated memory
	free(random_x);
	free(random_y); 
	free(random_z);
}

// Test performance of decoding a linear set of morton codes
//#pragma optimize( "", off ) // don't optimize this, we're measuring performance here
static void Decode_3D_LinearPerf(){
	cout << "++ Decoding " << MAX << "^3 morton codes in LINEAR order (" << total << " in total)" << endl;

	// Init timers
	Timer morton_decode_LUT,morton_decode_magicbits, morton_decode_for;
	// Test magicbits method
	morton_decode_LUT.reset(); morton_decode_LUT.start();
	for (size_t i = 0; i < total; i++){
		uint_fast32_t x, y, z = 0;
		morton3D_64_Decode_LUT(i,x,y,z);
	}
	morton_decode_LUT.stop();
	cout << "    LUT method: " << morton_decode_LUT.getTotalTimeMs() << " ms" << endl;
	
	// Test magicbits method
	morton_decode_magicbits.reset(); morton_decode_magicbits.start();
	for (size_t i = 0; i < total; i++){
		uint_fast32_t x, y, z = 0;
		morton3D_64_Decode_magicbits(i,x,y,z);
	}
	morton_decode_magicbits.stop();
	cout << "    Magicbits method: " << morton_decode_magicbits.getTotalTimeMs() << " ms" << endl; 

#if MAX<=256
	// Test For loop method
	morton_decode_for.reset(); morton_decode_for.start();
	for (size_t i = 0; i < total; i++){
		uint_fast32_t x, y, z = 0;
		morton3D_64_Decode_for(i, x, y, z);
	}
	morton_decode_for.stop();
	cout << "    For-loop method: " << morton_decode_for.getTotalTimeMs() << " ms" << endl;
#else
	cout << "    For-loop method: SKIPPED, TAKES WAY TOO LONG." << endl;
#endif
}

// Test performance of decoding a random set of morton codes
// #pragma optimize( "", off ) // don't optimize this, we're measuring performance here
static void Decode_3D_RandomPerf(){
	cout << "++ Decoding " << MAX << "^3 morton codes in RANDOM order (" << total << " in total)" << endl;

	// generate random coordinates in array
	cout << "    Generating random morton codes ... ";
	uint_fast64_t* arr = (uint_fast64_t *)malloc(total * sizeof(uint_fast64_t));
	for (size_t i = 0; i < total; i++){arr[i] = rand() % total;}
	cout << " done." << endl;

	// init timers
	Timer morton_decode_LUT,morton_decode_magicbits, morton_decode_for;

	// Test LUT method
	morton_decode_LUT.reset(); morton_decode_LUT.start();
	for (size_t i = 0; i < total; i++){
		uint_fast64_t current = arr[i];
		uint_fast32_t x, y, z = 0;
		morton3D_64_Decode_LUT(current, x, y, z);
	}
	morton_decode_LUT.stop();
	cout << "    LUT method: " << morton_decode_LUT.getTotalTimeMs() << " ms" << endl;

	// Test magicbits method
	morton_decode_magicbits.reset(); morton_decode_magicbits.start();
	for (size_t i = 0; i < total; i++){
		uint_fast64_t current = arr[i];
		uint_fast32_t x, y, z = 0;
		morton3D_64_Decode_magicbits(i, x, y, z);
	}
	morton_decode_magicbits.stop();
	cout << "    Magicbits method: " << morton_decode_magicbits.getTotalTimeMs() << " ms" << endl;

#if MAX<=256
	// Test for loop method
	morton_decode_for.reset(); morton_decode_for.start();
	for (size_t i = 0; i < total; i++){
		uint_fast64_t current = arr[i];
		unsigned int x, y, z = 0;
		morton3D_64_Decode_for(current, x, y, z);
	}
	morton_decode_for.stop();
	cout << "    For-loop method: " << morton_decode_for.getTotalTimeMs() << " ms" << endl;
#else
	cout << "    For-loop method: SKIPPED, TAKES WAY TOO LONG." << endl;
#endif

	// Free memory
	free(arr);
}

int main(int argc, char *argv[]) {
	cout << "LIBMORTON TEST SUITE" << endl;
	cout << "--------------------" << endl;
#ifdef LIBMORTON_USE_INTRINSICS
	cout << "++ Using hardware intrinsics." << endl;
#else
	cout << "++ Not using hardware intrinsics."<< endl;
#endif
	for (int i = 32; i <= 256; i = i * 2){
		MAX = i;
		total = MAX*MAX*MAX;
		// encoding
		check3D_EncodeCorrectness();
		Encode_3D_LinearPerf();
		Encode_3D_RandomPerf();
		// decoding
		check3D_DecodeCorrectness();
		Decode_3D_LinearPerf();
		Decode_3D_RandomPerf();
	}
}
