Libmorton
================

Library for for encoding/decoding coordinates to/from the Morton order (also known as [the Z-order curve](https://en.wikipedia.org/wiki/Z-order_curve)).

You can use the library by including *morton.h*. This will always include the most efficient way to date. The header *morton_alternatives.h* contains older /alternative implementations.

Cuda version is being devleoped separately, since it serves a slightly different purpose (batch encoding/decoding large batches of coordinates) and will not be a header-only library.

See this blogpost for explanation and benchmarks:

http://www.forceflow.be/2013/10/07/morton-encodingdecoding-through-bit-interleaving-implementations/
