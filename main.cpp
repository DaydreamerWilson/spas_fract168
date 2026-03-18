// main.cpp
#include "spas_fract168.hpp"
#include <iostream>
#include <string>
#include <cmath>
#include <iomanip>

// --- Testing Framework ---

size_t tests_run = 0;
size_t tests_passed = 0;

void assert_test(bool condition, const std::string& name) {
    tests_run++;
    if (condition) {
        tests_passed++;
        std::cout << "[ PASS ] " << name << "\n";
    } else {
        std::cout << "[ FAIL ] " << name << "\n";
    }
}

// Equality operator for comprehensive state-level validation
bool operator==(const spas_fract168_t& a, const spas_fract168_t& b) {
    return a.sign == b.sign &&
           a.big == b.big &&
           a.small == b.small &&
           a.offset == b.offset;
}

// Float equality strictly for resulting value validations
bool approx_eq(double a, double b) {
    return std::abs(a - b) < 1e-9;
}


// --- Test Suites ---

void test_constructors() {
    std::cout << "\n--- Testing Constructors ---\n";

    spas_fract168_t f_def;
    assert_test(f_def.big == 0 && f_def.small == 0 && f_def.offset == 0 && f_def.sign == 0, 
                "Default constructor initializes to zero");

    spas_fract168_t f_half(0.5);
    assert_test(f_half.big == 0x8000000000000000ULL && f_half.sign == 0, 
                "Double constructor correctly translates positive double (0.5)");

    spas_fract168_t f_mhalf(-0.5);
    assert_test(f_mhalf.big == 0x8000000000000000ULL && f_mhalf.sign == 0b1000, 
                "Double constructor correctly translates negative double (-0.5)");
}

void test_get_double() {
    std::cout << "\n--- Testing getDouble ---\n";

    assert_test(approx_eq(spas_fract168_t(0.5).getDouble(), 0.5), "getDouble accurately yields 0.5");
    assert_test(approx_eq(spas_fract168_t(-0.25).getDouble(), -0.25), "getDouble accurately yields -0.25");
    
    spas_fract168_t tiny(std::pow(2.0, -70));
    assert_test(tiny.big == 0, "Double too small for 'big' leaves big=0 correctly");
    assert_test(tiny.small != 0, "Double too small for 'big' accurately populates 'small'");
}

void test_unary_minus() {
    std::cout << "\n--- Testing Unary Minus ---\n";

    spas_fract168_t f(0.5);
    spas_fract168_t mf = -f;
    assert_test(approx_eq(mf.getDouble(), -0.5), "Unary minus flips positive fraction to negative");
}

void test_addition() {
    std::cout << "\n--- Testing Addition (Exception-Free) ---\n";

    spas_fract168_t f_0_25(0.25);
    spas_fract168_t f_0_5(0.5);
    spas_fract168_t f_m0_25(-0.25);
    
    assert_test(approx_eq((f_0_25 + f_0_5).getDouble(), 0.75), "0.25 + 0.5 = 0.75");
    assert_test(approx_eq((f_0_5 + f_m0_25).getDouble(), 0.25), "0.5 + (-0.25) = 0.25");
}

void test_subtraction() {
    std::cout << "\n--- Testing Subtraction (Exception-Free) ---\n";

    spas_fract168_t f_0_75(0.75);
    spas_fract168_t f_0_25(0.25);
    spas_fract168_t f_m0_25(-0.25);

    assert_test(approx_eq((f_0_75 - f_0_25).getDouble(), 0.5), "0.75 - 0.25 = 0.5");
    assert_test(approx_eq((f_0_25 - f_0_75).getDouble(), -0.5), "0.25 - 0.75 = -0.5");
}

void test_multiplication() {
    std::cout << "\n--- Testing Multiplication (Exception-Free) ---\n";

    spas_fract168_t f_0_5(0.5);
    spas_fract168_t f_m0_5(-0.5);

    assert_test(approx_eq((f_0_5 * f_0_5).getDouble(), 0.25), "0.5 * 0.5 = 0.25");
    assert_test(approx_eq((f_0_5 * f_m0_5).getDouble(), -0.25), "0.5 * (-0.5) = -0.25");
}

void test_shift_left() {
    std::cout << "\n--- Testing Shift Left (Exception-Free) ---\n";

    spas_fract168_t f_0_125(0.125);
    assert_test(approx_eq((f_0_125 << 1).getDouble(), 0.25), "0.125 << 1 = 0.25");
}

void test_small_offset_operations() {
    std::cout << "\n--- Testing Small & Offset Mathematical Behavior ---\n";

    spas_fract168_t t_m65(0, 0, 0, 0x8000000000000000ULL); 
    spas_fract168_t t_m64_expected(0, 1, 0, 0);
    assert_test((t_m65 + t_m65) == t_m64_expected, 
                "Addition carries from 'small' accurately over to 'big'");

    spas_fract168_t t_off10(0, 0, 10, 0x8000000000000000ULL);
    spas_fract168_t t_off9(0, 0, 9, 0x8000000000000000ULL);
    assert_test((t_off10 + t_off10) == t_off9, 
                "Addition correctly aligns, aggregates, and shifts identical offsets");

    spas_fract168_t t_mul_expected(0, 0, 65, 0x8000000000000000ULL);
    assert_test((t_m65 * t_m65) == t_mul_expected, 
                "Multiplication cascades 'small' correctly into dynamically deeper 'offsets'");
}

