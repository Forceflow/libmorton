#pragma once

// Libmorton - Common helper methods needed in Morton encoding/decoding

#include <stdint.h>
#if _MSC_VER
#include <intrin.h>
#endif

namespace libmorton {

#if _MSC_VER && !_WIN64
	namespace detail {

		/// Implementation of findFirstSetBitZeroIdx for MSVC in Win32 environment
		template <typename Morton>
		struct first_set_bit_win32{};

		/// Template specialization for 32 bits morton codes
		template <>
		struct first_set_bit_win32<uint_fast32_t> {
			static bool find(const uint_fast32_t x, unsigned long* loc) {
				return _BitScanReverse(loc, x) != 0;
			}
		};

		/// Template specialization for 64 bits morton codes
		template <>
		struct first_set_bit_win32<uint_fast64_t> {
			static bool find(const uint_fast64_t x, unsigned long* loc) {
				*loc = 0;
				if (_BitScanReverse(loc, (x >> 32))) { // check first part
					*loc += 32;
					return true;
				}
				else {
					return _BitScanReverse(loc, (x & 0xFFFFFFFF)) != 0;
				}
			}
		};

	} // namespace detail
#endif

	template<typename morton>
	inline bool findFirstSetBitZeroIdx(const morton x, unsigned long* firstbit_location) {
#if _MSC_VER && !_WIN64
		return detail::first_set_bit_win32<morton>::find(x, firstbit_location);
#elif  _MSC_VER && _WIN64
		// 32 or 64 BIT on 64 BIT
		return _BitScanReverse64(firstbit_location, x) != 0;
#elif __GNUC__
		if (x == 0) {
			return false;
		}
		else {
			*firstbit_location = static_cast<unsigned long>((sizeof(morton) * 8) - __builtin_clzll(x) - 1);
			return true;
		}
#endif
	}

	template<typename morton>
	inline bool findFirstSetBit(const morton x, unsigned long* firstbit_location) {
		if (findFirstSetBitZeroIdx(x, firstbit_location)) {
			*firstbit_location += 1;
			return true;
		}
		return false;
	}
}