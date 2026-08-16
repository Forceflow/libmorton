#include <iostream>
#include <iomanip>
#include <cstdint>
#include <cstdlib>

#include <libmorton/morton2D.h>
#include "morton_LUT_generators.h"

using namespace libmorton;

template<typename T>
void print_hex_array(const char* name, const T* arr, size_t n) {
    unsigned int width = (sizeof(T) <= 4) ? 8 : 16;
    std::cout << "static const ";
    if (sizeof(T) <= 4) std::cout << "uint_fast32_t "; else std::cout << "uint_fast64_t ";
    std::cout << name << "[" << n << "] = {\n";
    for (size_t i = 0; i < n; ++i) {
        if (i % 8 == 0) std::cout << "\t";
        std::cout << "0x" << std::hex << std::setfill('0') << std::setw(width) << (unsigned long long)arr[i];
        if (i + 1 < n) std::cout << ", ";
        if ((i + 1) % 8 == 0) std::cout << "\n";
    }
    std::cout << std::dec << "\n};\n\n";
}

void print_decode_array(const char* name, const uint_fast8_t* arr, size_t n) {
    std::cout << "static const uint_fast8_t " << name << "[" << n << "] = {\n";
    for (size_t i = 0; i < n; ++i) {
        if (i % 16 == 0) std::cout << "\t";
        std::cout << (unsigned int)arr[i];
        if (i + 1 < n) std::cout << ", ";
        if ((i + 1) % 16 == 0) std::cout << "\n";
    }
    std::cout << "\n};\n\n";
}

int main() {
    const size_t bits = 8; // 256 entries
    const size_t total = 1 << bits;

    // 16-bit encode tables used in the project are uint_fast16_t for 2D
    uint_fast32_t* x_enc_32 = (uint_fast32_t*)std::malloc(total * sizeof(uint_fast32_t));
    uint_fast32_t* y_enc_32 = (uint_fast32_t*)std::malloc(total * sizeof(uint_fast32_t));
    for (uint_fast32_t i = 0; i < total; ++i) {
        x_enc_32[i] = (uint_fast32_t)m2D_e_magicbits<uint_fast32_t, uint_fast32_t>(i, 0);
        y_enc_32[i] = (uint_fast32_t)m2D_e_magicbits<uint_fast32_t, uint_fast32_t>(0, i);
    }

    uint_fast64_t* x_enc_64 = (uint_fast64_t*)std::malloc(total * sizeof(uint_fast64_t));
    uint_fast64_t* y_enc_64 = (uint_fast64_t*)std::malloc(total * sizeof(uint_fast64_t));
    for (uint_fast64_t i = 0; i < total; ++i) {
        x_enc_64[i] = (uint_fast64_t)m2D_e_magicbits<uint_fast64_t, uint_fast32_t>(i, 0);
        y_enc_64[i] = (uint_fast64_t)m2D_e_magicbits<uint_fast64_t, uint_fast32_t>(0, i);
    }

    // decode tables for 2D
    const size_t dtotal = 256;
    uint_fast8_t* dx = (uint_fast8_t*)std::malloc(dtotal * sizeof(uint_fast8_t));
    uint_fast8_t* dy = (uint_fast8_t*)std::malloc(dtotal * sizeof(uint_fast8_t));
    for (size_t i = 0; i < dtotal; ++i) {
        dx[i] = (uint_fast8_t)morton2D_GetSecondBits<uint_fast64_t, uint_fast32_t>(i);
        dy[i] = (uint_fast8_t)morton2D_GetSecondBits<uint_fast64_t, uint_fast32_t>(i);
    }

    uint_fast8_t* preshifted_dx = (uint_fast8_t*)std::malloc(dtotal * sizeof(uint_fast8_t));
    uint_fast8_t* preshifted_dy = (uint_fast8_t*)std::malloc(dtotal * sizeof(uint_fast8_t));
    for (size_t i = 0; i < dtotal; ++i) {
        preshifted_dx[i] = (uint_fast8_t)morton2D_GetSecondBits<uint_fast64_t, uint_fast32_t>(i);
        preshifted_dy[i] = (uint_fast8_t)morton2D_GetSecondBits<uint_fast64_t, uint_fast32_t>(i >> 1);
    }

    std::cout << "// preshifted 2D Encode LUTs (32-bit)" << std::endl;
    print_hex_array("Morton2D_encode_x_256_32", x_enc_32, total);
    print_hex_array("Morton2D_encode_y_256_32", y_enc_32, total);

    std::cout << "// preshifted 2D Encode LUTs (64-bit)" << std::endl;
    print_hex_array("Morton2D_encode_x_256_64", x_enc_64, total);
    print_hex_array("Morton2D_encode_y_256_64", y_enc_64, total);

    std::cout << "// preshifted 2D Decode LUTs (256 entries)" << std::endl;
    print_decode_array("Morton2D_decode_preshifted_x_256", preshifted_dx, dtotal);
    print_decode_array("Morton2D_decode_preshifted_y_256", preshifted_dy, dtotal);

    std::cout << "// 2D Decode LUTs (256 entries)" << std::endl;
    print_decode_array("Morton2D_decode_x_256", dx, dtotal);
    print_decode_array("Morton2D_decode_y_256", dy, dtotal);

    return 0;
}
