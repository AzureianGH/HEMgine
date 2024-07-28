#include <basics.h>
#include <idt.h>
#include <debuggerprint.h>
uint32_t MemorySize = 0;
uint8_t* Memory = nullptr; // 1MB of memory

typedef enum VGAMODE_E
{
    TEXTMODE = 0,
    GRAPHICSMODE = 1
} VGAMode_t;

VGAMode_t VGAMode = VGAMode_t::TEXTMODE;

void SetupStack()
{
    if (Haltflag) return;
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
    if (Haltflag) return 0;
    return CPURegisters32[reg];
}

uint16_t GetRegister16(CPURegister16_t reg)
{
    if (Haltflag) return 0;
    //get lower 16 bits of 32-bit register
    return CPURegisters32[reg] & 0xFFFF;
}

uint8_t GetRegister8(CPURegister8_t reg)
{
    if (Haltflag) return 0;
    //get lower 8 bits of 32-bit register
    return CPURegisters32[reg] & 0xFF;
}

void SetRegister32(CPURegister32_t reg, uint32_t value)
{
    if (Haltflag) return;
    CPURegisters32[reg] = value;
}

void SetRegister16(CPURegister16_t reg, uint16_t value)
{
    if (Haltflag) return;
    //set lower 16 bits of 32-bit register
    CPURegisters32[reg] = (CPURegisters32[reg] & 0xFFFF0000) | value;
}

void SetRegister8(CPURegister8_t reg, uint8_t value)
{
    if (Haltflag) return;
    //set lower 8 bits of 32-bit register
    CPURegisters32[reg] = (CPURegisters32[reg] & 0xFFFFFF00) | value;
}

uint32_t GetFlag(CPUFlag_t flag)
{
    if (Haltflag) return 0;
    return (CPUFlags >> flag) & 1;
}

void SetFlag(CPUFlag_t flag, uint32_t value)
{
    if (Haltflag) return;
    CPUFlags = (CPUFlags & ~(1 << flag)) | (value << flag);
}

void PushStack(CPURegister32_t Register, int size)
{
    if (Haltflag) return;
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
    if (Haltflag) return;
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
    if (Haltflag) return;
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
    if (Haltflag) return;
    MemorySize = size;
    
    Memory = (uint8_t*)malloc(size);
    
    SetDefaultMemory();
}

void SetMemoryAddress8(uint32_t address, uint8_t value)
{
    if (Haltflag) return;
    if (IsProtectedMemory(address))
    {
        CallInterrupt(0xD); // General Protection Fault because we are trying to write to protected memory
    }
    Memory[address] = value;
}
void SetMemoryAddress16(uint32_t address, uint16_t value)
{
    if (Haltflag) return;
    if (IsProtectedMemory(address))
    {
        CallInterrupt(0xD); // General Protection Fault because we are trying to write to protected memory
    }
    Memory[address] = value & 0xFF;
    Memory[address + 1] = value >> 8;
}
void SetMemoryAddress32(uint32_t address, uint32_t value)
{
    if (Haltflag) return;
    if (IsProtectedMemory(address))
    {
        CallInterrupt(0xD); // General Protection Fault because we are trying to write to protected memory
    }
    Memory[address] = value & 0xFF;
    Memory[address + 1] = (value >> 8) & 0xFF;
    Memory[address + 2] = (value >> 16) & 0xFF;
    Memory[address + 3] = (value >> 24) & 0xFF;
}



uint8_t GetMemoryAddress8(uint32_t address)
{
    if (Haltflag) return 0;
    return Memory[address];
}
uint16_t GetMemoryAddress16(uint32_t address)
{
    if (Haltflag) return 0;
    return Memory[address] | (Memory[address + 1] << 8);
}
uint32_t GetMemoryAddress32(uint32_t address)
{
    if (Haltflag) return 0;
    return Memory[address] | (Memory[address + 1] << 8) | (Memory[address + 2] << 16) | (Memory[address + 3] << 24);
}

