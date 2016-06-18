#pragma once

// Libmorton - Methods to encode/decode 64-bit morton codes from/to 32-bit (x,y) coordinates
// Warning: morton.h will always point to the functions that use the fastest available method.

#include <stdint.h>
#include <math.h>
#include "morton2D_LUTs.h"
#include "morton_common.h"

using namespace std;

// Encode methods
template<typename morton, typename coord> inline morton m2D_e_sLUT(const coord x, const coord y);
template<typename morton, typename coord> inline morton m2D_e_sLUT_ET(const coord x, const coord y);
template<typename morton, typename coord> inline morton m2D_e_LUT(const coord x, const coord y);
template<typename morton, typename coord> inline morton m2D_e_LUT_ET(const coord x, const coord y);
template<typename morton, typename coord> inline morton m2D_e_magicbits(const coord x, const coord y);
template<typename morton, typename coord> inline morton m2D_e_for(const coord x, const coord y);
template<typename morton, typename coord> inline morton m2D_e_for_ET(const coord x, const coord y);

// Decode methods
template<typename morton, typename coord> inline void m2D_d_sLUT(const morton m, coord& x, coord& y);
template<typename morton, typename coord> inline void m2D_d_sLUT_ET(const morton m, coord& x, coord& y);
template<typename morton, typename coord> inline void m2D_d_LUT(const morton m, coord& x, coord& y);
template<typename morton, typename coord> inline void m2D_d_LUT_ET(const morton m, coord& x, coord& y);
template<typename morton, typename coord> inline void m2D_d_magicbits(const morton m, coord& x, coord& y);
template<typename morton, typename coord> inline void m2D_d_for(const morton m, coord& x, coord& y);

// ENCODE 2D Morton code : Pre-shifted LookUpTable (sLUT)
template<typename morton, typename coord>
inline morton m2D_e_sLUT(const coord x, const coord y) {
	morton answer = 0;
	const static morton EIGHTBITMASK = 0x000000FF;
	for (unsigned int i = sizeof(coord); i > 0; --i) {
		unsigned int shift = (i - 1) * 8;
		answer =
			answer << 16 | 
			Morton2D_encode_y_256[(y >> shift) & EIGHTBITMASK] |
			Morton2D_encode_x_256[(x >> shift) & EIGHTBITMASK];
	}
	return answer;
}

// ENCODE 2D Morton code : LookUpTable (LUT)
template<typename morton, typename coord>
inline morton m2D_e_LUT(const coord x, const coord y) {
	morton answer = 0;
	const static morton EIGHTBITMASK = 0x000000FF;
	for (unsigned int i = sizeof(coord); i > 0; --i) {
		unsigned int shift = (i - 1) * 8; 
		answer =
			answer << 16 |
			(Morton2D_encode_x_256[(y >> shift) & EIGHTBITMASK] << 1) |
			(Morton2D_encode_x_256[(x >> shift) & EIGHTBITMASK]);
	}
	return answer;
}

// HELPER METHOD for Early Termination LUT Encode
template<typename morton, typename coord>
inline morton compute2D_ET_LUT_encode(const coord c, const coord *LUT) {
	const static morton EIGHTBITMASK = 0x000000FF;
	unsigned long maxbit = 0;
	morton answer = 0;
	if (findFirstSetBit<coord>(c, &maxbit) == 0) { return 0; }
	unsigned int i = 0;
	while (maxbit >= i) {
		answer |= (LUT[(c >> i) & EIGHTBITMASK]) << i * 2;
		i += 8;
	}
	return answer;
}

// ENCODE 2D Morton code : Pre-shifted LUT (Early termination version)
// This version tries to terminate early when there are no more bits to process
// Figuring this out is probably too costly in most cases.
template<typename morton, typename coord>
inline morton m2D_e_sLUT_ET(const coord x, const coord y) {
	morton answer_x = compute2D_ET_LUT_encode<morton, coord>(x, Morton2D_encode_x_256);
	morton answer_y = compute2D_ET_LUT_encode<morton, coord>(y, Morton2D_encode_y_256);
	return answer_y | answer_x;
}

