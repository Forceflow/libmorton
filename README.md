# Libmorton

 * Libmorton is a **C++ header-only library** with methods to efficiently encode/decode between 64, 32 and 16-bit morton codes and coordinates, in 2D and 3D. *Morton order* is also known as *Z-order* or *[the Z-order curve](https://en.wikipedia.org/wiki/Z-order_curve)*.
 * Libmorton is a **lightweight and portable** library - in its most basic form it only depends on standard C++ headers. Architecture-specific optimizations are implemented incrementally.
 * This library is under active development. SHIT MIGHT BREAK.
 * More info and some benchmarks in these blogposts:
   * http://www.forceflow.be/2016/01/18/libmorton-a-library-for-morton-order-encoding-decoding/
   * http://www.forceflow.be/2013/10/07/morton-encodingdecoding-through-bit-interleaving-implementations/

## Usage
Just include *libmorton/morton.h*. This will always have functions that point to the most efficient way to encode/decode Morton codes. If you want to test out alternative (and possibly slower) methods, you can find them in *libmorton/morton2D.h* and *libmorton/morton3D.h*.

<pre>
// ENCODING 2D / 3D morton codes, of length 32 and 64 bits
inline uint_fast32_t morton2D_32_encode(const uint_fast16_t x, const uint_fast16_t y);
inline uint_fast64_t morton2D_64_encode(const uint_fast32_t x, const uint_fast32_t y);
inline uint_fast32_t morton3D_32_encode(const uint_fast16_t x, const uint_fast16_t y, const uint_fast16_t z);
inline uint_fast64_t morton3D_64_encode(const uint_fast32_t x, const uint_fast32_t y, const uint_fast32_t z);
// DECODING 2D / 3D morton codes, of length 32 and 64 bits
inline void morton2D_32_decode(const uint_fast32_t morton, uint_fast16_t& x, uint_fast16_t& y);
inline void morton2D_64_decode(const uint_fast64_t morton, uint_fast32_t& x, uint_fast32_t& y);
inline void morton3D_32_decode(const uint_fast32_t morton, uint_fast16_t& x, uint_fast16_t& y, uint_fast16_t& z);
inline void morton3D_64_decode(const uint_fast64_t morton, uint_fast32_t& x, uint_fast32_t& y, uint_fast32_t& z);
</pre>

## Testing
The *test* folder contains tools I use to test correctness and performance of the libmorton implementation. This section is under heavy re-writing, but might contain some useful code for advanced usage.

## TODO
 * Write better test suites
 * Implement BMI2 instruction set detection and optimization for newer Intel CPU's
 * More testing
