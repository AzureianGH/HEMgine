#include <printer.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <debuggerprint.h>

#define VGABLACK CLITERAL(Color){ 0, 0, 0, 255 }
#define VGABLUE CLITERAL(Color){0, 0, 170, 255}
#define VGAGREEN CLITERAL(Color){0, 170, 0, 255}
#define VGACYAN CLITERAL(Color){0, 170, 170, 255}
#define VGARED CLITERAL(Color){170, 0, 0, 255}
#define VGAMAGENTA CLITERAL(Color){170, 0, 170, 255}
#define VGABROWN CLITERAL(Color){170, 85, 0, 255}
#define VGALIGHTGRAY CLITERAL(Color){170, 170, 170, 255}
#define VGADARKGRAY CLITERAL(Color){85, 85, 85, 255}
#define VGALIGHTBLUE CLITERAL(Color){85, 85, 255, 255}
#define VGALIGHTGREEN CLITERAL(Color){85, 255, 85, 255}
#define VGALIGHTCYAN CLITERAL(Color){85, 255, 255, 255}
#define VGALIGHTRED CLITERAL(Color){255, 85, 85, 255}
#define VGALIGHTMAGENTA CLITERAL(Color){255, 85, 255, 255}
#define VGAYELLOW CLITERAL(Color){255, 255, 85, 255}
#define VGAWHITE CLITERAL(Color){255, 255, 255, 255}

using namespace std;
Font VGAFONTHD;
char VGABuffer[2000] = {0};
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
        VGAColor.FG = VGABLACK;
        break;
    case 1:
        VGAColor.FG = VGABLUE;
        break;
    case 2:
        VGAColor.FG = VGAGREEN;
        break;
    case 3:
        VGAColor.FG = VGACYAN;
        break;
    case 4:
        VGAColor.FG = VGARED;
        break;
    case 5:
        VGAColor.FG = VGAMAGENTA;
        break;
    case 6:
        VGAColor.FG = VGABROWN;
        break;
    case 7:
        VGAColor.FG = VGALIGHTGRAY;
        break;
    case 8:
        VGAColor.FG = VGADARKGRAY;
        break;
    case 9:
        VGAColor.FG = VGALIGHTBLUE;
        break;
    case 10:
        VGAColor.FG = VGALIGHTGREEN;
        break;
    case 11:
        VGAColor.FG = VGALIGHTCYAN;
        break;
    case 12:
        VGAColor.FG = VGALIGHTRED;
        break;
    case 13:
        VGAColor.FG = VGALIGHTMAGENTA;
        break;
    case 14:
        VGAColor.FG = VGAYELLOW;
        break;
    case 15:
        VGAColor.FG = VGAWHITE;
        break;
    }
    switch (bg)
    {
    case 0:
        VGAColor.BG = VGABLACK;
        break;
    case 1:
        VGAColor.BG = VGABLUE;
        break;
    case 2:
        VGAColor.BG = VGAGREEN;
        break;
    case 3:
        VGAColor.BG = VGACYAN;
        break;
    case 4:
        VGAColor.BG = VGARED;
        break;
    case 5:
        VGAColor.BG = VGAMAGENTA;
        break;
    case 6:
        VGAColor.BG = VGABROWN;
        break;
    case 7:
        VGAColor.BG = VGALIGHTGRAY;
        break;
    case 8:
        VGAColor.BG = VGADARKGRAY;
        break;
    case 9:
        VGAColor.BG = VGALIGHTBLUE;
        break;
    case 10:
        VGAColor.BG = VGALIGHTGREEN;
        break;
    case 11:
        VGAColor.BG = VGALIGHTCYAN;
        break;
    case 12:
        VGAColor.BG = VGALIGHTRED;
        break;
    case 13:
        VGAColor.BG = VGALIGHTMAGENTA;
        break;
    case 14:
        VGAColor.BG = VGAYELLOW;
        break;
    case 15:
        VGAColor.BG = VGAWHITE;
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
        //draw rect with 9 width and 16 height
        DrawRectangle(i * 9, 0, 9, 16, VGAColor.BG);
        DrawTextEx(VGAFONTHD, &text, {(float)i * 9, 0}, 16, 1, VGAColor.FG);
    }
}