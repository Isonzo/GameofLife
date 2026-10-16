#include "menu.h"
#include "raylib.h"
#include <stdint.h>
#include <stdlib.h>
#define FACTOR 25
#define WIDTH 800
#define HEIGHT 450

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

enum { UNCHANGED, DYING, BIRTHING };

typedef struct {
  uint8_t state : 2;
  uint8_t isDead : 1;
  uint8_t age : 5; // 32 numbers
} Cell;

_Static_assert(sizeof(Cell) == 1, "Cell should be 1 byte");

void DrawBlock(Cell *cell, int x, int y);
void ProcessBlock(Cell **cells, int x, int y);
void ApplyStates(Cell *cell);
void UpdateFrame(void);

Cell **cells;
unsigned int fpsCounter = 0;
bool playingSim = false;

int main(void) {
  InitWindow(WIDTH, HEIGHT, "Conway's Game of Life");
  SetTargetFPS(20);

  // Make Cell Matrix
  cells = malloc(WIDTH / FACTOR * sizeof(Cell *));

  for (int i = 0; i < WIDTH / FACTOR; ++i) {
    cells[i] = malloc(sizeof(Cell) * HEIGHT / FACTOR);
    for (int j = 0; j < HEIGHT / FACTOR; ++j) {
      cells[i][j].isDead = true;
      cells[i][j].state = UNCHANGED;
    }
  }

#ifdef __EMSCRIPTEN__
  // emscripten_set_main_loop(Function, FPS, simulate_infinite_loop)
  emscripten_set_main_loop(UpdateFrame, 20, 1);
#else
  SetTargetFPS(20);
  while (!WindowShouldClose()) {
    UpdateFrame();
  }
  CloseWindow();

#endif
  return 0;
}

void DrawBlock(Cell *cell, int x, int y) {
  if (cell->isDead)
    DrawRectangle(x, y, FACTOR, FACTOR, RAYWHITE);
  else {
    uint8_t shade = (uint8_t)((cell->age * 160) / 31);
    Color agedColor = {shade, shade, shade, 255};

    DrawRectangle(x, y, FACTOR, FACTOR, agedColor);
  }
}

void ProcessBlock(Cell **cells, int x, int y) {
  // Calculate surrounding cells
  int livingCells = 0;
  for (int i = -1; i < 2; ++i) {
    for (int j = -1; j < 2; ++j) {
      if (i == 0 && j == 0)
        continue;
      if (x + i < 0 || x + i >= WIDTH / FACTOR)
        continue;
      if (y + j < 0 || y + j >= HEIGHT / FACTOR)
        continue;
      if (!cells[x + i][y + j].isDead)
        ++livingCells;
    }
  }
  // Enact Conway's Law and mark their future
  Cell *cell = &cells[x][y];
  if (!cell->isDead && (livingCells == 2 || livingCells == 3))
    cell->state = UNCHANGED;
  if (cell->isDead && livingCells == 3)
    cell->state = BIRTHING;
  if (!cell->isDead && (livingCells > 3 || livingCells < 2))
    cell->state = DYING;
}

void ApplyStates(Cell *cell) {
  switch (cell->state) {
  case BIRTHING:
    cell->isDead = false;
    cell->age = 0;
    break;
  case DYING:
    cell->isDead = true;
    break;
  case UNCHANGED:
    if (!cell->isDead && cell->age < 31)
      cell->age++;
    break;
  }
}

void UpdateFrame(void) {
  if (fpsCounter < 40) {
    ShowMenu(WIDTH, HEIGHT);
    ++fpsCounter;
    return;
  }

  if (IsKeyPressed(KEY_G)) {
    playingSim = !playingSim;
  }

  if (!playingSim) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      Vector2 mousePos = GetMousePosition();
      Cell *clickedCell =
          &cells[(int)mousePos.x / FACTOR][(int)mousePos.y / FACTOR];

      clickedCell->isDead = !clickedCell->isDead;
      clickedCell->age = 0;
    }
  } else {
    for (int i = 0; i < WIDTH / FACTOR; ++i) {
      for (int j = 0; j < HEIGHT / FACTOR; ++j) {
        ProcessBlock(cells, i, j);
      }
    }
    for (int i = 0; i < WIDTH / FACTOR; ++i) {
      for (int j = 0; j < HEIGHT / FACTOR; ++j) {
        ApplyStates(&cells[i][j]);
      }
    }
  }

  BeginDrawing();
  ClearBackground(RAYWHITE);
  for (int i = 0; i < WIDTH / FACTOR; ++i) {
    for (int j = 0; j < HEIGHT / FACTOR; ++j) {
      DrawBlock(&cells[i][j], i * FACTOR, j * FACTOR);
    }
  }

  if (!playingSim) {
    DrawText("Click to place cells\nPress 'G' to start", WIDTH / 2 - 100,
             HEIGHT / 2, 20, LIGHTGRAY);
  } else {
    DrawText("Press 'G' to stop", WIDTH / 2 - 100, HEIGHT / 2, 20, LIGHTGRAY);
  }
  EndDrawing();
}
