#include "gameStates.h"

#include "client.h"
#include <raylib.h>
#include "renderer.h"
#include "UIManager.h"

TestState::TestState(){
  UIManager ui;
}

void TestState::Update(Client& client, float dT, Vector2 mousePos){
  Vector2 start = { 0, 0 };
  Vector2 end = { 0, 0 };
  bool hasStart = 0;
  bool hasEnd = 0;

  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
    if (hasStart && hasEnd) {
      start = mousePos;
      hasStart = 1;
      hasEnd = 0;
    } else if (hasStart && !hasEnd) {
      end = mousePos;
      hasEnd = 1;
    } else {
      start = mousePos;
      hasStart = true;
    }
  }

  if (hasStart) DrawCircleV(start, 7, RED);
  if (hasEnd) DrawCircleV(end, 7, BLUE);

  DrawText("Left click: set points", 10, 10, 20, WHITE);

  DrawTestPallet({ 20, 100 });
  DrawCursor(mousePos, 15, WHITE);
}

void MainMenuState::Update(Client& client,float dT, Vector2 mousePos){
  DrawCursor({ 500, 500 }, 40, RED);
}
