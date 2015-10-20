// Libmorton - Methods to encode/decode 64-bit morton codes from/to 32-bit (x,y,z) coordinates
#ifndef MORTON3D_32_H_
#define MORTON3D_32_H_

#include <stdint.h>
#include "morton3D_LUTs.h"

#if _MSC_VER
#include <intrin.h>
#endif

using namespace std;

// AVAILABLE METHODS
inline uint_fast32_t morton3D_32_Encode_LUT256_shifted(const uint_fast16_t x, const uint_fast16_t y, const uint_fast16_t z);
inline uint_fast32_t morton3D_32_Encode_LUT256_shifted_ET(const uint_fast16_t x, const uint_fast16_t y, const uint_fast16_t z);
inline uint_fast32_t morton3D_32_Encode_LUT256(const uint_fast16_t x, const uint_fast16_t y, const uint_fast16_t z);
inline uint_fast32_t morton3D_32_Encode_LUT256_ET(const uint_fast16_t x, const uint_fast16_t y, const uint_fast16_t z);
inline uint_fast32_t morton3D_32_Encode_magicbits(const uint_fast16_t x, const uint_fast16_t y, const uint_fast16_t z);
inline uint_fast32_t morton3D_32_Encode_for(const uint_fast16_t x, const uint_fast16_t y, const uint_fast16_t z);
inline uint_fast32_t morton3D_32_Encode_for_ET(const uint_fast16_t x, const uint_fast16_t y, const uint_fast16_t z);

inline void morton3D_32_Decode_LUT256_shifted(const uint_fast64_t morton, uint_fast32_t& x, uint_fast32_t& y, uint_fast32_t& z);
inline void morton3D_32_Decode_LUT256_shifted_ET(const uint_fast64_t morton, uint_fast32_t& x, uint_fast32_t& y, uint_fast32_t& z);
inline void morton3D_32_Decode_LUT256(const uint_fast64_t morton, uint_fast32_t& x, uint_fast32_t& y, uint_fast32_t& z);
inline void morton3D_32_Decode_LUT256_ET(const uint_fast64_t morton, uint_fast32_t& x, uint_fast32_t& y, uint_fast32_t& z);
inline void morton3D_32_Decode_magicbits(const uint_fast64_t morton, uint_fast32_t& x, uint_fast32_t& y, uint_fast32_t& z);
inline void morton3D_32_Decode_for(const uint_fast64_t morton, uint_fast32_t& x, uint_fast32_t& y, uint_fast32_t& z);
inline void morton3D_32_Decode_for_ET(const uint_fast64_t morton, uint_fast32_t& x, uint_fast32_t& y, uint_fast32_t& z);

// ENCODE 3D 32-bit morton code : Shifted LUT
inline uint_fast32_t morton3D_32_Encode_LUT256_shifted(const uint_fast16_t x, const uint_fast16_t y, const uint_fast16_t z) {
	uint_fast32_t answer =
		Morton3D_encode_z_256[(z >> 8) & 0x00FF] |
		Morton3D_encode_y_256[(y >> 8) & 0x00FF] |
		Morton3D_encode_x_256[(x >> 8) & 0x00FF];
	answer = answer << 24 |
		Morton3D_encode_z_256[z & 0x00FF] |
		Morton3D_encode_y_256[y & 0x00FF] |
		Morton3D_encode_x_256[x & 0x00FF];
	return answer;
}

// ENCODE 3D 32-bit morton code : Shifted LUT (Early termination version)
inline uint_fast32_t morton3D_32_Encode_LUT256_shifted_ET(const uint_fast16_t x, const uint_fast16_t y, const uint_fast16_t z) {
	unsigned long x_max = 0, y_max = 0, z_max = 0;
	uint_fast32_t answer_x = 0, answer_y = 0, answer_z = 0, total = 0;
	if (findFirstSetBit32(z, &z_max)) {
		answer_z |= (Morton3D_encode_z_256[z & 0x000000FF]);
		if (z_max >= 8) {
			answer_z |= ((Morton3D_encode_z_256[(z >> 8) & 0x000000FF]) << 24);
		}
		total = answer_z;
	}
	if (findFirstSetBit32(y, &y_max)) {
		answer_y |= (Morton3D_encode_y_256[y & 0x000000FF]);
		if (y_max >= 8) {
			answer_y |= ((Morton3D_encode_y_256[(y >> 8) & 0x000000FF]) << 24);
		}
		total |= answer_y;
	}
	if (findFirstSetBit32(x, &x_max)) {
		answer_x |= (Morton3D_encode_x_256[x & 0x000000FF]);
		if (x_max >= 8) {
			answer_x |= ((Morton3D_encode_x_256[(x >> 8) & 0x000000FF]) << 24);
		}
		total |= answer_x;
	}
	return total;
}

