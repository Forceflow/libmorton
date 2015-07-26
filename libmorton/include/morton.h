// Libmorton - Methods to encode/decode 64-bit morton codes from/to 32-bit (x,y,z) coordinates
#ifndef MORTON_H_
#define MORTON_H_

#include <stdint.h>
#include "morton_lookup_tables.h"

#if _MSC_VER
#include <intrin.h>
#endif

using namespace std;

#define LIBMORTON_USE_INTRINSICS

// THESE DEFAULT METHODS WILL ALWAYS POINT TO THE FASTEST IMPLEMENTED METHOD
// -------------------------------------------------------------------------
// ENCODE a 32-bit (x,y,z) coordinate to a 64-bit morton code
inline uint64_t morton3D_Encode(const uint32_t x, const uint32_t y, const uint32_t z);
// DECODE a 64-bit morton code into 32-bit (x,y,z) coordinates (use this if you need all three coordinates)
inline uint64_t morton3D_Decode(const uint64_t morton, uint32_t& x, uint32_t& y, uint32_t& z);
// DECODE a 64-bit morton code into a 32-bit coordinate (use this if you only need x, y or z)
inline uint32_t morton3D_Decode_X(const uint64_t morton);
inline uint32_t morton3D_Decode_Y(const uint64_t morton);
inline uint32_t morton3D_Decode_Z(const uint64_t morton);

// encoding with lookup table
inline uint64_t morton3D_Encode_LUT(const uint32_t x, const uint32_t y, const uint32_t z){
	uint64_t answer = 
			 encode_morton256_z[(z >> 16) & 0xFF ] |
			 encode_morton256_y[(y >> 16) & 0xFF] |
			 encode_morton256_x[(x >> 16) & 0xFF];
	answer = answer << 48 |
			 encode_morton256_z[(z >> 8) & 0xFF] |
			 encode_morton256_y[(y >> 8) & 0xFF] |
			 encode_morton256_x[(x >> 8) & 0xFF];
	answer = answer << 24 |
			 encode_morton256_z[(z)& 0xFF] |
			 encode_morton256_y[(y)& 0xFF] |
			 encode_morton256_x[(x)& 0xFF];
	return answer;
}

inline void morton3D_Decode_LUT(const uint64_t morton, uint32_t& x, uint32_t& y, uint32_t& z){
	x = 0; y = 0; z = 0;
#ifdef LIBMORTON_USE_INTRINSICS
	// For Microsoft compilers use _BitScanForward & _BitScanReverse.
	// For GCC use __builtin_ffs, __builtin_clz, __builtin_ctz.

	// use bit manipulation intrinsic to find out first bit, for early termination
	unsigned long firstbit_location;
#if _MSC_VER
	// are the casts necessary? and the blanking in the second one?
	// Does the pragma stop the compiler from optimizing this?
	// is it cheaper to do this using the 64 bit one
	if (_BitScanReverse(&firstbit_location, (morton >> 32))){ // check first part of morton code
		firstbit_location += 32;
	} else if ( ! _BitScanReverse(&firstbit_location, (morton & 0xFFFFFFFF))){ // also test last part of morton code
		return;
	}
#endif
	x = x | decode_morton512_x[morton & 0x1ff];
	y = y | decode_morton512_y[morton & 0x1ff];
	z = z | decode_morton512_z[morton & 0x1ff];
	if (firstbit_location < 9) return;
	x = x | (decode_morton512_x[((morton >> 9) & 0x1ff)] << 3);
	y = y | (decode_morton512_y[((morton >> 9) & 0x1ff)] << 3);
	z = z | (decode_morton512_z[((morton >> 9) & 0x1ff)] << 3);
	if (firstbit_location < 18) return;
	x = x | (decode_morton512_x[((morton >> 18) & 0x1ff)] << 6);
	y = y | (decode_morton512_y[((morton >> 18) & 0x1ff)] << 6);
	z = z | (decode_morton512_z[((morton >> 18) & 0x1ff)] << 6);
	if (firstbit_location < 27) return;
	x = x | (decode_morton512_x[((morton >> 27) & 0x1ff)] << 9);
	y = y | (decode_morton512_y[((morton >> 27) & 0x1ff)] << 9);
	z = z | (decode_morton512_z[((morton >> 27) & 0x1ff)] << 9);
	if (firstbit_location < 36) return;
	x = x | (decode_morton512_x[((morton >> 36) & 0x1ff)] << 12);
	y = y | (decode_morton512_y[((morton >> 36) & 0x1ff)] << 12);
	z = z | (decode_morton512_z[((morton >> 36) & 0x1ff)] << 12);
	if (firstbit_location < 46) return;
	x = x | (decode_morton512_x[((morton >> 46) & 0x1ff)] << 15);
	y = y | (decode_morton512_y[((morton >> 46) & 0x1ff)] << 15);
	z = z | (decode_morton512_z[((morton >> 46) & 0x1ff)] << 15);
	if (firstbit_location < 54) return;
	x = x | (decode_morton512_x[((morton >> 54) & 0x1ff)] << 18);
	y = y | (decode_morton512_y[((morton >> 54) & 0x1ff)] << 18);
	z = z | (decode_morton512_z[((morton >> 54) & 0x1ff)] << 18);
	return;
#else
	// standard portable version
	x = 0 | decode_morton512_x[morton & 0x1ff]
		| (decode_morton512_x[((morton >> 9) & 0x1ff)] << 3)
		| (decode_morton512_x[((morton >> 18) & 0x1ff)] << 6)
		| (decode_morton512_x[((morton >> 27) & 0x1ff)] << 9)
		| (decode_morton512_x[((morton >> 36) & 0x1ff)] << 12)
		| (decode_morton512_x[((morton >> 46) & 0x1ff)] << 15)
		| (decode_morton512_x[((morton >> 54) & 0x1ff)] << 18);
	y = 0 | decode_morton512_y[morton & 0x1ff]
		| (decode_morton512_y[((morton >> 9) & 0x1ff)] << 3)
		| (decode_morton512_y[((morton >> 18) & 0x1ff)] << 6)
		| (decode_morton512_y[((morton >> 27) & 0x1ff)] << 9)
		| (decode_morton512_y[((morton >> 36) & 0x1ff)] << 12)
		| (decode_morton512_y[((morton >> 46) & 0x1ff)] << 15)
		| (decode_morton512_y[((morton >> 54) & 0x1ff)] << 18);
	z = 0 | decode_morton512_z[morton & 0x1ff]
		| (decode_morton512_z[((morton >> 9) & 0x1ff)] << 3)
		| (decode_morton512_z[((morton >> 18) & 0x1ff)] << 6)
		| (decode_morton512_z[((morton >> 27) & 0x1ff)] << 9)
		| (decode_morton512_z[((morton >> 36) & 0x1ff)] << 12)
		| (decode_morton512_z[((morton >> 46) & 0x1ff)] << 15)
		| (decode_morton512_z[((morton >> 54) & 0x1ff)] << 18);
#endif
}

