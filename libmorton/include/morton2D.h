// Libmorton - Methods to encode/decode 64-bit morton codes from/to 32-bit (x,y,z) coordinates
#pragma once

#include <stdint.h>
#include "morton2D_LUTs.h"
#include "morton_common.h"

template<typename morton, typename coord> inline morton morton2D_Encode_for(const coord x, const coord y);
template<typename morton, typename coord> inline morton morton2D_Encode_magicbits(const coord x, const coord y);
template<typename morton, typename coord> inline morton morton2D_Encode_LUT256_shifted(const coord x, const coord y);
template<typename morton, typename coord> inline morton morton2D_Encode_LUT256_shifted_ET(const coord x, const coord y);
template<typename morton, typename coord> inline morton morton2D_Encode_LUT256(const coord x, const coord y);
template<typename morton, typename coord> inline morton morton2D_Encode_LUT256_ET(const coord x, const coord y);
template<typename morton, typename coord> inline void morton2D_Decode_LUT256_shifted(const morton m, coord& x, coord& y);
template<typename morton, typename coord> inline void morton2D_Decode_LUT256_shifted_ET(const morton m, coord& x, coord& y);
template<typename morton, typename coord> inline void morton2D_Decode_LUT256(const morton m, coord& x, coord& y);
template<typename morton, typename coord> inline void morton2D_Decode_LUT256_ET(const morton m, coord& x, coord& y);

