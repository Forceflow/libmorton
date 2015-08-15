// Libmorton Tests
// This is a program designed to test and benchmark the functionality offered by the libmorton library
//
// Jeroen Baert 2015

//#define LIBMORTON_EARLY_TERMINATION

// Utility headers
#include "util.h"
#include "libmorton_test.h"
// Standard headers
#include <iostream>
#include <chrono>
#include <ctime>

using namespace std;
using namespace std::chrono;

// Configuration
size_t MAX;
unsigned int times;
size_t total; 
bool x64 = false;

static void check3D_DecodeCorrectness(){
	printf("++ Checking correctness of decoding methods ... ");
	size_t failures = 0;
	for (size_t i = 0; i < 4096; i++){
		uint_fast32_t correct_x = control_coords[i][0], correct_y = control_coords[i][1], correct_z = control_coords[i][2];
		uint_fast32_t x_result_lutshift, y_result_lutshift, z_result_lutshift;
		uint_fast32_t x_result_lut, y_result_lut, z_result_lut;
		uint_fast32_t x_result_magicbits, y_result_magicbits, z_result_magicbits;
		uint_fast32_t x_result_for, y_result_for, z_result_for;
		morton3D_64_Decode_LUT_shifted(i, x_result_lutshift, y_result_lutshift, z_result_lutshift);
		morton3D_64_Decode_LUT(i, x_result_lut, y_result_lut, z_result_lut);
		morton3D_64_Decode_magicbits(i, x_result_magicbits, y_result_magicbits, z_result_magicbits);
		morton3D_64_Decode_for(i, x_result_for, y_result_for, z_result_for);
		if (x_result_lutshift != correct_x || y_result_lutshift != correct_y || z_result_lutshift != correct_z)
		{printf("    Problem with correctness of for Shifted LUT-table based decoding: %u, %u, %u does not match %u,%u,%u",
		x_result_lutshift, y_result_lutshift, z_result_lutshift, correct_x, correct_y, correct_z); failures++;}
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
				if (lut_shifted_result != correct_code){ printf("    Problem with correctness of LUT based encoding: %llu does not match %llu \n", lut_shifted_result, correct_code); failures++; }
				if (lut_result != correct_code){printf("    Problem with correctness of LUT based encoding: %llu does not match %llu \n", lut_result, correct_code); failures++; }
				if (magicbits_result != correct_code){printf("    Problem with correctness of Magicbits based encoding: %llu does not match %llu \n", magicbits_result, correct_code); failures++;}
				if (for_result != correct_code){printf("    Problem with correctness of For loop based encoding: %llu does not match %llu \n", for_result, correct_code); failures++;}
			}
		}
	}
  //test specific values for morton code greater than 32 bits
  if (morton3D_64_Encode_LUT_shifted(0x1fffff, 0x1fffff, 0x1fffff) != 0x7fffffffffffffff){ printf(" Problem with correctness of LUT based encoding \n");  failures++; }
  if (morton3D_64_Encode_LUT(0x1fffff, 0x1fffff, 0x1fffff) != 0x7fffffffffffffff){ printf(" Problem with correctness of LUT based encoding \n"); failures++; }
  if (morton3D_64_Encode_magicbits(0x1fffff, 0x1fffff, 0x1fffff) != 0x7fffffffffffffff){ printf(" Problem with correctness of Magicbits based encoding \n");  failures++; }
  if (morton3D_64_Encode_for(0x1fffff, 0x1fffff, 0x1fffff) != 0x7fffffffffffffff){ printf(" Problem with correctness of For loop based encoding \n");  failures++; }

	if (failures != 0){printf("Correctness test failed \n");} else {printf("Passed. \n");}
}

template <typename morton, typename coord>
static float testEncode_3D_Linear_Perf(morton(*function)(coord, coord, coord), int times){
	float duration = 0;
	morton runningsum = 0;
	for (int t = 0; t < times; t++){
		for (size_t i = 0; i < MAX; i++){
			for (size_t j = 0; j < MAX; j++){
				for (size_t k = 0; k < MAX; k++){
					high_resolution_clock::time_point t1 = high_resolution_clock::now();
					runningsum += function(i, j, k);
					high_resolution_clock::time_point t2 = high_resolution_clock::now();
					duration += std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
				}
			}
		}
	}
	return duration / (float) times;
}

