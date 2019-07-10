#pragma once
#if defined(__AVX512BITALG__)
#include <immintrin.h>
#include <stdint.h>

namespace libmorton {

	namespace bitalg_detail {
	}  // namespace bitalg_detail

	template<typename morton, typename coord>
	inline morton m2D_e_BITALG(const coord x, const coord y) {
		return morton(0);
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
