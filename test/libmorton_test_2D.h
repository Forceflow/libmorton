#pragma once
#include "libmorton_test.h"

using namespace std;

// Config variables (defined elsewhere)
extern size_t RAND_POOL_SIZE;
extern size_t total;
extern size_t MAX;
extern unsigned int times;
extern vector<uint_fast64_t> running_sums;

// Check a 2D Encode Function for correctness
template <typename morton, typename coord>
static bool check2D_EncodeFunction(const encode_f_2D_wrapper<morton, coord> &function) {
	bool everything_okay = true;
	morton computed_code, correct_code = 0;
	for (coord i = 0; i < 16; i++) {
		for (coord j = 0; j < 16; j++) {
			correct_code = control_encode(i, j);
			computed_code = function.encode(i, j);
			if (computed_code != correct_code) {
				everything_okay = false;
				cout << endl << "    Incorrect encoding of (" << i << ", " << j << ") in method " << function.description.c_str() << ": " << computed_code <<
					 " != " << correct_code << endl;
			}
		}
	}
	return everything_okay;
}

template <typename morton, typename coord>
inline void check2D_EncodeCorrectness(std::vector<encode_f_2D_wrapper<morton, coord>> encoders) {
	printf("++ Checking correctness of 2D encoders (%lu bit) methods ... ", sizeof(morton) * 8);
	bool ok = true;
	for (auto it = encoders.begin(); it != encoders.end(); it++) {
		ok &= check2D_EncodeFunction(*it);
	}
	ok ? printf(" Passed. \n") : printf("    One or more methods failed. \n");
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
