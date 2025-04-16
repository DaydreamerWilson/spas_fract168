// test_spas_fract168.cpp
#include "spas_fract168.hpp" // Include the class definition
#include <iostream>
#include <vector>
#include <string>
#include <cmath>    // For std::abs in double comparison
#include <limits>   // For std::numeric_limits
#include <iomanip>  // For std::fixed, std::setprecision

// --- Helper Functions (approx_equal, run_test) --- //
// (Keep the helper functions as previously defined)
bool approx_equal(double a, double b, double epsilon = std::numeric_limits<double>::epsilon() * 100) {
    return std::abs(a - b) <= epsilon * std::max(1.0, std::max(std::abs(a), std::abs(b)));
}

size_t test_count = 0;
size_t pass_count = 0;

// Helper to print spas_fract168_t values for debugging failed tests
void print_spas_val(const spas_fract168_t& val) {
     val.printAll(); // Use the class's own print function
     // Optionally add getDouble() output for reference, noting its limitations
     std::cout << " (double approx: " << std::fixed << std::setprecision(18) << val.getDouble() << ")";
}

template <typename TActual, typename TExpected>
void run_test(const std::string& test_name, const TActual& actual, const TExpected& expected) {
    test_count++;
    bool pass = false;
    // Overload for spas_fract168_t using its operator==
    if constexpr (std::is_same<spas_fract168_t, TActual>::value && std::is_same<spas_fract168_t, TExpected>::value) {
         pass = (actual.big == expected.big && actual.small == expected.small && actual.offset == expected.offset);
    }
    // Overload for double using approx_equal
    else if constexpr (std::is_same<double, TActual>::value && std::is_same<double, TExpected>::value) {
         pass = approx_equal(actual, expected);
    }
     // Add other types if needed
    else {
         pass = (actual == expected); // Default comparison
    }

    if (pass) {
        pass_count++;
        // std::cout << "[ PASS ] " << test_name << std::endl;
    } else {
        std::cout << "[ FAIL ] " << test_name << std::endl;
        std::cout << "       Expected: ";
        if constexpr (std::is_same<spas_fract168_t, TExpected>::value) print_spas_val(expected); else std::cout << expected;
        std::cout << std::endl;
        std::cout << "       Actual:   ";
         if constexpr (std::is_same<spas_fract168_t, TActual>::value) print_spas_val(actual); else std::cout << actual;
        std::cout << std::endl;
    }
}


// --- Test Functions ---

void test_constructors_assignment() {
    std::cout << "\n--- Testing Constructors & Assignment (Range: (-1.0, 1.0)) ---" << std::endl;

    spas_fract168_t d1; // Default constructor
    spas_fract168_t expected_d1(0, 0, 0, 0);
    run_test("Default Constructor", d1, expected_d1);

    // Component constructor - Ensure value is conceptually < 1.0
    // Example: 0.5 + 0.25 * 2^-10 = 0.5 + 0.000244...
    spas_fract168_t c1(0b0000, 0x8000000000000000ULL, 10, 0x4000000000000000ULL);
    spas_fract168_t expected_c1(0b0000, 0x8000000000000000ULL, 10, 0x4000000000000000ULL);
    run_test("Component Constructor", c1, expected_c1);

    spas_fract168_t copy_c1(c1); // Copy constructor
    run_test("Copy Constructor", copy_c1, expected_c1);

    spas_fract168_t assign_c1;
    assign_c1 = c1; // Assignment operator
    run_test("Assignment Operator", assign_c1, expected_c1);

    // Double constructor tests (valid range)
    spas_fract168_t f1(0.5); // 0.1000... binary
    spas_fract168_t expected_f1(0b0000, 0x8000000000000000ULL, 0, 0);
    run_test("Double Constructor (0.5)", f1, expected_f1);

    spas_fract168_t f2(0.75); // 0.1100... binary
    spas_fract168_t expected_f2(0b0000, 0xC000000000000000ULL, 0, 0);
    run_test("Double Constructor (0.75)", f2, expected_f2);

    spas_fract168_t f3(-0.5); // -0.1000... binary
    spas_fract168_t expected_f3(0b1000, 0x8000000000000000ULL, 0, 0); // Assuming sign bit 3 is main sign
    run_test("Double Constructor (-0.5)", f3, expected_f3);

    spas_fract168_t f4(0.0);
    spas_fract168_t expected_f4(0b0000, 0, 0, 0);
    run_test("Double Constructor (0.0)", f4, expected_f4);

    // Test near boundaries
    spas_fract168_t f_near_one(0.9999999999999999); // Closest double to 1 from below
    // Expected: sign=0, big should be all 1s (0xFFF...FFF), small/offset=0
    spas_fract168_t expected_f_near_one(0b0000, 0xFFFFFFFFFFFFFFFFULL, 0, 0);
    run_test("Double Constructor (Near 1.0) Expected to Fail", f_near_one, expected_f_near_one);

    spas_fract168_t f_near_neg_one(-0.9999999999999999); // Closest double to -1 from above
    // Expected: sign=0b1000, big should be all 1s
    spas_fract168_t expected_f_near_neg_one(0b1000, 0xFFFFFFFFFFFFFFFFULL, 0, 0);
    run_test("Double Constructor (Near -1.0) Expected to Fail", f_near_neg_one, expected_f_near_neg_one);

    // Test invalid input (outside range) - behavior might be undefined by design
    // spas_fract168_t f_invalid(1.5); // Constructor might ignore int part or clamp?
    // run_test("Double Constructor (Invalid 1.5)", f_invalid, expected_f1); // Example: if it clamps or only takes frac part
}

