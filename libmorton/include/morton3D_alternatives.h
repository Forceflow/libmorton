// Alternate methods of computing morton codes
// Check http://www.forceflow.be/2013/10/07/morton-encodingdecoding-through-bit-interleaving-implementations/ for more info
// by Jeroen Baert - jeroen.baert@cs.kuleuven.be - www.forceflow.be

#ifndef MORTON_ALTERNATIVES_H_
#define MORTON_ALTERNATIVES_H_

#include <stdint.h>
#include <limits.h>

inline uint32_t morton3D_32_Encode_for(const uint16_t x, const uint16_t y, const uint16_t z){
	uint32_t answer = 0;
	for (uint32_t i = 0; i < 10; ++i) {
		answer |=((x & (0x1 << i)) << 2 * i)
			   | ((y & (0x1 << i)) << ((2 * i) + 1))
			   | ((z & (0x1 << i)) << ((2 * i) + 2));
	}
	return answer;
}

inline uint64_t morton3D_64_Encode_for(const uint32_t x, const uint32_t y, const uint32_t z){
	uint64_t answer = 0;
	for (uint64_t i = 0; i < 21; ++i) {
		answer |=((x & (0x1 << i)) << 2 * i) 
			   | ((y & (0x1 << i)) << ((2*i) + 1)) 
			   | ((z & (0x1 << i)) << ((2*i) + 2));
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
	return (uint32_t) x;
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
		x |= (morton & (1ull << 3*i)) >> ((2*i));
		y |= (morton & (1ull << ((3*i)+1))) >> ((2*i)+1);
		z |= (morton & (1ull << ((3*i)+2))) >> ((2*i)+2);
	}
}

#endif // MORTON_ALTERNATIVES_H_