#include <printer.h>
Font VGAFONTHD = LoadFontEx("resource/MD.ttf", 16, NULL, 0);
Color VGAColorToColor(uint8_t color)
{
    uint8_t FirstByte = color & 0xF;
    uint8_t SecondByte = (color & 0xF0) >> 4;
    //first byte is background color, second byte is foreground color
    return (Color){FirstByte * 16, FirstByte * 16, FirstByte * 16, 255};
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
    DrawTextEx(VGAFONTHD, &text, (Vector2){x, y}, 16, 0, VGAColorToColor(color));
    VGAIndex++;
}

void VGAPrint(std::string text, int x, int y, uint8_t color)
{
    DrawTextEx(VGAFONTHD, text.c_str(), (Vector2){x, y}, 16, 0, VGAColorToColor(color));
    VGAIndex += text.length();
}