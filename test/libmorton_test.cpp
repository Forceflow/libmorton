// Libmorton Tests
// This is a program designed to test and benchmark the functionality offered by the libmorton library
//
// Jeroen Baert 2015

// Utility headers
#include "libmorton_test.h"
#include "test2D_correctness.h"
#include "test2D_performance.h"
#include "test3D_correctness.h"
#include "test3D_performance.h"

using namespace std;
using namespace std::chrono;
using namespace libmorton;

/// GLOBALS
// Program params
int MAXRUNSIZE = 256; // the max power of 2 we're going to run tests to (starting at 64)
size_t CURRENT_TEST_MAX; // current test run max
// Configuration
unsigned int times;
size_t total;
size_t RAND_POOL_SIZE = 10000;
// Runningsums
vector<uint_fast64_t> running_sums;
/// END GLOBALS

// 3D functions collections
vector<encode_3D_64_wrapper> f3D_64_encode; // 3D 64-bit encode functions
vector<encode_3D_32_wrapper> f3D_32_encode; // 3D 32_bit encode functions
vector<decode_3D_64_wrapper> f3D_64_decode; // 3D 64-bit decode functions
vector<decode_3D_32_wrapper> f3D_32_decode; // 3D 32_bit decode functions
// 2D functions collections
vector<encode_2D_64_wrapper> f2D_64_encode; // 2D 64-bit encode functions
vector<encode_2D_32_wrapper> f2D_32_encode; // 2D 32_bit encode functions
vector<decode_2D_64_wrapper> f2D_64_decode; // 2D 64-bit decode functions
vector<decode_2D_32_wrapper> f2D_32_decode; // 2D 32_bit decode functions

// Make a total of all running_sum checks and print it
// This is an elaborate way to ensure no function call gets optimized away
void printRunningSums(){
	uint_fast64_t t = 0;
	cout << "    Running sums check: ";
	for(size_t i = 0; i < running_sums.size(); i++) {
		t+= running_sums[i];
	}
	cout << t << endl;
}

void parseProgramParameters(int argc, char* argv[]) {
	if (argc > 1) {
		MAXRUNSIZE = atoi(argv[1]);
	}
}

void printHeader(){
	cout << "LIBMORTON TEST SUITE";
#if defined(_WIN64) || defined(__x86_64__)
	cout << " (64-bit version)";
#else
	cout << " (32-bit version)";
#endif
#if defined(_MSC_VER)
	cout << " (Compiler: MSVC " << _MSC_VER << ")" << endl;
#elif defined(__GNUC__)
    cout << " (Compiler: GCC) " << endl;
#endif
	cout << "++ Running tests until we've reached " << MAXRUNSIZE << "^3 codes" << endl;
}

