// Libmorton - Methods to encode/decode 64-bit morton codes from/to 32-bit (x,y,z) coordinates
#ifndef MORTON3D_64_H_
#define MORTON3D_64_H_

#include <stdint.h>
#include <algorithm>
#include "morton3D_LUTs.h"
#include "morton_common.h"

using namespace std;

// AVAILABLE METHODS
template<typename morton, typename coord> inline morton morton3D_Encode_LUT256_shifted(const coord x, const coord y, const coord z);
template<typename morton, typename coord> inline morton morton3D_Encode_LUT256_shifted_ET(const coord x, const coord y, const coord z);
template<typename morton, typename coord> inline morton morton3D_Encode_LUT256(const coord x, const coord y, const coord z);
template<typename morton, typename coord> inline morton morton3D_Encode_LUT256_ET(const coord x, const coord y, const coord z);
template<typename morton, typename coord> inline morton morton3D_Encode_magicbits(const coord x, const coord y, const coord z);
template<typename morton, typename coord> inline morton morton3D_Encode_for(const coord x, const coord y, const coord z);
template<typename morton, typename coord> inline morton morton3D_Encode_for_ET(const coord x, const coord y, const coord z);

template<typename morton, typename coord> inline void morton3D_Decode_LUT256_shifted(const morton m, coord& x, coord& y, coord& z);
template<typename morton, typename coord> inline void morton3D_Decode_LUT256_shifted_ET(const morton m, coord& x, coord& y, coord& z);
template<typename morton, typename coord> inline void morton3D_Decode_LUT256(const morton m, coord& x, coord& y, coord& z);
template<typename morton, typename coord> inline void morton3D_Decode_LUT256_ET(const morton m, coord& x, coord& y, coord& z);
template<typename morton, typename coord> inline void morton3D_Decode_magicbits(const morton m, coord& x, coord& y, coord& z);
template<typename morton, typename coord> inline void morton3D_Decode_for(const morton m, coord& x, coord& y, coord& z);
template<typename morton, typename coord> inline void morton3D_Decode_for_ET(const morton m, coord& x, coord& y, coord& z);

// ENCODE 3D Morton code : Pre-shifted LUT
template<typename morton, typename coord>
inline morton morton3D_Encode_LUT256_shifted(const coord x, const coord y, const coord z) {
	morton answer = 0;
	const static morton EIGHTBITMASK = 0x000000FF;
	if (sizeof(morton) > 4) {
		answer = 
			Morton3D_encode_z_256[(z >> 16) & EIGHTBITMASK] |
			Morton3D_encode_y_256[(y >> 16) & EIGHTBITMASK] |
			Morton3D_encode_x_256[(x >> 16) & EIGHTBITMASK];
	}
	answer = answer << 24 | // shift by 24 = 3 * 8bits
		Morton3D_encode_z_256[(z >> 8) & EIGHTBITMASK] |
		Morton3D_encode_y_256[(y >> 8) & EIGHTBITMASK] |
		Morton3D_encode_x_256[(x >> 8) & EIGHTBITMASK];
	answer = answer << 24 |
		Morton3D_encode_z_256[z & EIGHTBITMASK] |
		Morton3D_encode_y_256[y & EIGHTBITMASK] |
		Morton3D_encode_x_256[x & EIGHTBITMASK];
	return answer;
}

// Helper method for ET LUT encode
template<typename morton, typename coord>
inline morton compute3D_ET_LUT_encode(const coord c, const coord *LUT) {
	unsigned long maxbit = 0;
	if (findFirstSetBit<coord>(c, &maxbit) == 0) {return 0;}
	const static morton EIGHTBITMASK = 0x000000FF;
	morton answer = LUT[c & EIGHTBITMASK];
	unsigned int i = 8;
	while (maxbit >= i) {
		answer |= (LUT[(c >> i) & EIGHTBITMASK]) << i*3;
		i += 8;
	}
	return answer;
}

