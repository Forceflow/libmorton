// Libmorton - Methods to encode/decode 64-bit morton codes from/to 32-bit (x,y,z) coordinates
#ifndef MORTON3D_64_H_
#define MORTON3D_64_H_

#include <stdint.h>
#include <algorithm>
#include "morton3D_lookup_tables.h"
#include "morton_common.h"

using namespace std;

// ENCODE 3D 64-bit morton code : Pre-shifted LUT
inline uint_fast64_t morton3D_64_Encode_LUT_shifted(const uint_fast32_t x, const uint_fast32_t y, const uint_fast32_t z){
	uint_fast64_t answer =
		Morton3D_64_encode_z_256[(z >> 16) & 0x000000FF] |
		Morton3D_64_encode_y_256[(y >> 16) & 0x000000FF] |
		Morton3D_64_encode_x_256[(x >> 16) & 0x000000FF];
	answer = answer << 24 |
		Morton3D_64_encode_z_256[(z >> 8) & 0x000000FF] |
		Morton3D_64_encode_y_256[(y >> 8) & 0x000000FF] |
		Morton3D_64_encode_x_256[(x >> 8) & 0x000000FF];
	answer = answer << 24 |
		Morton3D_64_encode_z_256[z & 0x000000FF] |
		Morton3D_64_encode_y_256[y & 0x000000FF] |
		Morton3D_64_encode_x_256[x & 0x000000FF];
	return answer;
}

// ENCODE 3D 64-bit morton code : LUT
inline uint_fast64_t morton3D_64_Encode_LUT(const uint_fast32_t x, const uint_fast32_t y, const uint_fast32_t z){
	uint_fast64_t answer =
		(Morton3D_64_encode_x_256[(z >> 16) & 0x000000FF] << 2)
		| (Morton3D_64_encode_x_256[(y >> 16) & 0x000000FF] << 1)
		| Morton3D_64_encode_x_256[(x >> 16) & 0x000000FF];
	answer = answer << 24 | 
		(Morton3D_64_encode_x_256[(z >> 8) & 0x000000FF] << 2)
		| (Morton3D_64_encode_x_256[(y >> 8) & 0x000000FF] << 1)
		| Morton3D_64_encode_x_256[(x >> 8) & 0x000000FF];
	answer = answer << 24 |
		(Morton3D_64_encode_x_256[z & 0x000000FF] << 2)
		| (Morton3D_64_encode_x_256[y & 0x000000FF] << 1)
		| Morton3D_64_encode_x_256[x & 0x000000FF];
	return answer;
}

inline uint_fast64_t splitBy3(const uint_fast32_t a){
	uint_fast64_t x = a;
	x = (x | x << 32) & 0x1f00000000ffff;
	x = (x | x << 16) & 0x1f0000ff0000ff;
	x = (x | x << 8) & 0x100f00f00f00f00f;
	x = (x | x << 4) & 0x10c30c30c30c30c3;
	x = (x | x << 2) & 0x1249249249249249;
	return x;
}

// ENCODE 3D 64-bit morton code : Magic bits
inline uint_fast64_t morton3D_64_Encode_magicbits(const uint_fast32_t x, const uint_fast32_t y, const uint_fast32_t z){
	return splitBy3(x) | (splitBy3(y) << 1) | (splitBy3(z) << 2);
}

// ENCODE 3D 64-bit morton code : For loop
inline uint_fast64_t morton3D_64_Encode_for(const uint_fast32_t x, const uint_fast32_t y, const uint_fast32_t z){
	uint_fast64_t answer = 0;
	unsigned int checkbits = 21;
#ifdef LIBMORTON_EARLY_TERMINATION
	unsigned long x_max = 0;
	unsigned long y_max = 0;
	unsigned long z_max = 0;
	findFirstSetBit32(x, &x_max);
	findFirstSetBit32(y, &y_max);
	findFirstSetBit32(z, &z_max);
	checkbits = max(z_max,max(x_max, y_max)) + 1;
#endif
	for (uint_fast64_t i = 0; i <= checkbits; ++i) {
    //Here we need to cast 0x1 to 64bits, otherwise there is a bug when morton code is larger than 32 bits
    answer |= ((x & ((uint_fast64_t)0x1 << i)) << 2 * i)
      | ((y & ((uint_fast64_t)0x1 << i)) << ((2 * i) + 1))
      | ((z & ((uint_fast64_t)0x1 << i)) << ((2 * i) + 2));
	}
	return answer;
}

