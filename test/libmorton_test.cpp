// Libmorton Tests
// This is a program designed to test and benchmark the functionality offered by the libmorton library
//
// Jeroen Baert 2015

// Utility headers
#include "libmorton_test.h"
#include "libmorton_test_2D.h"
#include "libmorton_test_3D.h"

using namespace std;
using namespace std::chrono;
using namespace libmorton;

/// GLOBALS
// Configuration
size_t MAX;
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
	cout << "Running sums check: ";
	for(int i = 0; i < running_sums.size(); i++) {
		t+= running_sums[i];
	}
	cout << t << endl;
}

template <typename morton, typename coord>
static std::string testEncode_3D_Perf(morton(*function)(coord, coord, coord), size_t times) {
	stringstream os;
	os << setfill('0') << std::setw(6) << std::fixed << std::setprecision(3) << testEncode_3D_Linear_Perf<morton, coord>(function, times) << " ms " 
		<< testEncode_3D_Random_Perf<morton, coord>(function, times) << " ms";
	return os.str();
}

template <typename morton, typename coord>
static std::string testDecode_3D_Perf(void(*function)(const morton, coord&, coord&, coord&), size_t times) {
	stringstream os;
	os << setfill('0') << std::setw(6) << std::fixed << std::setprecision(3) << testDecode_3D_Linear_Perf<morton, coord>(function, times) << " ms "
		<< testDecode_3D_Random_Perf<morton, coord>(function, times) << " ms";
	return os.str();
}

static void Encode_3D_Perf() {
	cout << "++ Encoding " << MAX << "^3 morton codes (" << total << " in total)" << endl;
	for (auto it = f3D_64_encode.begin(); it != f3D_64_encode.end(); it++) {
		cout << "    " << testEncode_3D_Perf((*it).encode, times) << " : 64-bit " << (*it).description << endl;
	}
	for (auto it = f3D_32_encode.begin(); it != f3D_32_encode.end(); it++) {
		cout << "    " << testEncode_3D_Perf((*it).encode, times) << " : 32-bit " << (*it).description << endl;
	}
}

inline static void Decode_3D_Perf() {
	cout << "++ Decoding " << MAX << "^3 morton codes (" << total << " in total)" << endl;
	for (auto it = f3D_64_decode.begin(); it != f3D_64_decode.end(); it++) {
		cout << "    " << testDecode_3D_Perf((*it).decode, times) << " : 64-bit " << (*it).description << endl;
	}
	for (auto it = f3D_32_decode.begin(); it != f3D_32_decode.end(); it++) {
		cout << "    " << testDecode_3D_Perf((*it).decode, times) << " : 32-bit " << (*it).description << endl;
	}
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
    cout << "++ Compiled using GCC" << endl;
#endif
}

