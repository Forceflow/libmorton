#include <iostream>
#include <iomanip>
#include <cstdint>
#include <cstdlib>

#include <libmorton/morton3D.h>
#include "morton_LUT_generators.h"

using namespace libmorton;

// Always emits uint_fast32_t tables: every encode LUT entry fits in 32 bits
// (sizeof(uint_fast32_t) is 8 on Linux/glibc, so don't derive the type from sizeof).
void print_hex_array(const char* name, const uint_fast32_t* arr, size_t n) {
    const unsigned int width = 8;
    std::cout << "static const uint_fast32_t " << name << "[" << n << "] = {\n";
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

    // decode tables (512 entries)
    const size_t dtotal = 512;
    uint_fast8_t* dx = (uint_fast8_t*)std::malloc(dtotal * sizeof(uint_fast8_t));
    uint_fast8_t* preshifted_dy = (uint_fast8_t*)std::malloc(dtotal * sizeof(uint_fast8_t));
    uint_fast8_t* preshifted_dz = (uint_fast8_t*)std::malloc(dtotal * sizeof(uint_fast8_t));
    // x is read unshifted; y and z tables are pre-shifted by 1 and 2 bits.
    // Each entry holds the 3-bit coordinate chunk for a 9-bit window;
    // callers apply the runtime 3*i shift when assembling coordinates.
    for (size_t i = 0; i < dtotal; ++i) {
        dx[i] = (uint_fast8_t)morton3D_GetThirdBits<uint_fast64_t, uint_fast32_t>(i);
        preshifted_dy[i] = (uint_fast8_t)morton3D_GetThirdBits<uint_fast64_t, uint_fast32_t>(i >> 1);
        preshifted_dz[i] = (uint_fast8_t)morton3D_GetThirdBits<uint_fast64_t, uint_fast32_t>(i >> 2);
    }

    // print results
    std::cout << "// 3D Encode LUTs (x unshifted, y and z preshifted)" << std::endl;
    print_hex_array("Morton3D_encode_x_256", x32, total);
    print_hex_array("Morton3D_encode_y_256", y32, total);
    print_hex_array("Morton3D_encode_z_256", z32, total);

    std::cout << "// 3D Decode LUTs (x unshifted, y and z preshifted)" << std::endl;
    print_decode_array("Morton3D_decode_x_512", dx, dtotal);
    print_decode_array("Morton3D_decode_preshifted_y_512", preshifted_dy, dtotal);
    print_decode_array("Morton3D_decode_preshifted_z_512", preshifted_dz, dtotal);

    return 0;
}