void DumpMemory(int Start, int End)
{
    // 0x00000004: 0xFF 0xFF 0xFF 0xFF (example)
    //purple color
    printf("\033[0;35m");
    printf("Memory Dump: ");
    for (int i = Start; i < End; i++)
    {
        if (i % 16 == 0)
        {
            
            printf("\033[0;35m");
            printf("\n");
            printf("0x%08X: ", i);
        }
        printf("%02X ", Memory[i]);
        // print each byte on the right after all 16 bytes are printed
        if (i % 16 == 15)
        {
            printf("\033[0;32m");
            printf("\t");
            for (int j = i - 15; j <= i; j++)
            {
                if (Memory[j] < 32 || Memory[j] > 126)
                {
                    printf(". ");
                }
                else
                {
                    printf("%c ", Memory[j]);
                }
            }
        }
        
    }
    printf("\033[0m"); //reset color
    printf("\n");

}



bool IsProtectedMemory(int address)
{
    if (Haltflag) return false;
    if (ProtectedMemory.size() == 0)
    {
        return false;
    }
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
    if (Haltflag) return false;
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
    if (Haltflag) return;
    ProtectedMemory.push_back(memory);
}

BIOSProtectedMemory_t GetProtectedMemory()
{
    if (Haltflag) return {0, 0};
    return ProtectedMemory[0];
}

bool UnlockProtectedMemory(int address)
{
    if (Haltflag) return false;
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
    if (Haltflag) return;
    MemoryMappings.push_back(mapping);
}

MemoryMapping_t GetMemoryMapping(int address)
{
    if (Haltflag) return {0, 0, MemoryMappingType_t::NORMAL, 0};
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
    if (Haltflag) return false;
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
    if (Haltflag) return MemoryMappingType_t::NORMAL;
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
    if (Haltflag) return 0;
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
    if (Haltflag) return nullptr;
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
    if (Haltflag) return;
    return; // None for now
}

void SetDefaultMemoryMapping(int start, int end, MemoryMappingType_t type, int redirect)
{
    if (Haltflag) return;
    MemoryMapping_t mapping = {start, end, type, redirect};
    SetMemoryMapping(mapping);
}

void SetDefaultRegisters()
{
    if (Haltflag) return;
    for (int i = 0; i < 12; i++)
    {
        CPURegisters32[i] = 0;
        CPURegisters16[i] = 0;
        CPURegisters8[i] = 0;
    }
}

void SetDefaultFlags()
{
    if (Haltflag) return;
    CPUFlags = 0;
}

void SetDefaultStack()
{
    if (Haltflag) return;
    for (int i = 0; i < 0x100000; i++)
    {
        Stack[i] = 0;
    }
}



void SetDefaultProtectedMemory()
{
    if (Haltflag) return;
    //set the first mb of memory to be protected
    BIOSProtectedMemory_t protectedMemory = {0, 0x100000};
    ProtectedMemory.push_back(protectedMemory);

}

uint8_t* LoadFromMemory(int StartAddress, int EndAddress, bool BIOSOffsetted, bool TreatEndAsSize)
{
    
    if (Haltflag) return nullptr;
    if (BIOSOffsetted)
    {
        StartAddress += 0x100000;
    }
    if (TreatEndAsSize)
    {
        EndAddress += StartAddress;
    }
    uint8_t* data = new uint8_t[EndAddress - StartAddress];
    for (int i = 0; i < EndAddress - StartAddress; i++)
    {
        data[i] = Memory[StartAddress + i];
    }
    return data;
}

bool LoadIntoMemory(uint8_t *Data, int StartAddress, int Size, bool BIOSOffsetted)
{
    if (Haltflag) return false;

    if (BIOSOffsetted)
    {
        StartAddress += 0x100000;
    }

    //if protected memory, return false and call interrupt
    if (IsProtectedMemoryRange(StartAddress, StartAddress + sizeof(Data)))
    {
        CallInterrupt(0xD); // General Protection Fault
        return false;
    }

    for (int i = 0; i < Size; i++)
    {
        Memory[StartAddress + i] = Data[i];
    }

    return true;
}

void SetDefaultMemory()
{
    if (Haltflag) return;
    memset(Memory, 0, MemorySize);
    //set first bytes to "HEMgine" as the BIOS signature
    uint8_t BIOS[] = {0x48, 0x45, 0x4D, 0x67, 0x69, 0x6E, 0x65};
    for (int i = 0; i < sizeof(BIOS) / sizeof(uint8_t); i++)
    {
        Memory[i] = BIOS[i];
    }
}