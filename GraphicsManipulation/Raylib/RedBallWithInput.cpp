#include <raylib.h>

int main() {

  int ballX = 50;
  int ballY = 50;
  Color random = {26, 35, 50, 155};

  InitWindow(400, 400, "Miku");
  SetTargetFPS(60);

  // Game Loop
  while (WindowShouldClose() == false) {

    // 1. Event Handling
    if (IsKeyDown(KEY_D)) {
      ballX += 3;
    } else if (IsKeyDown(KEY_A)) {
      ballX -= 3;
    } else if (IsKeyDown(KEY_S)) {
      ballY += 3;
    } else if (IsKeyDown(KEY_W)) {
      ballY -= 3;
    }

    // 2. Updating positions

    // 3. Drawing
    BeginDrawing();
    ClearBackground(BLACK);
    DrawCircle(ballX, ballY, 50, RED);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
