// Libmorton - Methods to encode/decode 64-bit morton codes from/to 32-bit (x,y,z) coordinates
#ifndef MORTON2D_64_H_
#define MORTON2D_64_H_

#include <stdint.h>

#if _MSC_VER
#include <intrin.h>
#endif

// 64-bit morton code for loop
inline uint_fast64_t morton2D_64_Encode_for(const uint_fast32_t x, const uint_fast32_t y){
	uint_fast64_t answer = 0;
	for (uint_fast64_t i = 0; i < 32; ++i) {
		answer |= (x & (0x1 << i)) << (2 * i)
			   | (y & (0x1 << i)) << ((2 * i) + 1);
	}
	return answer;
}

inline uint64_t morton2D_64_splitby2(const uint32_t a){
	uint64_t x = a;
	x = (x | x << 32) & 0x00000000FFFFFFFF;
	x = (x | x << 16) & 0x0000FFFF0000FFFF;
	x = (x | x << 8)  & 0x00FF00FF00FF00FF;
	x = (x | x << 4)  & 0x0F0F0F0F0F0F0F0F;
	x = (x | x << 2)  & 0x3333333333333333;
	x = (x | x << 1)  & 0x5555555555555555;
	return x;
}

inline uint64_t morton2D_64_Encode_magicbits(const uint32_t x, const uint32_t y){
	return 0 | morton2D_64_splitby2(x) | morton2D_64_splitby2(y) << 1;
}

#endif // MORTON2D_64_H_