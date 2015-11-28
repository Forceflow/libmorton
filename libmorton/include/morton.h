#ifndef MORTON_H_
#define MORTON_H_

// This file will always contain pointers to the fastest Morton encoding/decoding implementation

#include "morton2D_32.h"
#include "morton2D_64.h"
#include "morton3D_32.h"
#include "morton3D_64.h"

inline uint_fast32_t morton2D_32_encode(const uint_fast16_t x, const uint_fast16_t y);
inline uint_fast64_t morton2D_64_encode(const uint_fast32_t x, const uint_fast32_t y);
inline uint_fast32_t morton3D_32_encode(const uint_fast16_t x, const uint_fast16_t y, const uint_fast16_t z);
inline uint_fast64_t morton3D_64_encode(const uint_fast32_t x, const uint_fast32_t y, const uint_fast32_t z);

inline uint_fast32_t morton2D_32_decode(const uint_fast32_t morton, uint_fast16_t& x, uint_fast16_t& y);
inline uint_fast64_t morton2D_64_decode(const uint_fast64_t morton, uint_fast32_t& x, uint_fast32_t& y);
inline uint_fast32_t morton3D_32_decode(const uint_fast32_t morton, uint_fast16_t& x, uint_fast16_t& y, uint_fast16_t& z);
inline uint_fast64_t morton3D_64_decode(const uint_fast64_t morton, uint_fast32_t& x, uint_fast32_t& y, uint_fast32_t& z);

inline uint_fast32_t morton2D_32_encode(const uint_fast16_t x, const uint_fast16_t y) {
	return morton2D_32_Encode_LUT256_shifted(x, y);
}

inline uint_fast64_t morton2D_64_encode(const uint_fast32_t x, const uint_fast32_t y) {
	return morton2D_64_Encode_LUT256_shifted(x, y);
}

inline uint_fast32_t morton3D_32_encode(const uint_fast16_t x, const uint_fast16_t y, const uint_fast16_t z) {
	return morton3D_32_Encode_LUT256_shifted(x, y, z);
}

inline uint_fast64_t morton3D_64_encode(const uint_fast32_t x, const uint_fast32_t y, const uint_fast32_t z){
	return morton3D_64_Encode_LUT256_shifted(x, y, z);
}


inline uint_fast32_t morton2D_32_decode(const uint_fast32_t morton, uint_fast16_t& x, uint_fast16_t& y) {
	//TODO
}

inline uint_fast64_t morton2D_64_decode(const uint_fast64_t morton, uint_fast32_t& x, uint_fast32_t& y) {
	//TODO
}

inline uint_fast32_t morton3D_32_decode(const uint_fast32_t morton, uint_fast16_t& x, uint_fast16_t& y, uint_fast16_t& z) {
	morton3D_32_Decode_LUT256_shifted(morton, x, y, z);
}

inline uint_fast64_t morton3D_64_decode(const uint_fast64_t morton, uint_fast32_t& x, uint_fast32_t& y, uint_fast32_t& z) {
	morton3D_64_Decode_LUT256_shifted(morton, x, y, z);
}

#endif