// ENCODE 2D Morton code : Pre-shifted LUT
template<typename morton, typename coord>
inline morton morton2D_Encode_LUT256_shifted(const coord x, const coord y) {
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

// ENCODE 2D Morton code : LUT
template<typename morton, typename coord>
inline morton morton2D_Encode_LUT256(const coord x, const coord y) {
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

// Helper method for ET LUT encode
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
template<typename morton, typename coord>
inline morton morton2D_Encode_LUT256_shifted_ET(const coord x, const coord y) {
	morton answer_x = compute2D_ET_LUT_encode<morton, coord>(x, Morton2D_encode_x_256);
	morton answer_y = compute2D_ET_LUT_encode<morton, coord>(y, Morton2D_encode_y_256);
	return answer_y | answer_x;
}

// ENCODE 2D 64-bit morton code : LUT (Early termination version)
template<typename morton, typename coord>
inline morton morton2D_Encode_LUT256_ET(const coord x, const coord y) {
	morton answer_x = compute2D_ET_LUT_encode<morton, coord>(x, Morton2D_encode_x_256);
	morton answer_y = compute2D_ET_LUT_encode<morton, coord>(y, Morton2D_encode_x_256);
	return (answer_y << 1) | answer_x;
}

// ENCODE 2D morton code : Magic bits (helper method)
template<typename morton, typename coord>
inline morton morton2D_splitby2(const coord a) {
	const morton* masks = (sizeof(morton) <= 4) ? reinterpret_cast<const morton*>(encode2D_masks32) : reinterpret_cast<const morton*>(encode2D_masks64);
	morton x = a;
	if (sizeof(morton) > 4) { x = (x | x << 32) & masks[0]; }
	x = (x | x << 16) & masks[1];
	x = (x | x << 8) & masks[2];
	x = (x | x << 4) & masks[3];
	x = (x | x << 2) & masks[4];
	x = (x | x << 1) & masks[5];
	return x;
}

// ENCODE 2D morton code : Magic bits
template<typename morton, typename coord>
inline morton morton2D_Encode_magicbits(const coord x, const coord y) {
	return morton2D_splitby2<morton, coord>(x) | (morton2D_splitby2<morton, coord>(y) << 1);
}

// ENCODE 2D morton code : For Loop
template<typename morton, typename coord>
inline morton morton2D_Encode_for(const coord x, const coord y){
	morton answer = 0;
	unsigned int checkbits = floor((sizeof(morton) * 4.0f);
	for (unsigned int i = 0; i <= checkbits; ++i) {
		morton mshifted = (morton)0x1 << i;
		unsigned int shift = 2 * i;
		answer |=
			  ((x & mshifted) << shift) //Here we need to cast 0x1 to the amount of bits in the morton code, 
			| ((y & mshifted) << (shift + 1));   //otherwise there is a bug when morton code is larger than 32 bits
	}
	return answer;
}

// DECODE
template<typename morton, typename coord>
inline coord morton2D_DecodeCoord_LUT256(const morton m, const uint_fast8_t *LUT, const unsigned int startshift) {
	morton a = 0;
	morton EIGHTBITMASK = 0x00000000000000ff;
	a =   (LUT[(m >> startshift) & EIGHTBITMASK])
		| (LUT[((m >> (startshift + 8)) & EIGHTBITMASK)] << 4)
		| (LUT[((m >> (startshift + 16)) & EIGHTBITMASK)] << 8)
		| (LUT[((m >> (startshift + 24)) & EIGHTBITMASK)] << 12);
	if (sizeof(morton) > 4) {
		a |=  (LUT[((m >> (startshift + 32)) & NINEBITMASK)] << 16)
			| (LUT[((m >> (startshift + 40)) & NINEBITMASK)] << 20)
			| (LUT[((m >> (startshift + 48)) & NINEBITMASK)] << 24);
			| (LUT[((m >> (startshift + 56)) & NINEBITMASK)] << 28);
	}
	return a;
}

// DECODE 2D morton code : Shifted LUT
template<typename morton, typename coord>
inline void morton2D_Decode_LUT256_shifted(const morton m, coord& x, coord& y) {
	x = morton2D_DecodeCoord_LUT256<morton, coord>(m, Morton2D_decode_x_256, 0);
	y = morton2D_DecodeCoord_LUT256<morton, coord>(m, Morton2D_decode_y_256, 0);
}

// DECODE 3D 64-bit morton code : LUT
template<typename morton, typename coord>
inline void morton2D_Decode_LUT256(const morton m, coord& x, coord& y) {
	x = morton2D_DecodeCoord_LUT256<morton, coord>(m, Morton2D_decode_x_256, 0);
	y = morton2D_DecodeCoord_LUT256<morton, coord>(m, Morton2D_decode_x_256, 1);
}

// DECODE 2D morton code : Shifted LUT (early termination)
template<typename morton, typename coord>
inline void morton2D_Decode_LUT256_shifted_ET(const morton m, coord& x, coord& y) {
	x = 0; y = 0;
	static const morton EIGHTBITMASK = 0x000000FF;
	unsigned long firstbit_location = 0;
	if (!findFirstSetBit<morton>(m, &firstbit_location)) { return; }
	x = x | Morton2D_decode_x_256[m & EIGHTBITMASK];
	y = y | Morton2D_decode_y_256[m & EIGHTBITMASK];
	if (firstbit_location < 8) { return; }
	x = x | (Morton2D_decode_x_256[((m >> 8) & EIGHTBITMASK)] << 4);
	y = y | (Morton2D_decode_y_256[((m >> 8) & EIGHTBITMASK)] << 4);
	if (firstbit_location < 16) { return; }
	x = x | (Morton2D_decode_x_256[((m >> 16) & EIGHTBITMASK)] << 8);
	y = y | (Morton2D_decode_y_256[((m >> 16) & EIGHTBITMASK)] << 8);
	if (firstbit_location < 24) { return; }
	x = x | (Morton2D_decode_x_256[((m >> 24) & EIGHTBITMASK)] << 12);
	y = y | (Morton2D_decode_y_256[((m >> 24) & EIGHTBITMASK)] << 12);
	if (firstbit_location < 32) { return; }
	x = x | (Morton2D_decode_x_256[((m >> 32) & EIGHTBITMASK)] << 16);
	y = y | (Morton2D_decode_y_256[((m >> 32) & EIGHTBITMASK)] << 16);
	if (firstbit_location < 40) { return; }
	x = x | (Morton2D_decode_x_256[((m >> 40) & EIGHTBITMASK)] << 20);
	y = y | (Morton2D_decode_y_256[((m >> 40) & EIGHTBITMASK)] << 20);
	if (firstbit_location < 48) { return; }
	x = x | (Morton2D_decode_x_256[((m >> 48) & EIGHTBITMASK)] << 24);
	y = y | (Morton2D_decode_y_256[((m >> 48) & EIGHTBITMASK)] << 24);
	if (firstbit_location < 56) { return; }
	x = x | (Morton2D_decode_x_256[((m >> 56) & EIGHTBITMASK)] << 28);
	y = y | (Morton2D_decode_y_256[((m >> 56) & EIGHTBITMASK)] << 28);
	return;
}

// DECODE 2D morton code : Shifted LUT (early termination)
template<typename morton, typename coord>
inline void morton2D_Decode_LUT256_ET(const morton m, coord& x, coord& y) {
	x = 0; y = 0;
	static const morton EIGHTBITMASK = 0x000000FF;
	unsigned long firstbit_location = 0;
	if (!findFirstSetBit<morton>(m, &firstbit_location)) { return; }
	x = x | Morton2D_decode_x_256[m & EIGHTBITMASK];
	y = y | Morton2D_decode_x_256[m & EIGHTBITMASK];
	if (firstbit_location < 8) { return; }
	x = x | (Morton2D_decode_x_256[((m >> 8) & EIGHTBITMASK)] << 4);
	y = y | (Morton2D_decode_x_256[((m >> 9) & EIGHTBITMASK)] << 4);
	if (firstbit_location < 16) { return; }
	x = x | (Morton2D_decode_x_256[((m >> 16) & EIGHTBITMASK)] << 8);
	y = y | (Morton2D_decode_x_256[((m >> 17) & EIGHTBITMASK)] << 8);
	if (firstbit_location < 24) { return; }
	x = x | (Morton2D_decode_x_256[((m >> 24) & EIGHTBITMASK)] << 12);
	y = y | (Morton2D_decode_x_256[((m >> 25) & EIGHTBITMASK)] << 12);
	if (firstbit_location < 32) { return; }
	x = x | (Morton2D_decode_x_256[((m >> 32) & EIGHTBITMASK)] << 16);
	y = y | (Morton2D_decode_x_256[((m >> 33) & EIGHTBITMASK)] << 16);
	if (firstbit_location < 40) { return; }
	x = x | (Morton2D_decode_x_256[((m >> 40) & EIGHTBITMASK)] << 20);
	y = y | (Morton2D_decode_x_256[((m >> 41) & EIGHTBITMASK)] << 20);
	if (firstbit_location < 48) { return; }
	x = x | (Morton2D_decode_x_256[((m >> 48) & EIGHTBITMASK)] << 24);
	y = y | (Morton2D_decode_x_256[((m >> 49) & EIGHTBITMASK)] << 24);
	if (firstbit_location < 56) { return; }
	x = x | (Morton2D_decode_x_256[((m >> 56) & EIGHTBITMASK)] << 28);
	y = y | (Morton2D_decode_x_256[((m >> 57) & EIGHTBITMASK)] << 28);
	return;
}

// DECODE 2D morton code : For loop
template<typename morton, typename coord>
inline void morton2D_Decode_for(const morton m, coord& x, coord& y) {
	x = 0; y = 0;
	for (morton i = 0; i <= (sizeof(m)*4); ++i) {
		x |= (m & (1ull << 2 * i)) >> i;
		y |= (m & (1ull << ((2 * i) + 1))) >> (i + 1);
	}
}