// ENCODE 3D 32-bit morton code : LUT
inline uint_fast32_t morton3D_32_Encode_LUT256(const uint_fast16_t x, const uint_fast16_t y, const uint_fast16_t z) {
	uint_fast32_t answer =
		(Morton3D_encode_x_256[(z >> 8) & 0x00FF] << 2)
		| (Morton3D_encode_x_256[(y >> 8) & 0x00FF] << 1)
		| Morton3D_encode_x_256[(x >> 8) & 0x00FF];
	answer = answer << 24 |
		(Morton3D_encode_x_256[z & 0x00FF] << 2)
		| (Morton3D_encode_x_256[y & 0x00FF] << 1)
		| Morton3D_encode_x_256[x & 0x00FF];
	return answer;
}

// ENCODE 3D 32-bit morton code : LUT (Early termination version)
inline uint_fast32_t morton3D_32_Encode_LUT256_ET(const uint_fast16_t x, const uint_fast16_t y, const uint_fast16_t z) {
	unsigned long x_max = 0, y_max = 0, z_max = 0;
	uint_fast32_t answer_x = 0, answer_y = 0, answer_z = 0, total = 0;
	if (findFirstSetBit32(z, &z_max)) {
		answer_z |= (Morton3D_encode_x_256[z & 0x000000FF]);
		if (z_max >= 8) {
			answer_z |= ((Morton3D_encode_x_256[(z >> 8) & 0x000000FF]) << 24);
		}
		total = answer_z << 2;
	}
	if (findFirstSetBit32(y, &y_max)) {
		answer_y |= (Morton3D_encode_x_256[y & 0x000000FF]);
		if (y_max >= 8) {
			answer_y |= ((Morton3D_encode_x_256[(y >> 8) & 0x000000FF]) << 24);
		}
		total |= answer_y << 1;
	}
	if (findFirstSetBit32(x, &x_max)) {
		answer_x |= (Morton3D_encode_x_256[x & 0x000000FF]);
		if (x_max >= 8) {
			answer_x |= ((Morton3D_encode_x_256[(x >> 8) & 0x000000FF]) << 24);
		}
		total |= answer_x;
	}
	return total;
}

// ENCODE 3D 32-bit morton code : Magic bits (helper method)
inline uint_fast32_t morton3D_32_splitby3(const uint_fast16_t a){
	uint_fast32_t x = a;
	x = (x | x << 16) & 0xff0000ff;
	x = (x | x << 8)  & 0x0f00f00f;
	x = (x | x << 4)  & 0xc30c30c3;
	x = (x | x << 2)  & 0x49249249;
	return x;
}

// ENCODE 3D 32-bit morton code : Magic bits
inline uint_fast32_t morton3D_32_Encode_magicbits(const uint_fast16_t x, const uint_fast16_t y, const uint_fast16_t z){
	return morton3D_32_splitby3(x) | (morton3D_32_splitby3(y) << 1) | (morton3D_32_splitby3(z) << 2);
}

// ENCODE 3D 32-bit morton code : For loop
inline uint_fast32_t morton3D_32_Encode_for(const uint_fast16_t x, const uint_fast16_t y, const uint_fast16_t z) {
	uint_fast32_t answer = 0;
	for (unsigned int i = 0; i < 10; ++i) {
		answer |= ((x & (0x1 << i)) << 2 * i)
			| ((y & (0x1 << i)) << ((2 * i) + 1))
			| ((z & (0x1 << i)) << ((2 * i) + 2));
	}
	return answer;
}

// ENCODE 3D 32-bit morton code : For loop
inline uint_fast32_t morton3D_32_Encode_for_ET(const uint_fast16_t x, const uint_fast16_t y, const uint_fast16_t z) {
	uint_fast32_t answer = 0;
	unsigned long x_max = 0, y_max = 0, z_max = 0;
	findFirstSetBit32(x, &x_max);
	findFirstSetBit32(y, &y_max);
	findFirstSetBit32(z, &z_max);
	unsigned int checkbits = min(10, max(z_max, max(x_max, y_max)) + 1);
	for (unsigned int i = 0; i < checkbits; i++) {
		answer |= ((x & (0x1 << i)) << 2 * i)
			| ((y & (0x1 << i)) << ((2 * i) + 1))
			| ((z & (0x1 << i)) << ((2 * i) + 2));
	}
	return answer;
}

