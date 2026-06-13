#pragma once
#include <raylib.h>
#include "gameStates.h"
#include <memory>

class GameState;

class Client {
  public:
    Client();
    ~Client();

    bool Start();
    void Run();
    void Stop();

    void ChangeState(std::unique_ptr<GameState> newState);
  private:
    int gameWidth;
    int gameHeight;
    std::unique_ptr<GameState> currentState;
    RenderTexture2D target;
    Shader crtShader;
};
