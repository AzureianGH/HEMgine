#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

enum class VCPUState
{
    RUNNING, // Normal operation
    HALTED, // HLT instruction
    PAUSED, // Debugging
    SHUTDOWN // Triple fault
};
class VCPU
{
    private:
        VCPUState state;
        int32_t registers[12];
        int32_t stack[0x100000]; // 1MB of stack
    public:
        void FetchInstruction(); // Fetches the instruction from memory
        void DecodeInstruction(uint32_t CIPADDRESS); // Decodes the instruction
        void ExecuteInstruction(); // Executes the instruction
        void Run(); // Runs the VCPU
        void Halt(); // Halts the VCPU
        void Pause(); // Pauses the VCPU
        void Shutdown(); // Shuts down the VCPU
        int32_t* DumpRegisters(); // Dumps the registers
        VCPUState GetState(); // Gets the state of the VCPU
        void SetState(VCPUState state); // Sets the state of the VCPU
        void SetRegister(int32_t reg, int32_t value); // Sets a register
        int32_t GetRegister(int32_t reg); // Gets a register
        void PushStack(int32_t* data, int size); // Pushes data onto the stack
        void PopStack(int32_t* data, int size); // Pops data off the stack
        void SetStack(int32_t* stack); // Sets the stack
        int32_t* GetStack(); // Gets the stack
        VCPU(); // Constructor
        ~VCPU(); // Destructor
};