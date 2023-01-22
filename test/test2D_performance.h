#pragma once
#include "libmorton_test.h"

using namespace std;

// Config variables (defined elsewhere)
extern size_t RAND_POOL_SIZE;
extern size_t total;
extern size_t CURRENT_TEST_MAX;
extern unsigned int times;
extern std::vector<uint_fast64_t> running_sums;

// Check 2D encode function performance (linear)
template <typename morton, typename coord>
static double testEncode_2D_Linear_Perf(morton(*function)(coord, coord), size_t times) {
	Timer timer = Timer();
	morton runningsum = 0;
	timer.start();
	for (size_t t = 0; t < times; t++) {
		for (coord i = 0; i < CURRENT_TEST_MAX; i++) {
			for (coord j = 0; j < CURRENT_TEST_MAX; j++) {
				runningsum += function(i, j);	
			}
		}
	}
	timer.stop();
	running_sums.push_back(runningsum);
	return timer.elapsed_time_milliseconds / (float)times;
}

// Check 2D encode function performance (random)
template <typename morton, typename coord>
static double testEncode_2D_Random_Perf(morton(*function)(coord, coord), size_t times) {
	Timer timer = Timer();
	coord maximum = ~0;
	morton runningsum = 0;
	coord x, y, z;

	for (size_t t = 0; t < times; t++) {
		// Create a pool of random numbers
		std::vector<coord> randnumbers;
		for (size_t i = 0; i < RAND_POOL_SIZE; i++) {
			randnumbers.push_back(rand() % maximum);
		}
		timer.start();
		// Do the performance test
		for (size_t i = 0; i < total; i++) {
			x = randnumbers[i % RAND_POOL_SIZE];
			y = randnumbers[(i + 1) % RAND_POOL_SIZE];
			
			runningsum += function(x, y);	
		}
		timer.stop();
	}
	running_sums.push_back(runningsum);
	return timer.elapsed_time_milliseconds / (float)times;
}

// TODO: Check 2D decode function performance (linear)
template <typename morton, typename coord>
static double testDecode_2D_Linear_Perf(void(*function)(const morton, coord&, coord&), size_t times) {
	size_t max_morton = CURRENT_TEST_MAX * CURRENT_TEST_MAX;
	Timer timer = Timer();
	coord x, y;
	coord runningsum = 0;
	timer.start();
	for (size_t t = 0; t < times; t++) {
		for (morton i = 0; i < max_morton; i += 8) {		
			function(i, x, y);
			runningsum += x + y;
			function(i + 1, x, y);
			runningsum += x + y;
			function(i + 2, x, y);
			runningsum += x + y;
			function(i + 3, x, y);
			runningsum += x + y;
			function(i + 4, x, y);
			runningsum += x + y;
			function(i + 5, x, y);
			runningsum += x + y;
			function(i + 6, x, y);
			runningsum += x + y;
			function(i + 7, x, y);
			runningsum += x + y;
		}
	}
	timer.stop();
	running_sums.push_back(runningsum);
	return timer.elapsed_time_milliseconds / (float)times;
}

// TODO: Check 2D decode function performance (random)
template <typename morton, typename coord>
static double testDecode_2D_Random_Perf(void(*function)(const morton, coord&, coord&), size_t times) {
	Timer timer = Timer();
	coord x, y, z;
	morton maximum = ~0; // maximum for the random morton codes
	morton runningsum = 0;
	morton m;

	size_t max_morton = CURRENT_TEST_MAX * CURRENT_TEST_MAX;

	// Create a pool of randum numbers
	std::vector<morton> randnumbers;
	for (size_t i = 0; i < RAND_POOL_SIZE; i++) {
		randnumbers.push_back((morton(rand()) + morton(rand())) % maximum);
	}

	timer.start();
	// Start performance test
	for (size_t t = 0; t < times; t++) {
		for (size_t i = 0; i < max_morton; i++) {
			m = randnumbers[i % RAND_POOL_SIZE];
			function(m, x, y);	
			runningsum += x + y;
		}
	}
	timer.stop();

	running_sums.push_back(runningsum);
	return timer.elapsed_time_milliseconds / (float)times;
}

static void test_2D_performance(vector<encode_2D_64_wrapper>* funcs64_encode, vector<encode_2D_32_wrapper>* funcs32_encode,
	vector<decode_2D_64_wrapper>* funcs64_decode, vector<decode_2D_32_wrapper>* funcs32_decode) {
	cout << "++ (2D) Encoding " << CURRENT_TEST_MAX << "^2 morton codes (" << total << " in total)" << endl;
	cout << "+++ (2D) Encoding 64-bit sized morton codes" << endl;
	stringstream os;
	//os << setfill('0') << std::setw(6) << std::fixed << std::setprecision(3);
	for (auto it = (*funcs64_encode).begin(); it != (*funcs64_encode).end(); it++) {
		os.str("");
		os << testEncode_2D_Linear_Perf((*it).encode, times) << " ms\t";
		os << testEncode_2D_Random_Perf((*it).encode, times) << " ms\t";
		cout << os.str() << "64-bit " << (*it).description << endl;
	}
	cout << "+++ (2D) Encoding 32-bit sized morton codes" << endl;
	for (auto it = (*funcs32_encode).begin(); it != (*funcs32_encode).end(); it++) {
		os.str("");
		os << testEncode_2D_Linear_Perf((*it).encode, times) << " ms\t";
		os << testEncode_2D_Random_Perf((*it).encode, times) << " ms\t";
		cout << os.str() << "32-bit " << (*it).description << endl;
	}
	 cout << "++ (2D) Decoding " << CURRENT_TEST_MAX << "^2 morton codes (" << total << " in total)" << endl;
	 cout << "+++ (2D) Decoding 64-bit sized morton codes" << endl;
	 for (auto it = (*funcs64_decode).begin(); it != (*funcs64_decode).end(); it++) {
	 	os.str("");
	 	os << testDecode_2D_Linear_Perf((*it).decode, times) << " ms\t";
	 	os << testDecode_2D_Random_Perf((*it).decode, times) << " ms\t";
	 	cout << os.str() << "64-bit " << (*it).description << endl;
	 }
	 cout << "+++ (2D) Decoding 32-bit sized morton codes" << endl;
	 for (auto it = (*funcs32_decode).begin(); it != (*funcs32_decode).end(); it++) {
	 	os.str("");
	 	os << setfill('0') << std::setw(6) << std::fixed << std::setprecision(3);
	 	os << testDecode_2D_Linear_Perf((*it).decode, times) << " ms\t";
	 	os << testDecode_2D_Random_Perf((*it).decode, times) << " ms\t";
	 	cout << os.str() << "32-bit " << (*it).description << endl;
	 }
}