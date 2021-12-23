#pragma once
#include "libmorton_test.h"

// Config variables (defined elsewhere)
extern size_t RAND_POOL_SIZE;
extern size_t total;
extern size_t CURRENT_TEST_MAX;
extern unsigned int times;
extern std::vector<uint_fast64_t> running_sums;

// Check a 2D encode function for correctness
template <typename morton, typename coord, size_t bits>
static bool check2D_EncodeFunction(const encode_f_2D_wrapper<morton, coord> &function) {
	
	// Number of bits which can be encoded for each field
	static const size_t fieldbits = bits / 2;

	static_assert(bits <= std::numeric_limits<uint64_t>::digits, "Control encoder cannot support > 64 bits.");
	static_assert(fieldbits >= 4, "At least 4 bits from each field must fit into 'morton'");
	static_assert(std::numeric_limits<morton>::digits >= 2 * fieldbits, "'morton' must support encoding width");
	static_assert(std::numeric_limits<coord>::digits >= fieldbits, "'coord' must support field width");

	bool everything_okay = true;
	morton computed_code;
	uint64_t correct_code;

	// For every set of 4 contiguous bits, test all possible values (0-15), with all other bits cleared
	for (size_t offset = 0; offset <= fieldbits - 4; offset++) {
		for (coord i = 0; i < 16; i++) {
			for (coord j = 0; j < 16; j++) {
				coord x = i << offset;
				coord y = j << offset;

				correct_code = control_encode(x, y);
				computed_code = function.encode(x, y);
				if (computed_code != (morton)correct_code) {
					everything_okay = false;
					std::cout << "\n    Incorrect encoding of (" << x << ", " << y << ") in method " << function.description.c_str() << ": " << computed_code <<
						" != " << (morton)correct_code << "\n";
				}
			}
		}
	}
	return everything_okay;
}

// Check a 2D decode function for correctness
template <typename morton, typename coord, size_t bits>
static bool check2D_DecodeFunction(const decode_f_2D_wrapper<morton, coord> &function) {

	// Number of bits usable by the encoding
	static const size_t encodingbits = (bits / 2) * 2;

	static_assert(bits <= std::numeric_limits<uint64_t>::digits, "Control decoder cannot support > 64 bits.");
	static_assert(encodingbits >= 8, "'morton' must support at least 8 bit encodings");
	static_assert(std::numeric_limits<morton>::digits >= encodingbits, "'morton' must support encoding width");
	static_assert(std::numeric_limits<coord>::digits >= encodingbits / 2, "'coord' must support field width");

	bool everything_okay = true;
	uint64_t xctrl, yctrl;
	coord x, y;

	for (size_t offset = 0; offset <= encodingbits - 8; offset++) {
		for (morton i = 0; i < 256; i++) {
			morton encoding = i << offset;
			control_decode(encoding, xctrl, yctrl);
			function.decode(encoding, x, y);
			if (x != (coord)xctrl || y != (coord)yctrl) {
				printIncorrectDecoding2D<morton, coord>(function.description, encoding, x, y, (coord)xctrl, (coord)yctrl);
				everything_okay = false;
			}
		}
	}

	if (encodingbits >= 32) {
		// Test max encoding which fits in uint32_t
		function.decode((morton)0xffffffff, x, y);
		if (x != 0xffff || y != 0xffff) {
			printIncorrectDecoding2D<morton, coord>(function.description, (morton)0xffffffff, x, y, 0xffff, 0xffff);
			everything_okay = false;
		}
	}

	if (encodingbits >= 64) {
		// Test max encoding which fits in uint64_t
		function.decode((morton)0xffffffffffffffff, x, y);
		if (x != 0xffffffff || y != 0xffffffff) {
			printIncorrectDecoding2D<morton, coord>(function.description, (morton)0xffffffffffffffff, x, y, 0xffffffff, 0xffffffff);
			everything_okay = false;
		}
	}
	return everything_okay;
}

// Check vector of 2D encode functions for correctness
template <typename morton, typename coord, size_t bits>
inline bool check2D_EncodeCorrectness(std::vector<encode_f_2D_wrapper<morton, coord>> encoders) {
	printf("++ Checking correctness of 2D encoders (%zu bit) methods ... ", bits);
	bool ok = true;
	for (auto it = encoders.begin(); it != encoders.end(); it++) {
		ok &= check2D_EncodeFunction<morton, coord, bits>(*it);
	}
	ok ? printf(" Passed. \n") : printf("    One or more methods failed. \n");
	return ok;
}

