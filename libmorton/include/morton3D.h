// Libmorton - Methods to encode/decode 64-bit morton codes from/to 32-bit (x,y,z) coordinates
#pragma once

#include <stdint.h>
#include <algorithm>
#include <math.h>
#include "morton3D_LUTs.h"
#include "morton_common.h"

using namespace std;

// AVAILABLE METHODS
template<typename morton, typename coord> inline morton m3D_e_sLUT(const coord x, const coord y, const coord z);
template<typename morton, typename coord> inline morton m3D_e_sLUT_ET(const coord x, const coord y, const coord z);
template<typename morton, typename coord> inline morton m3D_e_LUT(const coord x, const coord y, const coord z);
template<typename morton, typename coord> inline morton m3D_e_LUT_ET(const coord x, const coord y, const coord z);
template<typename morton, typename coord> inline morton m3D_e_magicbits(const coord x, const coord y, const coord z);
template<typename morton, typename coord> inline morton m3D_e_for(const coord x, const coord y, const coord z);
template<typename morton, typename coord> inline morton m3D_e_for_ET(const coord x, const coord y, const coord z);

template<typename morton, typename coord> inline void m3D_d_sLUT(const morton m, coord& x, coord& y, coord& z);
template<typename morton, typename coord> inline void m3D_d_sLUT_ET(const morton m, coord& x, coord& y, coord& z);
template<typename morton, typename coord> inline void m3D_d_LUT(const morton m, coord& x, coord& y, coord& z);
template<typename morton, typename coord> inline void m3D_d_LUT_ET(const morton m, coord& x, coord& y, coord& z);
template<typename morton, typename coord> inline void m3D_d_magicbits(const morton m, coord& x, coord& y, coord& z);
template<typename morton, typename coord> inline void m3D_d_for(const morton m, coord& x, coord& y, coord& z);
template<typename morton, typename coord> inline void m3D_d_for_ET(const morton m, coord& x, coord& y, coord& z);

// ENCODE 3D Morton code : Pre-shifted LUT
template<typename morton, typename coord>
inline morton m3D_e_sLUT(const coord x, const coord y, const coord z) {
	morton answer = 0;
	const static morton EIGHTBITMASK = 0x000000FF;
	for (unsigned int i = sizeof(coord); i > 0; --i) {
		unsigned int shift = (i - 1) * 8;
		answer =
			answer << 24 | 
			(Morton3D_encode_z_256[(z >> shift) & EIGHTBITMASK] |
			 Morton3D_encode_y_256[(y >> shift) & EIGHTBITMASK] |
			 Morton3D_encode_x_256[(x >> shift) & EIGHTBITMASK]);
	}
	return answer;
}

// ENCODE 3D Morton code : LUT
template<typename morton, typename coord>
inline morton m3D_e_LUT(const coord x, const coord y, const coord z) {
	morton answer = 0;
	const static morton EIGHTBITMASK = 0x000000FF;
	for (unsigned int i = sizeof(coord); i > 0; --i) {
		unsigned int shift = (i - 1) * 8; 
		answer =
			answer << 24 |
			(Morton3D_encode_x_256[(z >> shift) & EIGHTBITMASK] << 2) |
			(Morton3D_encode_x_256[(y >> shift) & EIGHTBITMASK] << 1) |
			 Morton3D_encode_x_256[(x >> shift) & EIGHTBITMASK];
	}
	return answer;
}

// Helper method for ET LUT encode
template<typename morton, typename coord>
inline morton compute3D_ET_LUT_encode(const coord c, const coord *LUT) {
	const static morton EIGHTBITMASK = 0x000000FF;
	unsigned long maxbit = 0;
	morton answer = 0;
	if (findFirstSetBit<coord>(c, &maxbit) == 0) {return 0;}
	unsigned int i = 0;
	while (maxbit >= i) {
		answer |= (LUT[(c >> i) & EIGHTBITMASK]) << i*3;
		i += 8;
	}
	return answer;
}

// ENCODE 3D Morton code : Pre-shifted LUT (Early termination version)
template<typename morton, typename coord>
inline morton m3D_e_sLUT_ET(const coord x, const coord y, const coord z) {
	morton answer_x = compute3D_ET_LUT_encode<morton, coord>(x, Morton3D_encode_x_256);
	morton answer_y = compute3D_ET_LUT_encode<morton, coord>(y, Morton3D_encode_y_256);
	morton answer_z = compute3D_ET_LUT_encode<morton, coord>(z, Morton3D_encode_z_256);
	return answer_z | answer_y | answer_x;
}

