// Libmorton - Methods to encode/decode 64-bit morton codes from/to 32-bit (x,y,z) coordinates
#ifndef MORTON3D_32_H_
#define MORTON3D_32_H_

#include <stdint.h>
#include "morton3D_lookup_tables.h"

#if _MSC_VER
#include <intrin.h>
#endif

using namespace std;

// #define LIBMORTON_EARLY_TERMINATION

inline uint_fast32_t morton3D_32_Encode_LUT_shifted(const uint_fast16_t x, const uint_fast16_t y, const uint_fast16_t z){
	uint_fast32_t answer =
		Morton3D_64_encode_z_256[(z >> 8) & 0x000000FF] |
		Morton3D_64_encode_y_256[(y >> 8) & 0x000000FF] |
		Morton3D_64_encode_x_256[(x >> 8) & 0x000000FF];
	answer = answer << 24 |
		Morton3D_64_encode_z_256[z & 0x000000FF] |
		Morton3D_64_encode_y_256[y & 0x000000FF] |
		Morton3D_64_encode_x_256[x & 0x000000FF];
	return answer;
}

inline uint_fast32_t morton3D_32_Encode_LUT(const uint_fast16_t x, const uint_fast16_t y, const uint_fast16_t z){
	uint_fast32_t answer =
		 (Morton3D_64_encode_x_256[(z >> 8) & 0x000000FF] << 2) 
		|(Morton3D_64_encode_x_256[(y >> 8) & 0x000000FF] << 1)
		| Morton3D_64_encode_x_256[(x >> 8) & 0x000000FF];
	answer = answer << 24 |
		 (Morton3D_64_encode_z_256[z & 0x000000FF] << 2) 
		|(Morton3D_64_encode_y_256[y & 0x000000FF] << 1)
		| Morton3D_64_encode_x_256[x & 0x000000FF];
	return answer;
}

inline uint_fast32_t morton3D_32_Encode_for(const uint_fast16_t x, const uint_fast16_t y, const uint_fast16_t z){
	uint_fast32_t answer = 0;
	for (uint_fast32_t i = 0; i <= 10; ++i) {
		answer |= ((x & (0x1 << i)) << 2 * i)
			| ((y & (0x1 << i)) << ((2 * i) + 1))
			| ((z & (0x1 << i)) << ((2 * i) + 2));
	}
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

#endif // MORTON3D_32_H_