inline void morton3D_32_Decode_LUT256_shifted(const uint_fast32_t morton, uint_fast16_t& x, uint_fast16_t& y, uint_fast16_t& z){
	x = 0; y = 0; z = 0;
	// standard portable version
	x = 0 | Morton3D_decode_x_512[morton & 0x000001ff]
		| (Morton3D_decode_x_512[((morton >> 9) & 0x000001ff)] << 3)
		| (Morton3D_decode_x_512[((morton >> 18) & 0x000001ff)] << 6)
		| (Morton3D_decode_x_512[((morton >> 27) & 0x000001ff)] << 9);
	y = 0 | Morton3D_decode_y_512[morton & 0x1ff]
		| (Morton3D_decode_y_512[((morton >> 9) & 0x000001ff)] << 3)
		| (Morton3D_decode_y_512[((morton >> 18) & 0x000001ff)] << 6)
		| (Morton3D_decode_y_512[((morton >> 27) & 0x000001ff)] << 9);
	z = 0 | Morton3D_decode_z_512[morton & 0x1ff]
		| (Morton3D_decode_z_512[((morton >> 9) & 0x000001ff)] << 3)
		| (Morton3D_decode_z_512[((morton >> 18) & 0x000001ff)] << 6)
		| (Morton3D_decode_z_512[((morton >> 27) & 0x000001ff)] << 9);
}

inline void morton3D_32_Decode_LUT256_shifted_ET(const uint_fast32_t morton, uint_fast16_t& x, uint_fast16_t& y, uint_fast16_t& z) {
	x = 0; y = 0; z = 0;
	unsigned long firstbit_location;
	if (!_BitScanReverse(&firstbit_location, morton)) return;
	x = x | Morton3D_decode_x_512[morton & 0x000001ff];
	y = y | Morton3D_decode_y_512[morton & 0x000001ff];
	z = z | Morton3D_decode_z_512[morton & 0x000001ff];
	if (firstbit_location < 9) return;
	x = x | (Morton3D_decode_x_512[((morton >> 9) & 0x000001ff)] << 3);
	y = y | (Morton3D_decode_y_512[((morton >> 9) & 0x000001ff)] << 3);
	z = z | (Morton3D_decode_z_512[((morton >> 9) & 0x000001ff)] << 3);
	if (firstbit_location < 18) return;
	x = x | (Morton3D_decode_x_512[((morton >> 18) & 0x000001ff)] << 6);
	y = y | (Morton3D_decode_y_512[((morton >> 18) & 0x000001ff)] << 6);
	z = z | (Morton3D_decode_z_512[((morton >> 18) & 0x000001ff)] << 6);
	if (firstbit_location < 27) return;
	x = x | (Morton3D_decode_x_512[((morton >> 27) & 0x000001ff)] << 9);
	y = y | (Morton3D_decode_y_512[((morton >> 27) & 0x000001ff)] << 9);
	z = z | (Morton3D_decode_z_512[((morton >> 27) & 0x000001ff)] << 9);
	return;
}

inline void morton3D_32_Decode_LUT256(const uint_fast32_t morton, uint_fast16_t& x, uint_fast16_t& y, uint_fast16_t& z){

	// standard portable version
	x = 0 | Morton3D_decode_x_512[morton & 0x000001ff]
		| (Morton3D_decode_x_512[((morton >> 9) & 0x000001ff)] << 3)
		| (Morton3D_decode_x_512[((morton >> 18) & 0x000001ff)] << 6)
		| (Morton3D_decode_x_512[((morton >> 27) & 0x000001ff)] << 9);
	y = 0 | Morton3D_decode_x_512[(morton >> 1) & 0x000001ff]
		| (Morton3D_decode_x_512[((morton >> 10) & 0x000001ff)] << 3)
		| (Morton3D_decode_x_512[((morton >> 19) & 0x000001ff)] << 6)
		| (Morton3D_decode_x_512[((morton >> 28) & 0x000001ff)] << 9);
	z = 0 | Morton3D_decode_x_512[(morton >> 2) & 0x000001ff]
		| (Morton3D_decode_x_512[((morton >> 11) & 0x000001ff)] << 3)
		| (Morton3D_decode_x_512[((morton >> 20) & 0x000001ff)] << 6)
		| (Morton3D_decode_x_512[((morton >> 29) & 0x000001ff)] << 9);
}

