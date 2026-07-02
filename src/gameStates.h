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
    ~MainMenuState() override = default;
    void Update(Client& client, float dT, Vector2 mousePos);
  private:
    UIManager ui;
};

class TestState : public GameState {
  public:
    TestState();
    ~TestState() override = default;
    void Update(Client& client, float dT, Vector2 mousePos);
  private:
};
