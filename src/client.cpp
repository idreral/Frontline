#include "client.h"
#include "renderer.h"
#include "gameStates.h"
#include <cmath>
#include <raylib.h>
#include <memory>

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
  ChangeState(std::make_unique<TestState>());

  bool shaderEnabled = 1;

  int lastWinW = gameWidth, lastWinH = gameHeight;

  while (!WindowShouldClose()) {
    if (IsKeyPressed(KEY_F11)) ToggleFullscreen();
    if (IsKeyPressed(KEY_F2)) shaderEnabled = !shaderEnabled; 

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

    BeginTextureMode(target);
      ClearBackground(BLACK);
      
      Vector2 currentGameMouse = GetGameMousePos();

      currentState->Update(*this, GetFrameTime(), currentGameMouse);

    EndTextureMode();

    BeginDrawing();
      ClearBackground(BLACK);
      if(shaderEnabled){
        BeginShaderMode(crtShader);
          DrawTexturePro(target.texture, (Rectangle){ 0, 0, (float)target.texture.width, (float)-target.texture.height }, (Rectangle){ dstX, dstY, gameWidth * scale, gameHeight * scale }, (Vector2){ 0, 0 }, 0.0f, WHITE);
        EndShaderMode();
      } else {
        DrawTexturePro(target.texture, (Rectangle){ 0, 0, (float)target.texture.width, (float)-target.texture.height }, (Rectangle){ dstX, dstY, gameWidth * scale, gameHeight * scale }, (Vector2){ 0, 0 }, 0.0f, WHITE); 
      }
    EndDrawing();
  }
}

void Client::Stop(){
  ShowCursor();
  UnloadShader(crtShader);
  UnloadRenderTexture(target);
  CloseWindow();
}

void Client::ChangeState(std::unique_ptr<GameState> newState){
  currentState = std::move(newState);
}
