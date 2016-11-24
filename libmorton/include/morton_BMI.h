#pragma once

#include <stdint.h>

#if defined(__BMI2__)
#include <immintrin.h>

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

template<typename morton, typename coord> 
inline morton m3D_e_BMI(const coord x, const coord y, const coord z){
	morton m = 0;
	m |= bmi2_detail::pdep(static_cast<morton>(x), static_cast<morton>(0x9249249249249249))
	| bmi2_detail::pdep(static_cast<morton>(y), static_cast<morton>(0x2492492492492492))
	| bmi2_detail::pdep(static_cast<morton>(z), static_cast<morton>(0x4924924924924924));
	return m;
}
#endif
