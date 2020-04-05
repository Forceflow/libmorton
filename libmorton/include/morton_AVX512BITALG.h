#pragma once
#include <immintrin.h>
#include <stdint.h>

namespace libmorton {

	namespace bitalg_detail {
		// "Zip" and interleave an m-vector of n-bit integers into a
		// new n*m-bit integer
		// 2D MORTONS
		inline uint32_t bitzip2D(uint32_t a, uint32_t b) noexcept {
			// Put both 32-bit integer into each 64-bit lane
			const __m256i CoordVec = _mm256_set1_epi64x(
				(static_cast<uint64_t>(b) << 32u) | a
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

		inline uint64_t bitzip2D(uint64_t a, uint64_t b) noexcept {
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
		// 3D MORTONS
		inline uint32_t bitzip3D(uint32_t a, uint32_t b, uint32_t c) noexcept {
			const __m256i CoordVec = _mm256_broadcastsi128_si256(
				_mm_set_epi32(0, c, b, a)
			);
			const __m256i ShuffleVec = _mm256_permutexvar_epi8(
				_mm256_set_epi64x(
					0xFFFFFFFFFF100800ul + 0x010101 * 1, // Lane 3 | ...000 | z[1] | y[1] | x[1]
					0xFFFFFFFFFF100800ul + 0x010101 * 0, // Lane 2 | ...000 | z[0] | y[0] | x[0]
					0xFFFFFFFFFF100800ul + 0x010101 * 0, // Lane 1 | ...000 | z[0] | y[0] | x[0]
					0xFFFFFFFFFF100800ul + 0x010101 * 0  // Lane 0 | ...000 | z[0] | y[0] | x[0]
				),
				CoordVec
			);
			const __mmask32 Interleave = _mm256_bitshuffle_epi64_mask(
				ShuffleVec,
				_mm256_set_epi64x(
					0x0202010101000000 + 0x0100020100020100 * 8,
					0x0707070606060505 + 0x0201000201000201 * 8,
					0x0504040403030302 + 0x0002010002010002 * 8,
					0x0202010101000000 + 0x0100020100020100 * 8 
				)
			);
			return _cvtmask32_u32(Interleave);
		}
		inline uint64_t bitzip3D(uint64_t a, uint64_t b, uint64_t c) noexcept {
			// Put both 32-bit integers into each 64-bit lane
			// Todo: _mm512_shuffle_epi8 version, 128-bit lane woes
			const __m512i CoordVec = _mm512_set_epi64(
				0, 0, 0, 0, 0, c, b, a
			);
			const __m512i ShuffleVec = _mm512_permutexvar_epi8(
				_mm512_set_epi64(
					0xFFFFFFFFFF100800ul + 0x010101 * 2, // Lane 7 | ...000 | z[2] | y[2] | x[2]
					0xFFFFFFFFFF100800ul + 0x010101 * 2, // Lane 6 | ...000 | z[2] | y[2] | x[2]
					0xFFFFFFFFFF100800ul + 0x010101 * 1, // Lane 5 | ...000 | z[1] | y[1] | x[1]
					0xFFFFFFFFFF100800ul + 0x010101 * 1, // Lane 4 | ...000 | z[1] | y[1] | x[1]
					0xFFFFFFFFFF100800ul + 0x010101 * 1, // Lane 3 | ...000 | z[1] | y[1] | x[1]
					0xFFFFFFFFFF100800ul + 0x010101 * 0, // Lane 2 | ...000 | z[0] | y[0] | x[0]
					0xFFFFFFFFFF100800ul + 0x010101 * 0, // Lane 1 | ...000 | z[0] | y[0] | x[0]
					0xFFFFFFFFFF100800ul + 0x010101 * 0  // Lane 0 | ...000 | z[0] | y[0] | x[0]
				),
				CoordVec
			);
			// Interleave bits from 32-bit X and Y and Z coordinate
			const __mmask64 Interleave = _mm512_bitshuffle_epi64_mask(
				ShuffleVec,
				_mm512_set_epi64(
					0x0504040403030302 + 0x0002010002010002 * 8,
					0x0202010101000000 + 0x0100020100020100 * 8,
					0x0707070606060505 + 0x0201000201000201 * 8,
					0x0504040403030302 + 0x0002010002010002 * 8,
					0x0202010101000000 + 0x0100020100020100 * 8,
					0x0707070606060505 + 0x0201000201000201 * 8,
					0x0504040403030302 + 0x0002010002010002 * 8,
					0x0202010101000000 + 0x0100020100020100 * 8 
				)
			);
			return _cvtmask64_u64(Interleave);
		}
	}  // namespace bitalg_detail

	template<typename morton, typename coord>
	inline morton m2D_e_BITALG(const coord x, const coord y) {
		return bitalg_detail::bitzip2D(
			static_cast<coord>(x), static_cast<coord>(y)
		);
	}

	template<typename morton, typename coord>
	inline void m2D_d_BITALG(const morton m, coord& x, coord& y) {
		// Placeholder BMI implementation for verification
		#define BMI_2D_X_MASK 0x5555555555555555
		#define BMI_2D_Y_MASK 0xAAAAAAAAAAAAAAAA
		x = static_cast<coord>(bmi2_detail::pext(m, static_cast<morton>(BMI_3D_X_MASK)));
		y = static_cast<coord>(bmi2_detail::pext(m, static_cast<morton>(BMI_3D_Y_MASK)));
	}

	template<typename morton, typename coord>
	inline morton m3D_e_BITALG(const coord x, const coord y, const coord z) {
		return bitalg_detail::bitzip3D(
			static_cast<coord>(x), static_cast<coord>(y),  static_cast<coord>(z)
		);
	}

	template<typename morton, typename coord>
	inline void m3D_d_BITALG(const morton m, coord& x, coord& y, coord& z) {
		// Placeholder BMI implementation for verification
		#define BMI_3D_X_MASK 0x9249249249249249
		#define BMI_3D_Y_MASK 0x2492492492492492
		#define BMI_3D_Z_MASK 0x4924924924924924
		x = static_cast<coord>(bmi2_detail::pext(m, static_cast<morton>(BMI_3D_X_MASK)));
		y = static_cast<coord>(bmi2_detail::pext(m, static_cast<morton>(BMI_3D_Y_MASK)));
		z = static_cast<coord>(bmi2_detail::pext(m, static_cast<morton>(BMI_3D_Z_MASK)));
	}
}