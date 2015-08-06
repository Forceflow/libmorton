// Libmorton - Methods to encode/decode 64-bit morton codes from/to 32-bit (x,y,z) coordinates
#ifndef MORTON3D_64_H_
#define MORTON3D_64_H_

#include <stdint.h>
#include "morton3D_lookup_tables.h"

#if _MSC_VER
#include <intrin.h>
#endif

using namespace std;

//#define LIBMORTON_USE_INTRINSICS

inline uint_fast64_t morton3D_64_Encode_LUT_shifted(const uint_fast32_t x, const uint_fast32_t y, const uint_fast32_t z){
	uint_fast64_t answer =
		Morton3D_64_encode_z_256[(z >> 16) & 0x000000FF] |
		Morton3D_64_encode_y_256[(y >> 16) & 0x000000FF] |
		Morton3D_64_encode_x_256[(x >> 16) & 0x000000FF];
	answer = answer << 48 |
		Morton3D_64_encode_z_256[(z >> 8) & 0x000000FF] |
		Morton3D_64_encode_y_256[(y >> 8) & 0x000000FF] |
		Morton3D_64_encode_x_256[(x >> 8) & 0x000000FF];
	answer = answer << 24 |
		Morton3D_64_encode_z_256[z & 0x000000FF] |
		Morton3D_64_encode_y_256[y & 0x000000FF] |
		Morton3D_64_encode_x_256[x & 0x000000FF];
	return answer;
}

// encoding with lookup table
inline uint_fast64_t morton3D_64_Encode_LUT(const uint_fast32_t x, const uint_fast32_t y, const uint_fast32_t z){
	uint_fast64_t answer =
		(Morton3D_64_encode_x_256[(z >> 16) & 0x000000FF] << 2)
		| (Morton3D_64_encode_x_256[(y >> 16) & 0x000000FF] << 1)
		| Morton3D_64_encode_x_256[(x >> 16) & 0x000000FF];
	answer = answer << 48 | 
		(Morton3D_64_encode_x_256[(z >> 8) & 0x000000FF] << 2)
		| (Morton3D_64_encode_x_256[(y >> 8) & 0x000000FF] << 1)
		| Morton3D_64_encode_x_256[(x >> 8) & 0x000000FF];
	answer = answer << 24 |
		(Morton3D_64_encode_x_256[z & 0x000000FF] << 2)
		| (Morton3D_64_encode_x_256[y & 0x000000FF] << 1)
		| Morton3D_64_encode_x_256[x & 0x000000FF];
	return answer;
}