inline void morton3D_32_Decode_LUT256_ET(const uint_fast32_t morton, uint_fast16_t& x, uint_fast16_t& y, uint_fast16_t& z) {
	x = 0; y = 0; z = 0;
	unsigned long firstbit_location;
	if (!_BitScanReverse(&firstbit_location, morton)) return;
	x = x | Morton3D_decode_x_512[morton & 0x000001ff];
	y = y | Morton3D_decode_x_512[(morton >> 1) & 0x000001ff];
	z = z | Morton3D_decode_x_512[(morton >> 2) & 0x000001ff];
	if (firstbit_location < 9) return;
	x = x | (Morton3D_decode_x_512[((morton >> 9) & 0x000001ff)] << 3);
	y = y | (Morton3D_decode_x_512[((morton >> 10) & 0x000001ff)] << 3);
	z = z | (Morton3D_decode_x_512[((morton >> 11) & 0x000001ff)] << 3);
	if (firstbit_location < 18) return;
	x = x | (Morton3D_decode_x_512[((morton >> 18) & 0x000001ff)] << 6);
	y = y | (Morton3D_decode_x_512[((morton >> 19) & 0x000001ff)] << 6);
	z = z | (Morton3D_decode_x_512[((morton >> 20) & 0x000001ff)] << 6);
	if (firstbit_location < 27) return;
	x = x | (Morton3D_decode_x_512[((morton >> 27) & 0x000001ff)] << 9);
	y = y | (Morton3D_decode_x_512[((morton >> 28) & 0x000001ff)] << 9);
	z = z | (Morton3D_decode_x_512[((morton >> 29) & 0x000001ff)] << 9);
	return;
}

// DECODE 3D 32-bit morton code : Magic bits
inline uint_fast16_t morton3D_32_getThirdBits(const uint_fast32_t a) {
	uint_fast32_t x = a & 0x49249249;
	x = (x ^ (x >> 2)) & 0x030c30c3;
	x = (x ^ (x >> 4)) & 0x0F00F00F;
	x = (x ^ (x >> 8)) & 0xFF0000FF;
	x = (x ^ (x >> 16)) & 0x0000FFFF;
	return (uint_fast16_t)x;
}

inline void morton3D_32_Decode_magicbits(const uint_fast32_t morton, uint_fast16_t& x, uint_fast16_t& y, uint_fast16_t& z) {
	x = morton3D_32_getThirdBits(morton);
	y = morton3D_32_getThirdBits(morton >> 1);
	z = morton3D_32_getThirdBits(morton >> 2);
}

// DECODE 3D 32-bit morton code : For loop
inline void morton3D_32_Decode_for(const uint_fast32_t morton, uint_fast16_t& x, uint_fast16_t& y, uint_fast16_t& z){
	x = 0;
	y = 0;
	z = 0;
	for (uint_fast32_t i = 0; i <= 10; i++) {
		x |= (morton & (1ull << 3 * i)) >> ((2 * i));
		y |= (morton & (1ull << ((3 * i) + 1))) >> ((2 * i) + 1);
		z |= (morton & (1ull << ((3 * i) + 2))) >> ((2 * i) + 2);
	}
}

// DECODE 3D 32-bit morton code : For loop
inline void morton3D_32_Decode_for_ET(const uint_fast32_t morton, uint_fast16_t& x, uint_fast16_t& y, uint_fast16_t& z) {
	x = 0;
	y = 0;
	z = 0;
	unsigned long firstbit_location = 0;
	if (!findFirstSetBit32(morton, &firstbit_location)) return;
	unsigned int checkbits = min(10,(firstbit_location / (float) 3.0));
	for (uint_fast32_t i = 0; i <= checkbits; i++) {
		x |= (morton & (1ull << 3 * i)) >> ((2 * i));
		y |= (morton & (1ull << ((3 * i) + 1))) >> ((2 * i) + 1);
		z |= (morton & (1ull << ((3 * i) + 2))) >> ((2 * i) + 2);
	}
}

#endif // MORTON3D_32_H_