// ENCODE 2D Morton code : LUT (Early termination version)
template<typename morton, typename coord>
inline morton m2D_e_LUT_ET(const coord x, const coord y) {
	morton answer_x = compute2D_ET_LUT_encode<morton, coord>(x, Morton2D_encode_x_256);
	morton answer_y = compute2D_ET_LUT_encode<morton, coord>(y, Morton2D_encode_x_256);
	return (answer_y << 1) | answer_x;
}

// HELPER METHOD for Magic bits encoding - split by 2
template<typename morton, typename coord>
inline morton morton2D_SplitBy2Bits(const coord a) {
	const morton* masks = (sizeof(morton) <= 4) ? reinterpret_cast<const morton*>(magicbit2D_masks32) : reinterpret_cast<const morton*>(magicbit2D_masks64);
	morton x = a;
	if (sizeof(morton) > 4) { x = (x | x << 32) & masks[0]; }
	x = (x | x << 16) & masks[1];
	x = (x | x << 8) & masks[2];
	x = (x | x << 4) & masks[3];
	x = (x | x << 2) & masks[4];
	x = (x | x << 1) & masks[5];
	return x;
}

// ENCODE 2D Morton code : Magic bits
template<typename morton, typename coord>
inline morton m2D_e_magicbits(const coord x, const coord y) {
	return morton2D_SplitBy2Bits<morton, coord>(x) | (morton2D_SplitBy2Bits<morton, coord>(y) << 1);
}

// ENCODE 2D Morton code : For Loop
template<typename morton, typename coord>
inline morton m2D_e_for(const coord x, const coord y){
	morton answer = 0;
	unsigned int checkbits = floor(sizeof(morton) * 4.0f);
	for (unsigned int i = 0; i <= checkbits; ++i) {
		morton mshifted = static_cast<morton>(0x1) << i; // Here we need to cast 0x1 to 64bits, otherwise there is a bug when morton code is larger than 32 bits
		unsigned int shift = 2 * i;
		answer |=
			  ((x & mshifted) << shift)
			| ((y & mshifted) << (shift + 1));
	}
	return answer;
}

// ENCODE 2D Morton code : For Loop (Early termination version)
template<typename morton, typename coord>
inline morton m2D_e_for_ET(const coord x, const coord y) {
	morton answer = 0;
	unsigned long x_max = 0, y_max = 0;
	unsigned int checkbits = sizeof(morton) * 4;
	findFirstSetBit<morton>(x, &x_max);
	findFirstSetBit<morton>(y, &y_max);
	checkbits = min(static_cast<unsigned long>(checkbits), max(x_max, y_max) + 1ul);
	for (unsigned int i = 0; i <= checkbits; ++i) {
		morton m_shifted = static_cast<morton>(0x1) << i; // Here we need to cast 0x1 to 64bits, otherwise there is a bug when morton code is larger than 32 bits
		unsigned int shift = 2 * i;
		answer |= ((x & m_shifted) << shift)
			| ((y & m_shifted) << (shift + 1));
	}
	return answer;
}

// HELPER METHODE for LUT decoding
template<typename morton, typename coord>
inline coord morton2D_DecodeCoord_LUT256(const morton m, const uint_fast8_t *LUT, const unsigned int startshift) {
	morton a = 0;
	morton EIGHTBITMASK = 0x000000ff;
	unsigned int loops = sizeof(morton);
	for (unsigned int i = 0; i < loops; ++i) {
		a |= (LUT[(m >> ((i * 8) + startshift)) & EIGHTBITMASK] << (4 * i));
	}
	return static_cast<coord>(a);
}

// DECODE 2D Morton code : Shifted LUT
template<typename morton, typename coord>
inline void m2D_d_sLUT(const morton m, coord& x, coord& y) {
	x = morton2D_DecodeCoord_LUT256<morton, coord>(m, Morton2D_decode_x_256, 0);
	y = morton2D_DecodeCoord_LUT256<morton, coord>(m, Morton2D_decode_y_256, 0);
}

// DECODE 2D 64-bit morton code : LUT
template<typename morton, typename coord>
inline void m2D_d_LUT(const morton m, coord& x, coord& y) {
	x = morton2D_DecodeCoord_LUT256<morton, coord>(m, Morton2D_decode_x_256, 0);
	y = morton2D_DecodeCoord_LUT256<morton, coord>(m, Morton2D_decode_x_256, 1);
}