// Check vector of 2D decode functions for correctness
template <typename morton, typename coord, size_t bits>
inline bool check2D_DecodeCorrectness(std::vector<decode_f_2D_wrapper<morton, coord>> decoders) {
	printf("++ Checking correctness of 2D decoding (%zu bit) methods ... ", bits);
	bool ok = true;
	for (auto it = decoders.begin(); it != decoders.end(); it++) {
		ok &= check2D_DecodeFunction<morton, coord, bits>(*it);
	}
	ok ? printf(" Passed. \n") : printf("    One or more methods failed. \n");
	return ok;
}

//// Check a 2D Encode/Decode function for correct encode-decode process
//template<typename morton, typename coord, size_t bits>
//inline bool check2D_Match(const encode_f_2D_wrapper<morton, coord>& encode, decode_f_2D_wrapper<morton, coord>& decode, unsigned int times) {
//	bool everythingokay = true;
//	for (unsigned int i = 0; i < times; ++i) {
//		coord maximum = (coord)(pow(2, floor(bits / 3.0f)) - 1.0f);
//		// generate random coordinates
//		coord x = rand() % maximum;
//		coord y = rand() % maximum;
//		coord z = rand() % maximum;
//		coord x_result, y_result, z_result;
//		morton mortonresult = encode.encode(x, y, z);
//		decode.decode(mortonresult, x_result, y_result, z_result);
//		if ((x != x_result) | (y != y_result) | (z != z_result)) {
//			std::cout << "\n" << "x: " << getBitString<coord>(x) << " (" << x << ")\n";
//			std::cout << "y: " << getBitString<coord>(y) << " (" << y << ")\n";
//			std::cout << "z: " << getBitString<coord>(z) << " (" << z << ")\n";
//			std::cout << "morton: " << getBitString<morton>(mortonresult) << "(" << mortonresult << ")\n";
//			std::cout << "x_result: " << getBitString<coord>(x_result) << " (" << x_result << ")\n";
//			std::cout << "y_result: " << getBitString<coord>(y_result) << " (" << y_result << ")\n";
//			std::cout << "z_result: " << getBitString<coord>(z_result) << " (" << z_result << ")\n";
//			std::cout << bits << "-bit ";
//			std::cout << "using methods encode " << encode.description << " and decode " << decode.description << "\n";
//			everythingokay = false;
//		}
//	}
//	return everythingokay;
//}
//
//template <typename morton, typename coord, size_t bits>
//inline bool check2D_EncodeDecodeMatch(std::vector<encode_f_2D_wrapper<morton, coord>> encoders, std::vector<decode_f_2D_wrapper<morton, coord>> decoders, unsigned int times) {
//	printf("++ Checking 3D methods (%zd bit) encode/decode match ... ", bits);
//	bool ok = true;
//	for (auto et = encoders.begin(); et != encoders.end(); et++) {
//		for (auto dt = decoders.begin(); dt != decoders.end(); dt++) {
//			ok &= check2D_Match<morton, coord, bits>(*et, *dt, times);
//		}
//	}
//	ok ? printf(" Passed. \n") : printf("    One or more methods failed. \n");
//	return ok;
//}

// Check 2D encode function performance (linear)
template <typename morton, typename coord>
static double testEncode_2D_Linear_Perf(morton(*function)(coord, coord), size_t times) {
	Timer timer = Timer();
	morton runningsum = 0;
	for (size_t t = 0; t < times; t++) {
		for (coord i = 0; i < CURRENT_TEST_MAX; i++) {
			for (coord j = 0; j < CURRENT_TEST_MAX; j++) {
				timer.start();
				runningsum += function(i, j);
				timer.stop();
			}
		}
	}
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

// TODO: Check 2D decode function performance (linear)
template <typename morton, typename coord>
static double testDecode_2D_Linear_Perf(morton(*function)(coord, coord), size_t times) {
	// TODO
}

// TODO: Check 2D decode function performance (random)
template <typename morton, typename coord>
static double testDecode_2D_Random_Perf(morton(*function)(coord, coord), size_t times) {
	// TODO
}