inline uint32_t morton3D_Decode_X_LUT(const uint64_t morton){
	return 0 | decode_morton512_x[morton & 0x1ff]
		| (decode_morton512_x[((morton >> 9) & 0x1ff)] << 3)
		| (decode_morton512_x[((morton >> 18) & 0x1ff)] << 6)
		| (decode_morton512_x[((morton >> 27) & 0x1ff)] << 9)
		| (decode_morton512_x[((morton >> 36) & 0x1ff)] << 12)
		| (decode_morton512_x[((morton >> 46) & 0x1ff)] << 15)
		| (decode_morton512_x[((morton >> 54) & 0x1ff)] << 18);
}

inline uint32_t morton3D_Decode_Y_LUT(const uint64_t morton){
	return 0 | decode_morton512_y[morton & 0x1ff]
		| (decode_morton512_y[((morton >> 9) & 0x1ff)] << 3)
		| (decode_morton512_y[((morton >> 18) & 0x1ff)] << 6)
		| (decode_morton512_y[((morton >> 27) & 0x1ff)] << 9)
		| (decode_morton512_y[((morton >> 36) & 0x1ff)] << 12)
		| (decode_morton512_y[((morton >> 46) & 0x1ff)] << 15)
		| (decode_morton512_y[((morton >> 54) & 0x1ff)] << 18);
}

inline uint32_t morton3D_Decode_Z_LUT(const uint64_t morton){
	return 0 | decode_morton512_z[morton & 0x1ff]
		| (decode_morton512_z[((morton >> 9) & 0x1ff)] << 3)
		| (decode_morton512_z[((morton >> 18) & 0x1ff)] << 6)
		| (decode_morton512_z[((morton >> 27) & 0x1ff)] << 9)
		| (decode_morton512_z[((morton >> 36) & 0x1ff)] << 12)
		| (decode_morton512_z[((morton >> 46) & 0x1ff)] << 15)
		| (decode_morton512_z[((morton >> 54) & 0x1ff)] << 18);
}

// define default methods
inline uint64_t morton3D_Encode(const uint32_t x, const uint32_t y, const uint32_t z){
	return morton3D_Encode_LUT(x, y, z);
}

inline uint64_t morton3D_Decode(const uint64_t morton, uint32_t& x, uint32_t& y, uint32_t& z){
	morton3D_Decode_LUT(morton, x, y, z);
}

inline uint32_t morton3D_Decode_X(const uint64_t morton){
	return morton3D_Decode_X_LUT(morton);
}

inline uint32_t morton3D_Decode_Y(const uint64_t morton){
	return morton3D_Decode_Y_LUT(morton);
}

inline uint32_t morton3D_Decode_Z(const uint64_t morton){
	return morton3D_Decode_Z_LUT(morton);
}

#endif // MORTON_H_