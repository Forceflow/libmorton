// Libmorton Tests
// This is a program designed to test and benchmark the functionality offered by the libmorton library
//
// Jeroen Baert 2015

// Utility headers
#include "libmorton_test.h"

// Standard headers
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <ctime>
#include <string>
#include <vector>

using namespace std;
using namespace std::chrono;

// Configuration
size_t MAX;
unsigned int times;
size_t total;
size_t RAND_POOL_SIZE = 9000;

//#define LIBMORTON_EARLY_TERMINATION

// Runningsums
vector<uint_fast64_t> running_sums;

// Make a total of all running_sum checks and print it
// This is an elaborate way to ensure no function call gets optimized away
void printRunningSums(){
	uint_fast64_t t = 0;
	cout << "Running sums check: ";
	for(int i = 0; i < running_sums.size(); i++) {
		t+= running_sums[i];
	}
	cout << t << endl;
}

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
	ok &= check3D_EncodeFunction<uint_fast64_t, uint_fast32_t>("64bit 3D Shifted LUT ", &morton3D_64_Encode_LUT256_shifted);
	ok &= check3D_EncodeFunction<uint_fast64_t, uint_fast32_t>("64bit 3D LUT ", &morton3D_64_Encode_LUT256);
	ok &= check3D_EncodeFunction<uint_fast64_t, uint_fast32_t>("64bit 3D Magic Bits", &morton3D_64_Encode_magicbits);
	ok &= check3D_EncodeFunction<uint_fast64_t, uint_fast32_t>("64bit 3D For-loop", &morton3D_64_Encode_for);
	if(ok){printf(" Passed. \n");}else{ printf("    One or more methods failed. \n");}
}

template <typename morton, typename coord>
static float testEncode_3D_Linear_Perf(morton(*function)(coord, coord, coord), size_t times){
	Timer timer = Timer();
	morton runningsum = 0;
	for (size_t t = 0; t < times; t++){
		for (size_t i = 0; i < MAX; i++){
			for (size_t j = 0; j < MAX; j++){
				for (size_t k = 0; k < MAX; k++){
					timer.start();
					runningsum += function(i, j, k);
					timer.stop();
				}
			}
		}
	}
	running_sums.push_back(runningsum);
	return timer.elapsed_time_milliseconds / (float) times;
}

template <typename morton, typename coord>
static float testEncode_3D_Random_Perf(morton(*function)(coord, coord, coord), size_t times){
	Timer timer = Timer();
	coord maximum = ~0;
	morton runningsum = 0;
	coord x, y, z;

	for (size_t t = 0; t < times; t++){
		// Create a pool of random numbers
		vector<coord> randnumbers;
		for (size_t i = 0; i < RAND_POOL_SIZE; i++) {
			randnumbers.push_back(rand() % maximum);
		}
		// Do the performance test
		for (size_t i = 0; i < total; i++){
			x = randnumbers[i % RAND_POOL_SIZE];
			y = randnumbers[(i + 1) % RAND_POOL_SIZE];
			z = randnumbers[(i + 2) % RAND_POOL_SIZE];
			timer.start();
			runningsum += function(x,y,z);
			timer.stop();
		}
	}
	running_sums.push_back(runningsum);
	return timer.elapsed_time_milliseconds / (float) times;
}

template <typename morton, typename coord>
static float testDecode_3D_Linear_Perf(void(*function)(const morton, coord&, coord&, coord&), size_t times){
	Timer timer = Timer();
	coord x, y, z;
	morton runningsum = 0;
	for (size_t t = 0; t < times; t++){
		for (morton i = 0; i < total; i++){
			timer.start();
			function(i,x,y,z);
			timer.stop();
			runningsum += x + y + z;
		}
	}
	running_sums.push_back(runningsum);
	return timer.elapsed_time_milliseconds / (float)times;
}

