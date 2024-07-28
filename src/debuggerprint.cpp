#include <debuggerprint.h>

std::string Colorstrings[] = {"Black", "Blue", "Green", "Cyan", "Red", "Magenta", "Brown", "Light Gray", "Dark Gray", "Light Blue", "Light Green", "Light Cyan", "Light Red", "Light Magenta", "Yellow", "White"};

std::string ColorToString(uint8_t color)
{
    // format: "foreground on background"
    std::string colorstring = "";
    uint8_t FirstByte = color & 0xF;
    uint8_t SecondByte = (color & 0xF0) >> 4;
    //convert foreground to string
     colorstring += Colorstrings[FirstByte];
     colorstring += " on ";
    colorstring += Colorstrings[SecondByte];
    
    //convert background to string
   
    return colorstring;
}
std::string IntToHexString(uint32_t integer)
{
    //no trailing 0's unless its only one 0-F
    std::string hexstring = "0x";
    std::string hexchars = "0123456789ABCDEF";
    bool foundnonzero = false;
    for (int i = 7; i >= 0; i--)
    {
        uint8_t hexchar = (integer >> (i * 4)) & 0xF;
        if (hexchar != 0 || foundnonzero)
        {
            foundnonzero = true;
            hexstring += hexchars[hexchar];
        }
    }
    if (!foundnonzero)
    {
        hexstring += "0";
    }
    return hexstring;
}