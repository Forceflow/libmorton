#pragma once
#include "libmorton_test.h"

using namespace std;

// Config variables (defined elsewhere)
extern size_t RAND_POOL_SIZE;
extern size_t total;
extern size_t CURRENT_TEST_MAX;
extern unsigned int times;
extern std::vector<uint_fast64_t> running_sums;



// Test performance of encoding a linearly increasing set of coordinates
template <typename morton, typename coord>
static double testEncode_3D_Linear_Perf(morton(*function)(coord, coord, coord), size_t times) {
	Timer timer = Timer();
	morton runningsum = 0;
	timer.start();
	for (size_t t = 0; t < times; t++) {
		for (coord i = 0; i < CURRENT_TEST_MAX; i++) {
			for (coord j = 0; j < CURRENT_TEST_MAX; j++) {
				for (coord k = 0; k < CURRENT_TEST_MAX; k += 8) { // inner loop is unrolled
					runningsum += function(i, j, k);
					runningsum += function(i, j, k + 1);
					runningsum += function(i, j, k + 2);
					runningsum += function(i, j, k + 3);
					runningsum += function(i, j, k + 4);
					runningsum += function(i, j, k + 5);
					runningsum += function(i, j, k + 6);
					runningsum += function(i, j, k + 7);
				}
			}
		}
	}
	timer.stop();
	running_sums.push_back(runningsum);
	return timer.elapsed_time_milliseconds / (float)times;
}

template <typename morton, typename coord>
static double testEncode_3D_Random_Perf(morton(*function)(coord, coord, coord), size_t times) {
	Timer timer = Timer();
	coord maximum = ~0;
	morton runningsum = 0;
	const unsigned int stride = 8;
	coord x[stride];
	coord y[stride];
	coord z[stride];

	timer.start();
	for (size_t t = 0; t < times; t++) {
		// Create a pool of random numbers
      std::vector<coord> randnumbers;
		for (size_t i = 0; i < RAND_POOL_SIZE; i++) {
			randnumbers.push_back(rand() % maximum);
		}
		// Do the performance test
		for (size_t i = 0; i < total; i += stride) {
			// grab all the random numbers for this stride
			for (unsigned int j = 0; j < stride; j++) {
				x[j] = randnumbers[(i + j) % RAND_POOL_SIZE];
				y[j] = randnumbers[(i + j + 1) % RAND_POOL_SIZE];
				z[j] = randnumbers[(i + j + 2) % RAND_POOL_SIZE];
			}
			runningsum += function(x[0], y[0], z[0]);
			runningsum += function(x[1], y[1], z[1]);
			runningsum += function(x[2], y[2], z[2]);
			runningsum += function(x[3], y[3], z[3]);
			runningsum += function(x[4], y[4], z[4]);
			runningsum += function(x[5], y[5], z[5]);
			runningsum += function(x[6], y[6], z[6]);
			runningsum += function(x[7], y[7], z[7]);	
		}
	}
	timer.stop();
	running_sums.push_back(runningsum);
	return timer.elapsed_time_milliseconds / (float)times;
}

template <typename morton, typename coord>
static double testDecode_3D_Linear_Perf(void(*function)(const morton, coord&, coord&, coord&), size_t times) {
	size_t max_morton = CURRENT_TEST_MAX * CURRENT_TEST_MAX * CURRENT_TEST_MAX;
	Timer timer = Timer();
	coord x, y, z;
	coord runningsum = 0;
	timer.start();
	for (size_t t = 0; t < times; t++) {
		for (morton i = 0; i < max_morton; i += 8) {		
			function(i, x, y, z);
			runningsum += x + y + z;
			function(i + 1, x, y, z);
			runningsum += x + y + z;
			function(i + 2, x, y, z);
			runningsum += x + y + z;
			function(i + 3, x, y, z);
			runningsum += x + y + z;
			function(i + 4, x, y, z);
			runningsum += x + y + z;
			function(i + 5, x, y, z);
			runningsum += x + y + z;
			function(i + 6, x, y, z);
			runningsum += x + y + z;
			function(i + 7, x, y, z);
			runningsum += x + y + z;	
		}
	}
	timer.stop();
	running_sums.push_back(runningsum);
	return timer.elapsed_time_milliseconds / (float)times;
}

template <typename morton, typename coord>
static double testDecode_3D_Random_Perf(void(*function)(const morton, coord&, coord&, coord&), size_t times) {
	Timer timer = Timer();
	coord x, y, z;
	morton maximum = ~0; // maximum for the random morton codes
	morton runningsum = 0;
	morton m;

	// Create a pool of randum numbers
   std::vector<morton> randnumbers;
	for (size_t i = 0; i < RAND_POOL_SIZE; i++) {
		randnumbers.push_back((morton(rand()) + morton(rand())) % maximum);
	}
	timer.start();
	// Start performance test
	for (size_t t = 0; t < times; t++) {
		for (size_t i = 0; i < total; i++) {
			m = randnumbers[i % RAND_POOL_SIZE];
			function(m, x, y, z);
			runningsum += x + y + z;
		}
	}
	timer.stop();
	running_sums.push_back(runningsum);
	return timer.elapsed_time_milliseconds / (float)times;
}

static void test_3D_performance(vector<encode_3D_64_wrapper>* funcs64_encode, vector<encode_3D_32_wrapper>* funcs32_encode,
	vector<decode_3D_64_wrapper>* funcs64_decode, vector<decode_3D_32_wrapper>* funcs32_decode) {
	stringstream os;
	//os << setfill('0') << std::setw(6) << std::fixed << std::setprecision(3);
	cout << "++ (3D) Encoding " << CURRENT_TEST_MAX << "^3 morton codes (" << total << " in total)" << endl;
	cout << "+++ (3D) Encoding 64-bit sized morton codes" << endl;
	// TODO: Indicate the fastest here
	for (auto it = (*funcs64_encode).begin(); it != (*funcs64_encode).end(); it++) {
		os.str("");
		os << testEncode_3D_Linear_Perf((*it).encode, times) << " ms\t";
		os << testEncode_3D_Random_Perf((*it).encode, times) << " ms\t";
		cout << os.str() << "64-bit " << (*it).description << endl;
	}
	cout << "+++ (3D) Encoding 32-bit sized morton codes" << endl;
	for (auto it = (*funcs32_encode).begin(); it != (*funcs32_encode).end(); it++) {
		os.str("");
		os << testEncode_3D_Linear_Perf((*it).encode, times) << " ms\t";
		os << testEncode_3D_Random_Perf((*it).encode, times) << " ms\t";
		cout << os.str() << "32-bit " << (*it).description << endl;
	}
	cout << "++ (3D) Decoding " << CURRENT_TEST_MAX << "^3 morton codes (" << total << " in total)" << endl;
	cout << "+++ (3D) Decoding 64-bit sized morton codes" << endl;
	for (auto it = (*funcs64_decode).begin(); it != (*funcs64_decode).end(); it++) {
		os.str("");
		os << testDecode_3D_Linear_Perf((*it).decode, times) << " ms\t";
		os << testDecode_3D_Random_Perf((*it).decode, times) << " ms\t";
		cout << os.str() << "64-bit " << (*it).description << endl;
	}
	cout << "+++ (3D) Decoding 32-bit sized morton codes" << endl;
	for (auto it = (*funcs32_decode).begin(); it != (*funcs32_decode).end(); it++) {
		os.str("");
		os << testDecode_3D_Linear_Perf((*it).decode, times) << " ms\t";
		os << testDecode_3D_Random_Perf((*it).decode, times) << " ms\t";
		cout << os.str() << "32-bit " << (*it).description << endl;
	}
}