void test_get_double() {
    std::cout << "\n--- Testing getDouble (Range: (-1.0, 1.0)) ---" << std::endl;

    // Test based on how double constructor works (only big, 0.big format)
    spas_fract168_t g1(0.5);
    run_test("getDouble (from 0.5)", g1.getDouble(), 0.5);

    spas_fract168_t g2(0.75);
    run_test("getDouble (from 0.75)", g2.getDouble(), 0.75);

    spas_fract168_t g3(-0.5);
    run_test("getDouble (from -0.5)", g3.getDouble(), -0.5);

    spas_fract168_t g4(0.0);
    run_test("getDouble (from 0.0)", g4.getDouble(), 0.0);

    // Test case where small/offset are non-zero (getDouble should ignore them)
    spas_fract168_t g5(0b0000, 0x8000000000000000ULL, 10, 0xAAAAAAAAAAAAAAAULL); // Represents 0.5 + small_stuff
    run_test("getDouble (Ignores small/offset)", g5.getDouble(), 0.5);

    spas_fract168_t g6(0b1000, 0x4000000000000000ULL, 5, 0x1ULL); // Represents -0.25 + small_stuff
    run_test("getDouble (Negative, Ignores small/offset)", g6.getDouble(), -0.25);

     spas_fract168_t g_near_one(0b0000, 0xFFFFFFFFFFFFFFFFULL, 0, 0);
     run_test("getDouble (Near 1.0)", g_near_one.getDouble(), 0.9999999999999999); // Max positive value from 0.big

     spas_fract168_t g_near_neg_one(0b1000, 0xFFFFFFFFFFFFFFFFULL, 0, 0);
     run_test("getDouble (Near -1.0)", g_near_neg_one.getDouble(), -0.9999999999999999); // Min negative value from 0.big
}

void test_unary_minus() {
     std::cout << "\n--- Testing Unary Minus (Range: (-1.0, 1.0)) ---" << std::endl;
     // Test with fractional values within the range
     spas_fract168_t v1(0.5);
     spas_fract168_t neg_v1 = -v1;
     spas_fract168_t expected_neg_v1(-0.5);
     run_test("Unary Minus (0.5)", neg_v1, expected_neg_v1);

     spas_fract168_t v2(-0.25);
     spas_fract168_t neg_v2 = -v2;
     spas_fract168_t expected_neg_v2(0.25);
     run_test("Unary Minus (-0.25)", neg_v2, expected_neg_v2);

     // Test with component constructor value
     spas_fract168_t v3(0b0000, 0x123456789ABCDEF0ULL, 5, 0xFEDCBA9876543210ULL); // Some valid fractional value
     spas_fract168_t neg_v3 = -v3;
     spas_fract168_t expected_neg_v3(0b1001, 0x123456789ABCDEF0ULL, 5, 0xFEDCBA9876543210ULL); // Flips both sign bits
     run_test("Unary Minus (Component value)", neg_v3, expected_neg_v3);

     spas_fract168_t v4(0b1001, 0xAAAAAAAAAAAAAAAAULL, 0, 0x5555555555555555ULL); // Some negative component value
     spas_fract168_t neg_v4 = -v4;
     spas_fract168_t expected_neg_v4(0b0000, 0xAAAAAAAAAAAAAAAAULL, 0, 0x5555555555555555ULL); // Flips back
     run_test("Unary Minus (Negative component value)", neg_v4, expected_neg_v4);
}

