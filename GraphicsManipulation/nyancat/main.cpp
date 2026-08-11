#include <chrono>
#include <iostream>
#include <raylib.h>
#include <thread>

#define MAX_BUILDINGS 100
#define MAX_COINS 100

bool is_running = true;
int score = 0;
int health = 3;
float iframetimer = 0.0f;
const float iframeduration = 1.0f;

typedef struct Cat {
  Rectangle dest_rect;
} Cat;

typedef struct Rainbow {
  Rectangle dest_rect;
  Texture2D naynText;
} Rainbow;

typedef struct Coin {
  Rectangle dest_rect;
  Texture2D coinText;
  bool is_used;
} Coin;

void MoveRainbow(Rainbow *rainbow) {
  rainbow->dest_rect.x += 100.0 * GetFrameTime();
  if (IsKeyDown(KEY_W)) {
    rainbow->dest_rect.y -= 200.0 * GetFrameTime();
  }
  if (IsKeyDown(KEY_S)) {
    rainbow->dest_rect.y += 200.0 * GetFrameTime();
  }
}
void setFullscreen() {
  if (IsKeyPressed(KEY_F)) {
    ToggleBorderlessWindowed();
  }
}

void Collisions(Rainbow *rainbow, Rectangle *b1) {
  for (int i = 0; i < MAX_BUILDINGS; i++) {
    if (CheckCollisionRecs(rainbow->dest_rect, b1[i])) {
      if (rainbow->dest_rect.y > b1[i].y) {
        rainbow->dest_rect.y = b1[i].y + b1[i].height;
        if (iframetimer <= 0.0f) {
          health--;
          iframetimer = iframeduration;
        }
      }

      else {
        rainbow->dest_rect.y = b1[i].y - rainbow->dest_rect.height;
        if (iframetimer <= 0.0f) {
          health--;
          iframetimer = iframeduration;
        }
      }

      if (rainbow->dest_rect.x < b1[i].x) {
        rainbow->dest_rect.x = b1[i].x - rainbow->dest_rect.width;
        if (iframetimer <= 0.0f) {
          health--;
          iframetimer = iframeduration;
        }
      }
    }
  }
}

void coinSystem(Rainbow *rainbow, Coin *coins) {
  for (int i = 0; i < MAX_BUILDINGS; i++) {
    if (CheckCollisionRecs(rainbow->dest_rect, coins[i].dest_rect) &&
        !coins[i].is_used) {
      score++;
      coins[i].is_used = true;
    }
  }
}

void healthSystem(Rainbow *rainbow, Rectangle *b1) {
  for (int i = 0; i < health; i++) {
    DrawRectangle(650, 50, 50 * health, 25, RED);
  }
  iframetimer -= GetFrameTime();
}

