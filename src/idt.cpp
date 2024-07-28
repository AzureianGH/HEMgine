#include <idt.h>
#include <iostream>
#include <basics.h>
#include <printer.h>
#include <debuggerprint.h>
using namespace std;
#define DEBUGVGA
IDTEntry_t BIOSIDT[BIOS_IDT_SIZE];
bool Haltflag = false;

/// @brief Called with 0x13 interrupt
void BIOSInterruptCall()
{
    if (GetRegister32(RA) == 0x0)
    {
        if (GetRegister32(RB) == 0x0)
        {
            // print character to screen at VGAIndex, 80 characters per line, 25 lines
            VGAPrintChar(GetRegister32(RC), GetVGAIndex() % 80, GetVGAIndex() / 80, GetRegister32(RD));

            SetRegister32(CIP, GetRegister32(CIP) + 1);
            return;
        }
        else if (GetRegister32(RB) == 0x1)
        {
            // set VGAIndex to RC
            SetVGAIndex(GetRegister32(RC));
            SetRegister32(CIP, GetRegister32(CIP) + 1);
            return;
        }
        else if (GetRegister32(RB) == 0x2)
        {
            // if RB is 0x2 then clear screen
            ClearBackground({0, 0, 0, 255});
            SetVGAIndex(0);
            SetRegister32(CIP, GetRegister32(CIP) + 1);
            return;
        }
        
    }

    cout << "BIOS Interrupt Called" << endl;
    return;
}
void NULLInterruptCall()
{
    cout << "NULL Interrupt Called" << endl;
    //dump registers
    for (int i = 0; i < 12; i++)
    {
        cout << "Register " << i << ": " << GetRegister32((CPURegister32_t)i) << endl;
    }
    //call double fault handler
    CallInterrupt(0x8); // Double Fault
    return;
}

void TripleFault()
{
    //set console color to red with printf
    printf("\033[1;31m");
    printf("Triple Fault Occured, System Halted\n");
    printf("\033[0m");
    VGAPrint("CPU ENTERED SHUTDOWN STATE, VM HALTED.", 0, 0, 0x0C);
    Haltflag = true;
    return;
}
void InitDefaultBIOSIDT() // First memory addresses are reserved for BIOS interrupts 0-255
{
    for (int i = 0; i < BIOS_IDT_SIZE; i++)
    {
        SetIDTEntry(i, 0x0);
    }
    //set the default bios interrupt 0x13
    SetIDTEntry(0x13, 0x13);

    //set the default bios interrupt 0x8
    SetIDTEntry(0x8, 0x8);

    //set int flag
    SetFlag(IF, 1);
}

void SetIDTEntry(uint8_t index, int MemoryAddress)
{
    BIOSIDT[index].MemoryAddress = MemoryAddress;
    BIOSIDT[index].SegmentSelector = 0x0;
    BIOSIDT[index].Flags = 0x0;
    BIOSIDT[index].Zero = 0x0;   
}

void DisableInterrupts()
{
    SetFlag(IF, 0);
    return;
}

void EnableInterrupts()
{
    SetFlag(IF, 1);
    return;
}

void CallInterrupt(uint8_t interrupt)
{
    //check int flag
    if (GetFlag(IF) == 0)
    {
        cout << "Interrupts Disabled" << endl;
        return;
    }
    #ifdef DEBUGVGA
        cout << "Interrupt Called: " << IntToHexString((int)interrupt) << endl;
        //print RA-RD and if any are a valid character, print the character next to the number with ('A') format
        for (int i = 1; i < 5; i++)
        {
            cout << "Register " << CPURegisterNames32[i] << ": " << IntToHexString(GetRegister32((CPURegister32_t)i));
            if (GetRegister32((CPURegister32_t)i) >= 0x20 && GetRegister32((CPURegister32_t)i) <= 0x7E)
            {
                cout << " ('" << (char)GetRegister32((CPURegister32_t)i) << "')";
            }
            //check if the hex is a valid 0-255, if so ColorToString in []
            if (GetRegister32((CPURegister32_t)i) >= 0x0 && GetRegister32((CPURegister32_t)i) <= 0xFF)
            {
                cout << " [" << ColorToString(GetRegister32((CPURegister32_t)i)) << "]";
            }
            cout << endl;
        }
    #endif

    //if interrupt handler is not set, call NULL handler
    if (BIOSIDT[interrupt].MemoryAddress == 0x0)
    {
        NULLInterruptCall();
    }
    //double fault
    else if (interrupt == 0x8)
    {
        TripleFault();
    }
    else if (interrupt == 0x13)
    {
        BIOSInterruptCall();
    }
    return;
}