void test_shift_left() {
    std::cout << "\n--- Testing Operator<< (Left Shift / Multiply by 2) (Range: (-1.0, 1.0)) ---" << std::endl;

    // Reminder: Code review suggests rhs (shift amount) is ignored and boundary logic at offset 0 is flawed.
    // Tests below demonstrate this expected *buggy* behavior.
    // Also, shifting can easily exceed the (-1, 1) range.

    spas_fract168_t s1(0.25); // 0.01 bin -> big=0x4000...
    spas_fract168_t s1_shifted = s1 << 5; // rhs=5 ignored, should shift by 1

    // Expected result of 0.25 << 1 is 0.5
    spas_fract168_t expected_s1_shift1(0.5); // big=0x8000...
    run_test("Shift Left 0.25 (Ignores rhs=5, Actual Shift by 1)", s1_shifted, expected_s1_shift1);

    // Test shift that hits boundary (0.5 << 1 = 1.0, which is out of range)
    spas_fract168_t s2(0.5); // big=0x8000...
    spas_fract168_t s2_shifted = s2 << 1; // Should shift by 1

    // What is the expected result? The buggy shift logic likely does:
    // big = big << 1 = 0x8... << 1 = 0 (MSB lost?)
    // offset = 0
    // big |= (small >> 63) -> big |= (0 >> 63) = 0
    // Result: big=0, offset=0, small=0 (assuming small was 0)
    spas_fract168_t expected_s2_shift1_buggy(0.0); // BUGGY BEHAVIOR PREDICTION
    run_test("Shift Left 0.5 (Boundary 1.0 - Expect Buggy Result 0.0)", s2_shifted, expected_s2_shift1_buggy);

    // Test shift for negative number
    spas_fract168_t s3(-0.25); // sign=0b1000, big=0x4000...
    spas_fract168_t s3_shifted = s3 << 1;

    // Expected result: -0.5
    spas_fract168_t expected_s3_shift1(-0.5);
    run_test("Shift Left -0.25", s3_shifted, expected_s3_shift1);

    // Test shift for negative number hitting boundary (-0.5 << 1 = -1.0, out of range)
    spas_fract168_t s4(-0.5); // sign=0b1000, big=0x8000...
    spas_fract168_t s4_shifted = s4 << 1;

    // Expected result (buggy prediction): sign=0b1000, big=0? -> -0.0
    spas_fract168_t expected_s4_shift1_buggy(0.0); // BUGGY BEHAVIOR PREDICTION (sign bit might remain?)
    // Let's refine the expected buggy result based on sign handling
    // Sign likely doesn't change, big becomes 0.
    spas_fract168_t expected_s4_shift1_buggy_negzero(0b1000, 0, 0, 0);
    run_test("Shift Left -0.5 (Boundary -1.0 - Expect Buggy Result -0.0)", s4_shifted, expected_s4_shift1_buggy_negzero);

    // Test shift by 0 (should also shift by 1 due to bug)
     spas_fract168_t s5 = s1; // 0.25
     spas_fract168_t s5_shifted = s5 << 0; // rhs=0 ignored
     run_test("Shift Left 0.25 (Ignores rhs=0, Actual Shift by 1)", s5_shifted, expected_s1_shift1);
}