// ENCODE 3D Morton code : Pre-shifted LUT (Early termination version)
template<typename morton, typename coord>
inline morton morton3D_Encode_LUT256_shifted_ET(const coord x, const coord y, const coord z) {
	morton answer_x = compute3D_ET_LUT_encode<morton, coord>(x, Morton3D_encode_x_256);
	morton answer_y = compute3D_ET_LUT_encode<morton, coord>(y, Morton3D_encode_y_256);
	morton answer_z = compute3D_ET_LUT_encode<morton, coord>(z, Morton3D_encode_z_256);
	return answer_z | answer_y | answer_x;
}

// ENCODE 3D Morton code : LUT
template<typename morton, typename coord>
inline morton morton3D_Encode_LUT256(const coord x, const coord y, const coord z) {
	morton answer = 0;
	const static morton EIGHTBITMASK = 0x000000FF;
	if (sizeof(morton) > 4) {
		answer =
			(Morton3D_encode_x_256[(z >> 16) & EIGHTBITMASK] << 2)
			| (Morton3D_encode_x_256[(y >> 16) & EIGHTBITMASK] << 1)
			| Morton3D_encode_x_256[(x >> 16) & EIGHTBITMASK];
	}
	answer = answer << 24 |
		(Morton3D_encode_x_256[(z >> 8) & EIGHTBITMASK] << 2)
		| (Morton3D_encode_x_256[(y >> 8) & EIGHTBITMASK] << 1)
		| Morton3D_encode_x_256[(x >> 8) & EIGHTBITMASK];
	answer = answer << 24 |
		(Morton3D_encode_x_256[z & EIGHTBITMASK] << 2)
		| (Morton3D_encode_x_256[y & EIGHTBITMASK] << 1)
		| Morton3D_encode_x_256[x & EIGHTBITMASK];
	return answer;
}

// ENCODE 3D 64-bit morton code : LUT (Early termination version)
template<typename morton, typename coord>
inline morton morton3D_Encode_LUT256_ET(const coord x, const coord y, const coord z) {
	morton answer_x = compute3D_ET_LUT_encode<morton, coord>(x, Morton3D_encode_x_256);
	morton answer_y = compute3D_ET_LUT_encode<morton, coord>(y, Morton3D_encode_x_256);
	morton answer_z = compute3D_ET_LUT_encode<morton, coord>(z, Morton3D_encode_x_256);
	return (answer_z << 2) | (answer_y << 1) | answer_x;
}

