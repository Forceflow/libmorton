// Libmorton - Methods to encode/decode 64-bit morton codes from/to 32-bit (x,y,z) coordinates
#ifndef MORTON3D_32_H_
#define MORTON3D_32_H_

#include <stdint.h>
#include "morton3D_lookup_tables.h"

#if _MSC_VER
#include <intrin.h>
#endif

using namespace std;

// #define LIBMORTON_USE_INTRINSICS

inline uint_fast32_t morton3D_32_Encode_LUT_shifted(const uint_fast16_t x, const uint_fast16_t y, const uint_fast16_t z){
	uint_fast32_t answer =
		Morton3D_64_encode_z_256[(z >> 8) & 0x00FF] |
		Morton3D_64_encode_y_256[(y >> 8) & 0x00FF] |
		Morton3D_64_encode_x_256[(x >> 8) & 0x00FF];
	answer = answer << 24 |
		Morton3D_64_encode_z_256[z & 0xFF] |
		Morton3D_64_encode_y_256[y & 0xFF] |
		Morton3D_64_encode_x_256[x & 0xFF];
	return answer;
}

inline uint_fast32_t morton3D_32_Encode_LUT(const uint_fast16_t x, const uint_fast16_t y, const uint_fast16_t z){
	uint_fast32_t answer =
		Morton3D_64_encode_z_256[(z >> 8) & 0x00FF] |
		Morton3D_64_encode_y_256[(y >> 8) & 0x00FF] |
		Morton3D_64_encode_x_256[(x >> 8) & 0x00FF];
	answer = answer << 24 |
		Morton3D_64_encode_z_256[(z)& 0x00FF] |
		Morton3D_64_encode_y_256[(y)& 0x00FF] |
		Morton3D_64_encode_x_256[(x)& 0x00FF];
	return answer;
}

inline void morton3D_32_Decode_LUT_shifted(const uint_fast32_t morton, uint_fast16_t& x, uint_fast16_t& y, uint_fast16_t& z){
	x = 0; y = 0; z = 0;
#ifdef LIBMORTON_USE_INTRINSICS
	// For Microsoft compilers use _BitScanForward & _BitScanReverse. For GCC use __builtin_ffs, __builtin_clz, __builtin_ctz.

	// use bit manipulation intrinsic to find out first bit, for early termination
	unsigned long firstbit_location;
#if _MSC_VER
	// are the casts necessary? and the blanking in the second one?
	// Does the pragma stop the compiler from optimizing this?
	// is it cheaper to do this using the 64 bit one
	if (!_BitScanReverse(&firstbit_location, morton)){ // also test last part of morton code
		return;
	}
#endif
	x = x | Morton3D_64_decode_x_512[morton & 0x1ff];
	y = y | Morton3D_64_decode_y_512[morton & 0x1ff];
	z = z | Morton3D_64_decode_z_512[morton & 0x1ff];
	if (firstbit_location < 9) return;
	x = x | (Morton3D_64_decode_x_512[((morton >> 9) & 0x1ff)] << 3);
	y = y | (Morton3D_64_decode_y_512[((morton >> 9) & 0x1ff)] << 3);
	z = z | (Morton3D_64_decode_z_512[((morton >> 9) & 0x1ff)] << 3);
	if (firstbit_location < 18) return;
	x = x | (Morton3D_64_decode_x_512[((morton >> 18) & 0x1ff)] << 6);
	y = y | (Morton3D_64_decode_y_512[((morton >> 18) & 0x1ff)] << 6);
	z = z | (Morton3D_64_decode_z_512[((morton >> 18) & 0x1ff)] << 6);
	if (firstbit_location < 27) return;
	x = x | (Morton3D_64_decode_x_512[((morton >> 27) & 0x1ff)] << 9);
	y = y | (Morton3D_64_decode_y_512[((morton >> 27) & 0x1ff)] << 9);
	z = z | (Morton3D_64_decode_z_512[((morton >> 27) & 0x1ff)] << 9);
	return;
#else
	// standard portable version
	x = 0 | Morton3D_64_decode_x_512[morton & 0x1ff]
		| (Morton3D_64_decode_x_512[((morton >> 9) & 0x1ff)] << 3)
		| (Morton3D_64_decode_x_512[((morton >> 18) & 0x1ff)] << 6)
		| (Morton3D_64_decode_x_512[((morton >> 27) & 0x1ff)] << 9);
	y = 0 | Morton3D_64_decode_y_512[morton & 0x1ff]
		| (Morton3D_64_decode_y_512[((morton >> 9) & 0x1ff)] << 3)
		| (Morton3D_64_decode_y_512[((morton >> 18) & 0x1ff)] << 6)
		| (Morton3D_64_decode_y_512[((morton >> 27) & 0x1ff)] << 9);
	z = 0 | Morton3D_64_decode_z_512[morton & 0x1ff]
		| (Morton3D_64_decode_z_512[((morton >> 9) & 0x1ff)] << 3)
		| (Morton3D_64_decode_z_512[((morton >> 18) & 0x1ff)] << 6)
		| (Morton3D_64_decode_z_512[((morton >> 27) & 0x1ff)] << 9);
#endif
}

inline uint32_t morton3D_32_Encode_for(const uint16_t x, const uint16_t y, const uint16_t z){
	uint32_t answer = 0;
	for (uint32_t i = 0; i < 10; ++i) {
		answer |= ((x & (0x1 << i)) << 2 * i)
			| ((y & (0x1 << i)) << ((2 * i) + 1))
			| ((z & (0x1 << i)) << ((2 * i) + 2));
	}
	return answer;
}

#endif
