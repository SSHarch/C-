#include <cmath>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <raylib.h>
#include <string>
#include <vector>

using json = nlohmann::json;
Vector2 movement = {0, 0};

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

typedef enum AnimationType {
  REPEATING = 1,
  ONESHOT = 2,
} AnimationType;

typedef struct Animation {
  int first;
  int last;
  int cur;
  int step;
  float speed;
  float duration_left;
  AnimationType type;
} Animation;

void animation_update(Animation *self) {
  float dt = GetFrameTime();
  self->duration_left -= dt;

  if (self->duration_left <= 0.0) {
    self->duration_left = self->speed;
    self->cur += self->step;

    if (self->cur > self->last) {
      switch (self->type) {
      case REPEATING:
        self->cur = self->first;
        break;
      case ONESHOT:
        self->cur = self->last;
        break;
      }
    } else if (self->cur < self->first) {
      switch (self->type) {
      case REPEATING:
        self->cur = self->last;
        break;
      case ONESHOT:
        self->cur = self->first;
        break;
      }
    }
  }
}

Rectangle animation_frame(Animation *self, int num_frames_per_row) {
  int x = (self->cur % num_frames_per_row) * 16;
  int y = (self->cur / num_frames_per_row) * 16;
  return (Rectangle){
      .x = (float)x, .y = (float)y, .width = 16.0, .height = 16.0};
}
Rectangle goblin_animation_frame(Animation *self, int num_frames_per_row) {
  int x = (self->cur % num_frames_per_row) * 48;
  int y = (self->cur / num_frames_per_row) * 16;
  return (Rectangle){
      .x = (float)x, .y = (float)y, .width = 16.0, .height = 16.0};
}

// ---- globals ----
int enemies_in_current_level = 1;
float health = 3;
float stamina = 3;
float player_health = 6;
float iframetimer = 0.0f;
const float iframeduration_with_stamina = 1.0f;

enum SpriteDirection { Left = -1, Right = 1 };

typedef struct Sprite {
  Texture2D *texture;         // currently active texture
  Animation *anim;            // currently active animation
  Texture2D *default_texture; // reverts here when a ONESHOT ends
  Animation *default_anim;    // reverts here when a ONESHOT ends
  Rectangle dest_rect;
  Vector2 vel;
  SpriteDirection dir;
  bool is_moving;
  bool is_jumping;
  bool is_attacking;
} Sprite;

void sprite_play(Sprite *s, Texture2D *tex, Animation *anim) {
  s->texture = tex;
  s->anim = anim;
  anim->cur = anim->first; // always restart from the beginning
}

// Call once per frame inside BeginMode2D. Handles update + draw + auto-revert.
void sprite_draw(Sprite *s, int frames_per_row) {
  animation_update(s->anim);

  // if a oneshot just finished, snap back to the default (e.g. idle)
  if (s->anim->type == ONESHOT && s->anim->cur == s->anim->last) {
    s->texture = s->default_texture;
    s->anim = s->default_anim;
  }

  Rectangle frame = animation_frame(s->anim, frames_per_row);
  frame.width *= (float)s->dir; // flip horizontally based on direction
  DrawTexturePro(*s->texture, frame, s->dest_rect, {0, 0}, 0.0f, WHITE);
}

void goblin_sprite_draw(Sprite *s, int frames_per_row) {
  animation_update(s->anim);

  // if a oneshot just finished, snap back to the default (e.g. idle)
  if (s->anim->type == ONESHOT && s->anim->cur == s->anim->last) {
    s->texture = s->default_texture;
    s->anim = s->default_anim;
  }

  Rectangle frame = goblin_animation_frame(s->anim, frames_per_row);
  frame.width *= (float)s->dir; // flip horizontally based on direction
  DrawTexturePro(*s->texture, frame, s->dest_rect, {0, 0}, 0.0f, WHITE);
}

// ---- game logic functions (no drawing!) ----

Rectangle swordrange(Sprite *player) {
  return (Rectangle){
      .x = player->dest_rect.x - 32.0f,
      .y = player->dest_rect.y,
      .width = 96.0f,
      .height = 32.0f,
  };
}

Rectangle player_hitbox(Sprite *player) {
  return (Rectangle){
      .x = player->dest_rect.x + 8.0f,
      .y = player->dest_rect.y + 8.0f,
      .width = 16.0f,
      .height = 24.0f,
  };
}