// Register all the functions we want to be tested here!
void registerFunctions() {
	// Register 3D 64-bit encode functions	
	f3D_64_encode.push_back(encode_3D_64_wrapper("LUT Shifted ET", &m3D_e_sLUT_ET<uint_fast64_t, uint_fast32_t>));
	f3D_64_encode.push_back(encode_3D_64_wrapper("LUT Shifted", &m3D_e_sLUT<uint_fast64_t, uint_fast32_t>));
	f3D_64_encode.push_back(encode_3D_64_wrapper("LUT ET", &m3D_e_LUT_ET<uint_fast64_t, uint_fast32_t>));
	f3D_64_encode.push_back(encode_3D_64_wrapper("LUT", &m3D_e_LUT<uint_fast64_t, uint_fast32_t>));
	f3D_64_encode.push_back(encode_3D_64_wrapper("Magicbits", &m3D_e_magicbits<uint_fast64_t, uint_fast32_t>));
	f3D_64_encode.push_back(encode_3D_64_wrapper("For ET", &m3D_e_for_ET<uint_fast64_t, uint_fast32_t>));
	f3D_64_encode.push_back(encode_3D_64_wrapper("For", &m3D_e_for<uint_fast64_t, uint_fast32_t>));

	// Register 3D 32-bit encode functions
	f3D_32_encode.push_back(encode_3D_32_wrapper("For", &m3D_e_for<uint_fast32_t, uint_fast16_t>));
	f3D_32_encode.push_back(encode_3D_32_wrapper("For ET", &m3D_e_for_ET<uint_fast32_t, uint_fast16_t>));
	f3D_32_encode.push_back(encode_3D_32_wrapper("Magicbits", &m3D_e_magicbits<uint_fast32_t, uint_fast16_t>));
	f3D_32_encode.push_back(encode_3D_32_wrapper("LUT", &m3D_e_LUT<uint_fast32_t, uint_fast16_t>));
	f3D_32_encode.push_back(encode_3D_32_wrapper("LUT ET", &m3D_e_LUT_ET<uint_fast32_t, uint_fast16_t>));
	f3D_32_encode.push_back(encode_3D_32_wrapper("LUT Shifted", &m3D_e_sLUT<uint_fast32_t, uint_fast16_t>));
	f3D_32_encode.push_back(encode_3D_32_wrapper("LUT Shifted ET", &m3D_e_sLUT_ET<uint_fast32_t, uint_fast16_t>));

	// Register 3D 64-bit decode functions
	f3D_64_decode.push_back(decode_3D_64_wrapper("LUT Shifted ET", &m3D_d_sLUT_ET<uint_fast64_t, uint_fast32_t>));
	f3D_64_decode.push_back(decode_3D_64_wrapper("LUT Shifted", &m3D_d_sLUT<uint_fast64_t, uint_fast32_t>));
	f3D_64_decode.push_back(decode_3D_64_wrapper("LUT ET", &m3D_d_LUT_ET<uint_fast64_t, uint_fast32_t>));
	f3D_64_decode.push_back(decode_3D_64_wrapper("LUT", &m3D_d_LUT<uint_fast64_t, uint_fast32_t>));
	f3D_64_decode.push_back(decode_3D_64_wrapper("Magicbits", &m3D_d_magicbits<uint_fast64_t, uint_fast32_t>));
	f3D_64_decode.push_back(decode_3D_64_wrapper("For ET", &m3D_d_for_ET<uint_fast64_t, uint_fast32_t>));
	f3D_64_decode.push_back(decode_3D_64_wrapper("For", &m3D_d_for<uint_fast64_t, uint_fast32_t>));

	// Register 3D 32-bit decode functions
	f3D_32_decode.push_back(decode_3D_32_wrapper("For", &m3D_d_for<uint_fast32_t, uint_fast16_t>));
	f3D_32_decode.push_back(decode_3D_32_wrapper("For ET", &m3D_d_for_ET<uint_fast32_t, uint_fast16_t>));
	f3D_32_decode.push_back(decode_3D_32_wrapper("Magicbits", &m3D_d_magicbits<uint_fast32_t, uint_fast16_t>));
	f3D_32_decode.push_back(decode_3D_32_wrapper("LUT", &m3D_d_LUT<uint_fast32_t, uint_fast16_t>));
	f3D_32_decode.push_back(decode_3D_32_wrapper("LUT ET", &m3D_d_LUT_ET<uint_fast32_t, uint_fast16_t>));
	f3D_32_decode.push_back(decode_3D_32_wrapper("LUT Shifted", &m3D_d_sLUT<uint_fast32_t, uint_fast16_t>));
	f3D_32_decode.push_back(decode_3D_32_wrapper("LUT Shifted ET", &m3D_d_sLUT_ET<uint_fast32_t, uint_fast16_t>));

	// Register 3D BMI intrinsics if available
#if defined(__BMI2__) || __AVX2__
	f3D_64_encode.push_back(encode_3D_64_wrapper("BMI2 instruction set", &m3D_e_BMI<uint_fast64_t, uint_fast32_t>));
	f3D_32_encode.push_back(encode_3D_32_wrapper("BMI2 instruction set", &m3D_e_BMI<uint_fast32_t, uint_fast16_t>));
	f3D_64_decode.push_back(decode_3D_64_wrapper("BMI2 Instruction set", &m3D_d_BMI<uint_fast64_t, uint_fast32_t>));
	f3D_32_decode.push_back(decode_3D_32_wrapper("BMI2 Instruction set", &m3D_d_BMI<uint_fast32_t, uint_fast16_t>));
#endif

	// Register 2D 64-bit encode functions	
	f2D_64_encode.push_back(encode_2D_64_wrapper("LUT Shifted ET", &m2D_e_sLUT_ET<uint_fast64_t, uint_fast32_t>));
	f2D_64_encode.push_back(encode_2D_64_wrapper("LUT Shifted", &m2D_e_sLUT<uint_fast64_t, uint_fast32_t>));
	f2D_64_encode.push_back(encode_2D_64_wrapper("LUT ET", &m2D_e_LUT_ET<uint_fast64_t, uint_fast32_t>));
	f2D_64_encode.push_back(encode_2D_64_wrapper("LUT", &m2D_e_LUT<uint_fast64_t, uint_fast32_t>));
	f2D_64_encode.push_back(encode_2D_64_wrapper("Magicbits", &m2D_e_magicbits<uint_fast64_t, uint_fast32_t>));
	f2D_64_encode.push_back(encode_2D_64_wrapper("For ET", &m2D_e_for_ET<uint_fast64_t, uint_fast32_t>));
	f2D_64_encode.push_back(encode_2D_64_wrapper("For", &m2D_e_for<uint_fast64_t, uint_fast32_t>));

	// Register 2D 32-bit encode functions
	f2D_32_encode.push_back(encode_2D_32_wrapper("For", &m2D_e_for<uint_fast32_t, uint_fast16_t>));
	f2D_32_encode.push_back(encode_2D_32_wrapper("For ET", &m2D_e_for_ET<uint_fast32_t, uint_fast16_t>));
	f2D_32_encode.push_back(encode_2D_32_wrapper("Magicbits", &m2D_e_magicbits<uint_fast32_t, uint_fast16_t>));
	f2D_32_encode.push_back(encode_2D_32_wrapper("LUT", &m2D_e_LUT<uint_fast32_t, uint_fast16_t>));
	f2D_32_encode.push_back(encode_2D_32_wrapper("LUT ET", &m2D_e_LUT_ET<uint_fast32_t, uint_fast16_t>));
	f2D_32_encode.push_back(encode_2D_32_wrapper("LUT Shifted", &m2D_e_sLUT<uint_fast32_t, uint_fast16_t>));
	f2D_32_encode.push_back(encode_2D_32_wrapper("LUT Shifted ET", &m2D_e_sLUT_ET<uint_fast32_t, uint_fast16_t>));

	// Register 2D 64-bit decode functions
	f2D_64_decode.push_back(decode_2D_64_wrapper("LUT Shifted ET", &m2D_d_sLUT_ET<uint_fast64_t, uint_fast32_t>));
	f2D_64_decode.push_back(decode_2D_64_wrapper("LUT Shifted", &m2D_d_sLUT<uint_fast64_t, uint_fast32_t>));
	f2D_64_decode.push_back(decode_2D_64_wrapper("LUT ET", &m2D_d_LUT_ET<uint_fast64_t, uint_fast32_t>));
	f2D_64_decode.push_back(decode_2D_64_wrapper("LUT", &m2D_d_LUT<uint_fast64_t, uint_fast32_t>));
	f2D_64_decode.push_back(decode_2D_64_wrapper("Magicbits", &m2D_d_magicbits<uint_fast64_t, uint_fast32_t>));
	f2D_64_decode.push_back(decode_2D_64_wrapper("For ET", &m2D_d_for_ET<uint_fast64_t, uint_fast32_t>));
	f2D_64_decode.push_back(decode_2D_64_wrapper("For", &m2D_d_for<uint_fast64_t, uint_fast32_t>));
	
	// Register 2D 32-bit decode functions
	f2D_32_decode.push_back(decode_2D_32_wrapper("For", &m2D_d_for<uint_fast32_t, uint_fast16_t>));
	f2D_32_decode.push_back(decode_2D_32_wrapper("For ET", &m2D_d_for_ET<uint_fast32_t, uint_fast16_t>));
	f2D_32_decode.push_back(decode_2D_32_wrapper("Magicbits", &m2D_d_magicbits<uint_fast32_t, uint_fast16_t>));
	f2D_32_decode.push_back(decode_2D_32_wrapper("LUT", &m2D_d_LUT<uint_fast32_t, uint_fast16_t>));
	f2D_32_decode.push_back(decode_2D_32_wrapper("LUT ET", &m2D_d_LUT_ET<uint_fast32_t, uint_fast16_t>));
	f2D_32_decode.push_back(decode_2D_32_wrapper("LUT Shifted", &m2D_d_sLUT<uint_fast32_t, uint_fast16_t>));
	f2D_32_decode.push_back(decode_2D_32_wrapper("LUT Shifted ET", &m2D_d_sLUT_ET<uint_fast32_t, uint_fast16_t>));
}

