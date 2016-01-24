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
template<typename morton, typename coord> inline morton morton3D_64_Encode_LUT256(const coord x, const coord y, const coord z);
template<typename morton, typename coord> inline morton morton3D_64_Encode_LUT256_ET(const coord x, const coord y, const coord z);

template<typename morton, typename coord> inline morton morton3D_Encode_magicbits(const coord x, const coord y, const coord z);
template<typename morton, typename coord> inline morton morton3D_Encode_for(const coord x, const coord y, const coord z);
template<typename morton, typename coord> inline morton morton3D_Encode_for_ET(const coord x, const coord y, const coord z);

inline void morton3D_64_Decode_LUT256_shifted(const uint_fast64_t morton, uint_fast32_t& x, uint_fast32_t& y, uint_fast32_t& z);
inline void morton3D_64_Decode_LUT256_shifted_ET(const uint_fast64_t morton, uint_fast32_t& x, uint_fast32_t& y, uint_fast32_t& z);
inline void morton3D_64_Decode_LUT256(const uint_fast64_t morton, uint_fast32_t& x, uint_fast32_t& y, uint_fast32_t& z);
inline void morton3D_64_Decode_LUT256_ET(const uint_fast64_t morton, uint_fast32_t& x, uint_fast32_t& y, uint_fast32_t& z);

template<typename morton, typename coord> inline void morton3D_Decode_magicbits(const morton m, coord& x, coord& y, coord& z);
template<typename morton, typename coord> inline void morton3D_Decode_for(const morton m, coord& x, coord& y, coord& z);
template<typename morton, typename coord> inline void morton3D_Decode_for_ET(const morton m, coord& x, coord& y, coord& z);

// ENCODE 3D Morton code : Pre-shifted LUT
template<typename morton, typename coord>
inline morton morton3D_Encode_LUT256_shifted(const coord x, const coord y, const coord z) {
	morton answer = 0;
	morton eightbitmask = (sizeof(morton) <= 4) ? 0x00FF : 0x000000FF;
	if (sizeof(morton) > 4) {
		answer = 
			Morton3D_encode_z_256[(z >> 16) & eightbitmask] |
			Morton3D_encode_y_256[(y >> 16) & eightbitmask] |
			Morton3D_encode_x_256[(x >> 16) & eightbitmask];
	}
	answer = answer << 24 | // shift by 24 = 3 * 8bits
		Morton3D_encode_z_256[(z >> 8) & eightbitmask] |
		Morton3D_encode_y_256[(y >> 8) & eightbitmask] |
		Morton3D_encode_x_256[(x >> 8) & eightbitmask];
	answer = answer << 24 |
		Morton3D_encode_z_256[z & eightbitmask] |
		Morton3D_encode_y_256[y & eightbitmask] |
		Morton3D_encode_x_256[x & eightbitmask];
	return answer;
}

// Helper method for ET LUT encode
template<typename morton, typename coord>
inline morton compute_ET_LUT_encode(const coord c, const uint_fast32_t *LUT) {
	unsigned long maxbit = 0;
	if (findFirstSetBit<coord>(c, &maxbit) == 0) {return 0;}
	morton eightbitmask = (sizeof(morton) <= 4) ? 0x00FF : 0x000000FF;
	morton answer = LUT[c & eightbitmask];
	unsigned int i = 8;
	while (maxbit >= i) {
		answer |= (LUT[(c >> i) & eightbitmask]) << i*3;
		i = i + 8;
	}
	return answer;
}

// ENCODE 3D Morton code : Pre-shifted LUT (Early termination version)
template<typename morton, typename coord>
inline morton morton3D_Encode_LUT256_shifted_ET(const coord x, const coord y, const coord z){
	morton answer_x = compute_ET_LUT_encode<morton, coord>(x, Morton3D_encode_x_256);
	morton answer_y = compute_ET_LUT_encode<morton, coord>(y, Morton3D_encode_y_256);
	morton answer_z = compute_ET_LUT_encode<morton, coord>(z, Morton3D_encode_z_256);
	return total | answer_z | answer_y | answer_x;
}