// ENCODE 3D 64-bit morton code : LUT (Early termination version)
template<typename morton, typename coord>
inline morton m3D_e_LUT_ET(const coord x, const coord y, const coord z) {
	morton answer_x = compute3D_ET_LUT_encode<morton, coord>(x, Morton3D_encode_x_256);
	morton answer_y = compute3D_ET_LUT_encode<morton, coord>(y, Morton3D_encode_x_256);
	morton answer_z = compute3D_ET_LUT_encode<morton, coord>(z, Morton3D_encode_x_256);
	return (answer_z << 2) | (answer_y << 1) | answer_x;
}

// ENCODE 3D 64-bit morton code : Magic bits (helper method)
template<typename morton, typename coord>
static inline morton morton3D_SplitBy3bits(const coord a) {
	const morton* masks = (sizeof(morton) <= 4) ? reinterpret_cast<const morton*>(magicbit3D_masks32) : reinterpret_cast<const morton*>(magicbit3D_masks64);
	morton x = a;
	if (sizeof(morton) > 4) { x = (x | x << 32) & masks[0];}
	x = (x | x << 16) & masks[1];
	x = (x | x << 8)  & masks[2];
	x = (x | x << 4)  & masks[3];
	x = (x | x << 2)  & masks[4];
	return x;
}

// ENCODE 3D 64-bit morton code : Magic bits
template<typename morton, typename coord>
inline morton m3D_e_magicbits(const coord x, const coord y, const coord z){
	return morton3D_SplitBy3bits<morton, coord>(x) | (morton3D_SplitBy3bits<morton, coord>(y) << 1) | (morton3D_SplitBy3bits<morton, coord>(z) << 2);
}

// ENCODE 3D 64-bit morton code : For loop
template<typename morton, typename coord>
inline morton m3D_e_for(const coord x, const coord y, const coord z){
	morton answer = 0;
	unsigned int checkbits = floor((sizeof(morton) * 8.0f / 3.0f));
	for (unsigned int i = 0; i <= checkbits; ++i) {
		morton mshifted= (morton) 0x1 << i; // Here we need to cast 0x1 to 64bits, otherwise there is a bug when morton code is larger than 32 bits
		unsigned int shift = 2 * i;
    answer |= 
		((x & mshifted) << shift)
      | ((y & mshifted) << (shift + 1))
      | ((z & mshifted) << (shift + 2));
	}
	return answer;
}

// ENCODE 3D 64-bit morton code : For loop (Early termination version)
template<typename morton, typename coord>
inline morton m3D_e_for_ET(const coord x, const coord y, const coord z) {
	morton answer = 0;
	unsigned long x_max = 0, y_max = 0, z_max = 0;
	unsigned int checkbits = floor((sizeof(morton) * 8.0f / 3.0f));
	findFirstSetBit<morton>(x, &x_max);
	findFirstSetBit<morton>(y, &y_max);
	findFirstSetBit<morton>(z, &z_max);
	checkbits = min((unsigned long)checkbits, max(z_max, max(x_max, y_max)) + (unsigned long) 1);
	for (unsigned int i = 0; i <= checkbits; ++i) {
		morton m_shifted = (morton)0x1 << i; // Here we need to cast 0x1 to 64bits, otherwise there is a bug when morton code is larger than 32 bits
		unsigned int shift = 2 * i;
		answer |= ((x & m_shifted) << shift)
			| ((y & m_shifted) << (shift + 1))
			| ((z & m_shifted) << (shift + 2));
	}
	return answer;
}

// todo: wouldn't this be better with 8-bit aligned decode LUT?
template<typename morton, typename coord>
inline coord morton3D_DecodeCoord_LUT256(const morton m, const uint_fast8_t *LUT, const unsigned int startshift) {
	morton a = 0;
	morton NINEBITMASK = 0x000001ff;
	unsigned int loops = floor((sizeof(morton) * 8.0f) / 9.0f);
	for (unsigned int i = 0; i < loops; ++i){
		a |= (LUT[(m >> ((i * 9) + startshift)) & NINEBITMASK] << (3 * i));
	}
	return a;
}

// DECODE 3D 64-bit morton code : Shifted LUT
template<typename morton, typename coord>
inline void m3D_d_sLUT(const morton m, coord& x, coord& y, coord& z) {
	x = morton3D_DecodeCoord_LUT256<morton, coord>(m, Morton3D_decode_x_512, 0);
	y = morton3D_DecodeCoord_LUT256<morton, coord>(m, Morton3D_decode_y_512, 0);
	z = morton3D_DecodeCoord_LUT256<morton, coord>(m, Morton3D_decode_z_512, 0);
}

// DECODE 3D 64-bit morton code : LUT
template<typename morton, typename coord>
inline void m3D_d_LUT(const morton m, coord& x, coord& y, coord& z) {
	x = morton3D_DecodeCoord_LUT256<morton, coord>(m, Morton3D_decode_x_512, 0);
	y = morton3D_DecodeCoord_LUT256<morton, coord>(m, Morton3D_decode_x_512, 1);
	z = morton3D_DecodeCoord_LUT256<morton, coord>(m, Morton3D_decode_x_512, 2);
}

