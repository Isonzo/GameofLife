#include "raylib.h"

void ShowMenu(int width, int height)
{
    ClearBackground(RAYWHITE);
    BeginDrawing();
    DrawText("Conway's Game of Life", width / 2 - 100, height/2, 30, BLACK);
    EndDrawing();
}