template <typename morton, typename coord>
static float testEncode_3D_Random_Perf(morton(*function)(coord, coord, coord), int times){
	float duration = 0;
	coord maximum = ~0;
	morton runningsum = 0;
	for (int t = 0; t < times; t++){
		for (size_t i = 0; i < total; i++){
			coord randx = rand_cmwc() % maximum;
			coord randy = rand_cmwc() % maximum;
			coord randz = rand_cmwc() % maximum;
			high_resolution_clock::time_point t1 = high_resolution_clock::now();
			runningsum += function(randx, randy, randz);
			high_resolution_clock::time_point t2 = high_resolution_clock::now();
			duration += std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
		}
	}
	return duration / (float) times;
}

// Test performance of encoding methods for a linear stream of coordinates
static void Encode_3D_LinearPerf(){
	cout << "++ Encoding " << MAX << "^3 morton codes in LINEAR order (" << total << " in total)" << endl;
#if _WIN64 || __x86_64__
	cout << "    64-bit LUT preshifted: " << testEncode_3D_Linear_Perf<uint_fast64_t, uint_fast32_t>(&morton3D_64_Encode_LUT_shifted, times) << " ms" << endl;
	cout << "    64-bit LUT:            " << testEncode_3D_Linear_Perf<uint_fast64_t, uint_fast32_t>(&morton3D_64_Encode_LUT, times) << " ms" << endl;
	cout << "    64-bit Magicbits:      " << testEncode_3D_Linear_Perf<uint_fast64_t, uint_fast32_t>(&morton3D_64_Encode_magicbits, times) << " ms" << endl;
	cout << "    64-bit For:            " << testEncode_3D_Linear_Perf<uint_fast64_t, uint_fast32_t>(&morton3D_64_Encode_for, times) << " ms" << endl;
#else
	cout << "    32-bit LUT preshifted: " << testEncode_3D_Linear_Perf<uint_fast32_t, uint_fast16_t>(&morton3D_32_Encode_LUT_shifted, times) << " ms" << endl;
	cout << "    32-bit LUT:            " << testEncode_3D_Linear_Perf<uint_fast32_t, uint_fast16_t>(&morton3D_32_Encode_LUT, times) << " ms" << endl;
#endif
}

// Test performance of encoding methods for a random stream of coordinates
static void Encode_3D_RandomPerf(){
	cout << "++ Encoding " << MAX << "^3 morton codes in RANDOM order (" << total << " in total)" << endl;
#if _WIN64 || __x86_64__
	cout << "    64-bit LUT preshifted: " << testEncode_3D_Random_Perf<uint_fast64_t, uint_fast32_t>(&morton3D_64_Encode_LUT_shifted, times) << " ms" << endl;
	cout << "    64-bit LUT:            " << testEncode_3D_Random_Perf<uint_fast64_t, uint_fast32_t>(&morton3D_64_Encode_LUT, times) << " ms" << endl;
	cout << "    64-bit Magicbits:      " << testEncode_3D_Random_Perf<uint_fast64_t, uint_fast32_t>(&morton3D_64_Encode_magicbits, times) << " ms" << endl;
	cout << "    64-bit For:            " << testEncode_3D_Random_Perf<uint_fast64_t, uint_fast32_t>(&morton3D_64_Encode_for, times) << " ms" << endl;
#else
	cout << "    32-bit LUT preshifted: " << testEncode_3D_Random_Perf<uint_fast32_t, uint_fast16_t>(&morton3D_32_Encode_LUT_shifted, times) << " ms" << endl;
	cout << "    32-bit LUT:            " << testEncode_3D_Random_Perf<uint_fast32_t, uint_fast16_t>(&morton3D_32_Encode_LUT, times) << " ms" << endl;
#endif
}

template <typename morton, typename coord>
static float testDecode_3D_Linear_Perf(void(*function)(const morton, coord&, coord&, coord&), int times){
	uint_fast64_t duration = 0;
	coord x, y, z;
	for (int t = 0; t < times; t++){
		for (morton i = 0; i < total; i++){
			high_resolution_clock::time_point t1 = high_resolution_clock::now();
			function(i,x,y,z);
			high_resolution_clock::time_point t2 = high_resolution_clock::now();
			duration += std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
		}
	}
	return duration / (float)times;
}

