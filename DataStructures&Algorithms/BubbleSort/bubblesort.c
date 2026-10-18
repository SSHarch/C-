#include <raylib.h>
#include <stdio.h>
#define MAX_BUILDINGS 8

float iframetimer = 0.0f;
const float iframeduration = 1.0f;

int arr[] = {1, 2, 4, 5, 3, 9, 7, 8};

int main() {
  const int screenHeight = 600;
  const int screenWidth = 800;
  InitWindow(screenWidth, screenHeight, "Bubble Sort");
  // buildings
  Rectangle buildings1[MAX_BUILDINGS] = {0};
  Color buildColors[MAX_BUILDINGS] = {0};

  
  // random terrain
  int spacing = -40;
  for (int i = 0; i < MAX_BUILDINGS; i++) {
    // ground lvl
    buildings1[i].width = 40;
    buildings1[i].height = arr[i] * 10;
    buildings1[i].y = screenHeight - 130.0f - buildings1[i].height;
    buildings1[i].x = spacing + buildings1[i].width;

    // space between blocks(EXTREMELY IMPORTANT)
    spacing += (int)buildings1[i].width;

    buildColors[i] = (Color){(unsigned char)GetRandomValue(200, 240),
                             (unsigned char)GetRandomValue(200, 240),
                             (unsigned char)GetRandomValue(200, 250), 255};
  }

 int i = 1;
 int j = 1;
 
  while (!WindowShouldClose()) {
  iframetimer += GetFrameTime();

    if (j < MAX_BUILDINGS) {
        if (iframetimer >= 0.2f) { // 0.2 seconds per step (much faster than 1.0f!)
            iframetimer = 0.0f;    // Reset the timer

            if (arr[i] < arr[i - 1]) {
                int temp = arr[i];
                arr[i] = arr[i - 1];
                arr[i - 1] = temp;
                
                float tempHeight = buildings1[i].height;
                buildings1[i].height = buildings1[i - 1].height;
                buildings1[i - 1].height = tempHeight;

                buildings1[i].y = screenHeight - 130.0f - buildings1[i].height;
                buildings1[i - 1].y = screenHeight - 130.0f - buildings1[i - 1].height;
            }

            i++;
            if (i >= MAX_BUILDINGS) {
                i = 1; // Reset to the beginning of the array
                j++;   // Advance to the next complete pass
            }
        }
        printf("Done!");
    }
    // Drawing
    BeginDrawing();
    ClearBackground(WHITE);
    for (int i = 0; i < MAX_BUILDINGS; i++)
      DrawRectangleRec(buildings1[i], buildColors[i]);
    EndDrawing();
  }

  CloseWindow();
}
