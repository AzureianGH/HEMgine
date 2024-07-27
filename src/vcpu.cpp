#include <vcpu.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <basics.h>

void VCPU::FetchInstruction()
{
    // Fetch the instruction from CIP
    // and increment CIP
    uint32_t cipinstance = GetRegister32(CIP);
     
}

void VCPU::DecodeInstruction(uint32_t CIPADDRESS)
{
    // Decode the instruction at CIP
    // and set the instruction to the decoded instruction
    // and set the instruction length to the length of the instruction
}

