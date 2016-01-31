// Libmorton - Methods to encode/decode 64-bit morton codes from/to 32-bit (x,y,z) coordinates
#ifndef MORTON2D_32_H_
#define MORTON2D_32_H_

#include <stdint.h>
#include "morton2D_LUTs.h"

#if _MSC_VER
#include <intrin.h>
#endif

// ENCODE 2D 64-bit morton code: LUT preshifted
inline uint_fast32_t morton2D_32_Encode_LUT256_shifted(const uint_fast16_t x, const uint_fast16_t y) {
	uint_fast32_t answer =
		Morton2D_64_encode_y_256[(y >> 8) & 0x00FF] | // select first 8 bits
		Morton2D_64_encode_x_256[(x >> 8) & 0x00FF];
	answer = answer << 16 | // shift by 16 = 2 * 8bits
		Morton2D_64_encode_y_256[(y) & 0x00FF] | // select next 8 bits
		Morton2D_64_encode_x_256[(x) & 0x00FF];
	return answer;
}

// ENCODE 2D 64-bit morton code: LUT preshifted
inline uint_fast64_t morton2D_32_Encode_LUT256(const uint_fast16_t x, const uint_fast16_t y) {
	uint_fast64_t answer = 
		(Morton2D_64_encode_x_256[(y >> 8) & 0x00FF]) << 1 | // select next 8 bits
		Morton2D_64_encode_x_256[(x >> 8) & 0x00FF];
	answer = answer << 16 | // shift by 16 = 2 * 8bits
		(Morton2D_64_encode_x_256[(y)& 0x00FF]) << 1 | // select next 8 bits
		Morton2D_64_encode_x_256[(x)& 0x00FF];
	return answer;
}

#endif // MORTON2D_32_H_