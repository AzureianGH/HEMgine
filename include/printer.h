#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
void INCVGAIndex();
int GetVGAIndex();
void SetVGAIndex(int index);
Color VGAColorToColor(uint8_t color);
void VGAPrintChar(char text, int x, int y, uint8_t color);
void VGAPrint(std::string text, int x, int y, uint8_t color);