// DECODE 3D 64-bit morton code : Shifted LUT (Early termination version)
template<typename morton, typename coord>
inline void m3D_d_sLUT_ET(const morton m, coord& x, coord& y, coord& z){
	x = 0; y = 0; z = 0;
	morton NINEBITMASK = 0x000001ff;
	unsigned long firstbit_location = 0;
	if (!findFirstSetBit<morton>(m, &firstbit_location)) { return; }
	unsigned int i = 0;
	unsigned int shiftback = 0;
	while (firstbit_location >= i) {
		morton m_shifted = (m >> i) & NINEBITMASK;
		x |= Morton3D_decode_x_512[m_shifted] << shiftback;
		y |= Morton3D_decode_y_512[m_shifted] << shiftback;
		z |= Morton3D_decode_z_512[m_shifted] << shiftback;
		shiftback += 3;
		i += 9;
	}
	return;
}

// DECODE 3D 64-bit morton code : LUT (Early termination version)
template<typename morton, typename coord>
inline void m3D_d_LUT_ET(const morton m, coord& x, coord& y, coord& z){
	x = 0; y = 0; z = 0;
	morton NINEBITMASK = 0x000001ff;
	unsigned long firstbit_location = 0;
	if (!findFirstSetBit<uint_fast64_t>(m, &firstbit_location)) { return; }
	unsigned int i = 0;
	unsigned int shiftback = 0;
	while (i <= firstbit_location) {
		x = x | Morton3D_decode_x_512[(m >> i) & NINEBITMASK] << shiftback;
		y = y | Morton3D_decode_x_512[(m >> (i+1)) & NINEBITMASK] << shiftback;
		z = z | Morton3D_decode_x_512[(m >> (i+2)) & NINEBITMASK] << shiftback;
		i += 9;
		shiftback += 3;
	}
	return;
}

// DECODE 3D 64-bit morton code : Magic bits (helper method)
template<typename morton, typename coord>
static inline coord morton3D_GetThirdBits(const morton m) {
	morton* masks = (sizeof(morton) <= 4) ? reinterpret_cast<morton*>(magicbit3D_masks32) : reinterpret_cast<morton*>(magicbit3D_masks64);
	morton x = m & masks[4];
	x = (x ^ (x >> 2)) & masks[3];
	x = (x ^ (x >> 4)) & masks[2];
	x = (x ^ (x >> 8)) & masks[1];
	if(sizeof(morton) > 4) x = (x ^ (x >> 16)) & masks[0];
	return (coord)x;
}

// DECODE 3D 64-bit morton code : Magic bits
template<typename morton, typename coord>
inline void m3D_d_magicbits(const morton m, coord& x, coord& y, coord& z){
	x = morton3D_GetThirdBits<morton, coord>(m);
	y = morton3D_GetThirdBits<morton, coord>(m >> 1);
	z = morton3D_GetThirdBits<morton, coord>(m >> 2);
}

// DECODE 3D 64-bit morton code : For loop
template<typename morton, typename coord>
inline void m3D_d_for(const morton m, coord& x, coord& y, coord& z){
	x = 0; y = 0; z = 0;
	unsigned int checkbits = floor((sizeof(morton) * 8.0f / 3.0f));
	for (morton i = 0; i <= checkbits; ++i) {
		morton selector = 1;
		unsigned int shift_selector = 3 * i;
		unsigned int shiftback = 2 * i;
		x |= (m & (selector << shift_selector)) >> (shiftback);
		y |= (m & (selector << (shift_selector + 1))) >> (shiftback + 1);
		z |= (m & (selector << (shift_selector + 2))) >> (shiftback + 2);
	}
}

// DECODE 3D 64-bit morton code : For loop (Early termination version)
template<typename morton, typename coord>
inline void m3D_d_for_ET(const morton m, coord& x, coord& y, coord& z) {
	x = 0; y = 0; z = 0;
	float defaultbits = floor((sizeof(morton) * 8.0f / 3.0f));
	unsigned long firstbit_location = 0;
	if(!findFirstSetBit<morton>(m, &firstbit_location)) return;
	unsigned int checkbits = (unsigned int) min(defaultbits, firstbit_location / 3.0f);
	for (morton i = 0; i <= checkbits; ++i) {
		morton selector = 1;
		unsigned int shift_selector = 3 * i;
		unsigned int shiftback = 2 * i;
		x |= (m & (selector << shift_selector)) >> (shiftback);
		y |= (m & (selector << (shift_selector + 1))) >> (shiftback + 1);
		z |= (m & (selector << (shift_selector + 2))) >> (shiftback + 2);
	}
}
