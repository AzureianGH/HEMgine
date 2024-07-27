#include <basics.h>

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

void PushStack(uint8_t* data, int size)
{
    //align into 4-byte chunks
    int alignedSize = size + (4 - (size % 4));
    //CBP is the base pointer
    SetRegister32(CBP, GetRegister32(CBP) - alignedSize);
    //copy data to stack
    for (int i = 0; i < size; i++)
    {
        Stack[GetRegister32(CBP) + i] = data[i];
    }
    //zero out the rest of the space
    for (int i = size; i < alignedSize; i++)
    {
        Stack[GetRegister32(CBP) + i] = 0;
    }
}

void PopStack(uint8_t* data, int size)
{
    //align into 4-byte chunks
    int alignedSize = size + (4 - (size % 4));
    //copy data from stack
    for (int i = 0; i < size; i++)
    {
        data[i] = Stack[GetRegister32(CBP) + i];
    }
    //CBP is the base pointer
    SetRegister32(CBP, GetRegister32(CBP) + alignedSize);
}

void SetMemorySize(uint32_t size)
{
    MemorySize = size;
}

void SetMemoryAddress(int address, int value)
{
    if (IsProtectedMemory(address))
    {
        return;
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