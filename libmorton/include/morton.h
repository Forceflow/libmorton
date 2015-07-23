// Fastest methods of computing morton codes
// Check http://www.forceflow.be/2013/10/07/morton-encodingdecoding-through-bit-interleaving-implementations/ for more info
// by Jeroen Baert - jeroen.baert@cs.kuleuven.be - www.forceflow.be

#ifndef MORTON_H_
#define MORTON_H_

#include <stdint.h>
#include <limits.h>
#include "morton_lookup_tables.h"

using namespace std;

// encode a given (x,y,z) coordinate to a 64-bit morton code
inline uint64_t mortonEncode(unsigned int x, unsigned int y, unsigned int z); 
// decode a given morton code into X,Y,Z coordinates
inline uint64_t mortonDecode(uint64_t morton, unsigned int& x, unsigned int& y, unsigned int& z);
inline unsigned int mortonDecode_X(uint64_t morton);
inline unsigned int mortonDecode_Y(uint64_t morton);
inline unsigned int mortonDecode_Z(uint64_t morton);

// encoding with lookup table
inline uint64_t mortonEncode_LUT(unsigned int x, unsigned int y, unsigned int z){
	uint64_t answer = 0;
	answer =	morton256_z[(z >> 16) & 0xFF ] |
				morton256_y[(y >> 16) & 0xFF ] |
				morton256_x[(x >> 16) & 0xFF ];
	answer = answer << 48 |
				morton256_z[(z >> 8) & 0xFF ] |
				morton256_y[(y >> 8) & 0xFF ] |
				morton256_x[(x >> 8) & 0xFF ];
	answer = answer << 24 |
				morton256_z[(z) & 0xFF ] |
				morton256_y[(y) & 0xFF ] |
				morton256_x[(x) & 0xFF ];
	return answer;
}

// tables for decoding

// Decoding with magic bits
inline unsigned int getThirdBits(uint64_t x){
	x &= 0x9249249249249249;
	x = (x ^ (x >> 2)) & 0x030c30c3030c30c3;
	x = (x ^ (x >> 4)) & 0xF00F00F00F00F00F;
	x = (x ^ (x >> 8)) & 0xFF0000FF0000FF;
	x = (x ^ (x >> 16)) & 0xFFFF;
	return (unsigned int) x;
}

inline unsigned int mortonDecode_magicbits_X(uint64_t morton){
	return getThirdBits(morton);
}

inline unsigned int mortonDecode_magicbits_Y(uint64_t morton){
	return getThirdBits(morton >> 1);
}

inline unsigned int mortonDecode_magicbits_Z(uint64_t morton){
	return getThirdBits(morton >> 2);
}

inline void mortonDecode_magicbits(uint64_t morton, unsigned int& x, unsigned int& y, unsigned int& z){
	x = mortonDecode_magicbits_X(morton);
	y = mortonDecode_magicbits_Y(morton);
	z = mortonDecode_magicbits_Z(morton);
}

// define default methods
inline uint64_t mortonEncode(unsigned int x, unsigned int y, unsigned int z){
	return mortonEncode_LUT(x, y, z);
}

inline uint64_t mortonDecode(uint64_t morton, unsigned int& x, unsigned int& y, unsigned int& z){
	mortonDecode_magicbits(morton, x, y, z);
}

#endif // MORTON_H_