# Libmorton v0.2.7
[![Build Status](https://travis-ci.org/Forceflow/libmorton.svg?branch=master)](https://travis-ci.org/Forceflow/libmorton) [![license](https://img.shields.io/github/license/mashape/apistatus.svg)](https://opensource.org/licenses/MIT) [![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.me/forceflow)

 * Libmorton is a **C++ header-only library** with methods to efficiently encode/decode 64, 32 and 16-bit Morton codes and coordinates, in 2D and 3D. *Morton order* is also known as *Z-order* or *[the Z-order curve](https://en.wikipedia.org/wiki/Z-order_curve)*.
 * Libmorton is a **lightweight and portable** library - the only dependencies are standard C++ headers. Architecture-specific optimizations are available.
 * More info and some benchmarks in these blogposts: [*Morton encoding*](http://www.forceflow.be/2013/10/07/morton-encodingdecoding-through-bit-interleaving-implementations/), [*Libmorton*](http://www.forceflow.be/2016/01/18/libmorton-a-library-for-morton-order-encoding-decoding/) and [*BMI2 instruction set*](http://www.forceflow.be/2016/11/25/using-the-bmi2-instruction-set-to-encode-decode-morton-codes/)

## Usage
Include `libmorton/morton.h`. This will always have stub functions that point to the most efficient way to encode/decode Morton codes. If you want to test out alternative (and possibly slower) methods, you can find them in `libmorton/morton2D.h` and `libmorton/morton3D.h`. **All libmorton functionality is in the `libmorton` namespace to avoid conflicts**.

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

## Installation
No installation is required (just download the headers and include them), but I was informed libmorton is packaged for [Microsoft's VCPKG system](https://github.com/Microsoft/vcpkg) as well, if you want a more controlled environment to install C++ packages in.

## Instruction sets
In the standard case, libmorton only uses operations that are supported on pretty much any CPU you can throw it at. If you know you're compiling for a specific architecture, you might gain a speed boost in encoding/decoding operations by enabling implementations for a specific instruction set. Libmorton ships with support for:
 * **BMI2 instruction set**: Intel: Haswell CPU's and newer. AMD: Ryzen CPU's and newer. Define `__BMI2__` before including `morton.h`. This is definitely a faster method when compared to the standard case.
 * **AVX512 instruction set (experimental)**: Intel Ice Lake CPU's and newer. Uses `_mm512_bitshuffle_epi64_mask`. Define `__AVX512BITALG__` before including `morton.h`. For more info on performance, see [this PR](https://github.com/Forceflow/libmorton/pull/40).
 
When using MSVC, these options can be found under _Project Properties -> Code Generation -> Enable Enhanced Instruction set_.
When using GCC (version 9.0 or higher), you can use `-march=haswell` (or `-march=znver2`) for BMI2 support and `-march=icelake-client` for AVX512 support.

## Testing
The `test` folder contains tools I use to test correctness and performance of the libmorton implementation. You can regard them as unit tests. This section is under heavy re-writing, but might contain some useful code for advanced usage. 

You can build the test suite:
 * With the included [Visual Studio 2019](https://visualstudio.microsoft.com/) project in `test\msvc*`
 * Using make: `test\makefile`
 * Using Cmake (thanks [@shohirose](https://github.com/shohirose))

## Citation
If you use libmorton in your published paper or work, please reference it, for example as follows:
<pre>
@Misc{libmorton18,
author = "Jeroen Baert",
title = "Libmorton: C++ Morton Encoding/Decoding Library",
howpublished = "\url{https://github.com/Forceflow/libmorton}",
year = "2018"}
</pre>
 
 ## Publications / products that use libmorton
 I'm always curious what libmorton ends up on. If you end up using it, send me an e-mail!
  * Thomas Bläsius, Tobias Friedrich et al, 2019. _Efficiently Generating Geometric Inhomogeneous and Hyperbolic Random Graphs_ ([link](https://arxiv.org/abs/1905.06706))
  * Alexander Dieckmann, Reinhard Klein, 2018. _Hierarchical additive poisson disk sampling_ ([link](https://dl.acm.org/citation.cfm?id=3307667))
  * Sylvain Rousseau and Tamy Boubekeur, 2017. _Fast lossy compression of 3D unit vector sets_ ([PDF](https://perso.telecom-paristech.fr/boubek/papers/UVC/UVC.pdf))
  * Jan Watter, 2018. _Generation of complex numerical meshes using space-filling curves_ ([PDF](http://www.cie.bgu.tum.de/publications/bachelorthesis/2018_Watter.pdf))
  * [Esri](https://www.esri.com/en-us/home)
  * [Cesium Ion](https://cesium.com/ion/opensource)
  * [CLAIRE](https://github.com/andreasmang/claire#clairedoc)
  
  ## Thanks / See ALso
 * To [@gnzlbg](https://github.com/gnzlbg) and his Rust implementation [bitwise](https://github.com/gnzlbg) for finding bugs in the Magicbits code 
 * [@kevinhartman](https://github.com/kevinhartman) made a C++14 library that supports N-dimensional morton codes [morton-nd](https://github.com/kevinhartman/morton-nd). He upstreamed a lot of fixes back to libmorton - thanks!
 * Everyone making comments and suggestions on the [original blogpost](http://www.forceflow.be/2013/10/07/morton-encodingdecoding-through-bit-interleaving-implementations/)
 * [@Wunkolo](https://github.com/Wunkolo) for AVX512 implementation
 * Fabian Giesen's [post](https://fgiesen.wordpress.com/2009/12/13/decoding-morton-codes/) on Morton Codes
 
 ## Contributing
 
 See [Contributing.md](https://github.com/Forceflow/libmorton/blob/master/CONTRIBUTING.md)

## TODO
 * Add morton operations like described [here](https://en.wikipedia.org/wiki/Z-order_curve#Coordinate_values)
 * Write better test suite (with L1/L2 trashing, better tests, ...)
 * A better naming system for the functions, because m3D_e_sLUT_shifted? That escalated quickly.
