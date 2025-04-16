Require CXX14 or above to compile
Natively programmed and tested in MinGW 14.2.0 environment

The type is strictly limited to fractions (-1.0>x>1.0)
Current available native features included
- Arithmetic operations including addition, subtraction and multiplication
- Constructing fraction from double (x>2^-64)

This data structure features lossless arithmetic operations within range of (x>2^-64) (~1e19)
Retaining high precision representation of floating point within range of (2^-64 > x > 2^(2^64))

spas_fract168_t{

  unsigned char sign; // MSB for big, LSB for small, 0 means positive, 1 means negative
  
  uint64_t big; // Contain a fraction series equivalent to its integer value / 2^64
  
  uint64_t small; // Contain a fraction series equivalent to its integer value / 2^(128+offset)
  
  uint32_t offset; // Exponential denominator of small
  
}

Allow high precision arithmetic operations with number smaller than 2^-64 while maintaining constant memory footprint.

This project is tested rigorously while compiling with CMake3.4. Natively supported by visual studio CMake.
This class may have compatibility issue since it used the following non-standard functions/data types
- __uint128_t
- __builtin_clzll()
