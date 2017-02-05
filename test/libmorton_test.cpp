// Libmorton Tests
// This is a program designed to test and benchmark the functionality offered by the libmorton library
//
// Jeroen Baert 2015

// Utility headers
#include "libmorton_test.h"
#include "libmorton_test_3D.h"

using namespace std;
using namespace std::chrono;

// Configuration
size_t MAX;
unsigned int times;
size_t total;
size_t RAND_POOL_SIZE = 10000;

// Runningsums
vector<uint_fast64_t> running_sums;

// 3D functions
vector<encode_3D_64_wrapper> f3D_64_encode; // 3D 64-bit encode functions
vector<encode_3D_32_wrapper> f3D_32_encode; // 3D 32_bit encode functions
vector<decode_3D_64_wrapper> f3D_64_decode; // 3D 64-bit decode functions
vector<decode_3D_32_wrapper> f3D_32_decode; // 3D 32_bit decode functions
// 2D functions
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

// Check a 3D Encode Function for correctness
template <typename morton, typename coord>
static bool check3D_EncodeFunction(const encode_f_3D_wrapper<morton, coord> &function){
	bool everything_okay = true;
	morton computed_code, correct_code = 0;
	for (coord i = 0; i < 16; i++) {
		for (coord j = 0; j < 16; j++) {
			for (coord k = 0; k < 16; k++) {
				correct_code = control_3D_Encode[k + (j * 16) + (i * 16 * 16)];
				computed_code = function.encode(i, j, k);
				if (computed_code != correct_code) {
					everything_okay = false;
					cout << endl << "    Incorrect encoding of (" << i << ", " << j << ", " << k << ") in method " << function.description.c_str() << ": " << computed_code <<
						" != " << correct_code << endl;
				}
			}
		}
	}
	return everything_okay;
}

// Check a 3D Decode Function for correctness
template <typename morton, typename coord>
static bool check3D_DecodeFunction(const decode_f_3D_wrapper<morton, coord> &function) {
	bool everything_okay = true;
	coord x, y, z;
	// check first items
	for (morton i = 0; i < 4096; i++) {
		function.decode(i, x, y, z);
		if (x != control_3D_Decode[i][0] || y != control_3D_Decode[i][1] || z != control_3D_Decode[i][2]) {
			printIncorrectDecoding3D<morton, coord>(function.description, i, x, y, z, control_3D_Decode[i][0], control_3D_Decode[i][1], control_3D_Decode[i][2]);
			everything_okay = false;
		}
	}
	if (sizeof(morton) > 4) { // Let's do some more tests
		function.decode(0x7fffffffffffffff, x, y, z);
		if (x != 0x1fffff || y != 0x1fffff || z != 0x1fffff) {
			printIncorrectDecoding3D<morton, coord>(function.description, 0x7fffffffffffffff, x, y, z, 0x1fffff, 0x1fffff, 0x1fffff);
			everything_okay = false;
		}
	}
	return everything_okay;
}

template <typename morton, typename coord>
static double testEncode_2D_Linear_Perf(morton(*function)(coord, coord), size_t times) {
	Timer timer = Timer();
	morton runningsum = 0;
	for (size_t t = 0; t < times; t++) {
		for (coord i = 0; i < MAX; i++) {
			for (coord j = 0; j < MAX; j++) {
					timer.start();
					runningsum += function(i, j);
					timer.stop();
			}
		}
	}
	running_sums.push_back(runningsum);
	return timer.elapsed_time_milliseconds / (float)times;
}

template <typename morton, typename coord>
static double testEncode_3D_Linear_Perf(morton(*function)(coord, coord, coord), size_t times){
	Timer timer = Timer();
	morton runningsum = 0;
	for (size_t t = 0; t < times; t++){
		for (coord i = 0; i < MAX; i++){
			for (coord j = 0; j < MAX; j++){
				for (coord k = 0; k < MAX; k += 8){
					timer.start();
					runningsum += function(i, j, k);
					runningsum += function(i, j, k+1);
					runningsum += function(i, j, k+2);
					runningsum += function(i, j, k+3);
					runningsum += function(i, j, k+4);
					runningsum += function(i, j, k+5);
					runningsum += function(i, j, k+6);
					runningsum += function(i, j, k+7);
					timer.stop();
				}
			}
		}
	}
	running_sums.push_back(runningsum);
	return timer.elapsed_time_milliseconds / (float) times;
}

template <typename morton, typename coord>
static double testEncode_2D_Random_Perf(morton(*function)(coord, coord), size_t times) {
	Timer timer = Timer();
	coord maximum = ~0;
	morton runningsum = 0;
	coord x, y, z;

	for (size_t t = 0; t < times; t++) {
		// Create a pool of random numbers
		vector<coord> randnumbers;
		for (size_t i = 0; i < RAND_POOL_SIZE; i++) {
			randnumbers.push_back(rand() % maximum);
		}
		// Do the performance test
		for (size_t i = 0; i < total; i++) {
			x = randnumbers[i % RAND_POOL_SIZE];
			y = randnumbers[(i + 1) % RAND_POOL_SIZE];
			timer.start();
			runningsum += function(x, y);
			timer.stop();
		}
	}
	running_sums.push_back(runningsum);
	return timer.elapsed_time_milliseconds / (float)times;
}

