#include <iostream>
#include <raylib.h>
#include <rlgl.h>
using namespace std;
int main(int argc, char** argv)
{
    InitWindow(720, 400, "HEMgine");
    SetTargetFPS(60);
    Image icon = LoadImage("resource/HEM.png");
    SetWindowIcon(icon);
    Font TextModeVGA = LoadFontEx("resource/MD.ttf", 16, NULL, 0);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTextEx(TextModeVGA, "Hewo Woarld", {0, 0}, 16, 0, WHITE);
        EndDrawing();
    }

    CloseWindow();
    //close all threads
    return EXIT_SUCCESS;
}