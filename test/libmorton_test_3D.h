#pragma once
#include "libmorton_test.h"

using namespace std;

// Config variables (defined elsewhere)
extern size_t RAND_POOL_SIZE;
extern size_t total;
extern size_t MAX;
extern unsigned int times;
extern vector<uint_fast64_t> running_sums;

// Check a 3D Encode Function for correctness
template <typename morton, typename coord, size_t bits>
static bool check3D_EncodeFunction(const encode_f_3D_wrapper<morton, coord> &function) {

	// Number of bits which can be encoded for each field
	static const size_t fieldbits = bits / 3;

	static_assert(bits <= std::numeric_limits<uint64_t>::digits, "Control encoder cannot support > 64 bits.");
	static_assert(fieldbits >= 4, "At least 4 bits from each field must fit into 'morton'");
	static_assert(std::numeric_limits<morton>::digits >= 3 * fieldbits, "'morton' must support encoding width");
	static_assert(std::numeric_limits<coord>::digits >= fieldbits, "'coord' must support field width");
	
	bool everything_okay = true;
	morton computed_code;
	uint64_t correct_code = 0;
	
	// For every set of 4 contiguous bits, test all possible values (0-15), with all other bits cleared
	for (size_t offset = 0; offset <= fieldbits - 4; offset++) {
		for (coord i = 0; i < 16; i++) {
			for (coord j = 0; j < 16; j++) {
				for (coord k = 0; k < 16; k++) {
					coord x = i << offset;
					coord y = j << offset;
					coord z = k << offset;

					correct_code = control_encode(x, y, z);
					computed_code = function.encode(x, y, z);
					if (computed_code != (morton)correct_code) {
						everything_okay = false;
						cout << endl << "    Incorrect encoding of (" << x << ", " << y << ", " << z << ") in method " << function.description.c_str() << ": " << computed_code <<
							" != " << (morton)correct_code << endl;
					}
				}
			}
		}
	}
	return everything_okay;
}

// Check a 3D Decode Function for correctness
template <typename morton, typename coord, size_t bits>
static bool check3D_DecodeFunction(const decode_f_3D_wrapper<morton, coord> &function) {

	// Number of bits usable by the encoding
	static const size_t encodingbits = (bits / 3) * 3;

	static_assert(bits <= std::numeric_limits<uint64_t>::digits, "Control decoder cannot support > 64 bits.");
	static_assert(encodingbits >= 12, "'morton' must support at least 12 bit encodings");
	static_assert(std::numeric_limits<morton>::digits >= encodingbits, "'morton' must support encoding width");
	static_assert(std::numeric_limits<coord>::digits >= encodingbits / 3, "'coord' must support field width");

	bool everything_okay = true;
	uint64_t xctrl, yctrl, zctrl;
	coord x, y, z;

	for (size_t offset = 0; offset <= encodingbits - 12; offset++) {
		for (morton i = 0; i < 4096; i++) {
			morton encoding = i << offset;
			control_decode(encoding, xctrl, yctrl, zctrl);
			function.decode(encoding, x, y, z);
			if (x != (coord)xctrl || y != (coord)yctrl || z != (coord)zctrl) {
				printIncorrectDecoding3D<morton, coord>(function.description, encoding, x, y, z, (coord)xctrl, (coord)yctrl, (coord)zctrl);
				everything_okay = false;
			}
		}
	}

	if (encodingbits >= 30) {
		// Test max encoding which fits in uint32_t
		function.decode((morton)0x3fffffff, x, y, z);
		if (x != 0x3ff || y != 0x3ff || z != 0x3ff) {
			printIncorrectDecoding3D<morton, coord>(function.description, (morton)0x7fffffffffffffff, x, y, z, 0x3ff, 0x3ff, 0x3ff);
			everything_okay = false;
		}
	}

	if (encodingbits >= 63) {
		// Test max encoding which fits in uint64_t
		function.decode((morton)0x7fffffffffffffff, x, y, z);
		if (x != 0x1fffff || y != 0x1fffff || z != 0x1fffff) {
			printIncorrectDecoding3D<morton, coord>(function.description, (morton)0x7fffffffffffffff, x, y, z, 0x1fffff, 0x1fffff, 0x1fffff);
			everything_okay = false;
		}
	}
	return everything_okay;
}