template <typename morton, typename coord>
static double testEncode_3D_Random_Perf(morton(*function)(coord, coord, coord), size_t times){
	Timer timer = Timer();
	coord maximum = ~0;
	morton runningsum = 0;
	const unsigned int stride = 8;
	coord x[stride];
	coord y[stride];
	coord z[stride];

	for (size_t t = 0; t < times; t++){
		// Create a pool of random numbers
		vector<coord> randnumbers;
		for (size_t i = 0; i < RAND_POOL_SIZE; i++) {
			randnumbers.push_back(rand() % maximum);
		}
		// Do the performance test
		for (size_t i = 0; i < total; i+=stride){
			// grab all the random numbers for this stride
			for (unsigned int j = 0; j < stride; j++) {
				x[j] = randnumbers[(i+j) % RAND_POOL_SIZE];
				y[j] = randnumbers[(i+j + 1) % RAND_POOL_SIZE];
				z[j] = randnumbers[(i+j + 2) % RAND_POOL_SIZE];
			}
			timer.start();
			runningsum += function(x[0], y[0], z[0]);
			runningsum += function(x[1], y[1], z[1]);
			runningsum += function(x[2], y[2], z[2]);
			runningsum += function(x[3], y[3], z[3]);
			runningsum += function(x[4], y[4], z[4]);
			runningsum += function(x[5], y[5], z[5]);
			runningsum += function(x[6], y[6], z[6]);
			runningsum += function(x[7], y[7], z[7]);
			timer.stop();
		}
	}
	running_sums.push_back(runningsum);
	return timer.elapsed_time_milliseconds / (float) times;
}

template <typename morton, typename coord>
static std::string testEncode_3D_Perf(morton(*function)(coord, coord, coord), size_t times) {
	stringstream os;
	os << setfill('0') << std::setw(6) << std::fixed << std::setprecision(3) << testEncode_3D_Linear_Perf<morton, coord>(function, times) << " ms " 
		<< testEncode_3D_Random_Perf<morton, coord>(function, times) << " ms";
	return os.str();
}

template <typename morton, typename coord>
static double testDecode_3D_Linear_Perf(void(*function)(const morton, coord&, coord&, coord&), size_t times){
	Timer timer = Timer();
	coord x, y, z;
	morton runningsum = 0;
	for (size_t t = 0; t < times; t++){
		for (morton i = 0; i < total; i += 8){
			timer.start();
			function(i, x, y, z);
			runningsum += x + y + z;
			function(i+1, x, y, z);
			runningsum += x + y + z;
			function(i+2, x, y, z);
			runningsum += x + y + z;
			function(i+3, x, y, z);
			runningsum += x + y + z;
			function(i+4, x, y, z);
			runningsum += x + y + z;
			function(i+5, x, y, z);
			runningsum += x + y + z;
			function(i+6, x, y, z);
			runningsum += x + y + z;
			function(i+7, x, y, z);
			runningsum += x + y + z;
			timer.stop();
		}
	}
	running_sums.push_back(runningsum);
	return timer.elapsed_time_milliseconds / (float)times;
}

template <typename morton, typename coord>
static double testDecode_3D_Random_Perf(void(*function)(const morton, coord&, coord&, coord&), size_t times){
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

template <typename morton, typename coord>
static std::string testDecode_3D_Perf(void(*function)(const morton, coord&, coord&, coord&), size_t times) {
	stringstream os;
	os << setfill('0') << std::setw(6) << std::fixed << std::setprecision(3) << testDecode_3D_Linear_Perf<morton, coord>(function, times) << " ms "
		<< testDecode_3D_Random_Perf<morton, coord>(function, times) << " ms";
	return os.str();
}

static void Encode_3D_Perf() {
	cout << "++ Encoding " << MAX << "^3 morton codes (" << total << " in total)" << endl;
	for (std::vector<encode_3D_64_wrapper>::iterator it = f3D_64_encode.begin(); it != f3D_64_encode.end(); it++) {
		cout << "    " << testEncode_3D_Perf((*it).encode, times) << " : 64-bit " << (*it).description << endl;
	}
	for (std::vector<encode_3D_32_wrapper>::iterator it = f3D_32_encode.begin(); it != f3D_32_encode.end(); it++) {
		cout << "    " << testEncode_3D_Perf((*it).encode, times) << " : 32-bit " << (*it).description << endl;
	}
}

inline static void Decode_3D_Perf() {
	cout << "++ Decoding " << MAX << "^3 morton codes (" << total << " in total)" << endl;
	for (std::vector<decode_3D_64_wrapper>::iterator it = f3D_64_decode.begin(); it != f3D_64_decode.end(); it++) {
		cout << "    " << testDecode_3D_Perf((*it).decode, times) << " : 64-bit " << (*it).description << endl;
	}
	for (std::vector<decode_3D_32_wrapper>::iterator it = f3D_32_decode.begin(); it != f3D_32_decode.end(); it++) {
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
	// TODO
	
	cout << "++ Running each performance test " << times << " times and averaging results" << endl;
	for (int i = 128; i <= 512; i = i * 2){
		MAX = i;
		total = MAX*MAX*MAX;
		Encode_3D_Perf();
		Decode_3D_Perf();
		printRunningSums();
	}
}