// DECODE 3D 64-bit morton code : Shifted LUT
inline void morton3D_64_Decode_LUT_shifted(const uint_fast64_t morton, uint_fast32_t& x, uint_fast32_t& y, uint_fast32_t& z){
	x = 0; y = 0; z = 0;
#ifdef LIBMORTON_EARLY_TERMINATION
	unsigned long firstbit_location = 0;
	if (!findFirstSetBit64(morton, &firstbit_location)) return;
	x = x | Morton3D_64_decode_x_512[morton & 0x000001ff];
	y = y | Morton3D_64_decode_y_512[morton & 0x000001ff];
	z = z | Morton3D_64_decode_z_512[morton & 0x000001ff];
	if (firstbit_location < 9) return;
	x = x | (Morton3D_64_decode_x_512[((morton >> 9) & 0x000001ff)] << 3);
	y = y | (Morton3D_64_decode_y_512[((morton >> 9) & 0x000001ff)] << 3);
	z = z | (Morton3D_64_decode_z_512[((morton >> 9) & 0x000001ff)] << 3);
	if (firstbit_location < 18) return;
	x = x | (Morton3D_64_decode_x_512[((morton >> 18) & 0x000001ff)] << 6);
	y = y | (Morton3D_64_decode_y_512[((morton >> 18) & 0x000001ff)] << 6);
	z = z | (Morton3D_64_decode_z_512[((morton >> 18) & 0x000001ff)] << 6);
	if (firstbit_location < 27) return;
	x = x | (Morton3D_64_decode_x_512[((morton >> 27) & 0x000001ff)] << 9);
	y = y | (Morton3D_64_decode_y_512[((morton >> 27) & 0x000001ff)] << 9);
	z = z | (Morton3D_64_decode_z_512[((morton >> 27) & 0x000001ff)] << 9);
	if (firstbit_location < 36) return;
	x = x | (Morton3D_64_decode_x_512[((morton >> 36) & 0x000001ff)] << 12);
	y = y | (Morton3D_64_decode_y_512[((morton >> 36) & 0x000001ff)] << 12);
	z = z | (Morton3D_64_decode_z_512[((morton >> 36) & 0x000001ff)] << 12);
	if (firstbit_location < 46) return;
	x = x | (Morton3D_64_decode_x_512[((morton >> 46) & 0x000001ff)] << 15);
	y = y | (Morton3D_64_decode_y_512[((morton >> 46) & 0x000001ff)] << 15);
	z = z | (Morton3D_64_decode_z_512[((morton >> 46) & 0x000001ff)] << 15);
	if (firstbit_location < 54) return;
	x = x | (Morton3D_64_decode_x_512[((morton >> 54) & 0x000001ff)] << 18);
	y = y | (Morton3D_64_decode_y_512[((morton >> 54) & 0x000001ff)] << 18);
	z = z | (Morton3D_64_decode_z_512[((morton >> 54) & 0x000001ff)] << 18);
	return;
#else
	x = 0 | Morton3D_64_decode_x_512[morton & 0x000001ff]
		| (Morton3D_64_decode_x_512[((morton >> 9) & 0x000001ff)] << 3)
		| (Morton3D_64_decode_x_512[((morton >> 18) & 0x000001ff)] << 6)
		| (Morton3D_64_decode_x_512[((morton >> 27) & 0x000001ff)] << 9)
		| (Morton3D_64_decode_x_512[((morton >> 36) & 0x000001ff)] << 12)
		| (Morton3D_64_decode_x_512[((morton >> 46) & 0x000001ff)] << 15)
		| (Morton3D_64_decode_x_512[((morton >> 54) & 0x000001ff)] << 18);
	y = 0 | Morton3D_64_decode_y_512[morton & 0x1ff]
		| (Morton3D_64_decode_y_512[((morton >> 9) & 0x000001ff)] << 3)
		| (Morton3D_64_decode_y_512[((morton >> 18) & 0x000001ff)] << 6)
		| (Morton3D_64_decode_y_512[((morton >> 27) & 0x000001ff)] << 9)
		| (Morton3D_64_decode_y_512[((morton >> 36) & 0x000001ff)] << 12)
		| (Morton3D_64_decode_y_512[((morton >> 46) & 0x000001ff)] << 15)
		| (Morton3D_64_decode_y_512[((morton >> 54) & 0x000001ff)] << 18);
	z = 0 | Morton3D_64_decode_z_512[morton & 0x1ff]
		| (Morton3D_64_decode_z_512[((morton >> 9) & 0x000001ff)] << 3)
		| (Morton3D_64_decode_z_512[((morton >> 18) & 0x000001ff)] << 6)
		| (Morton3D_64_decode_z_512[((morton >> 27) & 0x000001ff)] << 9)
		| (Morton3D_64_decode_z_512[((morton >> 36) & 0x000001ff)] << 12)
		| (Morton3D_64_decode_z_512[((morton >> 46) & 0x000001ff)] << 15)
		| (Morton3D_64_decode_z_512[((morton >> 54) & 0x000001ff)] << 18);
#endif
}

