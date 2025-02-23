#include <vcpu.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <basics.h>
#include <idt.h>
#include <raylib.h>

#define VERBOSEVCPU
bool executing = false;
bool firstRun = true;
VCPUState state = VCPUState::PAUSED;
void InitVCPU()
{
    SetRegister32(CPUREG32_E::CIP, 0);
}

#include <chrono>

void RunVCPU()
{
    if (state == VCPUState::PAUSED && firstRun)
    {
        state = VCPUState::RUNNING;
        firstRun = false;
    }

    if (state == VCPUState::RUNNING)
    {
        executing = true;
        uint32_t cip = GetRegister32(CIP);
        
        if (!IsInMemory(cip))
        {
            printf("Memory boundary access violation at CIP: %08X\n", cip);
            CallInterrupt(0x0D); // General Protection Fault
            state = VCPUState::HALTED;
            executing = false;
            return;
        }

        // Fetch the instruction
        uint8_t instruction = GetMemoryAddress8(cip);
        SetRegister32(CIP, cip + 1);

        // Decode the instruction
        switch (instruction)
        {
            case 0x00: // NOP
                break;
            case 0x01: // HLT
                state = VCPUState::HALTED;
            #ifdef VERBOSEVCPU
                printf("HLT, CIP: %08X\n", GetRegister32(CIP));
            #endif
                break;
            case 0x02: // INT
            #ifdef VERBOSEVCPU
                printf("INT, CIP: %08X\n", GetRegister32(CIP));
            #endif
                if (!IsInMemory(GetRegister32(CIP)))
                {
                    printf("Memory boundary access violation at CIP: %08X\n", GetRegister32(CIP));
                    CallInterrupt(0x0D); // General Protection Fault
                    state = VCPUState::HALTED;
                    break;
                }
                CallInterrupt(GetMemoryAddress8(GetRegister32(CIP)));
                SetRegister32(CIP, GetRegister32(CIP) + 1);
                break;
            case 0x03: // movb reg, imm
            #ifdef VERBOSEVCPU
                printf("movb reg, imm, CIP: %08X\n", GetRegister32(CIP));
            #endif
                if (!IsInMemory(GetRegister32(CIP)) || !IsInMemory(GetRegister32(CIP) + 1))
                {
                    printf("Memory boundary access violation at CIP: %08X\n", GetRegister32(CIP));
                    CallInterrupt(0x0D); // General Protection Fault
                    state = VCPUState::HALTED;
                    break;
                }
                //check if the register is a valid 0-11
                if (GetMemoryAddress8(GetRegister32(CIP)) < 0 || GetMemoryAddress8(GetRegister32(CIP)) > 11)
                {
                    printf("Invalid Register: %02X\n", GetMemoryAddress8(GetRegister32(CIP)));
                    CallInterrupt(0x06); // Invalid Opcode
                    break;
                }
                SetRegister8((CPURegister8_t)GetMemoryAddress8(GetRegister32(CIP)), GetMemoryAddress8(GetRegister32(CIP) + 1));
                SetRegister32(CIP, GetRegister32(CIP) + 2);
                break;
            case 0x04: // movw reg, imm
            #ifdef VERBOSEVCPU
                printf("movw reg, imm, CIP: %08X\n", GetRegister32(CIP));
            #endif

                if (!IsInMemory(GetRegister32(CIP)) || !IsInMemory(GetRegister32(CIP) + 2))
                {
                    printf("Memory boundary access violation at CIP: %08X\n", GetRegister32(CIP));
                    CallInterrupt(0x0D); // General Protection Fault
                    state = VCPUState::HALTED;
                    break;
                }
                //check if the register is a valid 0-11
                if (GetMemoryAddress8(GetRegister32(CIP)) < 0 || GetMemoryAddress8(GetRegister32(CIP)) > 11)
                {
                    printf("Invalid Register: %02X\n", GetMemoryAddress8(GetRegister32(CIP)));
                    CallInterrupt(0x06); // Invalid Opcode
                    break;
                }
                SetRegister16((CPURegister16_t)GetMemoryAddress8(GetRegister32(CIP)), GetMemoryAddress16(GetRegister32(CIP) + 1));
                SetRegister32(CIP, GetRegister32(CIP) + 3);
                break;
            case 0x05: // movl reg, imm
            #ifdef VERBOSEVCPU
                printf("movl reg, imm, CIP: %08X\n", GetRegister32(CIP));
            #endif
                if (!IsInMemory(GetRegister32(CIP)) || !IsInMemory(GetRegister32(CIP) + 4))
                {
                    printf("Memory boundary access violation at CIP: %08X\n", GetRegister32(CIP));
                    CallInterrupt(0x0D); // General Protection Fault
                    state = VCPUState::HALTED;
                    break;
                }
                //check if the register is a valid 0-11
                if (GetMemoryAddress8(GetRegister32(CIP)) < 0 || GetMemoryAddress8(GetRegister32(CIP)) > 11)
                {
                    printf("Invalid Register: %02X\n", GetMemoryAddress8(GetRegister32(CIP)));
                    CallInterrupt(0x06); // Invalid Opcode
                    break;
                }
                SetRegister32((CPURegister32_t)GetMemoryAddress8(GetRegister32(CIP)), GetMemoryAddress32(GetRegister32(CIP) + 1));
                SetRegister32(CIP, GetRegister32(CIP) + 5);
                break;
            case 0x06: // movb mem, imm 
            
                if (!IsInMemory(GetRegister32(CIP)) || !IsInMemory(GetRegister32(CIP) + 4))
                {
                    printf("Memory boundary access violation at CIP: %08X\n", GetRegister32(CIP));
                    CallInterrupt(0x0D); // General Protection Fault
                    state = VCPUState::HALTED;
                    break;
                }
                SetMemoryAddress8(GetMemoryAddress32(GetRegister32(CIP)), GetMemoryAddress8(GetRegister32(CIP) + 4));
                #ifdef VERBOSEVCPU
                    printf("movb %08X, %02X, CIP: %08X\n", GetMemoryAddress32(GetRegister32(CIP)), GetMemoryAddress8(GetRegister32(CIP) + 4), GetRegister32(CIP));
                #endif
                SetRegister32(CIP, GetRegister32(CIP) + 5);
                
                break;

            case 0x07: // movw mem, imm
                if (!IsInMemory(GetRegister32(CIP)) || !IsInMemory(GetRegister32(CIP) + 6))
                {
                    printf("Memory boundary access violation at CIP: %08X\n", GetRegister32(CIP));
                    CallInterrupt(0x0D); // General Protection Fault
                    state = VCPUState::HALTED;
                    break;
                }
                SetMemoryAddress16(GetMemoryAddress32(GetRegister32(CIP)), GetMemoryAddress16(GetRegister32(CIP) + 4));
                #ifdef VERBOSEVCPU
                    printf("movw %08X, %04X, CIP: %08X\n", GetMemoryAddress32(GetRegister32(CIP)), GetMemoryAddress16(GetRegister32(CIP) + 4), GetRegister32(CIP));
                #endif
                SetRegister32(CIP, GetRegister32(CIP) + 6);
                break;

            case 0x08: // movl mem, imm
                if (!IsInMemory(GetRegister32(CIP)) || !IsInMemory(GetRegister32(CIP) + 8))
                {
                    printf("Memory boundary access violation at CIP: %08X\n", GetRegister32(CIP));
                    CallInterrupt(0x0D); // General Protection Fault
                    state = VCPUState::HALTED;
                    break;
                }
                SetMemoryAddress32(GetMemoryAddress32(GetRegister32(CIP)), GetMemoryAddress32(GetRegister32(CIP) + 4));
                #ifdef VERBOSEVCPU
                    printf("movl %08X, %08X, CIP: %08X\n", GetMemoryAddress32(GetRegister32(CIP)), GetMemoryAddress32(GetRegister32(CIP) + 4), GetRegister32(CIP));
                #endif
                SetRegister32(CIP, GetRegister32(CIP) + 8);
                break;
            case 0x09: // movb reg, reg
                if (!IsInMemory(GetRegister32(CIP)))
                {
                    printf("Memory boundary access violation at CIP: %08X\n", GetRegister32(CIP));
                    CallInterrupt(0x0D); // General Protection Fault
                    state = VCPUState::HALTED;
                    break;
                }
                if (!IsInMemory(GetRegister32(CIP) + 1))
                {
                    printf("Memory boundary access violation at CIP: %08X\n", GetRegister32(CIP));
                    CallInterrupt(0x0D); // General Protection Fault
                    state = VCPUState::HALTED;
                    break;
                }
                if (!IsInMemory(GetRegister32(CIP) + 2))
                {
                    printf("Memory boundary access violation at CIP: %08X\n", GetRegister32(CIP));
                    CallInterrupt(0x0D); // General Protection Fault
                    state = VCPUState::HALTED;
                    break;
                }
                //check if the register is a valid 0-11
                if (GetMemoryAddress8(GetRegister32(CIP)) < 0 || GetMemoryAddress8(GetRegister32(CIP)) > 11)
                {
                    printf("Invalid Register: %02X\n", GetMemoryAddress8(GetRegister32(CIP)));
                    CallInterrupt(0x06); // Invalid Opcode
                    break;
                }
                if (GetMemoryAddress8(GetRegister32(CIP) + 1) < 0 || GetMemoryAddress8(GetRegister32(CIP) + 1) > 11)
                {
                    printf("Invalid Register: %02X\n", GetMemoryAddress8(GetRegister32(CIP) + 1));
                    CallInterrupt(0x06); // Invalid Opcode
                    break;
                }
                SetRegister8((CPURegister8_t)GetMemoryAddress8(GetRegister32(CIP)), GetRegister8((CPURegister8_t)GetMemoryAddress8(GetRegister32(CIP) + 1)));
                #ifdef VERBOSEVCPU
                    printf("movb reg, reg, CIP: %08X\n", GetRegister32(CIP));
                #endif
                SetRegister32(CIP, GetRegister32(CIP) + 2);
                break;
            case 0x0A: // movw reg, reg
                if (!IsInMemory(GetRegister32(CIP)))
                {
                    printf("Memory boundary access violation at CIP: %08X\n", GetRegister32(CIP));
                    CallInterrupt(0x0D); // General Protection Fault
                    state = VCPUState::HALTED;
                    break;
                }
                if (!IsInMemory(GetRegister32(CIP) + 1))
                {
                    printf("Memory boundary access violation at CIP: %08X\n", GetRegister32(CIP));
                    CallInterrupt(0x0D); // General Protection Fault
                    state = VCPUState::HALTED;
                    break;
                }
                if (!IsInMemory(GetRegister32(CIP) + 2))
                {
                    printf("Memory boundary access violation at CIP: %08X\n", GetRegister32(CIP));
                    CallInterrupt(0x0D); // General Protection Fault
                    state = VCPUState::HALTED;
                    break;
                }
                //check if the register is a valid 0-11
                if (GetMemoryAddress8(GetRegister32(CIP)) < 0 || GetMemoryAddress8(GetRegister32(CIP)) > 11)
                {
                    printf("Invalid Register: %02X\n", GetMemoryAddress8(GetRegister32(CIP)));
                    CallInterrupt(0x06); // Invalid Opcode
                    break;
                }
                if (GetMemoryAddress8(GetRegister32(CIP) + 1) < 0 || GetMemoryAddress8(GetRegister32(CIP) + 1) > 11)
                {
                    printf("Invalid Register: %02X\n", GetMemoryAddress8(GetRegister32(CIP) + 1));
                    CallInterrupt(0x06); // Invalid Opcode
                    break;
                }
                SetRegister16((CPURegister16_t)GetMemoryAddress8(GetRegister32(CIP)), GetRegister16((CPURegister16_t)GetMemoryAddress8(GetRegister32(CIP) + 1)));
                #ifdef VERBOSEVCPU
                    printf("movw reg, reg, CIP: %08X\n", GetRegister32(CIP));
                #endif
                SetRegister32(CIP, GetRegister32(CIP) + 2);
                break;
            case 0x0B: // movl reg, reg
                if (!IsInMemory(GetRegister32(CIP)))
                {
                    printf("Memory boundary access violation at CIP: %08X\n", GetRegister32(CIP));
                    CallInterrupt(0x0D); // General Protection Fault
                    state = VCPUState::HALTED;
                    break;
                }
                if (!IsInMemory(GetRegister32(CIP) + 1))
                {
                    printf("Memory boundary access violation at CIP: %08X\n", GetRegister32(CIP));
                    CallInterrupt(0x0D); // General Protection Fault
                    state = VCPUState::HALTED;
                    break;
                }
                if (!IsInMemory(GetRegister32(CIP) + 2))
                {
                    printf("Memory boundary access violation at CIP: %08X\n", GetRegister32(CIP));
                    CallInterrupt(0x0D); // General Protection Fault
                    state = VCPUState::HALTED;
                    break;
                }
                //check if the register is a valid 0-11
                if (GetMemoryAddress8(GetRegister32(CIP)) < 0 || GetMemoryAddress8(GetRegister32(CIP)) > 11)
                {
                    printf("Invalid Register: %02X\n", GetMemoryAddress8(GetRegister32(CIP)));
                    CallInterrupt(0x06); // Invalid Opcode
                    break;
                }
                if (GetMemoryAddress8(GetRegister32(CIP) + 1) < 0 || GetMemoryAddress8(GetRegister32(CIP) + 1) > 11)
                {
                    printf("Invalid Register: %02X\n", GetMemoryAddress8(GetRegister32(CIP) + 1));
                    CallInterrupt(0x06); // Invalid Opcode
                    break;
                }
                SetRegister32((CPURegister32_t)GetMemoryAddress8(GetRegister32(CIP)), GetRegister32((CPURegister32_t)GetMemoryAddress8(GetRegister32(CIP) + 1)));
                #ifdef VERBOSEVCPU
                    printf("movl reg, reg, CIP: %08X\n", GetRegister32(CIP));
                #endif
                SetRegister32(CIP, GetRegister32(CIP) + 2);
                break;
            case 0x0C: // movb mem, reg
                if (!IsInMemory(GetRegister32(CIP)))
                {
                    printf("Memory boundary access violation at CIP: %08X\n", GetRegister32(CIP));
                    CallInterrupt(0x0D); // General Protection Fault
                    state = VCPUState::HALTED;
                    break;
                }
                if (!IsInMemory(GetRegister32(CIP) + 1))
                {
                    printf("Memory boundary access violation at CIP: %08X\n", GetRegister32(CIP));
                    CallInterrupt(0x0D); // General Protection Fault
                    state = VCPUState::HALTED;
                    break;
                }
                //check if the register is a valid 0-11
                if (GetMemoryAddress8(GetRegister32(CIP) + 1) < 0 || GetMemoryAddress8(GetRegister32(CIP) + 1) > 11)
                {
                    printf("Invalid Register: %02X\n", GetMemoryAddress8(GetRegister32(CIP) + 1));
                    CallInterrupt(0x06); // Invalid Opcode
                    break;
                }
                SetMemoryAddress8(GetMemoryAddress32(GetRegister32(CIP)), GetRegister8((CPURegister8_t)GetMemoryAddress8(GetRegister32(CIP) + 1)));
                #ifdef VERBOSEVCPU
                    printf("movb %08X, reg, CIP: %08X\n", GetMemoryAddress32(GetRegister32(CIP)), GetRegister32(CIP));
                #endif
                SetRegister32(CIP, GetRegister32(CIP) + 2);
                break;
            default:
                printf("Unknown Instruction: %02X\n", instruction);
                CallInterrupt(0x06); // Invalid Opcode
                break;
        }
    }
    executing = false;
}

void ShutdownVCPU()
{
    state = VCPUState::SHUTDOWN;
}

void PauseVCPU()
{
    state = VCPUState::PAUSED;
}

void ResumeVCPU()
{
    state = VCPUState::RUNNING;
}

void HaltVCPU()
{
    state = VCPUState::HALTED;
}