// Check a 3D Encode/Decode function for correct encode-decode process
template<typename morton, typename coord, size_t bits>
inline bool check3D_Match(const encode_f_3D_wrapper<morton, coord> &encode, decode_f_3D_wrapper<morton, coord> &decode, unsigned int times) {
	bool everythingokay = true;
	for (unsigned int i = 0; i < times; ++i) {
		coord maximum = pow(2, floor(bits / 3.0f)) - 1;
		// generate random coordinates
		coord x = rand() % maximum;
		coord y = rand() % maximum;
		coord z = rand() % maximum;
		coord x_result, y_result, z_result;
		morton mortonresult = encode.encode(x, y, z);
		decode.decode(mortonresult, x_result, y_result, z_result);
		if ((x != x_result) | (y != y_result) | (z != z_result)) {
			cout << endl << "x: " << getBitString<coord>(x) << " (" << x << ")" << endl;
			cout << "y: " << getBitString<coord>(y) << " (" << y << ")" << endl;
			cout << "z: " << getBitString<coord>(z) << " (" << z << ")" << endl;
			cout << "morton: " << getBitString<morton>(mortonresult) << "(" << mortonresult << ")" << endl;
			cout << "x_result: " << getBitString<coord>(x_result) << " (" << x_result << ")" << endl;
			cout << "y_result: " << getBitString<coord>(y_result) << " (" << y_result << ")" << endl;
			cout << "z_result: " << getBitString<coord>(z_result) << " (" << z_result << ")" << endl;
			cout << bits << "-bit ";
			cout << "using methods encode " << encode.description << " and decode " << decode.description << endl;
			everythingokay = false;
		}
	}
	return everythingokay;
}

template <typename morton, typename coord, size_t bits>
inline void check3D_EncodeCorrectness(std::vector<encode_f_3D_wrapper<morton, coord>> encoders) {
	printf("++ Checking correctness of 3D encoders (%lu bit) methods ... ", bits);
	bool ok = true;
	for (auto it = encoders.begin(); it != encoders.end(); it++) {
		ok &= check3D_EncodeFunction<morton, coord, bits>(*it);
	}
	ok ? printf(" Passed. \n") : printf("    One or more methods failed. \n");
}

template <typename morton, typename coord, size_t bits>
inline void check3D_DecodeCorrectness(std::vector<decode_f_3D_wrapper<morton, coord>> decoders) {
	printf("++ Checking correctness of 3D decoding (%lu bit) methods ... ", bits);
	bool ok = true;
	for (auto it = decoders.begin(); it != decoders.end(); it++) {
		ok &= check3D_DecodeFunction<morton, coord, bits>(*it);
	}
	ok ? printf(" Passed. \n") : printf("    One or more methods failed. \n");
}

template <typename morton, typename coord, size_t bits>
inline void check3D_EncodeDecodeMatch(std::vector<encode_f_3D_wrapper<morton, coord>> encoders, std::vector<decode_f_3D_wrapper<morton, coord>> decoders, unsigned int times) {
	printf("++ Checking 3D methods (%lu bit) encode/decode match ... ", bits);
	bool ok = true;
	for (auto et = encoders.begin(); et != encoders.end(); et++) {
		for (auto dt = decoders.begin(); dt != decoders.end(); dt++) {
			ok &= check3D_Match<morton, coord, bits>(*et, *dt, times);
		}
	}
	ok ? printf(" Passed. \n") : printf("    One or more methods failed. \n");
}

// Test performance of encoding a linearly increasing set of coordinates
template <typename morton, typename coord>
static double testEncode_3D_Linear_Perf(morton(*function)(coord, coord, coord), size_t times) {
	Timer timer = Timer();
	morton runningsum = 0;
	for (size_t t = 0; t < times; t++) {
		for (coord i = 0; i < MAX; i++) {
			for (coord j = 0; j < MAX; j++) {
				for (coord k = 0; k < MAX; k += 8) {
					timer.start();
					runningsum += function(i, j, k);
					runningsum += function(i, j, k + 1);
					runningsum += function(i, j, k + 2);
					runningsum += function(i, j, k + 3);
					runningsum += function(i, j, k + 4);
					runningsum += function(i, j, k + 5);
					runningsum += function(i, j, k + 6);
					runningsum += function(i, j, k + 7);
					timer.stop();
				}
			}
		}
	}
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

	for (size_t t = 0; t < times; t++) {
		// Create a pool of random numbers
		vector<coord> randnumbers;
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
	return timer.elapsed_time_milliseconds / (float)times;
}

template <typename morton, typename coord>
static double testDecode_3D_Linear_Perf(void(*function)(const morton, coord&, coord&, coord&), size_t times) {
	Timer timer = Timer();
	coord x, y, z;
	morton runningsum = 0;
	for (size_t t = 0; t < times; t++) {
		for (morton i = 0; i < total; i += 8) {
			timer.start();
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
			timer.stop();
		}
	}
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
	vector<morton> randnumbers;
	for (size_t i = 0; i < RAND_POOL_SIZE; i++) {
		randnumbers.push_back((rand() + rand()) % maximum);
	}

	// Start performance test
	for (int t = 0; t < times; t++) {
		for (size_t i = 0; i < total; i++) {
			m = randnumbers[i % RAND_POOL_SIZE];
			timer.start();
			function(m, x, y, z);
			timer.stop();
			runningsum += x + y + z;
		}
	}
	running_sums.push_back(runningsum);
	return timer.elapsed_time_milliseconds / (float)times;
}