void attack_function(Sprite *player, Sprite *enemy, Texture2D *attack_tex,
                     Animation *attack_anim) {
  Rectangle swordhitbox = swordrange(player);
  if (!IsKeyPressed(KEY_X) && iframetimer <= 0.0f) {
    player->is_attacking = false;
  }
  if (IsKeyPressed(KEY_X) && iframetimer <= 0.0f && stamina >= 1) {

    sprite_play(player, attack_tex, attack_anim); // trigger attack animation
    player->is_attacking = true;
    if (CheckCollisionRecs(swordhitbox, enemy->dest_rect))
      health--;
    iframetimer = iframeduration_with_stamina;
    stamina--;
  }
}

void move_player(Sprite *player, Texture2D *walk_tex, Animation *walk_anim,
                 Texture2D *idle_tex, Animation *idle_anim, Texture2D *jump_tex,
                 Animation *jump_anim) {
  float playerSpeed = 4.0f;

  // apply_gravity
  // movement.y += playerSpeed;

  if (!IsKeyDown(KEY_D) && !IsKeyDown(KEY_A) && !player->is_attacking) {
    player->is_moving = false;
  }
  if (!IsKeyDown(KEY_SPACE) && !player->is_moving && !player->is_attacking &&
      iframetimer <= 0.0f) {
    player->is_jumping = false;
  }
  // Idling
  if (!player->is_moving && !player->is_attacking && !player->is_jumping) {
    player->anim = player->default_anim;
    player->texture = player->default_texture;
  }
  // Walking
  if (IsKeyDown(KEY_D) && !player->is_attacking) {
    movement.x += playerSpeed;
    player->dir = SpriteDirection::Right;
    if (!player->is_moving && !player->is_attacking) {
      sprite_play(player, walk_tex, walk_anim);
    }
    player->is_moving = true;
  }
  if (IsKeyDown(KEY_A) && !player->is_attacking) {
    movement.x -= playerSpeed;
    player->dir = SpriteDirection::Left;
    if (!player->is_moving && !player->is_attacking) {
      sprite_play(player, walk_tex, walk_anim);
    }
    player->is_moving = true;
  }
  // Jumping
  if (IsKeyPressed(KEY_SPACE) && !player->is_attacking) {
    movement.y -= playerSpeed;
    if (!player->is_jumping && !player->is_attacking && !player->is_moving) {
      sprite_play(player, jump_tex, jump_anim);
    }
    player->is_jumping = true;
  }
}

void simple_enemy_ai(Sprite *player, Sprite *enemy, Texture2D *attack_tex,
                     Animation *attack_anim, Texture2D *walk_tex,
                     Animation *walk_anim) {
  if (!CheckCollisionRecs(player->dest_rect, enemy->dest_rect) &&
      iframetimer <= 0.0f) {
    enemy->is_attacking = false;
  }
  if (CheckCollisionRecs(player->dest_rect, enemy->dest_rect) &&
      iframetimer <= 0.0f && health > 0) {
    sprite_play(enemy, attack_tex, attack_anim);
    enemy->is_attacking = true;
    player_health--;
    iframetimer = iframeduration_with_stamina;
  }
  if (player->dest_rect.x - enemy->dest_rect.x < 150 &&
      enemy->dest_rect.x - player->dest_rect.x < 150 && !enemy->is_attacking) {
    enemy->is_moving = false;
  }
  if (!enemy->is_moving && !enemy->is_attacking) {
    enemy->anim = enemy->default_anim;
    enemy->texture = enemy->default_texture;
  }

  if (player->dest_rect.x > enemy->dest_rect.x &&
      player->dest_rect.x - enemy->dest_rect.x < 150 && !enemy->is_attacking) {
    enemy->dest_rect.x += 0.03f;
    enemy->dir = SpriteDirection::Right;
    if (!enemy->is_moving && !enemy->is_attacking) {
      sprite_play(enemy, walk_tex, walk_anim);
    }
    enemy->is_moving = true;
  }
  if (player->dest_rect.x < enemy->dest_rect.x &&
      enemy->dest_rect.x - player->dest_rect.x < 150 && !enemy->is_attacking) {
    enemy->dest_rect.x -= 0.03f;
    enemy->dir = SpriteDirection::Left;
    if (!enemy->is_moving && !enemy->is_attacking) {
      sprite_play(enemy, walk_tex, walk_anim);
    }
    enemy->is_moving = true;
  }
}

void apply_gravity(Sprite *sprite) {
  sprite->vel.y += 32.0;
  if (sprite->vel.y > 600.0)
    sprite->vel.y = 600.0;
}

