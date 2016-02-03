// Libmorton - Methods to encode/decode 64-bit morton codes from/to 32-bit (x,y,z) coordinates
#ifndef MORTON2D_64_H_
#define MORTON2D_64_H_

#include <stdint.h>
#include "morton2D_LUTs.h"

#if _MSC_VER
#include <intrin.h>
#endif

template<typename morton, typename coord> inline morton morton2D_Encode_for(const coord x, const coord y);
template<typename morton, typename coord> inline morton morton2D_Encode_magicbits(const coord x, const coord y);
template<typename morton, typename coord> inline morton morton2D_Encode_LUT256_shifted(const coord x, const coord y);
template<typename morton, typename coord> inline morton morton2D_Encode_LUT256(const coord x, const coord y);

// ENCODE 2D morton code : For Loop
template<typename morton, typename coord>
inline morton morton2D_Encode_for(const coord x, const coord y){
	morton answer = 0;
	unsigned long checkbits = sizeof(coord)*8;
	for (morton i = 0; i < checkbits; ++i) {
		answer |= (x & ((morton)0x1 << i)) << (2 * i) | (y & ((morton)0x1 << i)) << ((2 * i) + 1);
	}
	return answer;
}

// ENCODE 3D morton code : Magic bits (helper method)
static const uint_fast32_t encode2D_masks32[6] = { 0, 0x0000FFFF, 0x00FF00FF, 0x0F0F0F0F, 0x33333333, 0x55555555};
static const uint_fast64_t encode2D_masks64[6] = { 0x00000000FFFFFFFF, 0x0000FFFF0000FFFF, 0x00FF00FF00FF00FF, 
											0x0F0F0F0F0F0F0F0F, 0x3333333333333333, 0x5555555555555555};

// ENCODE 2D morton code : Magic bits (helper method)
template<typename morton, typename coord>
inline morton morton2D_splitby2(const coord a){
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
inline morton morton2D_Encode_magicbits(const coord x, const coord y){
	return morton2D_splitby2<morton, coord>(x) | (morton2D_splitby2<morton, coord>(y) << 1);
}

// ENCODE 2D morton code: LUT preshifted
template<typename morton, typename coord>
inline morton morton2D_Encode_LUT256_shifted(const coord x, const coord y){
	morton answer = 0;
	const morton EIGHTBITMASK = (sizeof(morton) <= 4) ? 0x00FF : 0x000000FF;
	if (sizeof(morton) > 4) {
		answer =
			Morton2D_encode_y_256[(y >> 24) & EIGHTBITMASK] | // select 8 upper bits
			Morton2D_encode_x_256[(x >> 24) & EIGHTBITMASK];
		answer = answer << 16 | // shift by 16 = 2 * 8bits
			Morton2D_encode_y_256[(y >> 16) & EIGHTBITMASK] | // select next 8 bits
			Morton2D_encode_x_256[(x >> 16) & EIGHTBITMASK];
	}
	answer = answer << 16 | // shift by 16 = 2 * 8bits
		Morton2D_encode_y_256[(y >> 8) & EIGHTBITMASK] | // select next 8 bits
		Morton2D_encode_x_256[(x >> 8) & EIGHTBITMASK];
	answer = answer << 16 | // shift by 16 = 2 * 8bits
		Morton2D_encode_y_256[(y) & EIGHTBITMASK] | // select next 8 bits
		Morton2D_encode_x_256[(x) & EIGHTBITMASK];
	return answer;
}

// ENCODE 2D morton code: LUT
template<typename morton, typename coord>
inline morton morton2D_64_Encode_LUT256(const coord x, const coord y) {
	morton answer = 0;
	static const morton EIGHTBITMASK = (sizeof(morton) <= 4) ? 0x00FF : 0x000000FF;
	if (sizeof(morton) > 4) {
		answer =
			(Morton2D_encode_x_256[(y >> 24) & EIGHTBITMASK]) << 1 | // select 8 upper bits
			Morton2D_encode_x_256[(x >> 24) & EIGHTBITMASK];
		answer = answer << 16 | // shift by 16 = 2 * 8bits
			(Morton2D_encode_x_256[(y >> 16) & EIGHTBITMASK]) << 1 | // select next 8 bits
			Morton2D_encode_x_256[(x >> 16) & EIGHTBITMASK];
	}
	answer = answer << 16 | // shift by 16 = 2 * 8bits
		(Morton2D_encode_x_256[(y >> 8) & EIGHTBITMASK]) << 1 | // select next 8 bits
		Morton2D_encode_x_256[(x >> 8) & EIGHTBITMASK];
	answer = answer << 16 | // shift by 16 = 2 * 8bits
		(Morton2D_encode_x_256[(y)& EIGHTBITMASK]) << 1 | // select next 8 bits
		Morton2D_encode_x_256[(x)& EIGHTBITMASK];
	return answer;
}

// DECODE 2D morton code : For loop
template<typename morton, typename coord>
inline void morton2D_Decode_for(const morton m, coord& x, coord& y) {
	x = 0; y = 0;
	for (uint_fast64_t i = 0; i < (sizeof(m)*8); i++) {
		x |= (m & (1ull << 2 * i)) >> i;
		y |= (m & (1ull << ((2 * i) + 1))) >> (i + 1);
	}
}

#endif // MORTON2D_64_H_