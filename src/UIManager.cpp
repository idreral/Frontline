#include "UIManager.h"
#include "renderer.h"
#include <raylib.h>
#include <string>

void UIText::Draw(){}

void UIButton::Draw(){}

void UIManager::Update(float dT){}

void UIManager::CreateButton(Vector2 pos1, Vector2 pos2, std::string text, float textSize, Color color){
  DrawText(text, pos1.x, pos1.y, textSize, color);
}

void UIManager::CreateText(Vector2 pos, std::string text, float textSize, Color color){}