/*void apply_vel_x(Sprite *sprite) {
  sprite->dest_rect.x += sprite->vel.x * GetFrameTime();
}
void apply_vel_y(Sprite *sprite) {
  sprite->dest_rect.y += sprite->vel.y * GetFrameTime();
}*/

void check_collisions_y(Sprite *sprite,
                        std::vector<Rectangle> &worldColliders) {

  if (sprite->dest_rect.y > GetScreenHeight() - sprite->dest_rect.height) {
    sprite->dest_rect.y = GetScreenHeight() - sprite->dest_rect.height;
    sprite->vel.y = 0;
  }
  Rectangle hitbox = player_hitbox(sprite);

  sprite->dest_rect.y += movement.y;
  for (const auto &box : worldColliders) {
    if (CheckCollisionRecs(sprite->dest_rect, box)) {
      // Undo movement if we hit a solid floor/ceiling
      sprite->dest_rect.y -= movement.y;
      break;
    }
  }
}

void check_collisions_x(Sprite *sprite,
                        std::vector<Rectangle> &worldColliders) {
  Rectangle hitbox = player_hitbox(sprite);

  for (const auto &box : worldColliders) {
    // if player's rect intersects tile's rect, do something!!!
    sprite->dest_rect.x += movement.x;
    for (const auto &box : worldColliders) {
      if (CheckCollisionRecs(sprite->dest_rect, box)) {
        // Undo movement if we hit a solid wall
        sprite->dest_rect.x -= movement.x;
        break;
      }
    }
  }
}

void showplayerstats() {
  DrawRectangle(10, 10, 75 * player_health, 25, RED);
  DrawRectangle(10, 45, 45 * stamina, 15, GREEN);
}

