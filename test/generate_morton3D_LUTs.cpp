#include <iostream>
#include <iomanip>
#include <cstdint>
#include <cstdlib>

#include <libmorton/morton3D.h>
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
    const size_t bits = 8; // generate tables for 8-bit chunks => 256 entries
    const size_t total = 1 << bits;

    // 32-bit encode tables
    uint_fast32_t* x32 = (uint_fast32_t*)std::malloc(total * sizeof(uint_fast32_t));
    uint_fast32_t* y32 = (uint_fast32_t*)std::malloc(total * sizeof(uint_fast32_t));
    uint_fast32_t* z32 = (uint_fast32_t*)std::malloc(total * sizeof(uint_fast32_t));
    for (uint_fast32_t i = 0; i < total; ++i) {
        x32[i] = (uint_fast32_t)m3D_e_magicbits<uint_fast32_t, uint_fast32_t>(i, 0, 0);
        y32[i] = (uint_fast32_t)m3D_e_magicbits<uint_fast32_t, uint_fast32_t>(0, i, 0);
        z32[i] = (uint_fast32_t)m3D_e_magicbits<uint_fast32_t, uint_fast32_t>(0, 0, i);
    }

    // 64-bit encode tables
    uint_fast64_t* x64 = (uint_fast64_t*)std::malloc(total * sizeof(uint_fast64_t));
    uint_fast64_t* y64 = (uint_fast64_t*)std::malloc(total * sizeof(uint_fast64_t));
    uint_fast64_t* z64 = (uint_fast64_t*)std::malloc(total * sizeof(uint_fast64_t));
    for (uint_fast32_t i = 0; i < total; ++i) {
        x64[i] = (uint_fast64_t)m3D_e_magicbits<uint_fast64_t, uint_fast32_t>(i, 0, 0);
        y64[i] = (uint_fast64_t)m3D_e_magicbits<uint_fast64_t, uint_fast32_t>(0, i, 0);
        z64[i] = (uint_fast64_t)m3D_e_magicbits<uint_fast64_t, uint_fast32_t>(0, 0, i);
    }

    // decode tables (512 entries)
    const size_t dtotal = 512;
    uint_fast8_t* dx = (uint_fast8_t*)std::malloc(dtotal * sizeof(uint_fast8_t));
    uint_fast8_t* dy = (uint_fast8_t*)std::malloc(dtotal * sizeof(uint_fast8_t));
    uint_fast8_t* dz = (uint_fast8_t*)std::malloc(dtotal * sizeof(uint_fast8_t));
    // Generate decode tables without pre-shifting (all use the same 9-bit window)
    // This produces raw 3-bit values for each 9-bit pattern; callers should
    // apply the runtime 3*i shift when assembling coordinates.
    for (size_t i = 0; i < dtotal; ++i) {
        dx[i] = (uint_fast8_t)morton3D_GetThirdBits<uint_fast64_t, uint_fast32_t>(i);
        dy[i] = (uint_fast8_t)morton3D_GetThirdBits<uint_fast64_t, uint_fast32_t>(i);
        dz[i] = (uint_fast8_t)morton3D_GetThirdBits<uint_fast64_t, uint_fast32_t>(i);
    }

    uint_fast8_t* preshifted_dx = (uint_fast8_t*)std::malloc(dtotal * sizeof(uint_fast8_t));
    uint_fast8_t* preshifted_dy = (uint_fast8_t*)std::malloc(dtotal * sizeof(uint_fast8_t));
    uint_fast8_t* preshifted_dz = (uint_fast8_t*)std::malloc(dtotal * sizeof(uint_fast8_t));
    // Generate decode tables with pre-shifting (all use the same 9-bit window)
    // This produces raw 3-bit values for each 9-bit pattern; callers should
    // apply the runtime 3*i shift when assembling coordinates.
    for (size_t i = 0; i < dtotal; ++i) {
        preshifted_dx[i] = (uint_fast8_t)morton3D_GetThirdBits<uint_fast64_t, uint_fast32_t>(i);
        preshifted_dy[i] = (uint_fast8_t)morton3D_GetThirdBits<uint_fast64_t, uint_fast32_t>(i >> 1);
        preshifted_dz[i] = (uint_fast8_t)morton3D_GetThirdBits<uint_fast64_t, uint_fast32_t>(i >> 2);
    }

    // print results
    std::cout << "// 3D Encode LUTs (32-bit)" << std::endl;
    print_hex_array("Morton3D_encode_x_256_32", x32, total);
    print_hex_array("Morton3D_encode_y_256_32", y32, total);
    print_hex_array("Morton3D_encode_z_256_32", z32, total);

    std::cout << "// 3D Encode LUTs (64-bit)" << std::endl;
    print_hex_array("Morton3D_encode_x_256_64", x64, total);
    print_hex_array("Morton3D_encode_y_256_64", y64, total);
    print_hex_array("Morton3D_encode_z_256_64", z64, total);

    std::cout << "// 3D Decode preshifted LUTs (512 entries)" << std::endl;
    print_decode_array("Morton3D_decode_preshifted_x_512_gen", preshifted_dx, dtotal);
    print_decode_array("Morton3D_decode_preshifted_y_512_gen", preshifted_dy, dtotal);
    print_decode_array("Morton3D_decode_preshifted_z_512_gen", preshifted_dz, dtotal);

    std::cout << "// 3D Decode LUTs (512 entries)" << std::endl;
    print_decode_array("Morton3D_decode_x_512_gen", dx, dtotal);
    print_decode_array("Morton3D_decode_y_512_gen", dy, dtotal);
    print_decode_array("Morton3D_decode_z_512_gen", dz, dtotal);

    return 0;
}
