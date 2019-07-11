#pragma once
#if defined(__AVX512BITALG__)
#include <immintrin.h>
#include <stdint.h>

namespace libmorton {

	namespace bitalg_detail {
		// "Zip" and interleave an m-vector of n-bit integers into a
		// new n*m-bit integer

		// zip two 16-bit coords into a 32-bit morton
		inline uint32_t bitzip(uint16_t a, uint16_t b) noexcept {
			// Put both 16-bit integers into each 32-bit lane
			const __m256i CoordVec = _mm256_set1_epi32(
				(static_cast<uint32_t>(b) << 16u) | a
			);
			// Interleave bits from 32-bit X and Y coordinate
			const __mmask32 Interleave = _mm256_bitshuffle_epi64_mask(
				CoordVec,
				_mm256_set_epi16(
					0x1000 + 0x0101 * 15, 0x1000 + 0x0101 * 14,
					0x1000 + 0x0101 * 13, 0x1000 + 0x0101 * 12,
					0x1000 + 0x0101 * 11, 0x1000 + 0x0101 * 10,
					0x1000 + 0x0101 *  9, 0x1000 + 0x0101 *  8,
					0x1000 + 0x0101 *  7, 0x1000 + 0x0101 *  6,
					0x1000 + 0x0101 *  5, 0x1000 + 0x0101 *  4,
					0x1000 + 0x0101 *  3, 0x1000 + 0x0101 *  2,
					0x1000 + 0x0101 *  1, 0x1000 + 0x0101 *  0
				)
			);
			return _cvtmask32_u32(Interleave);
		}
		
		// zip two 32-bit coords into a 64-bit morton
		inline uint64_t bitzip(uint32_t a, uint32_t b) noexcept {
			// Put both 32-bit integers into each 64-bit lane
			const __m512i CoordVec = _mm512_set1_epi64(
				(static_cast<uint64_t>(b) << 32u) | a
			);
			// Interleave bits from 32-bit X and Y coordinate
			const __mmask64 Interleave = _mm512_bitshuffle_epi64_mask(
				CoordVec,
				_mm512_set_epi16(
					0x2000 + 0x0101 * 31, 0x2000 + 0x0101 * 30,
					0x2000 + 0x0101 * 29, 0x2000 + 0x0101 * 28,
					0x2000 + 0x0101 * 27, 0x2000 + 0x0101 * 26,
					0x2000 + 0x0101 * 25, 0x2000 + 0x0101 * 24,
					0x2000 + 0x0101 * 23, 0x2000 + 0x0101 * 22,
					0x2000 + 0x0101 * 21, 0x2000 + 0x0101 * 20,
					0x2000 + 0x0101 * 19, 0x2000 + 0x0101 * 18,
					0x2000 + 0x0101 * 17, 0x2000 + 0x0101 * 16,
					0x2000 + 0x0101 * 15, 0x2000 + 0x0101 * 14,
					0x2000 + 0x0101 * 13, 0x2000 + 0x0101 * 12,
					0x2000 + 0x0101 * 11, 0x2000 + 0x0101 * 10,
					0x2000 + 0x0101 *  9, 0x2000 + 0x0101 *  8,
					0x2000 + 0x0101 *  7, 0x2000 + 0x0101 *  6,
					0x2000 + 0x0101 *  5, 0x2000 + 0x0101 *  4,
					0x2000 + 0x0101 *  3, 0x2000 + 0x0101 *  2,
					0x2000 + 0x0101 *  1, 0x2000 + 0x0101 *  0
				)
			);
			return _cvtmask64_u64(Interleave);
		}
	}  // namespace bitalg_detail

	template<typename morton, typename coord>
	inline morton m2D_e_BITALG(const coord x, const coord y) {
		return bitalg_detail::bitzip(
			static_cast<coord>(x),
			static_cast<coord>(y)
		);
	}

	template<typename morton, typename coord>
	inline void m2D_d_BITALG(const morton m, coord& x, coord& y) {
	}

	template<typename morton, typename coord>
	inline morton m3D_e_BITALG(const coord x, const coord y, const coord z) {
		return morton(0);
	}

	template<typename morton, typename coord>
	inline void m3D_d_BITALG(const morton m, coord& x, coord& y, coord& z) {
	}
}
#endif
