#pragma once
#include "libmorton_test.h"

template <typename morton, typename coord>
inline void check3D_EncodeCorrectness(std::vector<encode_f_3D_wrapper<morton, coord>> encoders) {
	unsigned int bit = sizeof(morton) * 8;
	printf("++ Checking correctness of 3D encoders (%u bit) methods ... ", bit);
	bool ok = true;
	for (typename std::vector<encode_f_3D_wrapper<morton, coord>>::iterator it = encoders.begin(); it != encoders.end(); it++) {
		ok &= check3D_EncodeFunction(*it);
	}
	if (ok) { printf(" Passed. \n"); }
	else { printf("    One or more methods failed. \n"); }
}

template <typename morton, typename coord>
inline void check3D_DecodeCorrectness(std::vector<decode_f_3D_wrapper<morton, coord>> decoders) {
	unsigned int bit = sizeof(morton) * 8;
	printf("++ Checking correctness of 3D decoding (%u bit) methods ... ", bit);
	bool ok = true;
	for (typename std::vector<decode_f_3D_wrapper<morton, coord>>::iterator it = decoders.begin(); it != decoders.end(); it++) {
		ok &= check3D_DecodeFunction(*it);
	}
	if (ok) { printf(" Passed. \n"); }
	else { printf("    One or more methods failed. \n"); }
}

template <typename morton, typename coord>
inline void check3D_EncodeDecodeMatch(std::vector<encode_f_3D_wrapper<morton, coord>> encoders, std::vector<decode_f_3D_wrapper<morton, coord>> decoders, unsigned int times) {
	unsigned int bit = sizeof(morton) * 8;
	printf("++ Checking 3D methods (%u bit) encode/decode match ... ", bit);
	bool ok = true;
	for (typename std::vector<encode_f_3D_wrapper<morton, coord>>::iterator et = encoders.begin(); et != encoders.end(); et++) {
		for (typename std::vector<decode_f_3D_wrapper<morton, coord>>::iterator dt = decoders.begin(); dt != decoders.end(); dt++) {
			ok &= check3D_Match(*et, *dt, times);
		}
	}
	if (ok) { printf(" Passed. \n"); }
	else { printf("    One or more methods failed. \n"); }
}
