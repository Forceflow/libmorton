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

template <typename morton, typename coord>
static bool check3D_DecodeFunction(string method_tested, void (*decode_function)(const morton m, coord &x, coord &y, coord &z)){
	bool everything_okay = true;
	coord x, y, z;
	for (morton i = 0; i < 4096; i++){
		decode_function(i, x, y, z);
		if (x != control_3D_Decode[i][0] || y != control_3D_Decode[i][1] || z != control_3D_Decode[i][2]){
			cout << endl << "    Incorrect decoding of " << i << " in method " << method_tested.c_str() << ": (" << x << ", " << y << ", " << z
				<< ") != (" << control_3D_Decode[i][0] << ", " << control_3D_Decode[i][1] << ", " << control_3D_Decode[i][2] << ")" << endl;
			everything_okay = false;
		}
	}
	if (sizeof(morton) >= 4){ // Let's do some more tests
		decode_function(0x7fffffffffffffff, x, y, z);
		if (x != 0x1fffff || y != 0x1fffff || z != 0x1fffff){
			cout << endl << "    Incorrect decoding of " << 0x7fffffffffffffff << " in method " << method_tested.c_str() << ": (" << x << ", " << y << ", " << z
				<< ") != (" << 0x1fffff << ", " << 0x1fffff << ", " << 0x1fffff << ")" << endl; 
			everything_okay = false;
		}
	}
	return everything_okay;
}

template <typename morton, typename coord>
static bool check3D_EncodeFunction(string method_tested, morton (*encode_function)(const coord x, const coord y, const coord z)){
	bool everything_okay = true;
	morton computed_code, correct_code = 0;
	for (coord i = 0; i < 16; i++){
		for (coord j = 0; j < 16; j++){
			for (coord k = 0; k < 16; k++){
				correct_code = control_3D_Encode[k + (j * 16) + (i * 16 * 16)];
				computed_code = encode_function(i, j, k);
				if (computed_code != correct_code){
					everything_okay = false;
					cout << endl << "    Incorrect encoding of (" << i << ", " << j << ", " << k << ") in method " << method_tested.c_str() << ": " << computed_code <<
						" != " << correct_code << endl;
				}
			}
		}
	}
	return everything_okay;
}

static void check3D_DecodeCorrectness(){
	printf("++ Checking correctness of 3D decoding methods ... ");
	bool ok = true;
	ok &= check3D_DecodeFunction<uint_fast64_t, uint_fast32_t>("64bit 3D Shifted LUT ", &morton3D_64_Decode_LUT_shifted);
	ok &= check3D_DecodeFunction<uint_fast64_t, uint_fast32_t>("64bit 3D LUT ", &morton3D_64_Decode_LUT);
	ok &= check3D_DecodeFunction<uint_fast64_t, uint_fast32_t>("64bit 3D Magic Bits", &morton3D_64_Decode_magicbits);
	ok &= check3D_DecodeFunction<uint_fast64_t, uint_fast32_t>("64bit 3D For-loop", &morton3D_64_Decode_for);
	if(ok){printf(" Passed. \n");}else{ printf("    One or more methods failed. \n");}
}

static void check3D_EncodeCorrectness(){
	printf("++ Checking correctness of 3D encoding methods ... ");
	bool ok = true;
	ok &= check3D_EncodeFunction<uint_fast64_t, uint_fast32_t>("64bit 3D Shifted LUT ", &morton3D_64_Encode_LUT_shifted);
	ok &= check3D_EncodeFunction<uint_fast64_t, uint_fast32_t>("64bit 3D LUT ", &morton3D_64_Encode_LUT);
	ok &= check3D_EncodeFunction<uint_fast64_t, uint_fast32_t>("64bit 3D Magic Bits", &morton3D_64_Encode_magicbits);
	ok &= check3D_EncodeFunction<uint_fast64_t, uint_fast32_t>("64bit 3D For-loop", &morton3D_64_Encode_for);
	if(ok){printf(" Passed. \n");}else{ printf("    One or more methods failed. \n");}
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
	cout << "++ Checking all methods for correctness" << endl;
	check3D_EncodeCorrectness();
	check3D_DecodeCorrectness();
	cout << "++ Running each performance test " << times << " times and averaging results" << endl;
	for (int i = 128; i <= 512; i = i * 2){
		MAX = i;
		total = MAX*MAX*MAX;
		Encode_3D_LinearPerf();
		Encode_3D_RandomPerf();
		Decode_3D_LinearPerf();
		Decode_3D_RandomPerf();
	}
}