# Bresenham BMP Line Renderer

This program implements Bresenham’s line-drawing algorithm to rasterize a line
directly into a BMP image file using low-level binary file output in C++.

The program prompts the user for two coordinate pairs, computes the line using
integer-only arithmetic, and writes the resulting image to disk.

## Concepts Demonstrated

- Bresenham line-drawing algorithm
- Integer-based rasterization
- Binary file I/O
- BMP file format construction
- Low-level graphics algorithms

## Build and Run

This program was developed and tested using Microsoft Visual Studio.

### Build
- Create a new C++ console project
- Add `bresenham_bmp.cpp` to the project
- Build in Debug or Release mode

### Run
When executed, the program:
1. Opens a console window
2. Prompts the user for two sets of (x, y) coordinates
3. Generates a BMP image containing the rasterized line
4. Writes the BMP file to the program’s working directory

## Output

- A BMP image file is generated in the project directory
- The image can be opened using standard image viewers (e.g., MS Paint)

## Notes

This implementation avoids floating-point arithmetic and uses integer
incremental error calculations, consistent with the original Bresenham algorithm.
