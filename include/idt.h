#pragma once

#include <cstdint>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// BIOS IDT
#define BIOS_IDT_SIZE 256 // 256 IDT entries (0-255)

//define descriptor table
typedef struct IDTENTRY_S
{
    int MemoryAddress;
    int SegmentSelector;
    int Flags;
    int Zero;
} IDTEntry_t;

//define IDT
extern IDTEntry_t BIOSIDT[BIOS_IDT_SIZE]; // Extern because it is defined in idt.cpp
extern bool Haltflag;
//define IDT functions
void InitDefaultBIOSIDT();
void SetIDTEntry(uint8_t index, int MemoryAddress);
void CallInterrupt(uint8_t interrupt);