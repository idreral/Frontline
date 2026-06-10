#include "client.h"
#include "renderer.h"
#include <cmath>
#include <raylib.h>


void Client::Update(float dT){
}

Client::Client(){
  gameWidth = 1280;
  gameHeight = 720;
}

Client::~Client(){}

bool Client::Start(){
  SetConfigFlags(FLAG_MSAA_4X_HINT);
  InitWindow(gameWidth, gameHeight, "Frontline");
  SetWindowState(FLAG_WINDOW_RESIZABLE);
  SetTargetFPS(60);

  HideCursor();
  
  crtShader = LoadShader(0, "resources/crt_effect.fs");
  if (crtShader.id != 0) {
    Vector2 texSize = { (float)gameWidth, (float)gameHeight };
    float distortion = 0.07f;
    float chromaStrength = 0.008f;
    
    SetShaderValue(crtShader, GetShaderLocation(crtShader, "resolution"), &texSize, SHADER_UNIFORM_VEC2);
    SetShaderValue(crtShader, GetShaderLocation(crtShader, "distortion"), &distortion, SHADER_UNIFORM_FLOAT);
    SetShaderValue(crtShader, GetShaderLocation(crtShader, "chromaStrength"), &chromaStrength, SHADER_UNIFORM_FLOAT);
  }

  target = LoadRenderTexture(gameWidth, gameHeight);
  return 1;
}

void Client::Run(){
  Vector2 start = { 0, 0 };
  Vector2 end = { 0, 0 };
  bool hasStart = 0;
  bool hasEnd = 0;

  int lastWinW = gameWidth, lastWinH = gameHeight;

  while (!WindowShouldClose()) {
    if (IsKeyPressed(KEY_F11)) ToggleFullscreen();

    float scale = fminf((float)GetScreenWidth() / gameWidth, (float)GetScreenHeight() / gameHeight);
    float dstX = (GetScreenWidth() - gameWidth * scale) * 0.5f;
    float dstY = (GetScreenHeight() - gameHeight * scale) * 0.5f;

    auto GetGameMousePos = [&]() -> Vector2 {
      Vector2 mouse = GetMousePosition();
      float gx = (mouse.x - dstX) / scale;
      float gy = (mouse.y - dstY) / scale;
      if (gx < 0) gx = 0; if (gx > gameWidth) gx = gameWidth;
      if (gy < 0) gy = 0; if (gy > gameHeight) gy = gameHeight;
      return{ gx, gy };
    };

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      Vector2 gameMouse = GetGameMousePos();
      if (hasStart && hasEnd) {
        start = gameMouse;
        hasStart = 1;
        hasEnd = 0;
      } else if (hasStart && !hasEnd) {
        end = gameMouse;
        hasEnd = 1;
      } else {
        start = gameMouse;
        hasStart = true;
      }
    }

    BeginTextureMode(target);
  
      Vector2 currentGameMouse = GetGameMousePos();
      if (hasStart && !hasEnd) DrawArrow(start, currentGameMouse);
      else if (hasStart && hasEnd) DrawArrow(start, end); 
      
      ClearBackground(BLACK);
      if (hasStart) DrawCircleV(start, 7, RED);
      if (hasEnd) DrawCircleV(end, 7, BLUE);

      DrawText("Left click: set points", 10, 10, 20, WHITE);

      DrawCursor(currentGameMouse, 12.0f, WHITE);
    EndTextureMode();

    BeginDrawing();
      ClearBackground(BLACK);
      BeginShaderMode(crtShader);
        DrawTexturePro(target.texture, (Rectangle){ 0, 0, (float)target.texture.width, (float)-target.texture.height }, (Rectangle){ dstX, dstY, gameWidth * scale, gameHeight * scale }, (Vector2){ 0, 0 }, 0.0f, WHITE);
      EndShaderMode();
    EndDrawing();
  }
}

void Client::Stop(){
  ShowCursor();

  UnloadShader(crtShader);
  UnloadRenderTexture(target);
  CloseWindow();
}
