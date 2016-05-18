#pragma once

#include <stdint.h>
#if _MSC_VER
#include <intrin.h>
#endif

template<typename morton>
inline bool findFirstSetBit(const morton x, unsigned long* firstbit_location) {
#if _MSC_VER && !_WIN64
	// 32 BIT on 32 BIT
	if(sizeof(morton) <= 4) { 
		return _BitScanReverse(firstbit_location, x);
	}
	// 64 BIT on 32 BIT
	else {
		*firstbit_location = 0;
		if (_BitScanReverse(firstbit_location, (x >> 32))) { // check first part
			firstbit_location += 32;
			return true;
		}
		return _BitScanReverse(firstbit_location, (x & 0xFFFFFFFF));
	}
#elif  _MSC_VER && _WIN64
	// 32 or 64 BIT on 64 BIT
	return _BitScanReverse64(firstbit_location, x);
#elif __GNUC__
	unsigned int pos = __builtin_ffs(x);
	*firstbit_location = static_cast<unsigned long>(pos);
	return pos;
#endif
}