int main() {
  const int screenHeight = 600;
  const int screenWidth = 800;
  InitWindow(screenWidth, screenHeight, "Nyan Cat!");
  // buildings
  Rectangle buildings1[MAX_BUILDINGS] = {0};
  Rectangle buildings2[MAX_BUILDINGS] = {0};
  Rectangle buildings3[MAX_BUILDINGS] = {0};
  // coins
  bool is_usedcoins1[MAX_COINS] = {false};
  bool is_usedcoins2[MAX_COINS] = {false};
  bool is_usedcoins3[MAX_COINS] = {false};
  Color buildColors[MAX_BUILDINGS] = {0};

  Cat cat{.dest_rect = (Rectangle){
              .x = 200.0,
              .y = 200.0,
              .width = 100.0,
              .height = 100.0,
          }};
  Rainbow rainbow{.dest_rect =
                      (Rectangle){
                          .x = 200.0,
                          .y = 200.0,
                          .width = 75.0,
                          .height = 75.0,
                      },
                  .naynText = LoadTexture("assets/nyan-cat.png")};

  Coin coins[MAX_COINS] = {0};
  // random terrain
  int spacing = 1000;
  for (int i = 0; i < MAX_BUILDINGS; i++) {
    // ground lvl
    buildings1[i].width = (float)GetRandomValue(100, 400);
    buildings1[i].height = 40;
    buildings1[i].y = screenHeight - 130.0f - buildings1[i].height;
    buildings1[i].x = -6000.0f + spacing + buildings1[i].width;
    // air lvl
    buildings2[i].width = (float)GetRandomValue(100, 400);
    buildings2[i].height = 40;
    buildings2[i].y = screenHeight - 350.0f - buildings2[i].height;
    buildings2[i].x = -6000.0f + spacing + buildings2[i].width;
    // top level
    buildings3[i].width = (float)GetRandomValue(100, 400);
    buildings3[i].height = 40;
    buildings3[i].y = screenHeight - 500.0f - buildings3[i].height - 50;
    buildings3[i].x = -6000.0f + spacing + buildings3[i].width;
    // coins
    coins[i].dest_rect = (Rectangle){
        .x = (float)GetRandomValue(100, 800) + spacing,
        .y = (float)GetRandomValue(0, 470),
        .width = 50.0,
        .height = 50.0,
    };
    coins[i].coinText = LoadTexture("assets/poptart.png");
    coins[i].is_used = false;

    // space between blocks(EXTREMELY IMPORTANT)
    spacing += (int)buildings1[i].width * 3;

    buildColors[i] = (Color){(unsigned char)GetRandomValue(200, 240),
                             (unsigned char)GetRandomValue(200, 240),
                             (unsigned char)GetRandomValue(200, 250), 255};
  }

  Camera2D camera = {0};
  camera.target =
      (Vector2){rainbow.dest_rect.x + 20.0f, rainbow.dest_rect.y + 20.0f};
  camera.offset = (Vector2){800 / 2.0f, 450 / 2.0f};
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;

  while (!WindowShouldClose() && is_running) {
    // Updates
    camera.target =
        (Vector2){rainbow.dest_rect.x + 20, rainbow.dest_rect.y + 20};

    MoveRainbow(&rainbow);
    setFullscreen();

    Collisions(&rainbow, buildings1);
    Collisions(&rainbow, buildings2);
    Collisions(&rainbow, buildings3);
    healthSystem(&rainbow, buildings1);
    healthSystem(&rainbow, buildings2);
    healthSystem(&rainbow, buildings3);
    std::cout << health << '\n';

    // Drawing
    BeginDrawing();
    ClearBackground(WHITE);
    BeginMode2D(camera);

    // terrain

    for (int i = 0; i < MAX_BUILDINGS; i++)
      DrawRectangleRec(buildings1[i], buildColors[i]);
    for (int i = 0; i < MAX_BUILDINGS; i++)
      DrawRectangleRec(buildings2[i], buildColors[i]);
    for (int i = 0; i < MAX_BUILDINGS; i++) {
      coinSystem(&rainbow, &coins[i]);
      DrawRectangleRec(buildings3[i], buildColors[i]);
      DrawTexturePro(rainbow.naynText, {0, 0, 900, 660}, rainbow.dest_rect,
                     {0, 0}, 0.0, RAYWHITE);
      if (!CheckCollisionRecs(coins[i].dest_rect, buildings1[i]) &&
          !coins[i].is_used) {
        DrawTexturePro(coins[i].coinText, {0, 0, 225, 255}, coins[i].dest_rect,
                       {0, 0}, 0.0, RAYWHITE);
      }
      if (!CheckCollisionRecs(coins[i].dest_rect, buildings2[i]) &&
          !coins[i].is_used) {
        DrawTexturePro(coins[i].coinText, {0, 0, 225, 255}, coins[i].dest_rect,
                       {0, 0}, 0.0, RAYWHITE);
      }
      if (!CheckCollisionRecs(coins[i].dest_rect, buildings3[i]) &&
          !coins[i].is_used) {
        DrawTexturePro(coins[i].coinText, {0, 0, 225, 255}, coins[i].dest_rect,
                       {0, 0}, 0.0, RAYWHITE);
      }
    }
    EndMode2D();
    DrawText(TextFormat("Score: %i", score), 10, 10, 20, LIGHTGRAY);
    EndDrawing();
  }

  CloseWindow();
}