// ENCODE 3D Morton code : LUT
template<typename morton, typename coord>
inline morton morton3D_64_Encode_LUT256(const coord x, const coord y, const coord z) {
	morton answer = 0;
	morton eightbitmask = (sizeof(morton) <= 4) ? 0x00FF : 0x000000FF;
	if (sizeof(morton) > 4) {
		answer =
			(Morton3D_encode_x_256[(z >> 16) & eightbitmask] << 2)
			| (Morton3D_encode_x_256[(y >> 16) & eightbitmask] << 1)
			| Morton3D_encode_x_256[(x >> 16) & eightbitmask];
	}
	answer = answer << 24 |
		(Morton3D_encode_x_256[(z >> 8) & eightbitmask] << 2)
		| (Morton3D_encode_x_256[(y >> 8) & eightbitmask] << 1)
		| Morton3D_encode_x_256[(x >> 8) & eightbitmask];
	answer = answer << 24 |
		(Morton3D_encode_x_256[z & eightbitmask] << 2)
		| (Morton3D_encode_x_256[y & eightbitmask] << 1)
		| Morton3D_encode_x_256[x & eightbitmask];
	return answer;
}

// ENCODE 3D 64-bit morton code : LUT (Early termination version)
template<typename morton, typename coord>
inline morton morton3D_64_Encode_LUT256_ET(const coord x, const coord y, const coord z) {
	morton answer_x = compute_ET_LUT_encode<morton, coord>(x, Morton3D_encode_x_256);
	morton answer_y = compute_ET_LUT_encode<morton, coord>(y, Morton3D_encode_x_256);
	morton answer_z = compute_ET_LUT_encode<morton, coord>(z, Morton3D_encode_x_256);
	return total | (answer_z << 2) | (answer_y << 1) | answer_x;
}

