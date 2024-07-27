#include <iostream>
#include <raylib.h>
#include <rlgl.h>
#include <string.h>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>

using namespace std;
int main(int argc, char** argv)
{
    //check dpi of screen
    if (argc > 1)
    {
        // if --highdpi
        if (strcmp(argv[1], "--highdpi") == 0)
        {
            SetConfigFlags(FLAG_WINDOW_HIGHDPI);
            cout << "High DPI Enabled" << endl;
        }
        cout << "Args Handled: " << argv[1] << endl;
        //print args
        for (int i = 0; i < argc; i++)
        {
            cout << argv[i] << endl;
        }
    }
    InitWindow(720, 400, "HEMgine");
    SetTargetFPS(60);
    Image icon = LoadImage("resource/HEM.png");
    SetWindowIcon(icon);
    Font TextModeVGA = LoadFontEx("resource/MD.ttf", 16, NULL, 0);
    

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTextEx(TextModeVGA, "", {0, 0}, 16, 0, WHITE);
        EndDrawing();
    }

    CloseWindow();
    //close all threads
    return EXIT_SUCCESS;
}