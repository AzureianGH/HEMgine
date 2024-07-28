#include <basics.h>
#include <idt.h>

uint32_t MemorySize = 0;
uint8_t Memory[0x100000]; // 1MB of memory

void SetupStack()
{
    SetRegister32(CBP, 0x0);
    SetRegister32(CSP, 0x0);
    Stack = new uint8_t[0x100000];
    for (int i = 0; i < 0x100000; i++)
    {
        Stack[i] = 0;
    }
}

uint32_t GetRegister32(CPURegister32_t reg)
{
    return CPURegisters32[reg];
}

uint16_t GetRegister16(CPURegister16_t reg)
{
    //get lower 16 bits of 32-bit register
    return CPURegisters32[reg] & 0xFFFF;
}

uint8_t GetRegister8(CPURegister8_t reg)
{
    //get lower 8 bits of 32-bit register
    return CPURegisters32[reg] & 0xFF;
}

void SetRegister32(CPURegister32_t reg, uint32_t value)
{
    CPURegisters32[reg] = value;
}

void SetRegister16(CPURegister16_t reg, uint16_t value)
{
    //set lower 16 bits of 32-bit register
    CPURegisters32[reg] = (CPURegisters32[reg] & 0xFFFF0000) | value;
}

void SetRegister8(CPURegister8_t reg, uint8_t value)
{
    //set lower 8 bits of 32-bit register
    CPURegisters32[reg] = (CPURegisters32[reg] & 0xFFFFFF00) | value;
}

uint32_t GetFlag(CPUFlag_t flag)
{
    return (CPUFlags >> flag) & 1;
}

void SetFlag(CPUFlag_t flag, uint32_t value)
{
    CPUFlags = (CPUFlags & ~(1 << flag)) | (value << flag);
}

void PushStack(CPURegister32_t Register, int size)
{
    // Align 4 bytes
    if (size % 4 != 0)
    {
        size += 4 - (size % 4);
    }
    // Check if we have enough space
    if (GetRegister32(CSP) + size > 0x100000)
    {
        CallInterrupt(0xC); // Stack Overflow
    }
    // Copy the data to the stack
    for (int i = 0; i < size; i++)
    {
        Stack[GetRegister32(CSP) + i] = GetRegister32(Register) & 0xFF;
        SetRegister32(Register, GetRegister32(Register) >> 8);
    }
    // Update the stack pointer
    SetRegister32(CSP, GetRegister32(CSP) + size);
}
void DumpStack()
{
    //purple color
    printf("\033[0;35m");
    printf("Stack Dump: ");
    int loops = 0;
    for (int i = 0; i < GetRegister32(CSP); i++)
    {
        printf("%02X ", Stack[i]);
        loops++;
    }
    if (loops == 0)
    {
        printf("Empty \033[0;32m<CSP == CBP>");
    }
    printf("\033[0m"); //reset color
    printf("\n");
}
void PushStack(uint32_t value, int size)
{
    // Align 4 bytes
    if (size % 4 != 0)
    {
        size += 4 - (size % 4);
    }
    // Check if we have enough space
    if (GetRegister32(CSP) + size > 0x100000)
    {
        CallInterrupt(0xC); // Stack Overflow
    }
    // Copy the data to the stack
    for (int i = 0; i < size; i++)
    {
        Stack[GetRegister32(CSP) + i] = value & 0xFF;
        value = value >> 8;
    }
    // Update the stack pointer
    SetRegister32(CSP, GetRegister32(CSP) + size);
}
void PopStack(CPURegister32_t Register, int size)
{
    // Align 4 bytes
    if (size % 4 != 0)
    {
        size += 4 - (size % 4);
    }
    // Check if we have enough space
    if (GetRegister32(CSP) - size < 0)
    {
        CallInterrupt(0xC); // Stack Underflow
    }
    // Copy the data from the stack
    for (int i = 0; i < size; i++)
    {
        SetRegister32(Register, GetRegister32(Register) << 8 | Stack[GetRegister32(CSP) - i - 1]);
    }
    // Update the stack pointer
    SetRegister32(CSP, GetRegister32(CSP) - size);
}

void SetMemorySize(uint32_t size)
{
    MemorySize = size;
    Memory = new uint8_t[size];
    SetDefaultMemory();
}

void SetMemoryAddress(int address, int value)
{
    if (IsProtectedMemory(address))
    {
        CallInterrupt(0xD); // General Protection Fault because we are trying to write to protected memory
    }
    Memory[address] = value;
}

int GetMemoryAddress(int address)
{
    if (IsProtectedMemory(address))
    {
        return 0;
    }
    return Memory[address];
}