// DECODE 3D 64-bit morton code : LUT
inline void morton3D_64_Decode_LUT(const uint_fast64_t morton, uint_fast32_t& x, uint_fast32_t& y, uint_fast32_t& z){
	x = 0; y = 0; z = 0;
#ifdef LIBMORTON_EARLY_TERMINATION
	unsigned long firstbit_location = 0;
	if (!findFirstSetBit64(morton, &firstbit_location)) return;
	x = x | Morton3D_64_decode_x_512[morton & 0x000001ff];
	y = y | Morton3D_64_decode_x_512[(morton >> 1) & 0x000001ff];
	z = z | Morton3D_64_decode_x_512[(morton >> 2) & 0x000001ff];
	if (firstbit_location < 9) return;
	x = x | (Morton3D_64_decode_x_512[((morton >> 9) & 0x000001ff)] << 3);
	y = y | (Morton3D_64_decode_x_512[((morton >> 10) & 0x000001ff)] << 3);
	z = z | (Morton3D_64_decode_x_512[((morton >> 11) & 0x000001ff)] << 3);
	if (firstbit_location < 18) return;
	x = x | (Morton3D_64_decode_x_512[((morton >> 18) & 0x000001ff)] << 6);
	y = y | (Morton3D_64_decode_x_512[((morton >> 19) & 0x000001ff)] << 6);
	z = z | (Morton3D_64_decode_x_512[((morton >> 20) & 0x000001ff)] << 6);
	if (firstbit_location < 27) return;
	x = x | (Morton3D_64_decode_x_512[((morton >> 27) & 0x000001ff)] << 9);
	y = y | (Morton3D_64_decode_x_512[((morton >> 28) & 0x000001ff)] << 9);
	z = z | (Morton3D_64_decode_x_512[((morton >> 29) & 0x000001ff)] << 9);
	if (firstbit_location < 36) return;
	x = x | (Morton3D_64_decode_x_512[((morton >> 36) & 0x000001ff)] << 12);
	y = y | (Morton3D_64_decode_x_512[((morton >> 37) & 0x000001ff)] << 12);
	z = z | (Morton3D_64_decode_x_512[((morton >> 38) & 0x000001ff)] << 12);
	if (firstbit_location < 46) return;
	x = x | (Morton3D_64_decode_x_512[((morton >> 46) & 0x000001ff)] << 15);
	y = y | (Morton3D_64_decode_x_512[((morton >> 47) & 0x000001ff)] << 15);
	z = z | (Morton3D_64_decode_x_512[((morton >> 48) & 0x000001ff)] << 15);
	if (firstbit_location < 54) return;
	x = x | (Morton3D_64_decode_x_512[((morton >> 54) & 0x000001ff)] << 18);
	y = y | (Morton3D_64_decode_x_512[((morton >> 55) & 0x000001ff)] << 18);
	z = z | (Morton3D_64_decode_x_512[((morton >> 56) & 0x000001ff)] << 18);
	return;
#else
	// standard portable version
	x = 0 | Morton3D_64_decode_x_512[morton & 0x000001ff]
		| (Morton3D_64_decode_x_512[((morton >> 9) & 0x000001ff)] << 3)
		| (Morton3D_64_decode_x_512[((morton >> 18) & 0x000001ff)] << 6)
		| (Morton3D_64_decode_x_512[((morton >> 27) & 0x000001ff)] << 9)
		| (Morton3D_64_decode_x_512[((morton >> 36) & 0x000001ff)] << 12)
		| (Morton3D_64_decode_x_512[((morton >> 46) & 0x000001ff)] << 15)
		| (Morton3D_64_decode_x_512[((morton >> 54) & 0x000001ff)] << 18);
	y = 0 | Morton3D_64_decode_x_512[(morton >> 1) & 0x000001ff]
		| (Morton3D_64_decode_x_512[((morton >> 10) & 0x000001ff)] << 3)
		| (Morton3D_64_decode_x_512[((morton >> 19) & 0x000001ff)] << 6)
		| (Morton3D_64_decode_x_512[((morton >> 28) & 0x000001ff)] << 9)
		| (Morton3D_64_decode_x_512[((morton >> 37) & 0x000001ff)] << 12)
		| (Morton3D_64_decode_x_512[((morton >> 47) & 0x000001ff)] << 15)
		| (Morton3D_64_decode_x_512[((morton >> 55) & 0x000001ff)] << 18);
	z = 0 | Morton3D_64_decode_x_512[(morton >> 2) & 0x000001ff]
		| (Morton3D_64_decode_x_512[((morton >> 11) & 0x000001ff)] << 3)
		| (Morton3D_64_decode_x_512[((morton >> 20) & 0x000001ff)] << 6)
		| (Morton3D_64_decode_x_512[((morton >> 29) & 0x000001ff)] << 9)
		| (Morton3D_64_decode_x_512[((morton >> 38) & 0x000001ff)] << 12)
		| (Morton3D_64_decode_x_512[((morton >> 48) & 0x000001ff)] << 15)
		| (Morton3D_64_decode_x_512[((morton >> 56) & 0x000001ff)] << 18);
#endif
}