// ENCODE 3D 64-bit morton code : Magic bits (helper method)
static uint_fast32_t encode_masks32[5] = { 0, 0xff0000ff, 0x0f00f00f, 0xc30c30c3, 0x49249249};
static uint_fast64_t encode_masks64[5] = { 0xffff00000000ffff, 0x00ff0000ff0000ff, 0x100f00f00f00f00f, 0x10c30c30c30c30c3, 0x1249249249249249 };
template<typename morton, typename coord>
static inline morton morton3D_SplitBy3Bits(const coord a) {
	morton* masks = (sizeof(morton) <= 4) ? reinterpret_cast<morton*>(encode_masks32) : reinterpret_cast<morton*>(encode_masks64);
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
	unsigned int checkbits = (sizeof(morton) == 4) ? 10 : 21;
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
	unsigned long checkbits = (sizeof(morton) == 4) ? 10 : 21;

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

// DECODE 3D 64-bit morton code : Shifted LUT
inline void morton3D_64_Decode_LUT256_shifted(const uint_fast64_t morton, uint_fast32_t& x, uint_fast32_t& y, uint_fast32_t& z) {
	x = 0; y = 0; z = 0;
	x = Morton3D_decode_x_512[morton & 0x00000000000001ff]
		| (Morton3D_decode_x_512[((morton >> 9) & 0x000001ff)] << 3)
		| (Morton3D_decode_x_512[((morton >> 18) & 0x000001ff)] << 6)
		| (Morton3D_decode_x_512[((morton >> 27) & 0x000001ff)] << 9)
		| (Morton3D_decode_x_512[((morton >> 36) & 0x000001ff)] << 12)
		| (Morton3D_decode_x_512[((morton >> 46) & 0x000001ff)] << 15)
		| (Morton3D_decode_x_512[((morton >> 54) & 0x000001ff)] << 18);
	y = Morton3D_decode_y_512[morton & 0x1ff]
		| (Morton3D_decode_y_512[((morton >> 9) & 0x000001ff)] << 3)
		| (Morton3D_decode_y_512[((morton >> 18) & 0x000001ff)] << 6)
		| (Morton3D_decode_y_512[((morton >> 27) & 0x000001ff)] << 9)
		| (Morton3D_decode_y_512[((morton >> 36) & 0x000001ff)] << 12)
		| (Morton3D_decode_y_512[((morton >> 46) & 0x000001ff)] << 15)
		| (Morton3D_decode_y_512[((morton >> 54) & 0x000001ff)] << 18);
	z = Morton3D_decode_z_512[morton & 0x1ff]
		| (Morton3D_decode_z_512[((morton >> 9) & 0x000001ff)] << 3)
		| (Morton3D_decode_z_512[((morton >> 18) & 0x000001ff)] << 6)
		| (Morton3D_decode_z_512[((morton >> 27) & 0x000001ff)] << 9)
		| (Morton3D_decode_z_512[((morton >> 36) & 0x000001ff)] << 12)
		| (Morton3D_decode_z_512[((morton >> 46) & 0x000001ff)] << 15)
		| (Morton3D_decode_z_512[((morton >> 54) & 0x000001ff)] << 18);
}

// DECODE 3D 64-bit morton code : Shifted LUT (Early termination version)
inline void morton3D_64_Decode_LUT256_shifted_ET(const uint_fast64_t morton, uint_fast32_t& x, uint_fast32_t& y, uint_fast32_t& z){
	x = 0; y = 0; z = 0;
	unsigned long firstbit_location = 0;
	if (!findFirstSetBit<uint_fast64_t>(morton, &firstbit_location)) { return; }
	x = x | Morton3D_decode_x_512[morton & 0x000001ff];
	y = y | Morton3D_decode_y_512[morton & 0x000001ff];
	z = z | Morton3D_decode_z_512[morton & 0x000001ff];
	if (firstbit_location < 9) { return; }
	x = x | (Morton3D_decode_x_512[((morton >> 9) & 0x000001ff)] << 3);
	y = y | (Morton3D_decode_y_512[((morton >> 9) & 0x000001ff)] << 3);
	z = z | (Morton3D_decode_z_512[((morton >> 9) & 0x000001ff)] << 3);
	if (firstbit_location < 18) { return; }
	x = x | (Morton3D_decode_x_512[((morton >> 18) & 0x000001ff)] << 6);
	y = y | (Morton3D_decode_y_512[((morton >> 18) & 0x000001ff)] << 6);
	z = z | (Morton3D_decode_z_512[((morton >> 18) & 0x000001ff)] << 6);
	if (firstbit_location < 27) { return; }
	x = x | (Morton3D_decode_x_512[((morton >> 27) & 0x000001ff)] << 9);
	y = y | (Morton3D_decode_y_512[((morton >> 27) & 0x000001ff)] << 9);
	z = z | (Morton3D_decode_z_512[((morton >> 27) & 0x000001ff)] << 9);
	if (firstbit_location < 36) { return; }
	x = x | (Morton3D_decode_x_512[((morton >> 36) & 0x000001ff)] << 12);
	y = y | (Morton3D_decode_y_512[((morton >> 36) & 0x000001ff)] << 12);
	z = z | (Morton3D_decode_z_512[((morton >> 36) & 0x000001ff)] << 12);
	if (firstbit_location < 46) { return; }
	x = x | (Morton3D_decode_x_512[((morton >> 46) & 0x000001ff)] << 15);
	y = y | (Morton3D_decode_y_512[((morton >> 46) & 0x000001ff)] << 15);
	z = z | (Morton3D_decode_z_512[((morton >> 46) & 0x000001ff)] << 15);
	if (firstbit_location < 54) { return; }
	x = x | (Morton3D_decode_x_512[((morton >> 54) & 0x000001ff)] << 18);
	y = y | (Morton3D_decode_y_512[((morton >> 54) & 0x000001ff)] << 18);
	z = z | (Morton3D_decode_z_512[((morton >> 54) & 0x000001ff)] << 18);
	return;
}

// DECODE 3D 64-bit morton code : LUT
inline void morton3D_64_Decode_LUT256(const uint_fast64_t morton, uint_fast32_t& x, uint_fast32_t& y, uint_fast32_t& z) {
	x = 0; y = 0; z = 0;
	x = Morton3D_decode_x_512[morton & 0x000001ff]
		| (Morton3D_decode_x_512[((morton >> 9) & 0x000001ff)] << 3)
		| (Morton3D_decode_x_512[((morton >> 18) & 0x000001ff)] << 6)
		| (Morton3D_decode_x_512[((morton >> 27) & 0x000001ff)] << 9)
		| (Morton3D_decode_x_512[((morton >> 36) & 0x000001ff)] << 12)
		| (Morton3D_decode_x_512[((morton >> 46) & 0x000001ff)] << 15)
		| (Morton3D_decode_x_512[((morton >> 54) & 0x000001ff)] << 18);
	y = Morton3D_decode_x_512[(morton >> 1) & 0x000001ff]
		| (Morton3D_decode_x_512[((morton >> 10) & 0x000001ff)] << 3)
		| (Morton3D_decode_x_512[((morton >> 19) & 0x000001ff)] << 6)
		| (Morton3D_decode_x_512[((morton >> 28) & 0x000001ff)] << 9)
		| (Morton3D_decode_x_512[((morton >> 37) & 0x000001ff)] << 12)
		| (Morton3D_decode_x_512[((morton >> 47) & 0x000001ff)] << 15)
		| (Morton3D_decode_x_512[((morton >> 55) & 0x000001ff)] << 18);
	z = Morton3D_decode_x_512[(morton >> 2) & 0x000001ff]
		| (Morton3D_decode_x_512[((morton >> 11) & 0x000001ff)] << 3)
		| (Morton3D_decode_x_512[((morton >> 20) & 0x000001ff)] << 6)
		| (Morton3D_decode_x_512[((morton >> 29) & 0x000001ff)] << 9)
		| (Morton3D_decode_x_512[((morton >> 38) & 0x000001ff)] << 12)
		| (Morton3D_decode_x_512[((morton >> 48) & 0x000001ff)] << 15)
		| (Morton3D_decode_x_512[((morton >> 56) & 0x000001ff)] << 18);
}

// DECODE 3D 64-bit morton code : LUT (Early termination version)
inline void morton3D_64_Decode_LUT256_ET(const uint_fast64_t morton, uint_fast32_t& x, uint_fast32_t& y, uint_fast32_t& z){
	x = 0; y = 0; z = 0;
	unsigned long firstbit_location = 0;
	if (!findFirstSetBit<uint_fast64_t>(morton, &firstbit_location)) { return; }
	x = x | Morton3D_decode_x_512[morton & 0x000001ff];
	y = y | Morton3D_decode_x_512[(morton >> 1) & 0x000001ff];
	z = z | Morton3D_decode_x_512[(morton >> 2) & 0x000001ff];
	if (firstbit_location < 9) { return; }
	x = x | (Morton3D_decode_x_512[((morton >> 9) & 0x000001ff)] << 3);
	y = y | (Morton3D_decode_x_512[((morton >> 10) & 0x000001ff)] << 3);
	z = z | (Morton3D_decode_x_512[((morton >> 11) & 0x000001ff)] << 3);
	if (firstbit_location < 18) { return; }
	x = x | (Morton3D_decode_x_512[((morton >> 18) & 0x000001ff)] << 6);
	y = y | (Morton3D_decode_x_512[((morton >> 19) & 0x000001ff)] << 6);
	z = z | (Morton3D_decode_x_512[((morton >> 20) & 0x000001ff)] << 6);
	if (firstbit_location < 27) { return; }
	x = x | (Morton3D_decode_x_512[((morton >> 27) & 0x000001ff)] << 9);
	y = y | (Morton3D_decode_x_512[((morton >> 28) & 0x000001ff)] << 9);
	z = z | (Morton3D_decode_x_512[((morton >> 29) & 0x000001ff)] << 9);
	if (firstbit_location < 36) { return; }
	x = x | (Morton3D_decode_x_512[((morton >> 36) & 0x000001ff)] << 12);
	y = y | (Morton3D_decode_x_512[((morton >> 37) & 0x000001ff)] << 12);
	z = z | (Morton3D_decode_x_512[((morton >> 38) & 0x000001ff)] << 12);
	if (firstbit_location < 46) { return; }
	x = x | (Morton3D_decode_x_512[((morton >> 46) & 0x000001ff)] << 15);
	y = y | (Morton3D_decode_x_512[((morton >> 47) & 0x000001ff)] << 15);
	z = z | (Morton3D_decode_x_512[((morton >> 48) & 0x000001ff)] << 15);
	if (firstbit_location < 54) { return; }
	x = x | (Morton3D_decode_x_512[((morton >> 54) & 0x000001ff)] << 18);
	y = y | (Morton3D_decode_x_512[((morton >> 55) & 0x000001ff)] << 18);
	z = z | (Morton3D_decode_x_512[((morton >> 56) & 0x000001ff)] << 18);
	return;
}

static uint_fast32_t masks32[5] = { 0x49249249, 0x030c30c3, 0x0F00F00F, 0xFF0000FF, 0x0000FFFF };
static uint_fast64_t masks64[5] = { 0x9249249249249249, 0x030c30c3030c30c3, 0xF00F00F00F00F00F, 0x00FF0000FF0000FF, 0x000000000000FFFF };

// DECODE 3D 64-bit morton code : Magic bits (helper method)
template<typename morton, typename coord>
static inline coord morton3D_getThirdBits(const morton m) {
	morton* masks = (sizeof(morton) <= 4) ? reinterpret_cast<morton*>(masks32) : reinterpret_cast<morton*>(masks64);
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
	unsigned int checkbits;
	float defaultbits = (sizeof(morton) <= 4) ? 10.0f : 21.0f;
	unsigned long firstbit_location = 0;

	// Early termination
	if(!findFirstSetBit<morton>(m, &firstbit_location)) return;

	// How many bits to check?
	checkbits = (unsigned int) min(defaultbits, firstbit_location / 3.0f);
	for (morton i = 0; i <= checkbits; ++i) {
		x |= (m & (1ull << 3 * i)) >> ((2 * i));
		y |= (m & (1ull << ((3 * i) + 1))) >> ((2 * i) + 1);
		z |= (m & (1ull << ((3 * i) + 2))) >> ((2 * i) + 2);
	}
}

#endif // MORTON3D_64_H_
