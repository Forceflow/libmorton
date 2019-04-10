# Libmorton v0.2
[![Build Status](https://travis-ci.org/Forceflow/libmorton.svg?branch=master)](https://travis-ci.org/Forceflow/libmorton) [![license](https://img.shields.io/github/license/mashape/apistatus.svg)](https://opensource.org/licenses/MIT) [![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=4JAUNWWYUVRN4&currency_code=EUR&source=url)

 * Libmorton is a **C++11 header-only library** with methods to efficiently encode/decode 64, 32 and 16-bit Morton codes and coordinates, in 2D and 3D. *Morton order* is also known as *Z-order* or *[the Z-order curve](https://en.wikipedia.org/wiki/Z-order_curve)*.
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

If you want to take advantage of the BMI2 instruction set (only available on Intel Haswell processors and newer), make sure `__BMI2__` is defined before you include `morton.h`. This is definitely the fastest method, but limited to certain newer CPU's.

## Installation
No installation is required (just download the headers and include them), but I was informed libmorton is packaged for [Microsoft's VCPKG system](https://github.com/Microsoft/vcpkg) as well, if you want a more controlled environment to install C++ packages in.

## Testing
The *test* folder contains tools I use to test correctness and performance of the libmorton implementation. This section is under heavy re-writing, but might contain some useful code for advanced usage.

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
  * Sylvain Rousseau and Tamy Boubekeur, 2017. _Fast lossy compression of 3D unit vector sets_ ([PDF](https://perso.telecom-paristech.fr/boubek/papers/UVC/UVC.pdf))
  * Jan Watter, 2018. _Generation of complex numerical meshes using space-filling curves_ ([PDF](http://www.cie.bgu.tum.de/publications/bachelorthesis/2018_Watter.pdf))
  * [Esri](https://www.esri.com/en-us/home)
  * [Cesium Ion](https://cesium.com/ion/opensource)
  
  ## Thanks / See ALso
 * To [@gnzlbg](https://github.com/gnzlbg) and his Rust implementation [bitwise](https://github.com/gnzlbg) for finding bugs in the Magicbits code 
 * [@kevinhartman](https://github.com/kevinhartman) made a C++14 library that supports N-dimensional morton codes [morton-nd](https://github.com/kevinhartman/morton-nd). He upstreamed a lot of fixes back to libmorton - thanks!
 * Everyone making comments and suggestions on the [original blogpost](http://www.forceflow.be/2013/10/07/morton-encodingdecoding-through-bit-interleaving-implementations/)
 * Fabian Giesen's [post](https://fgiesen.wordpress.com/2009/12/13/decoding-morton-codes/) on Morton Codes

## TODO
 * Write better test suite (with L1/L2 trashing, better tests, ...)
 * A better naming system for the functions, because m3D_e_sLUT_shifted? That escalated quickly.