inline uint_fast32_t getThirdBits(const uint_fast64_t a){
	uint_fast64_t x = a & 0x9249249249249249;
	x = (x ^ (x >> 2)) & 0x030c30c3030c30c3;
	x = (x ^ (x >> 4)) & 0xF00F00F00F00F00F;
	x = (x ^ (x >> 8)) & 0xFF0000FF0000FF;
	x = (x ^ (x >> 16)) & 0xFFFF;
	return (uint_fast32_t)x;
}

// DECODE 3D 64-bit morton code : Magic bits
inline void morton3D_64_Decode_magicbits(const uint_fast64_t morton, uint_fast32_t& x, uint_fast32_t& y, uint_fast32_t& z){
	x = getThirdBits(morton);
	y = getThirdBits(morton >> 1);
	z = getThirdBits(morton >> 2);
}

// DECODE 3D 64-bit morton code : For loop
inline void morton3D_64_Decode_for(const uint_fast64_t morton, uint_fast32_t& x, uint_fast32_t& y, uint_fast32_t& z){
	x = 0;
	y = 0;
	z = 0;
	unsigned int checkbits = 21;
#ifdef LIBMORTON_EARLY_TERMINATION
	unsigned long firstbit_location = 0;
	if (!findFirstSetBit64(morton, &firstbit_location)) return;
	checkbits = (firstbit_location / (float) 3.0);
#endif
	for (uint_fast64_t i = 0; i <= checkbits; i++) {
		x |= (morton & (1ull << 3 * i)) >> ((2 * i));
		y |= (morton & (1ull << ((3 * i) + 1))) >> ((2 * i) + 1);
		z |= (morton & (1ull << ((3 * i) + 2))) >> ((2 * i) + 2);
	}
}

#endif // MORTON3D_64_H_