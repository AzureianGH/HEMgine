#include <idt.h>
#include <iostream>
#include <basics.h>
#include <printer.h>
using namespace std;

IDTEntry_t BIOSIDT[BIOS_IDT_SIZE];
bool Haltflag = false;

/// @brief Called with 0x13 interrupt
void BIOSInterruptCall()
{
    if (GetRegister32(RA) == 0x0)
    {
        if (GetRegister32(RB) == 0x0)
        {
            // if RB is 0x0 then print character
            //80x25 text mode, using VGAIndex
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
    return;
}
void TripleFault()
{
    //set console color to red with printf
    printf("\033[1;31m");
    printf("Triple Fault Occured, System Halted\n");
    printf("\033[0m");
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
}

void SetIDTEntry(uint8_t index, int MemoryAddress)
{
    BIOSIDT[index].MemoryAddress = MemoryAddress;
    BIOSIDT[index].SegmentSelector = 0x0;
    BIOSIDT[index].Flags = 0x0;
    BIOSIDT[index].Zero = 0x0;   
}

void CallInterrupt(uint8_t interrupt)
{
    //if interrupt handler is not set, call NULL handler
    if (BIOSIDT[interrupt].MemoryAddress == 0x0)
    {
        NULLInterruptCall();
    }
    //double fault
    else if (interrupt == 0x8)
    {
        //check if the double fault handler is set
        if (BIOSIDT[0x8].MemoryAddress == 0x0)
        {
            TripleFault();
        }
    }
    else if (interrupt == 0x13)
    {
        BIOSInterruptCall();
    }
    return;
}
