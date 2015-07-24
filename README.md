# Libmorton

 * Libmorton is a **C++ header-only library** with methods to efficiently encode/decode 64-bit morton codes from/to 32-bit (x,y,z) coordinates. *Morton order* is also known as *Z-order* or *[the Z-order curve](https://en.wikipedia.org/wiki/Z-order_curve)*.
 * Libmorton is a **portable** library - in its most basic form it only depends on standard C++ headers. Compiler/architecture optimizations can however be enabled.
 * Cuda version is being developed separately, since it serves a slightly different purpose (batch encoding/decoding large batches of coordinates) and will not be a header-only library.

## Usage
You can use the library by including *libmorton/morton.h*. This will always include the most efficient way to date. The header *morton_alternatives.h* contains older / alternative implementations, for reference only.

<pre>
// ENCODE a 32-bit (x,y,z) coordinate to a 64-bit morton code
inline uint64_t morton3D_Encode(const uint32_t x, const uint32_t y, const uint32_t z);
// DECODE a 64-bit morton code into 32-bit (x,y,z) coordinates (use this if you need all three coordinates)
inline uint64_t morton3D_Decode(const uint64_t morton, uint32_t& x, uint32_t& y, uint32_t& z);
// DECODE a 64-bit morton code into a 32-bit coordinate (use this if you only need x, y or z)
inline uint32_t morton3D_Decode_X(const uint64_t morton);
inline uint32_t morton3D_Decode_Y(const uint64_t morton);
inline uint32_t morton3D_Decode_Z(const uint64_t morton);
</pre>

Compiler/architecture-specific optimizations can be enabled by adding
<pre>
#define LIBMORTON_USE_INTRINSICS 
</pre>
before including the library

## Testing
The *test* folder contains tools I use to test correctness and performance of the libmorton implementation.

See this blogpost for explanation and benchmarks:

http://www.forceflow.be/2013/10/07/morton-encodingdecoding-through-bit-interleaving-implementations/
