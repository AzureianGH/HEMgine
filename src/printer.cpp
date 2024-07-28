#include <printer.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <debuggerprint.h>
using namespace std;
Font VGAFONTHD;
char VGABuffer[2000];
uint8_t VGABufferColor[2000];
void InitVGAPrinter()
{
    VGAFONTHD = LoadFontEx("resource/MD.ttf", 16, NULL, 0);
}

VGAColor_t VGAColorToColor(uint8_t color)
{
    uint8_t bg = (color & 0xF0) >> 4;
    uint8_t fg = color & 0xF;
    VGAColor_t VGAColor;
    switch (fg)
    {
    case 0:
        VGAColor.FG = BLACK;
        break;
    case 1:
        VGAColor.FG = BLUE;
        break;
    case 2:
        VGAColor.FG = GREEN;
        break;
    case 3:
        VGAColor.FG = CYAN;
        break;
    case 4:
        VGAColor.FG = RED;
        break;
    case 5:
        VGAColor.FG = MAGENTA;
        break;
    case 6:
        VGAColor.FG = BROWN;
        break;
    case 7:
        VGAColor.FG = LIGHTGRAY;
        break;
    case 8:
        VGAColor.FG = DARKGRAY;
        break;
    case 9:
        VGAColor.FG = LIGHTBLUE;
        break;
    case 10:
        VGAColor.FG = LIGHTGREEN;
        break;
    case 11:
        VGAColor.FG = LIGHTCYAN;
        break;
    case 12:
        VGAColor.FG = LIGHTRED;
        break;
    case 13:
        VGAColor.FG = LIGHTMAGENTA;
        break;
    case 14:
        VGAColor.FG = YELLOW;
        break;
    case 15:
        VGAColor.FG = WHITE;
        break;
    }
    switch (bg)
    {
    case 0:
        VGAColor.BG = BLACK;
        break;
    case 1:
        VGAColor.BG = BLUE;
        break;
    case 2:
        VGAColor.BG = GREEN;
        break;
    case 3:
        VGAColor.BG = CYAN;
        break;
    case 4:
        VGAColor.BG = RED;
        break;
    case 5:
        VGAColor.BG = MAGENTA;
        break;
    case 6:
        VGAColor.BG = BROWN;
        break;
    case 7:
        VGAColor.BG = LIGHTGRAY;
        break;
    case 8:
        VGAColor.BG = DARKGRAY;
        break;
    case 9:
        VGAColor.BG = LIGHTBLUE;
        break;
    case 10:
        VGAColor.BG = LIGHTGREEN;
        break;
    case 11:
        VGAColor.BG = LIGHTCYAN;
        break;
    case 12:
        VGAColor.BG = LIGHTRED;
        break;
    case 13:
        VGAColor.BG = LIGHTMAGENTA;
        break;
    case 14:
        VGAColor.BG = YELLOW;
        break;
    case 15:
        VGAColor.BG = WHITE;
        break;
    }
    return VGAColor;
}

int VGAIndex = 0;
void INCVGAIndex()
{
    VGAIndex++;
}

int GetVGAIndex()
{
    return VGAIndex;
}

void SetVGAIndex(int index)
{
    VGAIndex = index;
}

void VGAPrintChar(char text, int x, int y, uint8_t color)
{
    VGABuffer[VGAIndex] = text;
    VGABufferColor[VGAIndex] = color;
    VGAIndex++;
}

void VGAPrint(std::string text, int x, int y, uint8_t color)
{
    for (int i = 0; i < text.length(); i++)
    {
        VGABuffer[VGAIndex] = text[i];
        VGABufferColor[VGAIndex] = color;
        VGAIndex++;
    }
}

void PrintVGABuffer()
{
    for (int i = 0; i < VGAIndex; i++)
    {
        //get first byte of buffer and color
        char text = VGABuffer[i];
        uint8_t color = VGABufferColor[i];
        VGAColor_t VGAColor = VGAColorToColor(color);
        DrawRectangle(i * 16, 0, 9, 16, VGAColor.BG);
        DrawTextEx(VGAFONTHD, &text, {(float)i * 9, 0}, 16, 1, VGAColor.FG);
    }
}