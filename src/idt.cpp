#include <idt.h>
#include <iostream>
#include <basics.h>
#include <printer.h>
#include <debuggerprint.h>
#include <vcpu.h>
using namespace std;
#define DEBUGVGA
#define DEBUGIDT
IDTEntry_t BIOSIDT[BIOS_IDT_SIZE];
bool Haltflag = false;

/// @brief Called with 0x13 interrupt
void BIOSInterruptCall()
{
    if (Haltflag) return;
    if (GetRegister32(RA) == 0x1)
    {
        if (GetRegister32(RB) == 0x0)
        {
            // print character to screen at VGAIndex, 80 characters per line, 25 lines
            VGAPrintChar(GetRegister32(RC), GetVGAIndex() % 80, GetVGAIndex() / 80, GetRegister32(RD));
            return;
        }
        else if (GetRegister32(RB) == 0x1)
        {
            //if higher than 2000
            if (GetRegister32(RC) > 2000)
            {
                CallInterrupt(0xD); // General Protection Fault
                return;
            }
            if (GetRegister32(RC) < 0)
            {
                CallInterrupt(0xD); // General Protection Fault
                return;
            }
            // set VGAIndex to RC
            SetVGAIndex(GetRegister32(RC));
            
            return;
        }
        else if (GetRegister32(RB) == 0x2)
        {
            // if RB is 0x2 then clear screen
            for (int i = 0; i < 80 * 25; i++)
            {
                VGAPrintChar(' ', i % 80, i / 80, 0x0);
            }
            SetVGAIndex(0);
            
            return;
        }
        
    }

    cout << "BIOS Interrupt Called" << endl;
    return;
}
#undef DEBUGIDT
void DivByZero()
{
    VGAPrint("Divide By Zero Exception", 0, 0, 0x0C);
    Haltflag = true;
    return;
}
void NULLInterruptCall()
{
    
    #ifdef DEBUGIDT
        cout << "NULL Interrupt Called" << endl;
        //dump registers
        for (int i = 0; i < 12; i++)
        {
            cout << "Register " << i << ": " << GetRegister32((CPURegister32_t)i) << endl;
        }
    #endif
    //call double fault handler
    CallInterrupt(0x8); // Double Fault
    return;
}

void Debug()
{
    DumpStack();
    DumpMemory(0x0, 0xA5);
    //Dump the registers
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
    SetWindowTitle("HEMgine - Shutdown");
    Debug();
    return;
}
void InitDefaultBIOSIDT() // First memory addresses are reserved for BIOS interrupts 0-255
{
    for (int i = 0; i < BIOS_IDT_SIZE; i++)
    {
        SetIDTEntry(i, 0x0);
    }
    //set the default bios interrupt 0x13
    SetIDTEntry(0x15, 0x15);

    //set the default bios interrupt 0x8
    SetIDTEntry(0x8, 0x8);

    //set the default bios interrupt 0x0
    SetIDTEntry(0x0, 0x0);

    //set the debug interrupt 0x03
    SetIDTEntry(0x3, 0x3);

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
std::string Exceptions[] = {
    "Division By Zero", // 0x0
    "Debug", // 0x1
    "Non Maskable Interrupt", // 0x2
    "Breakpoint", // 0x3
    "Overflow", // 0x4
    "Bound Range Exceeded", // 0x5
    "Invalid Opcode", // 0x6
    "Device Not Available", // 0x7
    "Double Fault", // 0x8
    "Coprocessor Segment Overrun", 
    "Invalid TSS", // 0xA
    "Segment Not Present", // 0xB
    "Stack Segment Fault", // 0xC
    "General Protection Fault", // 0xD
    "Page Fault", // 0xE
    "Unknown Interrupt", // 0xF
    "Coprocessor Fault", // 0x10
    "Alignment Check", // 0x11
    "Machine Check", // 0x12
    "SIMD Floating Point Exception", // 0x13
    "Virtualization Exception", // 0x14
    "BIOS Interrupt", // 0x15
    "Reserved", // 0x16
    "Reserved", // 0x17
    "Reserved", // 0x18
    "Reserved", // 0x19
    "Reserved", // 0x1A
    "Reserved", // 0x1B
    "Hypervisor Injection Exception", // 0x1C
    "VMM Communication Exception", // 0x1D
    "Security Exception", // 0x1E
    "Reserved" // 0x1F
};
void CallInterrupt(uint8_t interrupt)
{
    if (Haltflag) return;
    //check int flag
    if (GetFlag(IF) == 0)
    {
        cout << "Interrupts Disabled" << endl;
        return;
    }
    #ifdef DEBUGVGA
        cout << "Interrupt Called: " << IntToHexString((int)interrupt) << " [" << Exceptions[(int)interrupt] << "]" << endl;
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
    else if (interrupt == 0x0)
    {
        DivByZero();
    }
    else if (interrupt == 0x3)
    {
        Debug();
    }
    //double fault
    else if (interrupt == 0x8)
    {
        TripleFault();
    }
    else if (interrupt == 0x15)
    {
        BIOSInterruptCall();
    }
    return;
}
