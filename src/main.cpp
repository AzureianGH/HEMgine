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
using namespace std;
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
    
    InitWindow(720, 400, "HEMgine");
    SetTargetFPS(60);
    Image icon = LoadImage("resource/HEM.png");
    SetWindowIcon(icon);
    Font TextModeVGA = LoadFontEx("resource/MD.ttf", 16, NULL, 0);
    //allocate 100 mb of memory
    
    InitVGAPrinter();
    InitDefaultBIOSIDT();
    SetupStack();
    //print 'A' to screen using interrupt
    SetRegister32(RA, 0x0);
    SetRegister32(RB, 0x0);
    SetMemoryAddress(0, 0x41);
    //push 0x0F to stack
    PushStack(0x0F, sizeof(uint32_t));
    DumpStack();
    PopStack(RD, sizeof(uint32_t));
    DumpStack();
    CallInterrupt(0x13);
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        PrintVGABuffer();
        //DrawFPS(10, 10);
        EndDrawing();
    }
    

    CloseWindow();
    //close all threads
    return EXIT_SUCCESS;
}