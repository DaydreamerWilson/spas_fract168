#ifndef spas_fract168
#define spas_fract168

#include <cstring>
#include <iostream>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>

#include <intrin.h>

#pragma intrinsic(_umul128)

// High precision fraction series
class spas_fract168_t{
    public:
        unsigned char sign; // MSB for big, LSB for small, 0 means positive, 1 means negative
        uint64_t big; // Contain a fraction series equivalent to its integer value / 2^64
        uint64_t small; // Contain a fraction series equivalent to its integer value / 2^(128+offset)
        uint32_t offset; // Exponential denominator of small

        // Print entire binary fraction in hex form
        void printAll() const;
        // Print entire binary fraction in binary form
        void printBinary() const;
        // Print only the offset
        void printOffset() const;
        // Print only the sign
        void printSign() const;
        // Print both the sign and offset
        void printSO() const;
        // Return double value of the sparse fraction series, suffer from aliasing!!!
        double getDouble() const;

        // Empty constructor for temporary variables
        spas_fract168_t();
        // Constructor for converting double into sparse fractions
        spas_fract168_t(double t);
        // Debug constructor, only use this if you know what you are doing!!!
        spas_fract168_t(uint8_t sign, uint64_t big, uint32_t offset, uint64_t small);
        // Copy constructor
        spas_fract168_t(const spas_fract168_t& t);
        // Assignment constructors

        spas_fract168_t& operator=(const spas_fract168_t& t);
        spas_fract168_t& operator+=(const spas_fract168_t& rhs);
        spas_fract168_t& operator-=(const spas_fract168_t& rhs);
        spas_fract168_t& operator*=(const spas_fract168_t& rhs);
};

spas_fract168_t operator+(spas_fract168_t lhs, const spas_fract168_t& rhs);
spas_fract168_t operator-(spas_fract168_t lhs, const spas_fract168_t& rhs);
spas_fract168_t operator*(spas_fract168_t lhs, const spas_fract168_t& rhs);

// Inverter
spas_fract168_t operator-(const spas_fract168_t& rhs);
// Left shift
spas_fract168_t operator<<(spas_fract168_t lhs, const uint32_t rhs);

uint64_t reverse_64(uint64_t t);
uint32_t reverse_32(uint32_t t);

// Multiply lhs by rhs and return value in big and small portions
void fraction_multiply(uint64_t lhs, uint64_t rhs, uint64_t &big, uint64_t &small);

uint8_t fraction_addition(uint64_t &lhs, uint64_t rhs, uint32_t offset);
uint8_t fraction_subtraction(uint64_t &lhs, uint64_t rhs, uint32_t offset);

uint8_t full_fraction_addition(unsigned char &sign, uint64_t &res, uint32_t &res_off, unsigned char l_sign, uint64_t lhs, uint32_t l_off, unsigned char r_sign, uint64_t rhs, uint32_t r_off);
uint8_t full_fraction_subtraction(unsigned char &sign, uint64_t &res, uint32_t &res_off, unsigned char l_sign, uint64_t lhs, uint32_t l_off, unsigned char r_sign, uint64_t rhs, uint32_t r_off);

void _fraction_multiply(uint64_t lhs, uint64_t rhs, uint64_t &big, uint64_t &small);
#endif