bool IsProtectedMemory(int address)
{
    for (int i = 0; i < sizeof(ProtectedMemory) / sizeof(BIOSProtectedMemory_t); i++)
    {
        if (address >= ProtectedMemory[i].StartAddress && address <= ProtectedMemory[i].EndAddress)
        {
            return true;
        }
    }
    return false;
}

bool IsProtectedMemoryRange(int start, int end)
{
    for (int i = 0; i < sizeof(ProtectedMemory) / sizeof(BIOSProtectedMemory_t); i++)
    {
        if (start >= ProtectedMemory[i].StartAddress && end <= ProtectedMemory[i].EndAddress)
        {
            return true;
        }
    }
    return false;
}

void SetProtectedMemory(BIOSProtectedMemory_t memory)
{
    ProtectedMemory.push_back(memory);
}

BIOSProtectedMemory_t GetProtectedMemory()
{
    return ProtectedMemory[0];
}

bool UnlockProtectedMemory(int address)
{
    //if its in the list, remove it
    for (int i = 0; i < sizeof(ProtectedMemory) / sizeof(BIOSProtectedMemory_t); i++)
    {
        if (address >= ProtectedMemory[i].StartAddress && address <= ProtectedMemory[i].EndAddress)
        {
            for (int j = i; j < sizeof(ProtectedMemory) / sizeof(BIOSProtectedMemory_t) - 1; j++)
            {
                ProtectedMemory[j] = ProtectedMemory[j + 1];
            }
            return true;
        }
    }
}

void SetMemoryMapping(MemoryMapping_t mapping)
{
    MemoryMappings.push_back(mapping);
}

MemoryMapping_t GetMemoryMapping(int address)
{
    for (int i = 0; i < sizeof(MemoryMappings) / sizeof(MemoryMapping_t); i++)
    {
        if (address >= MemoryMappings[i].StartAddress && address <= MemoryMappings[i].EndAddress)
        {
            return MemoryMappings[i];
        }
    }
}

bool IsMemoryMapped(int address)
{
    for (int i = 0; i < sizeof(MemoryMappings) / sizeof(MemoryMapping_t); i++)
    {
        if (address >= MemoryMappings[i].StartAddress && address <= MemoryMappings[i].EndAddress)
        {
            return true;
        }
    }
    return false;
}

MemoryMappingType_t GetMemoryMappingType(int address)
{
    for (int i = 0; i < sizeof(MemoryMappings) / sizeof(MemoryMapping_t); i++)
    {
        if (address >= MemoryMappings[i].StartAddress && address <= MemoryMappings[i].EndAddress)
        {
            return MemoryMappings[i].Type;
        }
    }
}

int GetMemoryMappingRedirect(int address)
{
    for (int i = 0; i < sizeof(MemoryMappings) / sizeof(MemoryMapping_t); i++)
    {
        if (address >= MemoryMappings[i].StartAddress && address <= MemoryMappings[i].EndAddress)
        {
            return MemoryMappings[i].RedirectAddress;
        }
    }
}

MemoryChunk_t* GetAddressesFromMappingType(MemoryMappingType_t type)
{
    MemoryChunk_t* addresses = new MemoryChunk_t[sizeof(MemoryMappings) / sizeof(MemoryMapping_t)];
    int count = 0;
    for (int i = 0; i < sizeof(MemoryMappings) / sizeof(MemoryMapping_t); i++)
    {
        if (MemoryMappings[i].Type == type)
        {
            addresses[count].StartAddress = MemoryMappings[i].StartAddress;
            addresses[count].EndAddress = MemoryMappings[i].EndAddress;
            count++;
        }
    }
    return addresses;
}

void SetDefaultMemoryMappings()
{
    return; // None for now
}

void SetDefaultMemoryMapping(int start, int end, MemoryMappingType_t type, int redirect)
{
    MemoryMapping_t mapping = {start, end, type, redirect};
    SetMemoryMapping(mapping);
}

void SetDefaultRegisters()
{
    for (int i = 0; i < 12; i++)
    {
        CPURegisters32[i] = 0;
        CPURegisters16[i] = 0;
        CPURegisters8[i] = 0;
    }
}

void SetDefaultFlags()
{
    CPUFlags = 0;
}

void SetDefaultStack()
{
    for (int i = 0; i < 0x100000; i++)
    {
        Stack[i] = 0;
    }
}

void SetDefaultMemory()
{
    for (int i = 0; i < MemorySize; i++)
    {
        Memory[i] = 0;
    }
}

void SetDefaultProtectedMemory()
{
    //set the first mb of memory to be protected
    BIOSProtectedMemory_t protectedMemory = {0, 0x100000};
    ProtectedMemory.push_back(protectedMemory);

}