int main(int argc, char *argv[]) {
	times = 1;
	printHeader();

	// register functions
	registerFunctions();

	cout << "++ Checking 3D methods for correctness" << endl;
	check3D_EncodeDecodeMatch<uint_fast64_t, uint_fast32_t>(f3D_64_encode, f3D_64_decode, times);
	check3D_EncodeDecodeMatch<uint_fast32_t, uint_fast16_t>(f3D_32_encode, f3D_32_decode, times);
	check3D_EncodeCorrectness<uint_fast64_t, uint_fast32_t>(f3D_64_encode);
	check3D_EncodeCorrectness<uint_fast32_t, uint_fast16_t>(f3D_32_encode);
	check3D_DecodeCorrectness<uint_fast64_t, uint_fast32_t>(f3D_64_decode);
	check3D_DecodeCorrectness<uint_fast32_t, uint_fast16_t>(f3D_32_decode);

	cout << "++ Checking 2D methods for correctness" << endl;
	check2D_EncodeCorrectness<uint_fast64_t, uint_fast32_t>(f2D_64_encode);
	check2D_EncodeCorrectness<uint_fast32_t, uint_fast16_t>(f2D_32_encode);
	// TODO: check 2D decode for correctness
	
	cout << "++ Running each performance test " << times << " times and averaging results" << endl;
	for (int i = 128; i <= 512; i = i * 2){
		MAX = i;
		total = MAX*MAX*MAX;
		Encode_3D_Perf();
		Decode_3D_Perf();
		printRunningSums();
	}
}
