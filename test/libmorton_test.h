#pragma once

// Standard headers
#include <cstdlib>
#include <math.h>
#include <iostream>
#include <sstream>
#include <chrono>
#include <bitset>
#include <string>
#include <ctime>
#include <vector>
#include <iomanip>

// Load utils
#include "util.h"
#include "timer.h"
#include "libmorton_test_controlvalues.h"

// Load libraries we're going to test
#include "../libmorton/include/morton_LUT_generators.h"
#include "../libmorton/include/morton2D.h"
#include "../libmorton/include/morton3D.h"
#include "../libmorton/include/morton.h"

template <typename morton, typename coord>
struct encode_f_2D_wrapper {
	string description;
	morton(*encode)(coord, coord);
	encode_f_2D_wrapper(string description, morton(*encode)(coord, coord)) : description(description), encode(encode) {}
	encode_f_2D_wrapper() : description(""), encode(0) {}
};

template <typename morton, typename coord>
struct decode_f_2D_wrapper {
	string description;
	void(*decode)(morton, coord&, coord&);
	decode_f_2D_wrapper(string description, void(*decode)(morton, coord&, coord&)) : description(description), decode(decode) {}
	decode_f_2D_wrapper() : description(	""), decode(0) {}
};

typedef encode_f_2D_wrapper<uint_fast64_t, uint_fast32_t> encode_2D_64_wrapper;
typedef encode_f_2D_wrapper<uint_fast32_t, uint_fast16_t> encode_2D_32_wrapper;
typedef decode_f_2D_wrapper<uint_fast64_t, uint_fast32_t> decode_2D_64_wrapper;
typedef decode_f_2D_wrapper<uint_fast32_t, uint_fast16_t> decode_2D_32_wrapper;

template <typename morton, typename coord>
struct encode_f_3D_wrapper {
	string description;
	morton (*encode)(coord, coord, coord);
	encode_f_3D_wrapper(string description, morton(*encode)(coord, coord, coord)) : description(description), encode(encode){}
	encode_f_3D_wrapper() : description(""), encode(0) {}
};

template <typename morton, typename coord>
struct decode_f_3D_wrapper {
	string description;
	void (*decode)(morton, coord&, coord&, coord&);
	decode_f_3D_wrapper(string description, void(*decode)(morton, coord&, coord&, coord&)) : description(description), decode(decode) {}
	decode_f_3D_wrapper() : description(""), decode(0) {}
};

typedef encode_f_3D_wrapper<uint_fast64_t, uint_fast32_t> encode_3D_64_wrapper;
typedef encode_f_3D_wrapper<uint_fast32_t, uint_fast16_t> encode_3D_32_wrapper;
typedef decode_f_3D_wrapper<uint_fast64_t, uint_fast32_t> decode_3D_64_wrapper;
typedef decode_f_3D_wrapper<uint_fast32_t, uint_fast16_t> decode_3D_32_wrapper;

template <typename valtype>
inline string getBitString(valtype val) {
	// bitset needs size to be known at runtime, and introducing boost dependency
	// for dynamic bitsets is undesirable, so we get crazy and cut the relevant bits. 
	// NASTY STUFF Don't try this at home kids
	std::bitset<128> bs(val); // we make the bitset a max of 128
	std::string s = bs.to_string(); // stringify it
	return s.substr(s.length() - (sizeof(valtype) * 8)); // cut to the actual length
}

template <typename valtype>
inline string getSpacedBitString(valtype val, unsigned int spaces) {
	std::string bitstring= getBitString<valtype>(val);
	std::string base = ""; // base string
	std::string spacestring(spaces, ' '); // spaces

	for (int i = 0; i < bitstring.size(); i++) {
		base = bitstring[i] + spacestring + base;
	}
	return base;
}

template <typename morton, typename coord>
void printIncorrectDecoding3D(string method_tested, morton m, coord x, coord y, coord z, coord correct_x, coord correct_y, coord correct_z) {
	cout << endl << "    Incorrect decoding of " << getBitString<morton>(m) << " in method " << method_tested.c_str() << ": ("
		<< getBitString<coord>(x) << ", " << getBitString<coord>(y) << ", " << getBitString<coord>(z)
		<< ") != (" << getBitString<coord>(correct_x) << ", " << getBitString<coord>(correct_y) << ", "
		<< getBitString<coord>(correct_z) << ")" << endl;
}