template <typename morton, typename coord>
static float testDecode_3D_Random_Perf(void(*function)(const morton, coord&, coord&, coord&), int times){
	uint_fast64_t duration = 0;
	init_randcmwc(std::time(0));
	coord x, y, z;
	morton maximum = ~0; // maximum for the random morton codes
	for (int t = 0; t < times; t++){
		for (size_t i = 0; i < total; i++){
			morton m = (rand_cmwc() + rand_cmwc()) % maximum;
			high_resolution_clock::time_point t1 = high_resolution_clock::now();
			function(i, x, y, z);
			high_resolution_clock::time_point t2 = high_resolution_clock::now();
			duration += std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
		}
	}
	return duration / (float)times;
}

// Test performance of decoding a linear set of morton codes
static void Decode_3D_LinearPerf(){
	cout << "++ Decoding " << MAX << "^3 morton codes in LINEAR order (" << total << " in total)" << endl;
#if _WIN64 || __x86_64__
	cout << "    64-bit LUT preshifted: " << testDecode_3D_Linear_Perf<uint_fast64_t, uint_fast32_t>(&morton3D_64_Decode_LUT_shifted, times) << " ms" << endl;
	cout << "    64-bit LUT:            " << testDecode_3D_Linear_Perf<uint_fast64_t, uint_fast32_t>(&morton3D_64_Decode_LUT, times) << " ms" << endl;
	cout << "    64-bit Magicbits:      " << testDecode_3D_Linear_Perf<uint_fast64_t, uint_fast32_t>(&morton3D_64_Decode_magicbits, times) << " ms" << endl;
	cout << "    64-bit For:            " << testDecode_3D_Linear_Perf<uint_fast64_t, uint_fast32_t>(&morton3D_64_Decode_for, times) << " ms" << endl;
#else
	//cout << "    32-bit LUT preshifted: " << testDecode_3D_Linear_Perf<uint_fast32_t, uint_fast16_t>(&morton3D_32_Decode_LUT_shifted, times) << " ms" << endl;
	//cout << "    32-bit LUT:            " << testDecode_3D_Linear_Perf<uint_fast32_t, uint_fast16_t>(&morton3D_32_Decode_LUT, times) << " ms" << endl;
#endif
}

// Test performance of decoding a random set of morton codes
static void Decode_3D_RandomPerf(){
	cout << "++ Decoding " << MAX << "^3 morton codes in RANDOM order (" << total << " in total)" << endl;
#if _WIN64 || __x86_64__
	cout << "    64-bit LUT preshifted: " << testDecode_3D_Linear_Perf<uint_fast64_t, uint_fast32_t>(&morton3D_64_Decode_LUT_shifted, times) << " ms" << endl;
	cout << "    64-bit LUT:            " << testDecode_3D_Random_Perf<uint_fast64_t, uint_fast32_t>(&morton3D_64_Decode_LUT, times) << " ms" << endl;
	cout << "    64-bit Magicbits:      " << testDecode_3D_Random_Perf<uint_fast64_t, uint_fast32_t>(&morton3D_64_Decode_magicbits, times) << " ms" << endl;
	cout << "    64-bit For:            " << testDecode_3D_Random_Perf<uint_fast64_t, uint_fast32_t>(&morton3D_64_Decode_for, times) << " ms" << endl;
#else
	//cout << "    32-bit LUT preshifted: " << testDecode_3D_Linear_Perf<uint_fast32_t, uint_fast16_t>(&morton3D_32_Decode_LUT_shifted, times) << " ms" << endl;
	//cout << "    32-bit LUT:            " << testDecode_3D_Linear_Perf<uint_fast32_t, uint_fast16_t>(&morton3D_32_Decode_LUT, times) << " ms" << endl;
#endif
}

void printHeader(){
	cout << "LIBMORTON TEST SUITE" << endl;
	cout << "--------------------" << endl;
#if _WIN64 || __x86_64__  
	cout << "++ 64-bit version" << endl;
	x64 = true;
#else
	cout << "++ 32-bit version" << endl;
#endif
#if _MSC_VER
	cout << "++ Compiled using MSVC" << endl;
#elif __GNUC__
	cout << "++ Compiled using GCC" << endl;
#endif
#ifdef LIBMORTON_EARLY_TERMINATION
	cout << "++ Using intrinsics optimization." << endl;
#else
	cout << "++ Not using intrinsics optimization." << endl;
#endif
}

int main(int argc, char *argv[]) {
	times = 20;
	printHeader();
	cout << "++ Running each test " << times << " times and averaging results" << endl;
	for (int i = 128; i <= 512; i = i * 2){
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