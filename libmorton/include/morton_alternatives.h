// Alternate methods of computing morton codes
// Check http://www.forceflow.be/2013/10/07/morton-encodingdecoding-through-bit-interleaving-implementations/ for more info
// by Jeroen Baert - jeroen.baert@cs.kuleuven.be - www.forceflow.be

#ifndef MORTON_ALTERNATIVES_H_
#define MORTON_ALTERNATIVES_H_

#include <stdint.h>
#include <limits.h>

inline uint64_t mortonEncode_for(unsigned int x, unsigned int y, unsigned int z); // slowest
inline uint64_t mortonEncode_magicbits(unsigned int x, unsigned int y, unsigned int z); // faster
inline void mortonDecode_for(uint64_t morton, unsigned int& x, unsigned int& y, unsigned int& z); // slowest

// Version with for loop
inline uint64_t mortonEncode_for(unsigned int x, unsigned int y, unsigned int z){
	uint64_t answer = 0;
	for (uint64_t i = 0; i < (sizeof(uint64_t)* CHAR_BIT) / 3; ++i) {
		answer |= ((x & ((uint64_t)1 << i)) << 2 * i) | ((y & ((uint64_t)1 << i)) << (2 * i + 1)) | ((z & ((uint64_t)1 << i)) << (2 * i + 2));
	}
	return answer;
}

// Version with magic bits
inline uint64_t splitBy3(int a){
	uint64_t x = a & 0x1fffff;
	x = (x | x << 32) & 0x1f00000000ffff;
	x = (x | x << 16) & 0x1f0000ff0000ff;
	x = (x | x << 8) & 0x100f00f00f00f00f;
	x = (x | x << 4) & 0x10c30c30c30c30c3;
	x = (x | x << 2) & 0x1249249249249249;
	return x;
}

inline uint64_t mortonEncode_magicbits(unsigned int x, unsigned int y, unsigned int z){
	uint64_t answer = 0;
	answer |= splitBy3(x) | splitBy3(y) << 1 | splitBy3(z) << 2;
	return answer;
}

// Decoding with for loop
inline void mortonDecode_for(uint64_t morton, unsigned int& x, unsigned int& y, unsigned int& z){
	x = 0;
	y = 0;
	z = 0;
	for (uint64_t i = 0; i < (sizeof(uint64_t) * CHAR_BIT) / 3; ++i) {
		x |= ((morton & (uint64_t(1ull) << uint64_t((3ull * i) + 0ull))) >> uint64_t(((3ull * i) + 0ull) - i));
		y |= ((morton & (uint64_t(1ull) << uint64_t((3ull * i) + 1ull))) >> uint64_t(((3ull * i) + 1ull) - i));
		z |= ((morton & (uint64_t(1ull) << uint64_t((3ull * i) + 2ull))) >> uint64_t(((3ull * i) + 2ull) - i));
	}
}

#endif // MORTON_ALTERNATIVES_H_