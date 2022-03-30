#include "raylib.h"
#include <stdlib.h>
#include "menu.h"
#define FACTOR 25
#define WIDTH 800
#define HEIGHT 450


typedef struct
{
    bool isDead;
    enum {unchanged, dying, birthing} state;
}Cell;

void DrawBlock(Cell* cell, int x, int y);
void ProcessBlock(Cell** cells, int x, int y);
void ApplyStates(Cell* cell);

int main(void)
{
    InitWindow(WIDTH, HEIGHT, "Conway's Game of Life");
    SetTargetFPS(20);
    unsigned int fpsCounter = 0;

    bool playingSim = false;

    // Make Cell Matrix

    Cell** cells = malloc(WIDTH/FACTOR * sizeof(Cell*));

    for (int i = 0; i < WIDTH/FACTOR; ++i)
    {
        cells[i] = malloc(sizeof(Cell) * HEIGHT/FACTOR);
        for (int j = 0; j < HEIGHT/FACTOR; ++j)
        {
            cells[i][j].isDead = true;
            cells[i][j].state = unchanged;
        }
    }


    while (!WindowShouldClose())
    {
        if (fpsCounter < 40)
        {
            ShowMenu(WIDTH, HEIGHT);
            ++fpsCounter;
            continue;
        }
        if (IsKeyPressed(KEY_G))
        {
            playingSim = !playingSim;
        }
        
        if (!playingSim)
        {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                Vector2 mousePos = GetMousePosition();
                cells[(int)mousePos.x/FACTOR][(int)mousePos.y/FACTOR].isDead = !cells[(int)mousePos.x/FACTOR][(int)mousePos.y/FACTOR].isDead;
            }
        }
        else 
        {
            for (int i = 0; i < WIDTH/FACTOR; ++i)
            {
                for (int j = 0; j < HEIGHT/FACTOR; ++j)
                {
                    ProcessBlock(cells, i, j);
                }
            }
            for (int i = 0; i < WIDTH/FACTOR; ++i)
            {
                for (int j = 0; j < HEIGHT/FACTOR; ++j)
                {
                    ApplyStates(&cells[i][j]);
                }
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        for (int i = 0; i < WIDTH/FACTOR; ++i)
        {
            for (int j = 0; j < HEIGHT/FACTOR; ++j)
            {
                DrawBlock(&cells[i][j], i * FACTOR, j * FACTOR);
            }
        }
        if (!playingSim)
        {
            DrawText("Click to place cells\nPress 'G' to start", WIDTH / 2 - 100, HEIGHT / 2, 20, LIGHTGRAY);
        }
        else
        {
            DrawText("Press 'G' to stop", WIDTH / 2 - 100, HEIGHT / 2, 20, LIGHTGRAY);
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

void DrawBlock(Cell* cell, int x, int y)
{
    if (cell->isDead) DrawRectangle(x, y, FACTOR, FACTOR, RAYWHITE);
    else DrawRectangle(x, y, FACTOR, FACTOR, BLACK);
}

void ProcessBlock(Cell** cells, int x, int y)
{
    // Calculate surrounding cells
    int livingCells = 0;
    for (int i = -1; i < 2; ++i)
    {
        for (int j = -1; j < 2; ++j)
        {
            if (i == 0 && j == 0) continue;
            if (x + i < 0 || x + i >= WIDTH/FACTOR) continue;
            if (y + j < 0 || y + j >= HEIGHT/FACTOR) continue;
            if (!cells[x + i][y + j].isDead) ++livingCells;
        }
    // Enact Conway's Law and mark their future
    Cell* cell = &cells[x][y];
    if (!cell->isDead && (livingCells == 2 || livingCells == 3)) cell->state = unchanged;
    if (cell->isDead && livingCells == 3) cell->state = birthing;
    if (!cell->isDead && (livingCells > 3 || livingCells < 2)) cell->state = dying;
}

void ApplyStates(Cell* cell)
{
    switch (cell->state)
    {
        case birthing:
            cell->isDead = false;
            break;
        case dying:
            cell->isDead = true;
            break;
        case unchanged:
            break;
    }
}