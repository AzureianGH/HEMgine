#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
typedef struct VGACOLOR_S
{
    Color FG;
    Color BG;
} VGAColor_t;
void InitVGAPrinter();
void INCVGAIndex();
int GetVGAIndex();
void SetVGAIndex(int index);
VGAColor_t VGAColorToColor(uint8_t color);
void VGAPrintChar(char text, int x, int y, uint8_t color);
void VGAPrint(std::string text, int x, int y, uint8_t color);
void PrintVGABuffer();