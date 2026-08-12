#include <raylib.h>
#include <vector>

// clang-format off
int level[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 1, 1, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};
// clang-format on

typedef struct Sprite {
  Texture2D texture;
  Rectangle dest_rect;
  Vector2 vel;
} Sprite;

void move_player(Sprite *player) {
  player->vel.x = 0.0;
  if (IsKeyDown(KEY_D)) {
    player->vel.x = 150.0;
  }
  if (IsKeyDown(KEY_A)) {
    player->vel.x = -150.0;
  }
  if (IsKeyPressed(KEY_SPACE)) {
    player->vel.y = -300.0;
  }
}

// creates a smaller rectangle out of the destination rectangle.
// e.g. [px, py, 16.0, 24.0]
Rectangle player_hitbox(Sprite *player) {
  return (Rectangle){
      .x = player->dest_rect.x + 8.0f,
      .y = player->dest_rect.y + 8.0f,
      .width = 16.0f,
      .height = 24.0f,
  };
}

void apply_gravity(Sprite *sprite) {
  sprite->vel.y += 32.0;
  if (sprite->vel.y > 600.0) {
    sprite->vel.y = 600.0;
  }
}

void apply_vel_x(Sprite *sprite) {
  sprite->dest_rect.x += sprite->vel.x * GetFrameTime();
}

void apply_vel_y(Sprite *sprite) {
  sprite->dest_rect.y += sprite->vel.y * GetFrameTime();
}

void check_collisions_y(Sprite *sprite, std::vector<Sprite> &tiles) {

  if (sprite->dest_rect.y > GetScreenHeight() - sprite->dest_rect.height) {
    sprite->dest_rect.y = GetScreenHeight() - sprite->dest_rect.height;
  }
  Rectangle hitbox = player_hitbox(sprite);

  for (const auto &tile : tiles) {
    // if player's rect intersects tile's rect, do something!!!
    if (CheckCollisionRecs(hitbox, tile.dest_rect)) {
      // reverse the overlap

      // moving sprite is on bottom
      if (hitbox.y > tile.dest_rect.y) {
        sprite->dest_rect.y = tile.dest_rect.y + tile.dest_rect.height - 8.0f;
        // moving sprite is on the top
      } else {
        sprite->dest_rect.y = tile.dest_rect.y - sprite->dest_rect.height;
      }
    }
  }
}

void check_collisions_x(Sprite *sprite, std::vector<Sprite> &tiles) {
  Rectangle hitbox = player_hitbox(sprite);

  for (const auto &tile : tiles) {
    // if player's rect intersects tile's rect, do something!!!
    if (CheckCollisionRecs(hitbox, tile.dest_rect)) {
      // reverse the overlap

      // moving sprite is on the right
      if (hitbox.x > tile.dest_rect.x) {
        sprite->dest_rect.x = tile.dest_rect.x + tile.dest_rect.width - 8.0f;
        // moving sprite is on the left
      } else {
        sprite->dest_rect.x = tile.dest_rect.x - sprite->dest_rect.width + 8.0f;
      }
    }
  }
}

std::vector<Sprite> load_level(Texture2D temp_texture,
                               Texture2D temp_texture2) {
  std::vector<Sprite> sprites;

  const int level_width = 24;
  const int level_height = 5;

  for (size_t i = 0; i < level_height * level_width; i++) {
    size_t x = i % level_width;
    size_t y = i / level_width;

    if (level[i] == 1) {
      sprites.push_back((Sprite){.texture = temp_texture,
                                 .dest_rect = (Rectangle){.x = x * 32.0f,
                                                          .y = y * 32.0f,
                                                          .width = 32.0f,
                                                          .height = 32.0f}});
    }
    if (level[i] == 2) {
      sprites.push_back((Sprite){.texture = temp_texture2,
                                 .dest_rect = (Rectangle){.x = x * 32.0f,
                                                          .y = y * 32.0f,
                                                          .width = 32.0f,
                                                          .height = 32.0f}});
    }
  }

  return sprites;
}
int main() {

  // init app
  InitWindow(600, 400, "awesome window");
  SetTargetFPS(60);

  Texture2D player_idle_texture =
      LoadTexture("assets/heros/herochar_idle_anim_strip_4.png");
  Texture2D tiles_texture = LoadTexture("assets/brickmario.png");
  Texture2D luckyblocktexture = LoadTexture("assets/luckyblock.png");

  Sprite player = (Sprite){.texture = player_idle_texture,
                           .dest_rect = (Rectangle){
                               .x = 10.0,
                               .y = -200.0,
                               .width = 32.0,
                               .height = 32.0,
                           }};
  std::vector<Sprite> level_tiles =
      load_level(tiles_texture, luckyblocktexture);
  // camera
  Camera2D camera = {0};
  camera.target =
      (Vector2){player.dest_rect.x + 20.0f, player.dest_rect.y + 20.0f};
  camera.offset = (Vector2){800 / 2.0f, 600 / 2.0f};
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;
  // run app
  while (!WindowShouldClose()) {

    // update section
    move_player(&player);
    apply_gravity(&player);

    // after all movement updates
    apply_vel_y(&player);
    check_collisions_y(&player, level_tiles);
    apply_vel_x(&player);
    check_collisions_x(&player, level_tiles);

    if (player.dest_rect.y > GetScreenHeight() - player.dest_rect.height) {
      player.dest_rect.y = GetScreenHeight() - player.dest_rect.height;
    }

    camera.target = (Vector2){player.dest_rect.x + 20, player.dest_rect.y + 20};
    // draw section
    BeginDrawing();

    // all drawing happens
    ClearBackground(SKYBLUE);

    BeginMode2D(camera);
    // draw the level tiles

    for (const auto &tile : level_tiles) {
      DrawTexturePro(tile.texture, {0, 0, 203, 199}, tile.dest_rect, {0, 0},
                     0.0, RAYWHITE);
    }
    DrawTexturePro(player.texture, {0, 0, 16, 16}, player.dest_rect, {0, 0},
                   0.0, WHITE);
    EndMode2D();

    EndDrawing();
  }

  // free your memory!!!
  UnloadTexture(player_idle_texture);
  UnloadTexture(tiles_texture);

  // close app
  CloseWindow();

  return 0;
}