// ENCODE 3D 64-bit morton code : Magic bits (helper method)
static const uint_fast32_t encode3D_masks32[5] = { 0, 0xff0000ff, 0x0f00f00f, 0xc30c30c3, 0x49249249};
static const uint_fast64_t encode3D_masks64[5] = { 0xffff00000000ffff, 0x00ff0000ff0000ff, 0x100f00f00f00f00f, 0x10c30c30c30c30c3, 0x1249249249249249};
template<typename morton, typename coord>
static inline morton morton3D_SplitBy3Bits(const coord a) {
	const morton* masks = (sizeof(morton) <= 4) ? reinterpret_cast<const morton*>(encode3D_masks32) : reinterpret_cast<const morton*>(encode3D_masks64);
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
inline morton morton3D_Encode_magicbits(const coord x, const coord y, const coord z){
	return morton3D_SplitBy3Bits<morton, coord>(x) | (morton3D_SplitBy3Bits<morton, coord>(y) << 1) | (morton3D_SplitBy3Bits<morton, coord>(z) << 2);
}

// ENCODE 3D 64-bit morton code : For loop
template<typename morton, typename coord>
inline morton morton3D_Encode_for(const coord x, const coord y, const coord z){
	morton answer = 0;
	unsigned int checkbits = (sizeof(morton) <= 4) ? 10 : 21;
	for (unsigned int i = 0; i <= checkbits; ++i) {
    answer |= ((x & ((morton)0x1 << i)) << 2 * i)     //Here we need to cast 0x1 to the amount of bits in the morton code, 
      | ((y & ((morton)0x1 << i)) << ((2 * i) + 1))   //otherwise there is a bug when morton code is larger than 32 bits
      | ((z & ((morton)0x1 << i)) << ((2 * i) + 2));
	}
	return answer;
}

// ENCODE 3D 64-bit morton code : For loop (Early termination version)
template<typename morton, typename coord>
inline morton morton3D_Encode_for_ET(const coord x, const coord y, const coord z) {
	morton answer = 0;
	unsigned long x_max = 0, y_max = 0, z_max = 0;
	unsigned long checkbits = (sizeof(morton) <= 4) ? 10 : 21;

	findFirstSetBit<morton>(x, &x_max);
	findFirstSetBit<morton>(y, &y_max);
	findFirstSetBit<morton>(z, &z_max);

	checkbits = min(checkbits, max(z_max, max(x_max, y_max)) + (unsigned long) 1);
	for (unsigned int i = 0; i <= checkbits; ++i) {
		//Here we need to cast 0x1 to 64bits, otherwise there is a bug when morton code is larger than 32 bits
		answer |= ((x & ((morton)0x1 << i)) << 2 * i)
			| ((y & ((morton)0x1 << i)) << ((2 * i) + 1))
			| ((z & ((morton)0x1 << i)) << ((2 * i) + 2));
	}
	return answer;
}

template<typename morton, typename coord>
inline coord morton3D_DecodeCoord_LUT256_shifted(const morton m, const uint_fast8_t *LUT, const unsigned int startshift) {
	morton a = 0;
	morton NINEBITMASK = 0x00000000000001ff;
	a = LUT[(m >> startshift) & NINEBITMASK]
		| (LUT[((m >> (startshift+9)) & NINEBITMASK)] << 3)
		| (LUT[((m >> (startshift+18)) & NINEBITMASK)] << 6)
		| (LUT[((m >> (startshift+27)) & NINEBITMASK)] << 9);
	if (sizeof(morton) > 4) {
		a |=
			  (LUT[((m >> (startshift+36)) & NINEBITMASK)] << 12)
			| (LUT[((m >> (startshift+46)) & NINEBITMASK)] << 15)
			| (LUT[((m >> (startshift+54)) & NINEBITMASK)] << 18);
	}
	return a;
}

// DECODE 3D 64-bit morton code : Shifted LUT
template<typename morton, typename coord>
inline void morton3D_Decode_LUT256_shifted(const morton m, coord& x, coord& y, coord& z) {
	x = morton3D_DecodeCoord_LUT256_shifted<morton, coord>(m, Morton3D_decode_x_512, 0);
	y = morton3D_DecodeCoord_LUT256_shifted<morton, coord>(m, Morton3D_decode_y_512, 0);
	z = morton3D_DecodeCoord_LUT256_shifted<morton, coord>(m, Morton3D_decode_z_512, 0);
}

// DECODE 3D 64-bit morton code : Shifted LUT (Early termination version)
template<typename morton, typename coord>
inline void morton3D_Decode_LUT256_shifted_ET(const morton m, coord& x, coord& y, coord& z){
	x = 0; y = 0; z = 0;
	morton NINEBITMASK = 0x00000000000001ff;
	unsigned long firstbit_location = 0;
	if (!findFirstSetBit<morton>(m, &firstbit_location)) { return; }
	x = x | Morton3D_decode_x_512[m & NINEBITMASK];
	y = y | Morton3D_decode_y_512[m & NINEBITMASK];
	z = z | Morton3D_decode_z_512[m & NINEBITMASK];
	if (firstbit_location < 9) { return; }
	x = x | (Morton3D_decode_x_512[((m >> 9) & NINEBITMASK)] << 3);
	y = y | (Morton3D_decode_y_512[((m >> 9) & NINEBITMASK)] << 3);
	z = z | (Morton3D_decode_z_512[((m >> 9) & NINEBITMASK)] << 3);
	if (firstbit_location < 18) { return; }
	x = x | (Morton3D_decode_x_512[((m >> 18) & NINEBITMASK)] << 6);
	y = y | (Morton3D_decode_y_512[((m >> 18) & NINEBITMASK)] << 6);
	z = z | (Morton3D_decode_z_512[((m >> 18) & NINEBITMASK)] << 6);
	if (firstbit_location < 27) { return; }
	x = x | (Morton3D_decode_x_512[((m >> 27) & NINEBITMASK)] << 9);
	y = y | (Morton3D_decode_y_512[((m >> 27) & NINEBITMASK)] << 9);
	z = z | (Morton3D_decode_z_512[((m >> 27) & NINEBITMASK)] << 9);
	if (firstbit_location < 36) { return; }
	x = x | (Morton3D_decode_x_512[((m >> 36) & NINEBITMASK)] << 12);
	y = y | (Morton3D_decode_y_512[((m >> 36) & NINEBITMASK)] << 12);
	z = z | (Morton3D_decode_z_512[((m >> 36) & NINEBITMASK)] << 12);
	if (firstbit_location < 46) { return; }
	x = x | (Morton3D_decode_x_512[((m >> 46) & NINEBITMASK)] << 15);
	y = y | (Morton3D_decode_y_512[((m >> 46) & NINEBITMASK)] << 15);
	z = z | (Morton3D_decode_z_512[((m >> 46) & NINEBITMASK)] << 15);
	if (firstbit_location < 54) { return; }
	x = x | (Morton3D_decode_x_512[((m >> 54) & NINEBITMASK)] << 18);
	y = y | (Morton3D_decode_y_512[((m >> 54) & NINEBITMASK)] << 18);
	z = z | (Morton3D_decode_z_512[((m >> 54) & NINEBITMASK)] << 18);
	return;
}

// DECODE 3D 64-bit morton code : LUT
template<typename morton, typename coord>
inline void morton3D_Decode_LUT256(const morton m, coord& x, coord& y, coord& z){
	x = morton3D_DecodeCoord_LUT256_shifted<morton, coord>(m, Morton3D_decode_x_512, 0);
	y = morton3D_DecodeCoord_LUT256_shifted<morton, coord>(m, Morton3D_decode_x_512, 1);
	z = morton3D_DecodeCoord_LUT256_shifted<morton, coord>(m, Morton3D_decode_x_512, 2);
}

// DECODE 3D 64-bit morton code : LUT (Early termination version)
template<typename morton, typename coord>
inline void morton3D_Decode_LUT256_ET(const morton m, coord& x, coord& y, coord& z){
	x = 0; y = 0; z = 0;
	morton NINEBITMASK = 0x00000000000001ff;
	unsigned long firstbit_location = 0;
	if (!findFirstSetBit<uint_fast64_t>(m, &firstbit_location)) { return; }
	x = x | Morton3D_decode_x_512[m & NINEBITMASK];
	y = y | Morton3D_decode_x_512[(m >> 1) & NINEBITMASK];
	z = z | Morton3D_decode_x_512[(m >> 2) & NINEBITMASK];
	if (firstbit_location < 9) { return; }
	x = x | (Morton3D_decode_x_512[((m >> 9) & NINEBITMASK)] << 3);
	y = y | (Morton3D_decode_x_512[((m >> 10) & NINEBITMASK)] << 3);
	z = z | (Morton3D_decode_x_512[((m >> 11) & NINEBITMASK)] << 3);
	if (firstbit_location < 18) { return; }
	x = x | (Morton3D_decode_x_512[((m >> 18) & NINEBITMASK)] << 6);
	y = y | (Morton3D_decode_x_512[((m >> 19) & NINEBITMASK)] << 6);
	z = z | (Morton3D_decode_x_512[((m >> 20) & NINEBITMASK)] << 6);
	if (firstbit_location < 27) { return; }
	x = x | (Morton3D_decode_x_512[((m >> 27) & NINEBITMASK)] << 9);
	y = y | (Morton3D_decode_x_512[((m >> 28) & NINEBITMASK)] << 9);
	z = z | (Morton3D_decode_x_512[((m >> 29) & NINEBITMASK)] << 9);
	if (firstbit_location < 36) { return; }
	x = x | (Morton3D_decode_x_512[((m >> 36) & NINEBITMASK)] << 12);
	y = y | (Morton3D_decode_x_512[((m >> 37) & NINEBITMASK)] << 12);
	z = z | (Morton3D_decode_x_512[((m >> 38) & NINEBITMASK)] << 12);
	if (firstbit_location < 46) { return; }
	x = x | (Morton3D_decode_x_512[((m >> 46) & NINEBITMASK)] << 15);
	y = y | (Morton3D_decode_x_512[((m >> 47) & NINEBITMASK)] << 15);
	z = z | (Morton3D_decode_x_512[((m >> 48) & NINEBITMASK)] << 15);
	if (firstbit_location < 54) { return; }
	x = x | (Morton3D_decode_x_512[((m >> 54) & NINEBITMASK)] << 18);
	y = y | (Morton3D_decode_x_512[((m >> 55) & NINEBITMASK)] << 18);
	z = z | (Morton3D_decode_x_512[((m >> 56) & NINEBITMASK)] << 18);
	return;
}

static uint_fast32_t decode3D_masks32[5] = {0x49249249, 0x030c30c3, 0x0F00F00F, 0xFF0000FF, 0x0000FFFF};
static uint_fast64_t decode3D_masks64[5] = {0x9249249249249249, 0x030c30c3030c30c3, 0xF00F00F00F00F00F, 0x00FF0000FF0000FF, 0x000000000000FFFF};

// DECODE 3D 64-bit morton code : Magic bits (helper method)
template<typename morton, typename coord>
static inline coord morton3D_getThirdBits(const morton m) {
	morton* masks = (sizeof(morton) <= 4) ? reinterpret_cast<morton*>(decode3D_masks32) : reinterpret_cast<morton*>(decode3D_masks64);
	morton x = m & masks[0];
	x = (x ^ (x >> 2)) & masks[1];
	x = (x ^ (x >> 4)) & masks[2];
	x = (x ^ (x >> 8)) & masks[3];
	x = (x ^ (x >> 16)) & masks[4];
	return (coord)x;
}

// DECODE 3D 64-bit morton code : Magic bits
template<typename morton, typename coord>
inline void morton3D_Decode_magicbits(const morton m, coord& x, coord& y, coord& z){
	x = morton3D_getThirdBits<morton, coord>(m);
	y = morton3D_getThirdBits<morton, coord>(m >> 1);
	z = morton3D_getThirdBits<morton, coord>(m >> 2);
}

// DECODE 3D 64-bit morton code : For loop
template<typename morton, typename coord>
inline void morton3D_Decode_for(const morton m, coord& x, coord& y, coord& z){
	x = 0; y = 0; z = 0;
	unsigned int checkbits = (sizeof(morton) <= 4) ? 10 : 21;

	for (morton i = 0; i <= checkbits; ++i) {
		x |= (m & (1ull << 3 * i)) >> ((2 * i));
		y |= (m & (1ull << ((3 * i) + 1))) >> ((2 * i) + 1);
		z |= (m & (1ull << ((3 * i) + 2))) >> ((2 * i) + 2);
	}
}

// DECODE 3D 64-bit morton code : For loop (Early termination version)
template<typename morton, typename coord>
inline void morton3D_Decode_for_ET(const morton m, coord& x, coord& y, coord& z) {
	x = 0; y = 0; z = 0;
	float defaultbits = (sizeof(morton) <= 4) ? 10.0f : 21.0f;
	unsigned long firstbit_location = 0;

	// Early termination
	if(!findFirstSetBit<morton>(m, &firstbit_location)) return;

	// How many bits to check?
	unsigned int checkbits = (unsigned int) min(defaultbits, firstbit_location / 3.0f);
	for (morton i = 0; i <= checkbits; ++i) {
		x |= (m & (1ull << 3 * i)) >> ((2 * i));
		y |= (m & (1ull << ((3 * i) + 1))) >> ((2 * i) + 1);
		z |= (m & (1ull << ((3 * i) + 2))) >> ((2 * i) + 2);
	}
}

#endif // MORTON3D_64_H_