// DECODE 2D Morton code : Shifted LUT (early termination)
template<typename morton, typename coord>
inline void m2D_d_sLUT_ET(const morton m, coord& x, coord& y) {
	x = 0; y = 0;
	morton EIGHTBITMASK = 0x000000ff;
	unsigned long firstbit_location = 0;
	if (!findFirstSetBit<morton>(m, &firstbit_location)) { return; }
	unsigned int i = 0;
	unsigned int shiftback = 0;
	while (firstbit_location >= i) {
		morton m_shifted = (m >> i) & EIGHTBITMASK;
		x |= Morton2D_decode_x_256[m_shifted] << shiftback;
		y |= Morton2D_decode_y_256[m_shifted] << shiftback;
		shiftback += 4;
		i += 8;
	}
}

// DECODE 2D Morton code : LUT (early termination)
template<typename morton, typename coord>
inline void m2D_d_LUT_ET(const morton m, coord& x, coord& y) {
	x = 0; y = 0;
	morton EIGHTBITMASK = 0x000000ff;
	unsigned long firstbit_location = 0;
	if (!findFirstSetBit<morton>(m, &firstbit_location)) { return; }
	unsigned int i = 0;
	unsigned int shiftback = 0;
	while (firstbit_location >= i) {
		morton m_shifted = (m >> i) & EIGHTBITMASK;
		x |= Morton2D_decode_x_256[(m >> i) & EIGHTBITMASK] << shiftback;
		y |= Morton2D_decode_x_256[(m >> (i+1)) & EIGHTBITMASK] << shiftback;
		shiftback += 4;
		i += 8;
	}
}

// HELPER method for Magicbits decoding
template<typename morton, typename coord>
static inline coord morton2D_GetSecondBits(const morton m) {
	morton* masks = (sizeof(morton) <= 4) ? reinterpret_cast<morton*>(magicbit2D_masks32) : reinterpret_cast<morton*>(magicbit2D_masks64);
	morton x = m & masks[4];
	x = (x ^ (x >> 2)) & masks[3];
	x = (x ^ (x >> 4)) & masks[2];
	x = (x ^ (x >> 8)) & masks[1];
	if (sizeof(morton) > 4) x = (x ^ (x >> 16)) & masks[0];
	return static_cast<coord>(x);
}

// DECODE 2D Morton code : Magic bits
// This method splits the morton codes bits by using certain patterns (magic bits)
template<typename morton, typename coord>
inline void m2D_d_magicbits(const morton m, coord& x, coord& y) {
	x = morton2D_GetSecondBits<morton, coord>(m);
	y = morton2D_GetSecondBits<morton, coord>(m >> 1);
}


// DECODE 2D morton code : For loop
template<typename morton, typename coord>
inline void m2D_d_for(const morton m, coord& x, coord& y) {
	x = 0; y = 0;
	unsigned int checkbits = sizeof(morton) * 4;
	for (unsigned int i = 0; i <= checkbits; ++i) {
		morton selector = 1;
		unsigned int shift_selector = 2 * i;
		x |= (m & (selector << shift_selector)) >> i;
		y |= (m & (selector << (shift_selector + 1))) >> (i + 1);
	}
}

// DECODE 3D Morton code : For loop (Early termination version)
template<typename morton, typename coord>
inline void m2D_d_for_ET(const morton m, coord& x, coord& y) {
	x = 0; y = 0;
	float defaultbits = sizeof(morton) * 4;
	unsigned long firstbit_location = 0;
	if (!findFirstSetBit<morton>(m, &firstbit_location)) return;
	unsigned int checkbits = static_cast<unsigned int>(min(defaultbits, firstbit_location / 2.0f));
	for (unsigned int i = 0; i <= checkbits; ++i) {
		morton selector = 1;
		unsigned int shift_selector = 2 * i;
		x |= (m & (selector << shift_selector)) >> i;
		y |= (m & (selector << (shift_selector + 1))) >> (i + 1);
	}
}
