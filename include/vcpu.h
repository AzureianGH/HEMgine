#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <basics.h>
enum class VCPUState
{
    RUNNING, // Normal operation
    HALTED, // HLT instruction
    PAUSED, // Debugging
    SHUTDOWN // Triple fault
};

void InitVCPU(uint8_t* Program, uint32_t Size);

void RunVCPU();

void ShutdownVCPU();

void PauseVCPU();

void ResumeVCPU();

void HaltVCPU();