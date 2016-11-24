#pragma once
#if defined(__BMI2__)
#include <immintrin.h>
#include <stdint.h>

namespace bmi2_detail {
	inline uint32_t pdep(uint32_t source, uint32_t mask) noexcept {
		return _pdep_u32(source, mask);
	}
	inline uint64_t pdep(uint64_t source, uint64_t mask) noexcept {
		return _pdep_u64(source, mask);
	}
	inline uint32_t pext(uint32_t source, uint32_t mask) noexcept {
		return _pext_u32(source, mask);
	}
	inline uint64_t pext(uint64_t source, uint64_t mask) noexcept {
		return _pext_u64(source, mask);
	}
}  // namespace bmi2_detail

#define BMI_X_MASK 0x9249249249249249
#define BMI_Y_MASK 0x2492492492492492
#define BMI_Z_MASK 0x4924924924924924

template<typename morton, typename coord> 
inline morton m3D_e_BMI(const coord x, const coord y, const coord z){
	morton m = 0;
	m |=  bmi2_detail::pdep(static_cast<morton>(x), static_cast<morton>(BMI_X_MASK))
		| bmi2_detail::pdep(static_cast<morton>(y), static_cast<morton>(BMI_Y_MASK))
		| bmi2_detail::pdep(static_cast<morton>(z), static_cast<morton>(BMI_Z_MASK));
	return m;
}

template<typename morton, typename coord>
inline morton m3d_d_BMI(const morton m, coord& x, coord& y, coord& z) {
	x = static_cast<coord>(bmi2_detail::pext(m, static_cast<morton>(BMI_X_MASK)));
	y = static_cast<coord>(bmi2_detail::pext(m, static_cast<morton>(BMI_Y_MASK)));
	z = static_cast<coord>(bmi2_detail::pext(m, static_cast<morton>(BMI_Z_MASK)));
}
#endif
