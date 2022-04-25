#pragma once

// Standard headers
#include <cstdlib>
#include <math.h>
#include <iostream>
#include <sstream>
#include <chrono>
#include <bitset>
#include <string>
#include <vector>
#include <iomanip>
#include <limits>
#include <valarray>
#include <functional>

// Load utils
#include "util.h"
#include "timer.h"
#include "morton_LUT_generators.h"

// Load libraries we're going to test
#include <libmorton/morton2D.h>
#include <libmorton/morton3D.h>
#if defined(__BMI2__) || defined(__AVX2__)
#include <libmorton/morton_BMI.h>
#endif
#if defined(__AVX512BITALG__)
#include <libmorton/morton_AVX512BITALG.h>
#endif

// Load main morton include file (should be unnecessary)
#include <libmorton/morton.h>

using std::string;

template <typename morton, typename coord>
struct encode_f_2D_wrapper {
	string description;
	morton(*encode)(coord, coord);
	encode_f_2D_wrapper(const string &description, morton(*encode)(coord, coord)) : description(description), encode(encode) {}
	encode_f_2D_wrapper() : description(""), encode(0) {}
};

template <typename morton, typename coord>
struct decode_f_2D_wrapper {
	string description;
	void(*decode)(morton, coord&, coord&);
	decode_f_2D_wrapper(const string &description, void(*decode)(morton, coord&, coord&)) : description(description), decode(decode) {}
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
	encode_f_3D_wrapper(const string &description, morton(*encode)(coord, coord, coord)) : description(description), encode(encode){}
	encode_f_3D_wrapper() : description(""), encode(0) {}
};

template <typename morton, typename coord>
struct decode_f_3D_wrapper {
	string description;
	void (*decode)(morton, coord&, coord&, coord&);
	decode_f_3D_wrapper(const string &description, void(*decode)(morton, coord&, coord&, coord&)) : description(description), decode(decode) {}
	decode_f_3D_wrapper() : description(""), decode(0) {}
};

typedef encode_f_3D_wrapper<uint_fast64_t, uint_fast32_t> encode_3D_64_wrapper;
typedef encode_f_3D_wrapper<uint_fast32_t, uint_fast16_t> encode_3D_32_wrapper;
typedef decode_f_3D_wrapper<uint_fast64_t, uint_fast32_t> decode_3D_64_wrapper;
typedef decode_f_3D_wrapper<uint_fast32_t, uint_fast16_t> decode_3D_32_wrapper;

template<std::size_t FieldCount>
uint64_t split_by_n(uint64_t input, size_t bitsRemaining) {
	return (bitsRemaining == 0) ? input : (split_by_n<FieldCount>(input >> 1, bitsRemaining - 1) << FieldCount) | (input & (uint64_t)1);
}

template<std::size_t FieldCount>
uint64_t join_by_n(uint64_t input, size_t bitsRemaining) {
	return (bitsRemaining == 0) ? 0 : (join_by_n<FieldCount>(input >> FieldCount, bitsRemaining - 1) << 1) | (input & (uint64_t)1);
}

template<std::size_t FieldCount>
uint64_t control_encode_impl(std::size_t bitCount, const std::valarray<uint64_t>& fields) {
	if (fields.size() == 1) {
		return split_by_n<FieldCount>(fields[std::slice(0, 1, 1)][0], bitCount);
	}

	return (control_encode_impl<FieldCount>(bitCount, fields[std::slice(1, fields.size() - 1, 1)]) << 1)
		   | split_by_n<FieldCount>(fields[std::slice(0, 1, 1)][0], bitCount);
}

/**
 * Runtime Morton encoding using 64-bit fields. Only the first floor(64 / |fields|) bits will be encoded from each field.
 *
 * @param fields The fields to encode.
 * @return The Morton encoding.
 */
template<typename...T>
uint64_t control_encode(T... fields) {
	return control_encode_impl<sizeof...(fields)>(std::numeric_limits<uint64_t>::digits / sizeof...(fields), { fields... });
}

template<std::size_t FieldCount>
void control_decode_impl(uint64_t encoding, std::size_t bitCount, const std::valarray<std::reference_wrapper<uint64_t>>& fields) {
	if (fields.size() == 1) {
		fields[std::slice(0, 1, 1)][0].get() = join_by_n<FieldCount>(encoding, bitCount);
		return;
	}

	fields[std::slice(0, 1, 1)][0].get() = join_by_n<FieldCount>(encoding, bitCount);
	control_decode_impl<FieldCount>(encoding >> 1, bitCount, fields[std::slice(1, fields.size() - 1, 1)]);
}

template<typename...T>
void control_decode(uint64_t encoding, T&... fields) {
	const std::valarray<std::reference_wrapper<uint64_t>> list = { fields... };
	return control_decode_impl<sizeof...(fields)>(encoding, std::numeric_limits<uint64_t>::digits / sizeof...(fields), list);
}

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
inline string getSpacedBitString(valtype val, unsigned int frequency, unsigned int spaces) {
	std::string bitstring= getBitString<valtype>(val); // converted bitstring
	std::string base = ""; // base string
	std::string spacestring(spaces, ' '); // spaces
	for (unsigned int i = 0; i < bitstring.size(); i++) {
		base = (i % frequency == 0) ? base + spacestring + bitstring[i] : base + bitstring[i];
	}
	return base;
}

template <typename morton, typename coord>
void printIncorrectDecoding2D(string method_tested, morton m, coord x, coord y, coord correct_x, coord correct_y) {
	std::cout << "\n    Incorrect decoding of " << getBitString<morton>(m) << " in method " << method_tested.c_str() << ": ("
		<< getBitString<coord>(x) << ", " << getBitString<coord>(y)
		<< ") != (" << getBitString<coord>(correct_x) << ", " << getBitString<coord>(correct_y) << ")\n";
}

template <typename morton, typename coord>
void printIncorrectDecoding3D(string method_tested, morton m, coord x, coord y, coord z, coord correct_x, coord correct_y, coord correct_z) {
   std::cout << "\n    Incorrect decoding of " << getBitString<morton>(m) << " in method " << method_tested.c_str() << ": ("
		<< getBitString<coord>(x) << ", " << getBitString<coord>(y) << ", " << getBitString<coord>(z)
		<< ") != (" << getBitString<coord>(correct_x) << ", " << getBitString<coord>(correct_y) << ", "
		<< getBitString<coord>(correct_z) << ")\n";
}

void printFailed() {
	printf("\033[1;31m");
	printf("Failed");
	printf("\033[0m \n");
}

void printPassed() {
	printf("\033[1;32m");
	printf("Passed");
	printf("\033[0m \n");
}

