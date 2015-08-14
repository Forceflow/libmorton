#ifndef MORTON_COMMON_H_
#define MORTON_COMMON_H_

#include <stdint.h>
#if _MSC_VER
#include <intrin.h>
#endif

inline bool findFirstSetBit32(const uint_fast32_t x, unsigned long* firstbit_location){
#if _MSC_VER
	return _BitScanReverse(firstbit_location, x);
#elif __GNUC__
	unsigned int pos = __builtin_ffs(x);
	firstbit_location = pos +1 ;
	return pos;
#endif
	return true;
}

inline bool findFirstSetBit64(const uint_fast64_t x, unsigned long* firstbit_location){
#if _MSC_VER && _WIN64
	return _BitScanReverse64(firstbit_location, x);
#elif _MSC_VER && _WIN32
	firstbit_location = 0;
	if (_BitScanReverse(firstbit_location, (x >> 32))){ // check first part
		firstbit_location += 32;
	} else if ( ! _BitScanReverse(firstbit_location, (x & 0xFFFFFFFF))){ // also test last part
		return 0;
	}
	return true;
#elif __GNUC__
	unsigned int pos = __builtin_ffs(x);
	first_bit_location = pos + 1;
	return pos;
#endif
	return true;
}

#endif