#include <cmath>
#include <fstream>
#include <nlohmann/json.hpp>
#include <raylib.h>
#include <string>
#include <vector>

using json = nlohmann::json;

struct TiledObject {
  int id;
  int gid;
  float x, y, width, height;
};

struct MapLayer {
  std::string name;
  std::string type; // Keeps track of "tilelayer" vs "objectgroup"
  std::vector<int> data;
  std::vector<TiledObject> objects;
};

struct TiledMap {
  int width;
  int height;
  int tileWidth;
  int tileHeight;
  std::vector<MapLayer> layers;
};

TiledMap LoadLevelMap(const char *filepath) {
  std::ifstream file(filepath);
  json mapJson = json::parse(file);
  TiledMap map;

  map.width = mapJson["width"];
  map.height = mapJson["height"];
  map.tileWidth = mapJson["tilewidth"];
  map.tileHeight = mapJson["tileheight"];

  for (const auto &jsonLayer : mapJson["layers"]) {
    MapLayer layer;
    layer.name = jsonLayer["name"];
    layer.type = jsonLayer["type"];

    if (layer.type == "tilelayer") {
      layer.data = jsonLayer["data"].get<std::vector<int>>();
    } else if (layer.type == "objectgroup") {
      for (const auto &jsonObj : jsonLayer["objects"]) {
        TiledObject obj;
        obj.id = jsonObj["id"];
        obj.gid = jsonObj.value("gid", 0); // Safely fallback if gid missing
        obj.x = jsonObj["x"];
        obj.y = jsonObj["y"];
        obj.width = jsonObj["width"];
        obj.height = jsonObj["height"];
        layer.objects.push_back(obj);
      }
    }
    map.layers.push_back(layer);
  }
  return map;
}
void RenderLevelMap(const TiledMap &map, Texture2D terrainSheet,
                    Texture2D bgTexture) {
  for (const auto &layer : map.layers) {

    // 1. Process Background Image Layer
    if (layer.type == "objectgroup") {
      for (const auto &obj : layer.objects) {
        if (obj.gid >= 73) { // GID matches background.tsx
          // Fix Tiled bottom-left to Raylib top-left coordinate system layout
          int drawX = (int)std::round(obj.x);
          int drawY = (int)std::round(obj.y - obj.height);
          int drawW = (int)std::round(obj.width);
          int drawH = (int)std::round(obj.height);

          Rectangle src = {0, 0, (float)bgTexture.width,
                           (float)bgTexture.height};
          Rectangle dest = {(float)drawX, (float)drawY, (float)drawW,
                            (float)drawH};

          DrawTexturePro(bgTexture, src, dest, Vector2{0, 0}, 0.0f, WHITE);
        }
      }
    }

    // 2. Process Level Terrain Grid Layer
    else if (layer.type == "tilelayer") {
      int sheetColumns = terrainSheet.width / map.tileWidth;

      for (size_t i = 0; i < layer.data.size(); ++i) {
        int tileID = layer.data[i];
        if (tileID == 0)
          continue; // Skip empty air

        // Normalize Tiled 1-indexed identifier to a 0-indexed layout
        int actualIndex = tileID - 1;

        int sheetX = (actualIndex % sheetColumns) * map.tileWidth;
        int sheetY = (actualIndex / sheetColumns) * map.tileHeight;

        int screenX = (i % map.width) * map.tileWidth;
        int screenY = (i / map.width) * map.tileHeight;

        Rectangle src = {(float)sheetX, (float)sheetY, (float)map.tileWidth,
                         (float)map.tileHeight};
        Vector2 pos = {(float)screenX, (float)screenY};

        DrawTextureRec(terrainSheet, src, pos, WHITE);
      }
    }
  }
}
std::vector<Rectangle> CreateMapColliders(const TiledMap &map) {
  std::vector<Rectangle> colliders;

  for (const auto &layer : map.layers) {
    // Only gather colliders from your solid gameplay layer, skip the
    // background!
    if (layer.type == "tilelayer" && layer.name == "Foreground") {

      for (size_t i = 0; i < layer.data.size(); ++i) {
        int tileID = layer.data[i];

        // If the number is NOT 0, it means it is solid ground
        if (tileID != 0) {
          // Use the exact same math as the rendering loop to find where it
          // lives
          float screenX = (float)((i % map.width) * map.tileWidth);
          float screenY = (float)((i / map.width) * map.tileHeight);

          // Create a Raylib rectangle matching the tile dimensions
          Rectangle solidBox = {screenX, screenY, (float)map.tileWidth,
                                (float)map.tileHeight};

          // Save it to our vector list
          colliders.push_back(solidBox);
        }
      }
    }
  }
  return colliders;
}

int main() {
  InitWindow(960, 640, "Raylib Tiled Level Engine");
  SetTargetFPS(60);

  // Load assets
  TiledMap currentLevel = LoadLevelMap("tiled/test.json");
  Texture2D terrainTex = LoadTexture("assets/tiles/tileset_32x32(new).png");
  Texture2D backgroundTex = LoadTexture("assets/tiles/background.png");

  // Generate our world collision boxes using our new function!
  std::vector<Rectangle> worldColliders = CreateMapColliders(currentLevel);

  // Create a simple player box for testing
  Rectangle player = {100.0f, 100.0f, 24.0f, 40.0f}; // X, Y, Width, Height
  float playerSpeed = 4.0f;

  while (!WindowShouldClose()) {
    // ---- 1. HANDLE PLAYER INPUT MOVEMENT ----
    Vector2 movement = {0, 0};
    if (IsKeyDown(KEY_RIGHT))
      movement.x += playerSpeed;
    if (IsKeyDown(KEY_LEFT))
      movement.x -= playerSpeed;
    if (IsKeyDown(KEY_DOWN))
      movement.y += playerSpeed;
    if (IsKeyDown(KEY_UP))
      movement.y -= playerSpeed;

    // ---- 2. HANDLE WALL COLLISIONS ----
    // Move X first, check collisions, then move Y to prevent getting stuck in
    // corners
    player.x += movement.x;
    for (const auto &box : worldColliders) {
      if (CheckCollisionRecs(player, box)) {
        // Undo movement if we hit a solid wall
        player.x -= movement.x;
        break;
      }
    }

    player.y += movement.y;
    for (const auto &box : worldColliders) {
      if (CheckCollisionRecs(player, box)) {
        // Undo movement if we hit a solid floor/ceiling
        player.y -= movement.y;
        break;
      }
    }

    // ---- 3. RENDER EVERYTHING ----
    BeginDrawing();
    ClearBackground(BLACK);

    // Draw Tiled map
    RenderLevelMap(currentLevel, terrainTex, backgroundTex);

    // Draw player bounding box so you can see it move and collide!
    DrawRectangleRec(player, RED);

    EndDrawing();
  }

  UnloadTexture(terrainTex);
  UnloadTexture(backgroundTex);
  CloseWindow();
  return 0;
}