inline void morton3D_64_Decode_LUT_shifted(const uint_fast64_t morton, uint_fast32_t& x, uint_fast32_t& y, uint_fast32_t& z){
	x = 0; y = 0; z = 0;
#ifdef LIBMORTON_USE_INTRINSICS
	// todo move this to separate, 32/64-bit aware function
	// For Microsoft compilers use _BitScanForward & _BitScanReverse. For GCC use __builtin_ffs, __builtin_clz, __builtin_ctz.
	// use bit manipulation intrinsic to find out first bit, for early termination
	unsigned long firstbit_location;
#if _MSC_VER
	// are the casts necessary? and the blanking in the second one?
	// is it cheaper to do this using the 64 bit one
	if (_BitScanReverse(&firstbit_location, (morton >> 32))){ // check first part of morton code
		firstbit_location += 32;
	} else if ( ! _BitScanReverse(&firstbit_location, (morton & 0xFFFFFFFF))){ // also test last part of morton code
		return;
	}
#endif
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
	// standard portable version
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

inline void morton3D_64_Decode_LUT(const uint_fast64_t morton, uint_fast32_t& x, uint_fast32_t& y, uint_fast32_t& z){
	x = 0; y = 0; z = 0;
#ifdef LIBMORTON_USE_INTRINSICS
	// For Microsoft compilers use _BitScanForward & _BitScanReverse. For GCC use __builtin_ffs, __builtin_clz, __builtin_ctz.
	// use bit manipulation intrinsic to find out first bit, for early termination
	unsigned long firstbit_location;
#if _MSC_VER
	// are the casts necessary? and the blanking in the second one?
	// is it cheaper to do this using the 64 bit one
	if (_BitScanReverse(&firstbit_location, (morton >> 32))){ // check first part of morton code
		firstbit_location += 32;
	}
	else if (!_BitScanReverse(&firstbit_location, (morton & 0xFFFFFFFF))){ // also test last part of morton code
		return;
	}
#endif
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

inline uint_fast32_t morton3D_64_Decode_X_LUT(const uint_fast64_t morton){
	return 0 | Morton3D_64_decode_x_512[morton & 0x000001ff]
		| (Morton3D_64_decode_x_512[((morton >> 9) & 0x000001ff)] << 3)
		| (Morton3D_64_decode_x_512[((morton >> 18) & 0x000001ff)] << 6)
		| (Morton3D_64_decode_x_512[((morton >> 27) & 0x000001ff)] << 9)
		| (Morton3D_64_decode_x_512[((morton >> 36) & 0x000001ff)] << 12)
		| (Morton3D_64_decode_x_512[((morton >> 46) & 0x000001ff)] << 15)
		| (Morton3D_64_decode_x_512[((morton >> 54) & 0x000001ff)] << 18);
}

inline uint_fast32_t morton3D_64_Decode_Y_LUT(const uint_fast64_t morton){
	return 0 | Morton3D_64_decode_y_512[morton & 0x1ff]
		| (Morton3D_64_decode_y_512[((morton >> 9) & 0x1ff)] << 3)
		| (Morton3D_64_decode_y_512[((morton >> 18) & 0x1ff)] << 6)
		| (Morton3D_64_decode_y_512[((morton >> 27) & 0x1ff)] << 9)
		| (Morton3D_64_decode_y_512[((morton >> 36) & 0x1ff)] << 12)
		| (Morton3D_64_decode_y_512[((morton >> 46) & 0x1ff)] << 15)
		| (Morton3D_64_decode_y_512[((morton >> 54) & 0x1ff)] << 18);
}

inline uint_fast32_t morton3D_64_Decode_Z_LUT(const uint_fast64_t morton){
	return 0 | Morton3D_64_decode_z_512[morton & 0x1ff]
		| (Morton3D_64_decode_z_512[((morton >> 9) & 0x1ff)] << 3)
		| (Morton3D_64_decode_z_512[((morton >> 18) & 0x1ff)] << 6)
		| (Morton3D_64_decode_z_512[((morton >> 27) & 0x1ff)] << 9)
		| (Morton3D_64_decode_z_512[((morton >> 36) & 0x1ff)] << 12)
		| (Morton3D_64_decode_z_512[((morton >> 46) & 0x1ff)] << 15)
		| (Morton3D_64_decode_z_512[((morton >> 54) & 0x1ff)] << 18);
}

inline uint64_t morton3D_64_Encode_for(const uint32_t x, const uint32_t y, const uint32_t z){
	uint64_t answer = 0;
	for (uint64_t i = 0; i < 21; ++i) {
		answer |= ((x & (0x1 << i)) << 2 * i)
			| ((y & (0x1 << i)) << ((2 * i) + 1))
			| ((z & (0x1 << i)) << ((2 * i) + 2));
	}
	return answer;
}

inline uint64_t splitBy3(const uint32_t a){
	uint64_t x = a & 0x1fffff;
	x = (x | x << 32) & 0x1f00000000ffff;
	x = (x | x << 16) & 0x1f0000ff0000ff;
	x = (x | x << 8) & 0x100f00f00f00f00f;
	x = (x | x << 4) & 0x10c30c30c30c30c3;
	x = (x | x << 2) & 0x1249249249249249;
	return x;
}

inline uint64_t morton3D_64_Encode_magicbits(const uint32_t x, const uint32_t y, const uint32_t z){
	uint64_t answer = 0 | splitBy3(x) | splitBy3(y) << 1 | splitBy3(z) << 2;
	return answer;
}

inline uint32_t getThirdBits(const uint64_t a){
	uint64_t x = a & 0x9249249249249249;
	x = (x ^ (x >> 2)) & 0x030c30c3030c30c3;
	x = (x ^ (x >> 4)) & 0xF00F00F00F00F00F;
	x = (x ^ (x >> 8)) & 0xFF0000FF0000FF;
	x = (x ^ (x >> 16)) & 0xFFFF;
	return (uint32_t)x;
}

inline uint32_t morton3D_64_Decode_X_magicbits(const uint64_t morton){
	return getThirdBits(morton);
}

inline uint32_t morton3D_64_Decode_Y_magicbits(const uint64_t morton){
	return getThirdBits(morton >> 1);
}

inline uint32_t morton3D_64_Decode_Z_magicbits(const uint64_t morton){
	return getThirdBits(morton >> 2);
}

inline void morton3D_64_Decode_magicbits(const uint64_t morton, uint32_t& x, uint32_t& y, uint32_t& z){
	x = morton3D_64_Decode_X_magicbits(morton);
	y = morton3D_64_Decode_Y_magicbits(morton);
	z = morton3D_64_Decode_Z_magicbits(morton);
}

inline void morton3D_64_Decode_for(const uint64_t morton, uint32_t& x, uint32_t& y, uint32_t& z){
	x = 0;
	y = 0;
	z = 0;
	for (uint64_t i = 0; i < 21; i++) {
		x |= (morton & (1ull << 3 * i)) >> ((2 * i));
		y |= (morton & (1ull << ((3 * i) + 1))) >> ((2 * i) + 1);
		z |= (morton & (1ull << ((3 * i) + 2))) >> ((2 * i) + 2);
	}
}

#endif // MORTON3D_64_H_