int main() {
  InitWindow(800, 600, "Attack!");

  TiledMap currentLevel = LoadLevelMap("tiled/test.json");
  Texture2D terrainTex = LoadTexture("assets/tiles/tileset_32x32(new).png");
  Texture2D backgroundTex = LoadTexture("assets/tiles/background.png");

  std::vector<Rectangle> worldColliders = CreateMapColliders(currentLevel);
  // textures
  Texture2D player_idle_texture =
      LoadTexture("assets/heros/herochar_idle_anim_strip_4.png");
  Texture2D player_walk_texture =
      LoadTexture("assets/heros/herochar_run_anim_strip_6.png");
  Texture2D player_attack_texture =
      LoadTexture("assets/heros/herochar_attack_anim_strip_4.png");
  Texture2D player_jump_texture =
      LoadTexture("assets/heros/herochar_jump_up_anim_strip_3.png");
  Texture2D enemy_idle_texture =
      LoadTexture("assets/enemies/goblin/goblin_idle_anim_strip_4.png");
  Texture2D enemy_walk_texture =
      LoadTexture("assets/enemies/goblin/goblin_run_anim_strip_6.png");
  Texture2D enemy_attack_texture =
      LoadTexture("assets/enemies/goblin/goblin_attack_anim_strip_4.png");
  Texture2D tiles_texture = LoadTexture("assets/brickmario.png");
  Texture2D luckyblocktexture = LoadTexture("assets/luckyblock.png");

  Animation player_idle_anim = {.first = 0,
                                .last = 4,
                                .cur = 0,
                                .step = 1,
                                .speed = 0.15,
                                .duration_left = 0.15,
                                .type = REPEATING};
  Animation player_walk_anim = {.first = 0,
                                .last = 6,
                                .cur = 0,
                                .step = 1,
                                .speed = 0.15,
                                .duration_left = 0.15,
                                .type = REPEATING};
  Animation player_attack_anim = {.first = 0,
                                  .last = 4,
                                  .cur = 0,
                                  .step = 1,
                                  .speed = 0.20,
                                  .duration_left = 0.08,
                                  .type = ONESHOT};
  Animation player_jump_anim = {.first = 0,
                                .last = 3,
                                .cur = 0,
                                .step = 1,
                                .speed = 0.15,
                                .duration_left = 0.15,
                                .type = ONESHOT};
  Animation enemy_walk_anim = {.first = 0,
                               .last = 6,
                               .cur = 0,
                               .step = 1,
                               .speed = 0.15,
                               .duration_left = 0.15,
                               .type = REPEATING};

  Sprite player = {
      .texture = &player_idle_texture,
      .anim = &player_idle_anim,
      .default_texture = &player_idle_texture, // reverts to idle
      .default_anim = &player_idle_anim,
      .dest_rect = {10.0, -200.0, 32.0, 32.0},
      .dir = SpriteDirection::Right,
  };

  Sprite enemy = {
      .texture = &enemy_idle_texture,
      .anim = &player_idle_anim,
      .default_texture = &enemy_idle_texture,
      .default_anim = &player_idle_anim,
      .dest_rect = {400.0, 568.0, 32.0, 32.0},
      .dir = SpriteDirection::Left,
  };

  Camera2D camera = {0};
  camera.target =
      (Vector2){player.dest_rect.x + 20.0f, player.dest_rect.y + 20.0f};
  camera.offset = (Vector2){800 / 2.0f, 600 / 2.0f};
  camera.zoom = 1.0f;

  while (!WindowShouldClose()) {
    // --- logic ---
    // random shit i wrote by myself
    iframetimer -= GetFrameTime();
    if (stamina < 3)
      stamina += 0.001f;

    move_player(&player, &player_walk_texture, &player_walk_anim,
                &player_idle_texture, &player_idle_anim, &player_jump_texture,
                &player_jump_anim);
    apply_gravity(&player);
    attack_function(&player, &enemy, &player_attack_texture,
                    &player_attack_anim);
    simple_enemy_ai(&player, &enemy, &enemy_attack_texture, &player_attack_anim,
                    &enemy_walk_texture, &player_walk_anim);
    /*  apply_vel_y(&player);
      apply_vel_x(&player);

      check_collisions_x(&player, worldColliders);
      check_collisions_y(&player, worldColliders);*/

    /*  player.dest_rect.x += movement.x;
      for (const auto &box : worldColliders) {
        if (CheckCollisionRecs(player.dest_rect, box)) {
          player.dest_rect.x -= movement.x;
          break;
        }
      }

      player.dest_rect.y += movement.y;
      for (const auto &box : worldColliders) {
        if (CheckCollisionRecs(player.dest_rect, box)) {
          player.dest_rect.y -= movement.y;
          break;
        }
      }*/

    // ---- 1. MOVE & COLLIDE Y AXIS (Gravity / Jumping) ----
    float deltaY =
        player.vel.y *
        GetFrameTime();           // Exactly how much we want to move this frame
    player.dest_rect.y += deltaY; // Try moving

    Rectangle hitboxY =
        player_hitbox(&player); // Get the current hitbox at the new position
    for (const auto &box : worldColliders) {
      if (CheckCollisionRecs(hitboxY, box)) {
        player.dest_rect.y -= deltaY; // UNDO MOVEMENT (jsonloader style!)

        if (player.vel.y > 0) {
          player.is_jumping = false; // Landed on floor safely, reset jump
        }
        player.vel.y = 0; // Kill gravity buildup
        break;            // Stop checking other blocks this frame
      }
    }

    // ---- 2. MOVE & COLLIDE X AXIS (Walking / Running) ----
    float deltaX =
        player.vel.x *
        GetFrameTime(); // Exactly how much we want to move horizontally
    player.dest_rect.x += deltaX; // Try moving

    Rectangle hitboxX = player_hitbox(&player); // Update hitbox position
    for (const auto &box : worldColliders) {
      if (CheckCollisionRecs(hitboxX, box)) {
        player.dest_rect.x -= deltaX; // UNDO MOVEMENT (jsonloader style!)
        player.vel.x = 0;             // Stop horizontal speed
        break;                        // Stop checking other blocks this frame
      }
    }

    if (player.dest_rect.y > GetScreenHeight() - player.dest_rect.height)
      player.dest_rect.y = GetScreenHeight() - player.dest_rect.height;

    camera.target = (Vector2){player.dest_rect.x + 20, player.dest_rect.y + 20};

    // --- draw ---
    BeginDrawing();
    ClearBackground(BLACK);
    BeginMode2D(camera);
    RenderLevelMap(currentLevel, terrainTex, backgroundTex);

    sprite_draw(&player, 4); // Draw player

    if (health > 0) {
      goblin_sprite_draw(&enemy, 4); // Draw enemy

      DrawRectangle(enemy.dest_rect.x, enemy.dest_rect.y - 10, 10 * health, 7,
                    RED); // Draw enemy health
    }

    DrawRectangle(0, 600, 10000, 10, BLUE); // ground

    EndMode2D();
    showplayerstats(); // drawn outside camera so it stays fixed on screen
    EndDrawing();
  }

  UnloadTexture(player_idle_texture);
  UnloadTexture(player_walk_texture);
  UnloadTexture(player_attack_texture);
  UnloadTexture(enemy_idle_texture);
  UnloadTexture(player_idle_texture);
  UnloadTexture(tiles_texture);

  CloseWindow();
  return 0;
}
