#include <iostream>
#include <fstream>
#include "windows.h"
using namespace std;

#define IMAGE_SIZE 256

char bits[IMAGE_SIZE][IMAGE_SIZE] = { 0 };

int main() {
    BITMAPFILEHEADER bmfh;
    BITMAPINFOHEADER bmih;
    char colorTable[1024];

    cout << "Enter two pairs of point coordinates in the range of 0 to 255.\n";

    int y0, x0, y1, x1;
    cin >> y0 >> x0 >> y1 >> x1;

    //input validation, output error message for each invalid input and end program after checking all 4
    bool invalid = false;
    if (y0 < 0 || y0 >= IMAGE_SIZE) {
        cout << "Value " << y0 << " out of range, ending." << endl;
        invalid = true;
    }
    if (x0 < 0 || x0 >= IMAGE_SIZE) {
        cout << "Value " << x0 << " out of range, ending." << endl;
        invalid = true;
    }
    if (y1 < 0 || y1 >= IMAGE_SIZE) {
        cout << "Value " << y1 << " out of range, ending." << endl;
        invalid = true;
    }
    if (x1 < 0 || x1 >= IMAGE_SIZE) {
        cout << "Value " << x1 << " out of range, ending." << endl;
        invalid = true;
    }
    if (invalid) return -1;

    //build grayscale array of bits for gradient
    for (int i = 0; i < IMAGE_SIZE; i++)
        for (int j = 0; j < IMAGE_SIZE; j++)
            bits[i][j] = (j == 255) ? 255 : j + 1;

    //build colortable with offset for custom color, index 0 for aqua, 1-255 for grayscale gradient
    //sacrificed one shade of gray to allow for custom color, any other way to do this in 8bit?
    for (int i = 0; i < 256; i++) {
        int j = i * 4;
        if (i == 0) {
            colorTable[j] = 255;
            colorTable[j + 1] = 255;
            colorTable[j + 2] = 0;
            colorTable[j + 3] = 0;
        }
        else {
            colorTable[j] = colorTable[j + 1] = colorTable[j + 2] = i - 1;
            colorTable[j + 3] = 0;
        }
    }

    int deltax, deltay, stepx, stepy, err, e2;
    int currentX = x0;
    int currentY = y0;

    __asm {
        ; x - axis absolute difference
        mov eax, x1
        sub eax, x0
        jns dx_ok
        neg eax
        dx_ok :
        mov dword ptr[deltax], eax; put absolute difference of x1 and x0 into deltax using memory location

            ; y - axis absolute difference
            mov eax, y1
            sub eax, y0
            jns dy_ok
            neg eax
            dy_ok :
        mov dword ptr[deltay], eax; put absolute difference of y1 and y0 into deltay using memory location

            ; x - axis step calculation
            mov eax, x0
            cmp eax, x1
            jl sx_pos
            mov dword ptr[stepx], -1; decrement x if x0 >= x1->move left
            jmp done_sx
            sx_pos :
        mov dword ptr[stepx], 1; increment x if x0 < x1->move right
            done_sx :

        ; y - axis step calculation
            mov eax, y0
            cmp eax, y1
            jl sy_pos
            mov dword ptr[stepy], -1; decrement if y0 >= y1->move up
            jmp done_sy
            sy_pos :
        mov dword ptr[stepy], 1; increment if y0 < y1->move down
            done_sy :

        ; initialize error->deltax - deltay
            mov eax, dword ptr[deltax]
            sub eax, dword ptr[deltay]
            mov dword ptr[err], eax; store difference of deltax and deltay in err using memory location
    }

    while (true) {
        __asm {
            ; check bounds of Y coordinate, skip if out of bounds 0 > Y >= 256
            mov eax, currentY
            cmp eax, 0
            jl skip_plot
            cmp eax, IMAGE_SIZE
            jge skip_plot

            ; check bounds of X coordinate, skip if out of bounds 0 > X >= 256
            mov ebx, currentX
            cmp ebx, 0
            jl skip_plot
            cmp ebx, IMAGE_SIZE
            jge skip_plot

            ; calculate memory address of pixel at coordinates(currentX, currentY)
            mov ecx, eax; ecx = currentY
            imul ecx, IMAGE_SIZE; ecx = currentY * 256
            add ecx, ebx; ecx = (currentY * 256 + currentX)
            mov edi, offset bits; edi = point address to start of bits[0, 0], adjust starting point using offset
            add edi, ecx; move edi to correct pixel
            mov byte ptr[edi], 0; write 0 (aqua)to memory address stored in edi, set pixel to line color

            ; line thickening, fill x + 1, x - 1, y + 1, y - 1 to create plus sign shape -> 5 pixels total, 4 surrounding coordinate
            ; Pixel at currentX + 1
            cmp ebx, IMAGE_SIZE - 1; check if close to bounds
            jge skip_px1
            mov ecx, eax
            imul ecx, IMAGE_SIZE
            add ecx, ebx
            inc ecx
            mov edi, offset bits
            add edi, ecx
            mov byte ptr[edi], 0; write 0 to address of bits[y][x + 1]
            skip_px1:

            ; Pixel at currentX - 1
                cmp ebx, 0; check if close to bounds
                jle skip_px2
                mov ecx, eax
                imul ecx, IMAGE_SIZE
                add ecx, ebx
                dec ecx
                mov edi, offset bits
                add edi, ecx
                mov byte ptr[edi], 0; write 0 to address of bits[y][x - 1]
                skip_px2:

            ; Pixel at currentY + 1
                cmp eax, IMAGE_SIZE - 1; check if close to bounds
                jge skip_py1
                mov ecx, eax
                inc ecx
                imul ecx, IMAGE_SIZE
                add ecx, ebx
                mov edi, offset bits
                add edi, ecx
                mov byte ptr[edi], 0; write 0 to address of bits[y + 1][x]
                skip_py1:

            ; Pixel at currentY - 1
                cmp eax, 0; check if close to bounds
                jle skip_py2
                mov ecx, eax
                dec ecx
                imul ecx, IMAGE_SIZE
                add ecx, ebx
                mov edi, offset bits
                add edi, ecx
                mov byte ptr[edi], 0; write 0 to address of bits[y - 1][x]
                skip_py2:

        skip_plot:
        }

        if (currentX == x1 && currentY == y1) break;

        __asm {
            ; calculate doubled error term e2, decide if moving in x direction or y direction
            mov eax, dword ptr[err]; put err into eax
            mov edx, eax; copy eax into edx->err = eax, edx
            shl eax, 1; shift left by 1->multiplying eax by 2, eax = 2 * err
            mov dword ptr[e2], eax; e2 = 2 * err, using memory address

            cmp edx, -1; if e2 <= -1 skip x - step
            jle skip_x
            mov eax, dword ptr[err]
            sub eax, dword ptr[deltay]; err -= deltay
            mov dword ptr[err], eax
            mov eax, currentX
            add eax, dword ptr[stepx]; currentX = +stepx, step in x - direction + 1 or -1
            mov currentX, eax
            skip_x :

            cmp edx, dword ptr[deltax]; if e2 >= deltax skip y - step
                jge skip_y
                mov eax, dword ptr[err]
                add eax, dword ptr[deltax]; err += deltax
                mov dword ptr[err], eax
                mov eax, currentY
                add eax, dword ptr[stepy]; currentY = +stepy, step in y - direction + 1 or -1
                mov currentY, eax
                skip_y :
        }
    }

    ofstream bmpOut("line.bmp", ios::out | ios::binary);
    if (!bmpOut) {
        cout << "...could not open file, ending.\n";
        return -1;
    }

    bmfh.bfType = 0x4D42;
    bmfh.bfReserved1 = 0;
    bmfh.bfReserved2 = 0;
    bmfh.bfOffBits = sizeof(bmfh) + sizeof(bmih) + sizeof(colorTable);
    bmfh.bfSize = bmfh.bfOffBits + sizeof(bits);

    bmih.biSize = 40;
    bmih.biWidth = IMAGE_SIZE;
    bmih.biHeight = IMAGE_SIZE;
    bmih.biPlanes = 1;
    bmih.biBitCount = 8;
    bmih.biCompression = 0;
    bmih.biSizeImage = IMAGE_SIZE * IMAGE_SIZE;
    bmih.biXPelsPerMeter = 2835;
    bmih.biYPelsPerMeter = 2835;
    bmih.biClrUsed = 256;
    bmih.biClrImportant = 0;

    bmpOut.write((char*)&bmfh, sizeof(bmfh));
    bmpOut.write((char*)&bmih, sizeof(bmih));
    bmpOut.write(colorTable, sizeof(colorTable));
    bmpOut.write(&bits[0][0], IMAGE_SIZE * IMAGE_SIZE);
    bmpOut.close();

    system("mspaint line.bmp");
    return 0;
}
