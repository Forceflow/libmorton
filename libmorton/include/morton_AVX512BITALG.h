#pragma once
#if defined(__AVX512BITALG__)
#include <immintrin.h>
#include <stdint.h>

namespace libmorton {

	namespace bitalg_detail {
		// "Zip" and interleave an m-vector of n-bit integers into a
		// new n*m-bit integer
		// 2D MORTONS
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
		// 3D MORTONS
		// zip three 16-bit coords into a 32-bit morton
		inline uint32_t bitzip(uint16_t a, uint16_t b, uint16_t c) noexcept {
			// Put both 32-bit integers into each 64-bit lane
			const __m256i CoordVec = _mm256_set1_epi64x(
				static_cast<std::uint64_t>(a)
				| static_cast<std::uint64_t>(b << 16)
				| static_cast<std::uint64_t>(c << 32)
			);
			// Interleave bits from 16-bit X and Y and Z coordinate
			const __mmask32 Interleave = _mm256_bitshuffle_epi64_mask(
				CoordVec,
				_mm256_set_epi64x(
					0x1A'0A'29'19'09'28'18'08, // Byte 3
					0x27'17'07'26'16'06'25'15, // Byte 2
					0x05'24'14'04'23'13'03'22, // Byte 1
					0x12'02'21'11'01'20'10'00  // Byte 0
				)
			);
			return _cvtmask32_u32(Interleave));
		}
		// zip three 32-bit coords into a 64-bit morton
		inline uint64_t bitzip(uint32_t a, uint32_t b, uint32_t c) noexcept {
			// Put both 32-bit integers into each 64-bit lane
			const __m512i CoordVec = _mm512_broadcast_i32x4(
				_mm_set_epi32(0, c, b, a)
			);
			const __m512i ShuffleVec = _mm512_shuffle_epi8(
				CoordVec,
				_mm512_set_epi64(
					0x0A'06'02ul, // Lane 7 | z2 | y2 | x2
					0x0A'06'02ul, // Lane 6 | z2 | y2 | x2
					0x09'05'01ul, // Lane 5 | z1 | y1 | x1
					0x09'05'01ul, // Lane 4 | z1 | y1 | x1
					0x09'05'01ul, // Lane 3 | z1 | y1 | x1
					0x08'04'00ul, // Lane 2 | z0 | y0 | x0
					0x08'04'00ul, // Lane 1 | z0 | y0 | x0
					0x08'04'00ul  // Lane 0 | z0 | y0 | x0
				)
			);
			// Interleave bits from 32-bit X and Y and Z coordinate
			const __mmask64 Interleave = _mm512_bitshuffle_epi64_mask(
				ShuffleVec,
				_mm512_set_epi64(
					0x15'14'14'14'13'13'13'12, // Byte 7
					0x12'12'11'11'11'10'10'10, // Byte 6
					0x0F'0F'0F'0E'0E'0E'0D'0D, // Byte 5
					0x0D'0C'0C'0C'0B'0B'0B'0A, // Byte 4
					0x0A'0A'09'09'09'08'08'08, // Byte 3
					0x07'07'07'06'06'06'05'05, // Byte 2
					0x05'04'04'04'03'03'03'02, // Byte 1
					0x02'02'01'01'01'00'00'00  // Byte 0
				)
			);
			return _cvtmask64_u64(Interleave);
		}
	}  // namespace bitalg_detail

	template<typename morton, typename coord>
	inline morton m2D_e_BITALG(const coord x, const coord y) {
		return bitalg_detail::bitzip(
			static_cast<coord>(x), static_cast<coord>(y)
		);
	}

	template<typename morton, typename coord>
	inline void m2D_d_BITALG(const morton m, coord& x, coord& y) {
	}

	template<typename morton, typename coord>
	inline morton m3D_e_BITALG(const coord x, const coord y, const coord z) {
		return bitalg_detail::bitzip(
			static_cast<coord>(x), static_cast<coord>(y),  static_cast<coord>(z)
		);
	}

	template<typename morton, typename coord>
	inline void m3D_d_BITALG(const morton m, coord& x, coord& y, coord& z) {
	}
}
#endif
