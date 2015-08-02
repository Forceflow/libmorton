// Libmorton - Methods to encode/decode 64-bit morton codes from/to 32-bit (x,y,z) coordinates
#ifndef MORTON2D_H_
#define MORTON2D_H_

#include <stdint.h>

#if _MSC_VER
#include <intrin.h>
#endif

// 32-bit morton code for loop
inline uint_fast32_t morton2D_32_Encode_for(const uint16_t x, const uint16_t y){
	uint_fast32_t answer = 0;
	for (uint_fast64_t i = 0; i < 16; ++i) {
		answer |=((x & ((0x1 << i)) << (2*i))
			   | ((y & ((0x1 << i)) << ((2*i) + 1));
	}
	return answer;
}

// 64-bit morton code for loop
inline uint_fast64_t morton2D_64_Encode_for(const uint_fast32_t x, const uint_fast32_t y){
	uint_fast64_t answer = 0;
	for (uint_fast64_t i = 0; i < 32; ++i) {
		answer |=((x & ((0x1 << i)) << (2*i)) 
			   | ((y & ((0x1 << i)) << ((2*i) + 1));
	}
	return answer;
}


#endif // MORTON2D_H_