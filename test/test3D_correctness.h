#pragma once
#include "libmorton_test.h"

using namespace std;

// Config variables (defined elsewhere)
extern size_t RAND_POOL_SIZE;
extern size_t total;
extern size_t CURRENT_TEST_MAX;
extern unsigned int times;
extern std::vector<uint_fast64_t> running_sums;

// CORRECTNESS CHECKS

// Check a 3D Encode Function for correctness
template <typename morton, typename coord, size_t bits>
static bool check3D_EncodeFunction(const encode_f_3D_wrapper<morton, coord>& function) {

	// Number of bits which can be encoded for each field
	static const size_t fieldbits = bits / 3;

	static_assert(bits <= std::numeric_limits<uint64_t>::digits, "Control encoder cannot support > 64 bits.");
	static_assert(fieldbits >= 4, "At least 4 bits from each field must fit into 'morton'");
	static_assert(std::numeric_limits<morton>::digits >= 3 * fieldbits, "'morton' must support encoding width");
	static_assert(std::numeric_limits<coord>::digits >= fieldbits, "'coord' must support field width");

	bool everything_okay = true;
	morton computed_code;
	uint64_t correct_code;

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
						std::cout << "\n    Incorrect encoding of (" << x << ", " << y << ", " << z << ") in method " << function.description.c_str() << ": " << computed_code <<
							" != " << (morton)correct_code << "\n";
					}
				}
			}
		}
	}
	return everything_okay;
}

// Check a 3D Decode Function for correctness
template <typename morton, typename coord, size_t bits>
static bool check3D_DecodeFunction(const decode_f_3D_wrapper<morton, coord>& function) {

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
inline bool check3D_Match(const encode_f_3D_wrapper<morton, coord>& encode, decode_f_3D_wrapper<morton, coord>& decode, unsigned int times) {
	bool everythingokay = true;
	for (unsigned int i = 0; i < times; ++i) {
		coord maximum = (coord)(pow(2, floor(bits / 3.0f)) - 1.0f);
		// generate random coordinates
		coord x = rand() % maximum;
		coord y = rand() % maximum;
		coord z = rand() % maximum;
		coord x_result, y_result, z_result;
		morton mortonresult = encode.encode(x, y, z);
		decode.decode(mortonresult, x_result, y_result, z_result);
		if ((x != x_result) || (y != y_result) || (z != z_result)) {
			std::cout << "\n" << "x: " << getBitString<coord>(x) << " (" << x << ")\n";
			std::cout << "y: " << getBitString<coord>(y) << " (" << y << ")\n";
			std::cout << "z: " << getBitString<coord>(z) << " (" << z << ")\n";
			std::cout << "morton: " << getBitString<morton>(mortonresult) << "(" << mortonresult << ")\n";
			std::cout << "x_result: " << getBitString<coord>(x_result) << " (" << x_result << ")\n";
			std::cout << "y_result: " << getBitString<coord>(y_result) << " (" << y_result << ")\n";
			std::cout << "z_result: " << getBitString<coord>(z_result) << " (" << z_result << ")\n";
			std::cout << bits << "-bit ";
			std::cout << "using methods encode " << encode.description << " and decode " << decode.description << "\n";
			everythingokay = false;
		}
	}
	return everythingokay;
}

template <typename morton, typename coord, size_t bits>
inline bool check3D_EncodeCorrectness(std::vector<encode_f_3D_wrapper<morton, coord>> encoders) {
	printf("++ Checking correctness of 3D encoders (%zd bit) methods ... ", bits);
	bool ok = true;
	for (auto it = encoders.begin(); it != encoders.end(); it++) {
		ok &= check3D_EncodeFunction<morton, coord, bits>(*it);
	}
	ok ? printPassed() : printFailed();
	return ok;
}

template <typename morton, typename coord, size_t bits>
inline bool check3D_DecodeCorrectness(std::vector<decode_f_3D_wrapper<morton, coord>> decoders) {
	printf("++ Checking correctness of 3D decoding (%zd bit) methods ... ", bits);
	bool ok = true;
	for (auto it = decoders.begin(); it != decoders.end(); it++) {
		ok &= check3D_DecodeFunction<morton, coord, bits>(*it);
	}
	ok ? printPassed() : printFailed();
	return ok;
}

template <typename morton, typename coord, size_t bits>
inline bool check3D_EncodeDecodeMatch(std::vector<encode_f_3D_wrapper<morton, coord>> encoders, std::vector<decode_f_3D_wrapper<morton, coord>> decoders, unsigned int times) {
	printf("++ Checking 3D methods (%zd bit) encode/decode match ... ", bits);
	bool ok = true;
	for (auto et = encoders.begin(); et != encoders.end(); et++) {
		for (auto dt = decoders.begin(); dt != decoders.end(); dt++) {
			ok &= check3D_Match<morton, coord, bits>(*et, *dt, times);
		}
	}
	ok ? printPassed() : printFailed();
	return ok;
}