// --- Deep Structural Math Tests (Cross Interactions between Big & Small) ---
void test_big_small_cross_interactions() {
    std::cout << "\n--- Testing Big/Small Cross Interactions ---\n";

    spas_fract168_t a_half(0.5); // big=0x8..0, small=0
    spas_fract168_t b_small(0, 0, 0, 0x8000000000000000ULL); // 2^-65

    // Test 1: Implicit mixed-sign state mapping
    // 0.5 - 2^-65 effectively makes 'big' positive and 'small' mathematically negative.
    // The class represents this by assigning 0b0001 (small is negative) to `sign` without modifying `big` natively.
    spas_fract168_t a_minus_b = a_half - b_small;
    spas_fract168_t exp1(0b0001, 0x8000000000000000ULL, 0, 0x8000000000000000ULL); 
    assert_test(a_minus_b == exp1, 
                "Subtraction correctly encodes 'borrow' as a mixed-sign state (+big, -small)");

    // Test 2: Addition resolving mixed-sign state cancellation
    // (0.5 - 2^-65) + 2^-65 = 0.5. The negative small component should perfectly cancel the added small component.
    assert_test((a_minus_b + b_small) == a_half, 
                "Addition successfully resolves mixed-sign states back to a clean 'big' value");

    // Test 3: Subtraction resolving positive mixed states
    // (0.5 + 2^-65) - 2^-65 = 0.5
    spas_fract168_t a_plus_b(0b0000, 0x8000000000000000ULL, 0, 0x8000000000000000ULL);
    assert_test((a_plus_b - b_small) == a_half, 
                "Subtraction successfully targets and cancels out 'small' components safely");

    // Test 4: Carry propagation overflowing 'small' into 'big' strictly (Positive to Positive)
    // 2 * (0.25 + 2^-65) = 0.5 + 2^-64
    spas_fract168_t q_plus_small(0b0000, 0x4000000000000000ULL, 0, 0x8000000000000000ULL);
    spas_fract168_t exp_q_add(0b0000, 0x8000000000000001ULL, 0, 0); // LSB of big is 2^-64
    assert_test((q_plus_small + q_plus_small) == exp_q_add, 
                "Addition carry perfectly transfers from small boundary up to big (+ to +)");

    // Test 5: Carry propagation modifying 'big' oppositely due to mixed signs (Borrow-through)
    // 2 * (0.25 - 2^-65) = 0.5 - 2^-64. Since 2^-64 natively fits in the LSB of 'big', it forces a cascade decrement.
    // Effectively: 0x8000000000000000ULL - 1 = 0x7FFFFFFFFFFFFFFFULL
    spas_fract168_t q_minus_small(0b0001, 0x4000000000000000ULL, 0, 0x8000000000000000ULL);
    spas_fract168_t exp_q_sub(0b0000, 0x7FFFFFFFFFFFFFFFULL, 0, 0); 
    assert_test((q_minus_small + q_minus_small) == exp_q_sub, 
                "Addition carry correctly borrows/decrements from 'big' when component signs differ (+big, -small)");

    // Test 6: Cross-term Distribution in Multiplication (Positive Small)
    // (0.5 + 2^-65) * 0.5 = 0.25 + 2^-66
    spas_fract168_t exp_mul(0b0000, 0x4000000000000000ULL, 1, 0x8000000000000000ULL); // offset=1 pushes small down 1 bit
    assert_test((a_plus_b * a_half) == exp_mul, 
                "Multiplication of mixed Big/Small scales exactly and aligns 'small' offset properly");

    // Test 7: Cross-term Distribution in Multiplication (Negative Small)
    // (0.5 - 2^-65) * 0.5 = 0.25 - 2^-66
    spas_fract168_t exp_mul_mix(0b0001, 0x4000000000000000ULL, 1, 0x8000000000000000ULL);
    assert_test((a_minus_b * a_half) == exp_mul_mix, 
                "Multiplication correctly isolates and distributes negative sign recursively to 'small' cross-terms");
}

int main() {
    std::cout << "Starting spas_fract168_t Testing Suite (Safe & Exception-Free)...\n";

    test_constructors();
    test_get_double();
    test_unary_minus();
    test_addition();
    test_subtraction();
    test_multiplication();
    test_shift_left();
    test_small_offset_operations();
    test_big_small_cross_interactions(); // NEW: Interactions between variables

    std::cout << "\n--- Test Summary ---\n";
    std::cout << "Total Tests Run: " << tests_run << "\n";
    std::cout << "Tests Passed:    " << tests_passed << "\n";
    std::cout << "Tests Failed:    " << (tests_run - tests_passed) << "\n";

    if (tests_run == tests_passed) {
        std::cout << "\nALL TESTS PASSED SUCCESSFULLY.\n";
        return 0;
    } else {
        std::cout << "\nSOME TESTS FAILED.\n";
        return 1;
    }
}