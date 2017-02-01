#pragma once
#include "libmorton_test.h"

// Check a 3D Encode/Decode function for correct encode-decode process
template<typename morton, typename coord>
inline bool check3D_Match(const encode_f_3D_wrapper<morton, coord> &encode, decode_f_3D_wrapper<morton, coord> &decode, unsigned int times) {
	bool everythingokay = true;
	for (unsigned int i = 0; i < times; ++i) {
		coord maximum = pow(2, floor((sizeof(morton) * 8) / 3.0f)) - 1;
		// generate random coordinates
		coord x = rand() % maximum;
		coord y = rand() % maximum;
		coord z = rand() % maximum;
		coord x_result, y_result, z_result;
		morton mortonresult = encode.encode(x, y, z);
		decode.decode(mortonresult, x_result, y_result, z_result);
		if (x != x_result | y != y_result | z != z_result) {
			cout << endl << "x: " << getBitString<coord>(x) << " (" << x << ")" << endl;
			cout << "y: " << getBitString<coord>(y) << " (" << y << ")" << endl;
			cout << "z: " << getBitString<coord>(z) << " (" << z << ")" << endl;
			cout << "morton: " << getBitString<morton>(mortonresult) << "(" << mortonresult << ")" << endl;
			cout << "x_result: " << getBitString<coord>(x_result) << " (" << x_result << ")" << endl;
			cout << "y_result: " << getBitString<coord>(y_result) << " (" << y_result << ")" << endl;
			cout << "z_result: " << getBitString<coord>(z_result) << " (" << z_result << ")" << endl;
			cout << sizeof(morton)*8 << "-bit ";
			cout << "using methods encode " << encode.description << " and decode " << decode.description << endl;
			everythingokay = false;
		}
	}
	return everythingokay;
}

template <typename morton, typename coord>
inline void check3D_EncodeCorrectness(std::vector<encode_f_3D_wrapper<morton, coord>> encoders) {
	printf("++ Checking correctness of 3D encoders (%u bit) methods ... ", sizeof(morton) * 8);
	bool ok = true;
	for (typename std::vector<encode_f_3D_wrapper<morton, coord>>::iterator it = encoders.begin(); it != encoders.end(); it++) {
		ok &= check3D_EncodeFunction(*it);
	}
	ok ? printf(" Passed. \n") : printf("    One or more methods failed. \n");
}

template <typename morton, typename coord>
inline void check3D_DecodeCorrectness(std::vector<decode_f_3D_wrapper<morton, coord>> decoders) {
	printf("++ Checking correctness of 3D decoding (%u bit) methods ... ", sizeof(morton) * 8);
	bool ok = true;
	for (typename std::vector<decode_f_3D_wrapper<morton, coord>>::iterator it = decoders.begin(); it != decoders.end(); it++) {
		ok &= check3D_DecodeFunction(*it);
	}
	ok ? printf(" Passed. \n") : printf("    One or more methods failed. \n");
}

template <typename morton, typename coord>
inline void check3D_EncodeDecodeMatch(std::vector<encode_f_3D_wrapper<morton, coord>> encoders, std::vector<decode_f_3D_wrapper<morton, coord>> decoders, unsigned int times) {
	printf("++ Checking 3D methods (%u bit) encode/decode match ... ", sizeof(morton) * 8);
	bool ok = true;
	for (typename std::vector<encode_f_3D_wrapper<morton, coord>>::iterator et = encoders.begin(); et != encoders.end(); et++) {
		for (typename std::vector<decode_f_3D_wrapper<morton, coord>>::iterator dt = decoders.begin(); dt != decoders.end(); dt++) {
			ok &= check3D_Match(*et, *dt, times);
		}
	}
	ok ? printf(" Passed. \n") : printf("    One or more methods failed. \n");
}
