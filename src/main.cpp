#include <iostream>
#include <raylib.h>
#include <rlgl.h>
#include <string.h>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>
#include <basics.h>
#include <vcpu.h>
#include <idt.h>
#include <printer.h>
#include <debuggerprint.h>
#include <atomic>
using namespace std;

uint64_t cycles_per_sec = 0;

std::atomic<uint64_t> cycleCount;


uint32_t ParseMemorySize(char* memsize)
{
    //there should be a number at the start of the string and a letter at the end
    //parse the number
    uint32_t number = 0;
    for (int i = 0; i < strlen(memsize); i++)
    {
        if (isdigit(memsize[i]))
        {
            number = number * 10 + (memsize[i] - '0');
        }
        else
        {
            break;
        }
    }
    //parse the letter
    char letter = memsize[strlen(memsize) - 1];
    switch (letter)
    {
    case 'b':
        return number;
    case 'k':
        return number * 1024;
    case 'm':
        return number * 1024 * 1024;
    case 'g':
        return number * 1024 * 1024 * 1024;
    default:
        return 0;
    }
    
    return number;

}
bool MemorySet = false;

void PrintVGAString(char* str)
{
    for (int i = 0; i < strlen(str); i++)
    {
        SetRegister32(RA, 0x1);
        SetRegister32(RB, 0x0);
        SetRegister32(RC, str[i]);
        SetRegister32(RD, 0x0F);
        CallInterrupt(0x15);
    }
}

void RunThread()
{
    while (!WindowShouldClose())
    {
        RunVCPU();
        //increment cycle count using atomic
        std::atomic_fetch_add(&cycleCount, 1);
    }
}

int main(int argc, char** argv)
{
    //check dpi of screen
    if (argc > 1)
    {
        for (int i = 0; i < argc; i++)
        {
            // if --highdpi
            if (strcmp(argv[i], "--highdpi") == 0)
            {
                SetConfigFlags(FLAG_WINDOW_HIGHDPI);
                cout << "High DPI Enabled" << endl;
            }
            //check for -m flag, and the arg after it can be the amount of memory to allocate (1b, 1kb, 1mb, 1gb) etc.
            if (!MemorySet)
            {
                if (strcmp(argv[i], "-m") == 0)
                {
                    if (i + 1 < argc)
                    {
                        //parse the memory size
                        SetMemorySize(ParseMemorySize(argv[i + 1]));
                        //light cyan
                        printf("\033[0;36m");
                        printf("HEMgine Virtual Machine\n", 0, 0, 0x0B);
                        //light green memory size
                        printf("\033[0;32m");
                        printf("Memory Size: %d bytes\n", MemorySize);
                        printf("\033[0m"); //reset color
                        MemorySet = true;
                    }
                    else
                    {
                        if (MemorySize != 0) continue;
                        SetMemorySize(0x6400000); // 100mb
                        //light cyan
                        printf("\033[0;36m");
                        printf("HEMgine Virtual Machine\n", 0, 0, 0x0B);
                        //light green memory size
                        printf("\033[0;32m");
                        printf("Memory Size: %d bytes\n", MemorySize);
                        printf("\033[0m"); //reset color
                        MemorySet = true;
                        
                    }
                }
                
            }
        }
    }
    else
    {
        SetMemorySize(0x10000); // 64kb
        //light cyan
        printf("\033[0;36m");
        printf("HEMgine Virtual Machine\n", 0, 0, 0x0B);
        //light green memory size
        printf("\033[0;32m");
        printf("Memory Size: %d bytes\n", MemorySize);
        printf("\033[0m"); //reset color
        MemorySet = true;
    }
    
    InitWindow(720, 400, "HEMgine");
    SetTargetFPS(60);
    Image icon = LoadImage("resource/HEM.png");
    SetWindowIcon(icon);
    Font TextModeVGA = LoadFontEx("resource/MD.ttf", 16, NULL, 0);
    //allocate 100 mb of memory
    int* fileSize = (int*)malloc(sizeof(int));
    uint8_t* BIOS = LoadFileData("resource/hem-bios-0x00001.leaf", fileSize);
    printf("BIOS Size: %d bytes\n", *fileSize);
    InitVGAPrinter();
    InitDefaultBIOSIDT();
    SetupStack();
    SetDefaultMemory();
    SetDefaultProtectedMemory((uint64_t)*fileSize);
    BIOSProtectedMemory_t mem = GetProtectedMemory();
    printf("Protected Memory: %08X - %08X\n", mem.StartAddress, mem.EndAddress);
    LoadIntoMemory(BIOS, 0x0, *fileSize, false, true);
    InitVCPU();
    thread vcpuThread(RunThread);
    auto lastTime = std::chrono::steady_clock::now();
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        PrintVGABuffer();
        EndDrawing();
        SetWindowTitle(TextFormat("HEMgine - %llu Cycles/Sec (GHz: %f) (MHz %f)", cycles_per_sec, cycles_per_sec / 1000000000.0, cycles_per_sec / 1000000.0));
        auto currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsedTime = currentTime - lastTime;
        if (elapsedTime.count() >= 1.0)
        {
            cycles_per_sec = cycleCount;
            cycleCount = 0;
            lastTime = currentTime;
        }
    }
    CloseWindow();
    vcpuThread.join();
    return EXIT_SUCCESS;
}