// Register all the functions we want to be tested here!
void registerFunctions() {
	// Register 3D 64-bit encode functions	
	f3D_64_encode.push_back(encode_3D_64_wrapper("LUT Pre-shifted Early Termination", &m3D_e_sLUT_ET<uint_fast64_t, uint_fast32_t>));
	f3D_64_encode.push_back(encode_3D_64_wrapper("LUT Pre-shifted", &m3D_e_sLUT<uint_fast64_t, uint_fast32_t>));
	f3D_64_encode.push_back(encode_3D_64_wrapper("LUT Early Termination", &m3D_e_LUT_ET<uint_fast64_t, uint_fast32_t>));
	f3D_64_encode.push_back(encode_3D_64_wrapper("LUT", &m3D_e_LUT<uint_fast64_t, uint_fast32_t>));
	f3D_64_encode.push_back(encode_3D_64_wrapper("Magicbits", &m3D_e_magicbits<uint_fast64_t, uint_fast32_t>));
	f3D_64_encode.push_back(encode_3D_64_wrapper("For ET", &m3D_e_for_ET<uint_fast64_t, uint_fast32_t>));
	f3D_64_encode.push_back(encode_3D_64_wrapper("For", &m3D_e_for<uint_fast64_t, uint_fast32_t>));

	// Register 3D 32-bit encode functions
	f3D_32_encode.push_back(encode_3D_32_wrapper("LUT Pre-shifted Early Termination", &m3D_e_sLUT_ET<uint_fast32_t, uint_fast16_t>));
	f3D_32_encode.push_back(encode_3D_32_wrapper("LUT Pre-shifted", &m3D_e_sLUT<uint_fast32_t, uint_fast16_t>));
	f3D_32_encode.push_back(encode_3D_32_wrapper("LUT Early Termination", &m3D_e_LUT_ET<uint_fast32_t, uint_fast16_t>));
	f3D_32_encode.push_back(encode_3D_32_wrapper("LUT", &m3D_e_LUT<uint_fast32_t, uint_fast16_t>));
	f3D_32_encode.push_back(encode_3D_32_wrapper("Magicbits", &m3D_e_magicbits<uint_fast32_t, uint_fast16_t>));
	f3D_32_encode.push_back(encode_3D_32_wrapper("For ET", &m3D_e_for_ET<uint_fast32_t, uint_fast16_t>));
	f3D_32_encode.push_back(encode_3D_32_wrapper("For", &m3D_e_for<uint_fast32_t, uint_fast16_t>));
	
	// Register 3D 64-bit decode functions
	f3D_64_decode.push_back(decode_3D_64_wrapper("LUT Pre-shifted Early Termination", &m3D_d_sLUT_ET<uint_fast64_t, uint_fast32_t>));
	f3D_64_decode.push_back(decode_3D_64_wrapper("LUT Pre-shifted", &m3D_d_sLUT<uint_fast64_t, uint_fast32_t>));
	f3D_64_decode.push_back(decode_3D_64_wrapper("LUT Early Termination", &m3D_d_LUT_ET<uint_fast64_t, uint_fast32_t>));
	f3D_64_decode.push_back(decode_3D_64_wrapper("LUT", &m3D_d_LUT<uint_fast64_t, uint_fast32_t>));
	f3D_64_decode.push_back(decode_3D_64_wrapper("Magicbits", &m3D_d_magicbits<uint_fast64_t, uint_fast32_t>));
	f3D_64_decode.push_back(decode_3D_64_wrapper("For ET", &m3D_d_for_ET<uint_fast64_t, uint_fast32_t>));
	f3D_64_decode.push_back(decode_3D_64_wrapper("For", &m3D_d_for<uint_fast64_t, uint_fast32_t>));

	// Register 3D 32-bit decode functions
	f3D_32_decode.push_back(decode_3D_32_wrapper("LUT Pre-shifted Early Termination", &m3D_d_sLUT_ET<uint_fast32_t, uint_fast16_t>));
	f3D_32_decode.push_back(decode_3D_32_wrapper("LUT Pre-shifted", &m3D_d_sLUT<uint_fast32_t, uint_fast16_t>));
	f3D_32_decode.push_back(decode_3D_32_wrapper("LUT Early Termination", &m3D_d_LUT_ET<uint_fast32_t, uint_fast16_t>));
	f3D_32_decode.push_back(decode_3D_32_wrapper("LUT", &m3D_d_LUT<uint_fast32_t, uint_fast16_t>));
	f3D_32_decode.push_back(decode_3D_32_wrapper("Magicbits", &m3D_d_magicbits<uint_fast32_t, uint_fast16_t>));
	f3D_32_decode.push_back(decode_3D_32_wrapper("For ET", &m3D_d_for_ET<uint_fast32_t, uint_fast16_t>));
	f3D_32_decode.push_back(decode_3D_32_wrapper("For", &m3D_d_for<uint_fast32_t, uint_fast16_t>));

	// Register 3D BMI intrinsics if available
#if defined(__BMI2__) || defined(__AVX2__)
	f3D_64_encode.push_back(encode_3D_64_wrapper("BMI2 instruction set", &m3D_e_BMI<uint_fast64_t, uint_fast32_t>));
	f3D_32_encode.push_back(encode_3D_32_wrapper("BMI2 instruction set", &m3D_e_BMI<uint_fast32_t, uint_fast16_t>));
	f3D_64_decode.push_back(decode_3D_64_wrapper("BMI2 Instruction set", &m3D_d_BMI<uint_fast64_t, uint_fast32_t>));
	f3D_32_decode.push_back(decode_3D_32_wrapper("BMI2 Instruction set", &m3D_d_BMI<uint_fast32_t, uint_fast16_t>));
#endif

	// Register 3D AVX512 intrinsics if available
#if defined(__AVX512BITALG__)
	f3D_64_encode.push_back(encode_3D_64_wrapper("AVX512 instruction set", &m3D_e_BITALG<uint_fast64_t, uint_fast32_t>));
	f3D_32_encode.push_back(encode_3D_32_wrapper("AVX512 instruction set", &m3D_e_BITALG<uint_fast32_t, uint_fast16_t>));
	f3D_64_decode.push_back(decode_3D_64_wrapper("AVX512 Instruction set", &m3D_d_BITALG<uint_fast64_t, uint_fast32_t>));
	f3D_32_decode.push_back(decode_3D_32_wrapper("AVX512 Instruction set", &m3D_d_BITALG<uint_fast32_t, uint_fast16_t>));
#endif

	// Register 2D 64-bit encode functions	
	f2D_64_encode.push_back(encode_2D_64_wrapper("LUT Pre-shifted Early Termination", &m2D_e_sLUT_ET<uint_fast64_t, uint_fast32_t>));
	f2D_64_encode.push_back(encode_2D_64_wrapper("LUT Pre-shifted", &m2D_e_sLUT<uint_fast64_t, uint_fast32_t>));
	f2D_64_encode.push_back(encode_2D_64_wrapper("LUT Early Termination", &m2D_e_LUT_ET<uint_fast64_t, uint_fast32_t>));
	f2D_64_encode.push_back(encode_2D_64_wrapper("LUT", &m2D_e_LUT<uint_fast64_t, uint_fast32_t>));
	f2D_64_encode.push_back(encode_2D_64_wrapper("Magicbits", &m2D_e_magicbits<uint_fast64_t, uint_fast32_t>));
	f2D_64_encode.push_back(encode_2D_64_wrapper("For ET", &m2D_e_for_ET<uint_fast64_t, uint_fast32_t>));
	f2D_64_encode.push_back(encode_2D_64_wrapper("For", &m2D_e_for<uint_fast64_t, uint_fast32_t>));

	// Register 2D 32-bit encode functions
	f2D_32_encode.push_back(encode_2D_32_wrapper("LUT Pre-shifted Early Termination", &m2D_e_sLUT_ET<uint_fast32_t, uint_fast16_t>));
	f2D_32_encode.push_back(encode_2D_32_wrapper("LUT Pre-shifted", &m2D_e_sLUT<uint_fast32_t, uint_fast16_t>));
	f2D_32_encode.push_back(encode_2D_32_wrapper("LUT Early Termination", &m2D_e_LUT_ET<uint_fast32_t, uint_fast16_t>));
	f2D_32_encode.push_back(encode_2D_32_wrapper("LUT", &m2D_e_LUT<uint_fast32_t, uint_fast16_t>));
	f2D_32_encode.push_back(encode_2D_32_wrapper("Magicbits Combined", &m2D_e_magicbits_combined));
	f2D_32_encode.push_back(encode_2D_32_wrapper("Magicbits", &m2D_e_magicbits<uint_fast32_t, uint_fast16_t>));
	f2D_32_encode.push_back(encode_2D_32_wrapper("For ET", &m2D_e_for_ET<uint_fast32_t, uint_fast16_t>));
	f2D_32_encode.push_back(encode_2D_32_wrapper("For", &m2D_e_for<uint_fast32_t, uint_fast16_t>));
	
	// Register 2D 64-bit decode functions
	f2D_64_decode.push_back(decode_2D_64_wrapper("LUT Pre-shifted Early Termination", &m2D_d_sLUT_ET<uint_fast64_t, uint_fast32_t>));
	f2D_64_decode.push_back(decode_2D_64_wrapper("LUT Pre-shifted", &m2D_d_sLUT<uint_fast64_t, uint_fast32_t>));
	f2D_64_decode.push_back(decode_2D_64_wrapper("LUT Early Termination", &m2D_d_LUT_ET<uint_fast64_t, uint_fast32_t>));
	f2D_64_decode.push_back(decode_2D_64_wrapper("LUT", &m2D_d_LUT<uint_fast64_t, uint_fast32_t>));
	f2D_64_decode.push_back(decode_2D_64_wrapper("Magicbits", &m2D_d_magicbits<uint_fast64_t, uint_fast32_t>));
	f2D_64_decode.push_back(decode_2D_64_wrapper("For ET", &m2D_d_for_ET<uint_fast64_t, uint_fast32_t>));
	f2D_64_decode.push_back(decode_2D_64_wrapper("For", &m2D_d_for<uint_fast64_t, uint_fast32_t>));
	
	// Register 2D 32-bit decode functions
	f2D_32_decode.push_back(decode_2D_32_wrapper("LUT Shifted Early Termination", &m2D_d_sLUT_ET<uint_fast32_t, uint_fast16_t>));
	f2D_32_decode.push_back(decode_2D_32_wrapper("LUT Pre-shifted", &m2D_d_sLUT<uint_fast32_t, uint_fast16_t>));
	f2D_32_decode.push_back(decode_2D_32_wrapper("LUT Early Termination", &m2D_d_LUT_ET<uint_fast32_t, uint_fast16_t>));
	f2D_32_decode.push_back(decode_2D_32_wrapper("LUT", &m2D_d_LUT<uint_fast32_t, uint_fast16_t>));
	f2D_32_decode.push_back(decode_2D_32_wrapper("Magicbits Combined", &m2D_d_magicbits_combined));
	f2D_32_decode.push_back(decode_2D_32_wrapper("Magicbits", &m2D_d_magicbits<uint_fast32_t, uint_fast16_t>));
	f2D_32_decode.push_back(decode_2D_32_wrapper("For", &m2D_d_for<uint_fast32_t, uint_fast16_t>));
	f2D_32_decode.push_back(decode_2D_32_wrapper("For ET", &m2D_d_for_ET<uint_fast32_t, uint_fast16_t>));
	
}