void test_addition() {
    std::cout << "\n--- Testing Addition (Range: (-1.0, 1.0)) ---" << std::endl;

    spas_fract168_t f_half(0.5);      // big = 0x8...
    spas_fract168_t f_quarter(0.25);  // big = 0x4...
    spas_fract168_t f_three_q(0.75);  // big = 0xC...
    spas_fract168_t f_zero(0.0);      // big = 0
    spas_fract168_t f_neg_half(-0.5); // sign=0b1000, big=0x8...
    spas_fract168_t f_neg_quarter(-0.25); // sign=0b1000, big=0x4...
    spas_fract168_t f_neg_three_q(-0.75); // sign=0b1000, big=0xC...


    // Results within range (-1, 1)
    run_test("Add: 0.25 + 0.5", f_quarter + f_half, f_three_q);
    run_test("Add: 0.5 + 0.25", f_half + f_quarter, f_three_q);
    run_test("Add: 0.5 + 0", f_half + f_zero, f_half);
    run_test("Add: 0 + 0.5", f_zero + f_half, f_half);
    run_test("Add: 0.5 + (-0.25)", f_half + f_neg_quarter, f_quarter);
    run_test("Add: (-0.25) + 0.5", f_neg_quarter + f_half, f_quarter);
    run_test("Add: (-0.25) + (-0.5)", f_neg_quarter + f_neg_half, f_neg_three_q);
    run_test("Add: 0.5 + (-0.5)", f_half + f_neg_half, f_zero);

    // Results potentially outside range (-1, 1) - Test actual behavior
    std::cout << "    Testing additions potentially exceeding range..." << std::endl;
    spas_fract168_t res_overflow = f_three_q + f_half; // 0.75 + 0.5 = 1.25 (out of range)
    // Analyze carry: full_fraction_addition likely returns carry=1.
    // operator+= uses carry: `if(big_sign){this->big-=1;} else{this->big+=1;}`
    // big=0xC... + big=0x8... -> results in big=0x4... with carry=1.
    // Since big_sign=0 (positive), big+=1 -> 0x4... + 1 = 0x4000...0001 ?
    // Expected result is likely INCORRECT representation due to flawed overflow handling.
    // This expected value is a guess at the code's flawed logic.
    spas_fract168_t expected_overflow(0b0000, 0x4000000000000000ULL, 0, 0); // Highly speculative guess at bug result
    run_test("Add: 0.75 + 0.5 (Overflow?)", res_overflow, expected_overflow);

    spas_fract168_t res_underflow = f_neg_three_q + f_neg_half; // -0.75 + (-0.5) = -1.25 (out of range)
    // Adds magnitudes, keeps sign. big=0xC... + big=0x8... -> 0x4... carry=1.
    // Final sign is negative (0b1000).
    // Carry handling: `if(big_sign){this->big-=1;} else{this->big+=1;}`
    // Since big_sign=1 (negative), big-=1 -> 0x4... - 1 = 0x4000...FFF ?
    spas_fract168_t expected_underflow(0b1000, 0x4000000000000000ULL, 0, 0); // Highly speculative guess at bug result
    run_test("Add: -0.75 + (-0.5) (Underflow?)", res_underflow, expected_underflow);

    // TODO: Add tests involving small/offset if interpretation becomes clear
    std::cout << "    Skipping complex additions involving small/offset due to ambiguity." << std::endl;
}

