#pragma once
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <cstdint>



typedef struct MEMORYCHUNK_S
{
    int StartAddress;
    int EndAddress;
    int Size;
} MemoryChunk_t;

typedef enum MEMORYMAPPINGTYPE_E
{
    NORMAL = 0,
    REDIRECT = 1,
    FRAMEBUFFER = 2,
    VGATEXT = 3
} MemoryMappingType_t;

typedef struct MEMORYMAPPING_S
{
    int StartAddress;
    int EndAddress;
    MemoryMappingType_t Type;
    int RedirectAddress;
} MemoryMapping_t;

typedef enum MEMORYTYPE_E
{
    BIOS_RESERVED = 0,
    UNKNOWN_PROTECTED = 1,
    FREE_UNPROTECTED = 2
} BIOSMemoryType_t;

typedef struct BIOSPM_S
{
    int StartAddress;
    int EndAddress;
    int Size;
} BIOSProtectedMemory_t;

typedef enum CPUREG32_E
{
    AS = 0, // Argument Stack
    RA = 1, // Accumulator
    RB = 2, // Base
    RC = 3, // Counter
    RD = 4, // Data
    RR = 5, // Return Register
    CSP = 6, // Stack Pointer
    CBP = 7, // Base Pointer
    CIP = 8, // Instruction Pointer
    CFLAGS = 9, // Flags
    CSI = 10, // Source Index
    CDI = 11 // Destination Index
} CPURegister32_t;

typedef enum CPUREG16_E
{
    ASW = 0, // Argument Stack
    RAW = 1, // Accumulator
    RBW = 2, // Base
    RCW = 3, // Counter
    RDW = 4, // Data
    RRW = 5, // Return Register
    CSPW = 6, // Stack Pointer
    CBPW = 7, // Base Pointer
    CIPW = 8, // Instruction Pointer
    CFLAGSW = 9, // Flags
    CSIW = 10, // Source Index
    CDIW = 11 // Destination Index
} CPURegister16_t;

typedef enum CPUREG8_E
{
    ASB = 0, // Argument Stack
    RAB = 1, // Accumulator
    RBB = 2, // Base
    RCB = 3, // Counter
    RDB = 4, // Data
    RRB = 5, // Return Register
    CSPB = 6, // Stack Pointer
    CBPB = 7, // Base Pointer
    CIPB = 8, // Instruction Pointer
    CFLAGSB = 9, // Flags
    CSIB = 10, // Source Index
    CDIB = 11 // Destination Index
} CPURegister8_t;

typedef enum CPUFLAG_E
{
    CF = 0, // Carry Flag
    PF = 1, // Parity Flag
    AF = 2, // Auxiliary Flag
    ZF = 3, // Zero Flag
    SF = 4, // Sign Flag
    TF = 5, // Trap Flag
    IF = 6, // Interrupt Flag
    DF = 7, // Direction Flag
    OF = 8, // Overflow Flag
    IOF = 9, // Interrupt Overflow Flag
    RF = 10, // Resume Flag
    VMF = 11, // Virtual Mode Flag
    AC = 12, // Alignment Check
    VIF = 13, // Virtual Interrupt Flag
    VIP = 14, // Virtual Interrupt Pending
    ID = 15 // Identification Flag
} CPUFlag_t;



static uint32_t CPURegisters32[12] = {0}; // 12 32-bit registers
static std::string CPURegisterNames32[12] = {"AS", "RA", "RB", "RC", "RD", "RR", "CSP", "CBP", "CIP", "CFLAGS", "CSI", "CDI"};
static uint16_t CPURegisters16[12] = {0}; // 12 16-bit registers
static uint8_t CPURegisters8[12] = {0}; // 12 8-bit registers

static uint32_t CPUFlags = 0; // 32-bit flags register

static uint8_t* Stack; // 1MB stack
static uint8_t ArgumentStack[0x100000] = {0}; // 1MB argument stack

extern uint32_t MemorySize;

bool IsInMemory(uint32_t address);

// Vector of protected memory
static std::vector<BIOSProtectedMemory_t> ProtectedMemory;
static std::vector<MemoryMapping_t> MemoryMappings;
void SetupStack();
uint32_t GetRegister32(CPURegister32_t reg);
uint16_t GetRegister16(CPURegister16_t reg);
uint8_t GetRegister8(CPURegister8_t reg);

void SetRegister32(CPURegister32_t reg, uint32_t value);
void SetRegister16(CPURegister16_t reg, uint16_t value);
void SetRegister8(CPURegister8_t reg, uint8_t value);

uint32_t GetFlag(CPUFlag_t flag);
void SetFlag(CPUFlag_t flag, uint32_t value);

void PushStack(CPURegister32_t Register, int size);
void PopStack(CPURegister32_t Register, int size);
void PushStack(uint32_t value, int size);
void DumpStack();

void SetMemorySize(uint32_t size);

bool IsProtectedMemory(int address);
bool IsProtectedMemoryRange(int start, int end);

void SetProtectedMemory(BIOSProtectedMemory_t memory);
BIOSProtectedMemory_t GetProtectedMemory();
bool UnlockProtectedMemory(int address);

void SetMemoryMapping(MemoryMapping_t mapping);
MemoryMapping_t GetMemoryMapping(int address);
bool IsMemoryMapped(int address);
MemoryMappingType_t GetMemoryMappingType(int address);
int GetMemoryMappingRedirect(int address);
MemoryChunk_t* GetAddressesFromMappingType(MemoryMappingType_t type);

void SetDefaultRegisters();
void SetDefaultFlags();
void SetDefaultStack();
void SetDefaultMemory();
void SetDefaultProtectedMemory(uint64_t BiosSize);

void SetDefaultMemoryMappings();
void SetDefaultMemoryMapping(int start, int end, MemoryMappingType_t type, int redirect);

void SetMemoryAddress8(uint32_t address, uint8_t value);
void SetMemoryAddress16(uint32_t address, uint16_t value);
void SetMemoryAddress32(uint32_t address, uint32_t value);
uint8_t GetMemoryAddress8(uint32_t address);
uint16_t GetMemoryAddress16(uint32_t address);
uint32_t GetMemoryAddress32(uint32_t address);
void DumpMemory(int Start, int End);
bool LoadIntoMemory(uint8_t *Data, int StartAddress, int Size, bool BIOSOffsetted = true, bool Force = false);
uint8_t* LoadFromMemory(int StartAddress, int EndAddress, bool BIOSOffsetted = true, bool TreatEndAsSize = false);