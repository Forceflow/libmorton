// Libmorton - Methods to encode/decode 64-bit morton codes from/to 32-bit (x,y,z) coordinates
#ifndef MORTON3D_32_H_
#define MORTON3D_32_H_

#include <stdint.h>
#include "morton3D_LUTs.h"

#if _MSC_VER
#include <intrin.h>
#endif

using namespace std;

// #define LIBMORTON_EARLY_TERMINATION

// ENCODE 3D 32-bit morton code : Fpr loop
inline uint_fast32_t morton3D_32_Encode_for(const uint_fast16_t x, const uint_fast16_t y, const uint_fast16_t z){
	uint_fast32_t answer = 0;
	for (uint_fast32_t i = 0; i < 10; ++i) {
		answer |= ((x & (0x1 << i)) << 2 * i)
			| ((y & (0x1 << i)) << ((2 * i) + 1))
			| ((z & (0x1 << i)) << ((2 * i) + 2));
	}
	return answer;
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

// ENCODE 3D 32-bit morton code : Shifted LUT
inline uint_fast32_t morton3D_32_Encode_LUT256_shifted(const uint_fast16_t x, const uint_fast16_t y, const uint_fast16_t z){
	uint_fast32_t answer =
		Morton3D_64_encode_z_256[(z >> 8) & 0x00FF] |
		Morton3D_64_encode_y_256[(y >> 8) & 0x00FF] |
		Morton3D_64_encode_x_256[(x >> 8) & 0x00FF];
	answer = answer << 24 |
		Morton3D_64_encode_z_256[z & 0x00FF] |
		Morton3D_64_encode_y_256[y & 0x00FF] |
		Morton3D_64_encode_x_256[x & 0x00FF];
	return answer;
}

// ENCODE 3D 32-bit morton code : LUT
inline uint_fast32_t morton3D_32_Encode_LUT256(const uint_fast16_t x, const uint_fast16_t y, const uint_fast16_t z){
	uint_fast32_t answer =
		 (Morton3D_64_encode_x_256[(z >> 8) & 0x00FF] << 2) 
		|(Morton3D_64_encode_x_256[(y >> 8) & 0x00FF] << 1)
		| Morton3D_64_encode_x_256[(x >> 8) & 0x00FF];
	answer = answer << 24 |
		 (Morton3D_64_encode_z_256[z & 0x00FF] << 2) 
		|(Morton3D_64_encode_y_256[y & 0x00FF] << 1)
		| Morton3D_64_encode_x_256[x & 0x00FF];
	return answer;
}

inline void morton3D_32_Decode_LUT_shifted(const uint_fast32_t morton, uint_fast16_t& x, uint_fast16_t& y, uint_fast16_t& z){
	x = 0; y = 0; z = 0;
#ifdef LIBMORTON_EARLY_TERMINATION
	unsigned long firstbit_location;
	if (!_BitScanReverse(&firstbit_location, morton)) return;
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
	return;
#else
	// standard portable version
	x = 0 | Morton3D_64_decode_x_512[morton & 0x000001ff]
		| (Morton3D_64_decode_x_512[((morton >> 9) & 0x000001ff)] << 3)
		| (Morton3D_64_decode_x_512[((morton >> 18) & 0x000001ff)] << 6)
		| (Morton3D_64_decode_x_512[((morton >> 27) & 0x000001ff)] << 9);
	y = 0 | Morton3D_64_decode_y_512[morton & 0x1ff]
		| (Morton3D_64_decode_y_512[((morton >> 9) & 0x000001ff)] << 3)
		| (Morton3D_64_decode_y_512[((morton >> 18) & 0x000001ff)] << 6)
		| (Morton3D_64_decode_y_512[((morton >> 27) & 0x000001ff)] << 9);
	z = 0 | Morton3D_64_decode_z_512[morton & 0x1ff]
		| (Morton3D_64_decode_z_512[((morton >> 9) & 0x000001ff)] << 3)
		| (Morton3D_64_decode_z_512[((morton >> 18) & 0x000001ff)] << 6)
		| (Morton3D_64_decode_z_512[((morton >> 27) & 0x000001ff)] << 9);
#endif
}

inline void morton3D_32_Decode_LUT(const uint_fast32_t morton, uint_fast16_t& x, uint_fast16_t& y, uint_fast16_t& z){
	x = 0; y = 0; z = 0;
#ifdef LIBMORTON_EARLY_TERMINATION
	unsigned long firstbit_location;
	if (!_BitScanReverse(&firstbit_location, morton)) return;
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
	return;
#else
	// standard portable version
	x = 0 | Morton3D_64_decode_x_512[morton & 0x000001ff]
		| (Morton3D_64_decode_x_512[((morton >> 9) & 0x000001ff)] << 3)
		| (Morton3D_64_decode_x_512[((morton >> 18) & 0x000001ff)] << 6)
		| (Morton3D_64_decode_x_512[((morton >> 27) & 0x000001ff)] << 9);
	y = 0 | Morton3D_64_decode_x_512[(morton >> 1) & 0x000001ff]
		| (Morton3D_64_decode_x_512[((morton >> 10) & 0x000001ff)] << 3)
		| (Morton3D_64_decode_x_512[((morton >> 19) & 0x000001ff)] << 6)
		| (Morton3D_64_decode_x_512[((morton >> 28) & 0x000001ff)] << 9);
	z = 0 | Morton3D_64_decode_x_512[(morton >> 2) & 0x000001ff]
		| (Morton3D_64_decode_x_512[((morton >> 11) & 0x000001ff)] << 3)
		| (Morton3D_64_decode_x_512[((morton >> 20) & 0x000001ff)] << 6)
		| (Morton3D_64_decode_x_512[((morton >> 29) & 0x000001ff)] << 9);
#endif
}

// DECODE 3D 32-bit morton code : For loop
inline void morton3D_32_Decode_for(const uint_fast32_t morton, uint_fast16_t& x, uint_fast16_t& y, uint_fast16_t& z){
	x = 0;
	y = 0;
	z = 0;
	unsigned int checkbits = 10;
#ifdef LIBMORTON_EARLY_TERMINATION
	unsigned long firstbit_location = 0;
	if (!findFirstSetBit(morton, &firstbit_location)) return;
	checkbits = (firstbit_location / (float) 3.0);
#endif
	for (uint_fast32_t i = 0; i <= checkbits; i++) {
		x |= (morton & (1ull << 3 * i)) >> ((2 * i));
		y |= (morton & (1ull << ((3 * i) + 1))) >> ((2 * i) + 1);
		z |= (morton & (1ull << ((3 * i) + 2))) >> ((2 * i) + 2);
	}
}

#endif // MORTON3D_32_H_