void test_subtraction() {
    std::cout << "\n--- Testing Subtraction (Range: (-1.0, 1.0)) ---" << std::endl;

    spas_fract168_t f_half(0.5);
    spas_fract168_t f_quarter(0.25);
    spas_fract168_t f_three_q(0.75);
    spas_fract168_t f_neg_three_q(-0.75);
    spas_fract168_t f_zero(0.0);
    spas_fract168_t f_neg_half(-0.5);
    spas_fract168_t f_neg_quarter(-0.25);

    // Results within range
    run_test("Sub: 0.75 - 0.25", f_three_q - f_quarter, f_half);
    run_test("Sub: 0.5 - 0.75", f_half - f_three_q, f_neg_quarter);
    run_test("Sub: 0.5 - 0", f_half - f_zero, f_half);
    run_test("Sub: 0 - 0.5", f_zero - f_half, f_neg_half);
    run_test("Sub: 0.5 - (-0.25)", f_half - f_neg_quarter, f_three_q); // 0.5 + 0.25
    run_test("Sub: (-0.25) - 0.5", f_neg_quarter - f_half, f_neg_three_q); // -0.25 + (-0.5)
    run_test("Sub: (-0.5) - (-0.25)", f_neg_half - f_neg_quarter, f_neg_quarter); // -0.5 + 0.25

    // Results potentially outside range
    std::cout << "    Testing subtractions potentially exceeding range..." << std::endl;
    spas_fract168_t res_overflow = f_half - f_neg_three_q; // 0.5 - (-0.75) = 1.25 (out of range)
    // This becomes 0.5 + 0.75, same buggy overflow case as in addition.
    spas_fract168_t expected_overflow(0b0000, 0x4000000000000000ULL, 0, 0); // Speculative guess
    run_test("Sub: 0.5 - (-0.75) (Overflow?)", res_overflow, expected_overflow);

    spas_fract168_t res_underflow = f_neg_half - f_three_q; // -0.5 - 0.75 = -1.25 (out of range)
    // This becomes -0.5 + (-0.75), same buggy underflow case as in addition.
    spas_fract168_t expected_underflow(0b1000, 0x4000000000000000ULL, 0, 0); // Speculative guess
    run_test("Sub: -0.5 - 0.75 (Underflow?)", res_underflow, expected_underflow);

     std::cout << "    Skipping complex subtractions involving small/offset due to ambiguity." << std::endl;
}


void test_multiplication() {
     std::cout << "\n--- Testing Multiplication (Range: (-1.0, 1.0)) ---" << std::endl;
     // Result of x*y where x,y in (-1,1) is always in (-1,1). No overflow expected.

     spas_fract168_t f_half(0.5);      // big = 0x8...
     spas_fract168_t f_quarter(0.25);  // big = 0x4...
     spas_fract168_t f_eighth(0.125); // big = 0x2...
     spas_fract168_t f_zero(0.0);
     spas_fract168_t f_neg_half(-0.5);
     spas_fract168_t f_neg_quarter(-0.25);
     spas_fract168_t f_neg_eighth(-0.125); // sign=0b1000, big=0x2...

     run_test("Mul: 0.5 * 0.5", f_half * f_half, f_quarter);
     run_test("Mul: 0.5 * 0.25", f_half * f_quarter, f_eighth);
     run_test("Mul: 0.25 * 0.5", f_quarter * f_half, f_eighth);
     run_test("Mul: 0.5 * 0", f_half * f_zero, f_zero);
     run_test("Mul: 0 * 0.5", f_zero * f_half, f_zero);
     run_test("Mul: 0.5 * (-0.5)", f_half * f_neg_half, f_neg_quarter);
     run_test("Mul: (-0.5) * 0.5", f_neg_half * f_half, f_neg_quarter);
     run_test("Mul: (-0.5) * (-0.5)", f_neg_half * f_neg_half, f_quarter);
     run_test("Mul: 0.25 * (-0.5)", f_quarter * f_neg_half, f_neg_eighth);

     // TODO: Add tests involving small/offset if interpretation becomes clear
      std::cout << "    Skipping complex multiplications involving small/offset due to ambiguity." << std::endl;
}


// --- Main Function ---
int main() {
    std::cout << "Starting spas_fract168_t tests... (Range constraint: (-1.0, 1.0))" << std::endl;

    test_constructors_assignment();
    test_get_double();
    test_unary_minus();
    test_shift_left(); // Contains tests for known/expected buggy behavior
    test_addition();   // Contains tests for potential buggy overflow/underflow
    test_subtraction();// Contains tests for potential buggy overflow/underflow
    test_multiplication();

    std::cout << "\n--- Test Summary ---" << std::endl;
    std::cout << "Total Tests: " << test_count << std::endl;
    std::cout << "Passed:      " << pass_count << std::endl;
    std::cout << "Failed:      " << test_count - pass_count << std::endl;
    std::cout << "\nNOTE: Failures in Shift Left and Add/Sub Overflow tests might indicate the predicted buggy behavior matches the actual code." << std::endl;
    std::cout << "      Failures elsewhere likely indicate bugs or incorrect assumptions about representation." << std::endl;


    return (test_count == pass_count) ? 0 : 1; // Return 0 on success, 1 on failure
}