void printFunctionStats() {
	printf("Registered 3D encoders: %ull (64-bit) %i (32-bit) \n", f3D_64_encode.size(), f3D_32_encode.size());
	printf("Registered 3D decoders: %ull (64-bit) %i (32-bit) \n", f3D_64_decode.size(), f3D_32_decode.size());
	printf("Registered 2D encoders: %ull (64-bit) %i (32-bit) \n", f2D_64_encode.size(), f2D_32_encode.size());
	printf("Registered 2D decoders: %ull (64-bit) %i (32-bit) \n", f2D_64_decode.size(), f2D_32_decode.size());
}

int main(int argc, char *argv[]) {

	// TODO: fix the mess that is the times, average runs, etc ... parameter settings
	times = 1;
	parseProgramParameters(argc, argv);
	printHeader();

	// register functions
	registerFunctions();
	printFunctionStats();

	// CORRECTNESS TESTS
	bool correct = true;
	cout << "++ Checking 3D methods for correctness" << endl;
	correct = correct && check3D_EncodeDecodeMatch<uint_fast64_t, uint_fast32_t, 64>(f3D_64_encode, f3D_64_decode, times);
	correct = correct && check3D_EncodeDecodeMatch<uint_fast32_t, uint_fast16_t, 32>(f3D_32_encode, f3D_32_decode, times);
	correct = correct && check3D_EncodeCorrectness<uint_fast64_t, uint_fast32_t, 64>(f3D_64_encode);
	correct = correct && check3D_EncodeCorrectness<uint_fast32_t, uint_fast16_t, 32>(f3D_32_encode);
	correct = correct && check3D_DecodeCorrectness<uint_fast64_t, uint_fast32_t, 64>(f3D_64_decode);
	correct = correct && check3D_DecodeCorrectness<uint_fast32_t, uint_fast16_t, 32>(f3D_32_decode);

	cout << "++ Checking 2D methods for correctness" << endl;
	correct = correct && check2D_EncodeDecodeMatch<uint_fast64_t, uint_fast32_t, 64>(f2D_64_encode, f2D_64_decode, times);
	correct = correct && check2D_EncodeDecodeMatch<uint_fast32_t, uint_fast16_t, 32>(f2D_32_encode, f2D_32_decode, times);
	correct = correct && check2D_EncodeCorrectness<uint_fast64_t, uint_fast32_t, 64>(f2D_64_encode);
	correct = correct && check2D_EncodeCorrectness<uint_fast32_t, uint_fast16_t, 32>(f2D_32_encode);
	correct = correct && check2D_DecodeCorrectness<uint_fast64_t, uint_fast32_t, 64>(f2D_64_decode);
	correct = correct && check2D_DecodeCorrectness<uint_fast32_t, uint_fast16_t, 32>(f2D_32_decode);
	if (!correct) {
		cout << "++ ERROR: One of the correctness tests failed." << endl;
		exit(1);
	}
	
	// PERFORMANCE TESTS
	cout << "++ Running each performance test " << times << " times and averaging results" << endl;
	for (int i = 64; i <= MAXRUNSIZE; i = i * 2){
		CURRENT_TEST_MAX = i;
		total = CURRENT_TEST_MAX * CURRENT_TEST_MAX;
		test_2D_performance(&f2D_64_encode, &f2D_32_encode, &f2D_64_decode, &f2D_32_decode);
		total = CURRENT_TEST_MAX * CURRENT_TEST_MAX * CURRENT_TEST_MAX;
		test_3D_performance(&f3D_64_encode, &f3D_32_encode, &f3D_64_decode, &f3D_32_decode);
		printRunningSums();
	}

	cout << "++ Stopped because we were instructed only to run tests until we hit " << MAXRUNSIZE << "^3 codes" << endl;
}
