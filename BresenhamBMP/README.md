# Bresenham BMP Line Renderer

A C++ program that draws a line between two points using Bresenham's line algorithm, with the core algorithm written in **x86 inline assembly**. The result is rasterized into a 256x256 8-bit BMP file using hand-built binary file output.

Written for CS230 (Systems Programming), Spring 2025.

## What It Does

1. Prompts for two coordinate pairs in the range 0 to 255
2. Validates all four values and reports each one that is out of range
3. Builds a 256x256 grayscale gradient background and a custom color table (index 0 is aqua, the rest are grays)
4. Runs Bresenham's algorithm in inline assembly to step from the first point to the second, plotting each pixel with bounds checks
5. Thickens the line into a plus shape (5 pixels per step) so it is visible at 256x256
6. Writes the BMP headers, color table, and pixel data to `foo.bmp` and opens it in MS Paint

## Concepts Demonstrated

- Bresenham's line-drawing algorithm (integer-only, incremental error term)
- x86 inline assembly (MSVC `__asm`): register use, conditional jumps, and direct memory addressing
- Manual pixel address calculation in a 2D byte array
- BMP file format construction: `BITMAPFILEHEADER`, `BITMAPINFOHEADER`, and an 8-bit palette
- Binary file I/O and input validation

## Input Format

Coordinates are entered in **row, column order**:

```
y0 x0 y1 x1
```

For example, `10 20 200 240` draws a line from row 10, column 20 to row 200, column 240. Because BMP files are stored bottom-up, row 0 is the bottom edge of the image.

## Build and Run

**Requirements:** Windows and Microsoft Visual Studio. This program uses `windows.h` and launches MS Paint, so it will not compile on macOS or Linux.

**Important:** Build with the platform set to **x86 (Win32)**. MSVC does not support inline `__asm` blocks in x64 builds.

1. Create a new C++ Console App project in Visual Studio
2. Add `bresenham_bmp.cpp` to the project
3. Set the platform dropdown to **x86**
4. Build and run (Debug or Release)

When run, the program asks for the four coordinates, writes `foo.bmp` to the working directory, and opens it in MS Paint.

## Output

A 256x256 BMP with a left-to-right black-to-white gradient background and the requested line drawn in aqua.

## Notes

- No floating-point math is used. The line is traced with integer error accumulation, consistent with the original Bresenham algorithm.
- Pixel writes are bounds-checked before any memory is touched, including the extra pixels used to thicken the line.
- The plotting loop and the per-step error update are separate assembly blocks, with the loop exit condition handled in C++.
