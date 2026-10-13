# Float Decoder

A C++ program that takes four hex digits, treats them as a 16-bit half-precision float (IEEE 754 binary16), breaks out the sign, exponent, and significand, and converts it to a decimal value.

## Binary16 Format

A half-precision float is 16 bits:

```
| sign (1 bit) | exponent (5 bits) | significand (10 bits) |
```

For a normal number the value is:

```
(-1)^sign * 2^(exponent - 15) * (1 + significand / 1024)
```

## What It Does

1. Asks for four hex digits (upper or lower case) and checks the input
2. Pulls out the sign, exponent, and significand using bit masks and shifts, and prints each one in binary
3. Checks for the special cases first (see below)
4. Otherwise calculates the value and prints it
5. Asks if you want to decode another one

### Special Cases

| Input | Output |
|---|---|
| Exponent 0, significand 0 | `Zero` |
| Exponent all 1s, significand 0 | `Positive Infinity` or `Negative Infinity`, based on the sign |
| Exponent all 1s, significand not 0 | The infinity message, then `...significand should be zero.` |
| Exponent 0, significand not 0 | `Subnormal numbers are not allowed` |

## Sample Session

```
Enter four hexadecimal digits: 
3555

Sign: 0 Exponent: 01101 Significand: 0101010101
Value: 0.333252

Continue? Enter Y or N: Y

Enter four hexadecimal digits: 
C000

Sign: 1 Exponent: 10000 Significand: 0000000000
Value: -2

Continue? Enter Y or N: Y

Enter four hexadecimal digits: 
7C00

Sign: 0 Exponent: 11111 Significand: 0000000000
Positive Infinity

Continue? Enter Y or N: N

Done, program ending.
```

## Concepts Demonstrated

- IEEE 754 half-precision layout (sign, biased exponent, significand)
- Bit masking and shifting
- Handling special cases (zero, infinity, subnormal)
- Input validation
- `std::bitset` for printing binary

## Build and Run

I wrote this in Visual Studio on Windows. It also builds with g++ on a Mac:

```
g++ -std=c++11 float_decoder.cpp -o float_decoder
./float_decoder
```

On Linux, add `#include <cmath>` and `#include <cstring>` at the top of `float_decoder.cpp` first, since the compiler there doesn't pull them in automatically.

## Notes

- This only decodes. It doesn't convert a decimal number into half-precision.
- Subnormal numbers aren't decoded, and a zero doesn't keep its sign (`0000` and `8000` both print `Zero`).
- Typing more than four characters overflows the input buffer.
