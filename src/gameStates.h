#pragma once
#include "client.h"
#include <raylib.h>

class Client;

class GameState {
  public:
    virtual ~GameState() = default;
    virtual void Update(Client& client, float dT, Vector2 mousePos) = 0;
};

class MainMenuState : public GameState {
  public:
    void Update(Client& client, float dT, Vector2 mousePos);
};

class TestState : public GameState {
  public:
    void Update(Client& client, float dT, Vector2 mousePos);
};