template <typename morton, typename coord>
static float testDecode_3D_Random_Perf(void(*function)(const morton, coord&, coord&, coord&), size_t times){
	Timer timer = Timer();
	coord x, y, z;
	morton maximum = ~0; // maximum for the random morton codes
	morton runningsum = 0;
	morton m;

	// Create a pool of randum numbers
	vector<morton> randnumbers;
	for (size_t i = 0; i < RAND_POOL_SIZE; i++) {
		randnumbers.push_back((rand() + rand()) % maximum);
	}
	
	// Start performance test
	for (int t = 0; t < times; t++){
		for (size_t i = 0; i < total; i++){
			m = randnumbers[i % RAND_POOL_SIZE];
			timer.start();
			function(m,x,y,z);
			timer.stop();
			runningsum += x + y + z;
		}
	}
	running_sums.push_back(runningsum);
	return timer.elapsed_time_milliseconds / (float)times;
}

// Test performance of encoding methods for a linear stream of coordinates
static void Encode_3D_LinearPerf() {
	cout << "++ Encoding " << MAX << "^3 morton codes in LINEAR order (" << total << " in total)" << endl;
#if _WIN64 || __x86_64__
	cout << "    64-bit LUT preshifted: " << testEncode_3D_Linear_Perf<uint_fast64_t, uint_fast32_t>(&morton3D_64_Encode_LUT256_shifted, times) << " ms" << endl;
	cout << "    64-bit LUT:            " << testEncode_3D_Linear_Perf<uint_fast64_t, uint_fast32_t>(&morton3D_64_Encode_LUT256, times) << " ms" << endl;
	cout << "    64-bit Magicbits:      " << testEncode_3D_Linear_Perf<uint_fast64_t, uint_fast32_t>(&morton3D_64_Encode_magicbits, times) << " ms" << endl;
	cout << "    64-bit For:            " << testEncode_3D_Linear_Perf<uint_fast64_t, uint_fast32_t>(&morton3D_64_Encode_for, times) << " ms" << endl;
#else
	cout << "    32-bit LUT preshifted: " << testEncode_3D_Linear_Perf<uint_fast32_t, uint_fast16_t>(&morton3D_32_Encode_LUT_shifted, times) << " ms" << endl;
	cout << "    32-bit LUT:            " << testEncode_3D_Linear_Perf<uint_fast32_t, uint_fast16_t>(&morton3D_32_Encode_LUT, times) << " ms" << endl;
#endif
}

// Test performance of encoding methods for a random stream of coordinates
static void Encode_3D_RandomPerf() {
	cout << "++ Encoding " << MAX << "^3 morton codes in RANDOM order (" << total << " in total)" << endl;
#if _WIN64 || __x86_64__
	cout << "    64-bit LUT preshifted: " << testEncode_3D_Random_Perf<uint_fast64_t, uint_fast32_t>(&morton3D_64_Encode_LUT256_shifted, times) << " ms" << endl;
	cout << "    64-bit LUT:            " << testEncode_3D_Random_Perf<uint_fast64_t, uint_fast32_t>(&morton3D_64_Encode_LUT256, times) << " ms" << endl;
	cout << "    64-bit Magicbits:      " << testEncode_3D_Random_Perf<uint_fast64_t, uint_fast32_t>(&morton3D_64_Encode_magicbits, times) << " ms" << endl;
	cout << "    64-bit For:            " << testEncode_3D_Random_Perf<uint_fast64_t, uint_fast32_t>(&morton3D_64_Encode_for, times) << " ms" << endl;
#else
	cout << "    32-bit LUT preshifted: " << testEncode_3D_Random_Perf<uint_fast32_t, uint_fast16_t>(&morton3D_32_Encode_LUT_shifted, times) << " ms" << endl;
	cout << "    32-bit LUT:            " << testEncode_3D_Random_Perf<uint_fast32_t, uint_fast16_t>(&morton3D_32_Encode_LUT, times) << " ms" << endl;
#endif
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
#else
	cout << "++ 32-bit version" << endl;
#endif
#if _MSC_VER
	cout << "++ Compiled using MSVC" << endl;
#elif __GNUC__
	cout << "++ Compiled using GCC" << endl;<
#endif
#ifdef LIBMORTON_EARLY_TERMINATION
	cout << "++ Using intrinsics optimization." << endl;
#else
	cout << "++ Not using intrinsics optimization." << endl;
#endif
}

//void prepareLUTs{
//
//}

int main(int argc, char *argv[]) {
	times = 10;
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
		printRunningSums();
	}
}