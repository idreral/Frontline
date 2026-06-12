#include "renderer.h"
#include <raylib.h>
#include <cmath>

inline float Vec2Distance(Vector2 a, Vector2 b) {
  float dx = a.x - b.x, dy = a.y - b.y;
  return sqrtf(dx*dx + dy*dy);
}

inline Vector2 Vec2Sub(Vector2 a, Vector2 b) {
  return {a.x -b.x, a.y - b.y};
}

inline Vector2 Vec2Add(Vector2 a, Vector2 b) {
  return {a.x+b.x, a.y + b.y};
}

inline Vector2 Vec2Normalize(Vector2 v) {
  float len = sqrtf(v.x*v.x + v.y*v.y);
  if (len == 0) return {0,0};
  return {v.x / len, v.y / len};
}

inline Vector2 Vec2Scale(Vector2 v, float s) {
  return {v.x * s, v.y * s};
}

inline Vector2 Vec2Rotate(Vector2 v, float angleRad) {
  float c = cosf(angleRad), s = sinf(angleRad);
  return {v.x * c - v.y * s, v.x * s + v.y * c};
}    

void DrawArrow(Vector2 from, Vector2 to, float lineThickness, float arrowLen, float featherLen, float featherAngleDeg, Color color) {
  if (Vec2Distance(from, to) < 5.0f) return;
  DrawLineEx(from, to, lineThickness, color);
  Vector2 dir = Vec2Normalize(Vec2Sub(to, from));
  Vector2 perp = {-dir.y, dir.x};
  Vector2 tip = to;
  Vector2 base = Vec2Sub(tip, Vec2Scale(dir, arrowLen));
  Vector2 left = Vec2Add(base, Vec2Scale(perp, arrowLen * 0.4f));
  Vector2 right = Vec2Sub(base, Vec2Scale(perp, arrowLen * 0.4f));
  DrawTriangle(tip, left, right, color);
  Vector2 mid = Vec2Scale(Vec2Add(from, to), 0.5f);
  float featherRad = featherAngleDeg * 3.14159f / 180.0f;
  Vector2 backward = Vec2Scale(dir, -1.0f);
  Vector2 leftFeatherDir = Vec2Rotate(backward, featherRad);
  Vector2 rightFeatherDir = Vec2Rotate(backward, -featherRad);
  Vector2 leftFeatherEnd = Vec2Add(mid, Vec2Scale(leftFeatherDir, featherLen));
  Vector2 rightFeatherEnd = Vec2Add(mid, Vec2Scale(rightFeatherDir, featherLen));
  DrawLineEx(mid, leftFeatherEnd, lineThickness, color);
  DrawLineEx(mid, rightFeatherEnd, lineThickness, color);
}

void DrawCursor(Vector2 pos, float size, Color color, float thickness) {
  Vector2 leftLeg = { pos.x, pos.y+size};
  Vector2 rightLeg = { pos.x+size/2, pos.y+size};
  DrawLineEx(pos, leftLeg, thickness, color);
  DrawLineEx(pos, rightLeg, thickness, color);
  //DrawCircleV(pos, 3, RED);
}

void DrawSquare(Vector2 pos, float radius, Color color, float thickness) {
  float left   = pos.x - radius/2;
  float right  = pos.x + radius/2;
  float top    = pos.y - radius/2;
  float bottom = pos.y + radius/2;

  DrawLineEx({ left,  top   }, { right, top   }, thickness, color);
  DrawLineEx({ right, top   }, { right, bottom}, thickness, color);
  DrawLineEx({ right, bottom}, { left,  bottom}, thickness, color);
  DrawLineEx({ left,  bottom}, { left,  top   }, thickness, color);
}

void DrawDiamond(Vector2 pos, float radius, Color color, float thickness) {
  Vector2 top    = { pos.x, pos.y - radius/2 };
  Vector2 right  = { pos.x + radius/2, pos.y };
  Vector2 bottom = { pos.x, pos.y + radius/2 };
  Vector2 left   = { pos.x - radius/2, pos.y };

  DrawLineEx(top, right, thickness, color);
  DrawLineEx(right, bottom, thickness, color);
  DrawLineEx(bottom, left, thickness, color);
  DrawLineEx(left, top, thickness, color);
}
void DrawSoldier(Vector2 pos, float size, Color color, float thickness){
  DrawLineEx({ pos.x-size/2, pos.y+size/2 }, { pos.x+size/2, pos.y-size/2 }, thickness, color);
  DrawLineEx({ pos.x+size/2, pos.y+size/2 }, { pos.x-size/2, pos.y-size/2 }, thickness, color);
}

void DrawTank(Vector2 pos, float size, Color color, float thickness){
  DrawDiamond(pos, size, color, thickness);
}

void DrawRocketLauncher(Vector2 pos, float size, Color color, float thickness){
  DrawLineEx({ pos.x, pos.y-size/2 }, { pos.x, pos.y+size/2 }, thickness, color);
  DrawLineEx({ pos.x-size/2, pos.y },{ pos.x+size/2, pos.y}, thickness, color);
}

void DrawTower(Vector2 pos, float size, Color color, float thickness){
  DrawLineEx({ pos.x, pos.y-size/2 }, { pos.x, pos.y+size/2 }, thickness, color);
  DrawLineEx({ pos.x-size/4, pos.y }, { pos.x+size/4, pos.y }, thickness, color);
  DrawLineEx({ pos.x-size/4, pos.y+size/2 }, { pos.x+size/4, pos.y+size/2 }, thickness, color);
  DrawLineEx({ pos.x, pos.y-size/2 }, { pos.x-size/4, pos.y-size/4 }, thickness, color);
  DrawLineEx({ pos.x, pos.y-size/2 }, { pos.x+size/4, pos.y-size/4 }, thickness, color);
}

void DrawRocketSilo(Vector2 pos, float size, Color color, float thickness){
  float radius = size/4;
  DrawRing({ pos.x, pos.y-size/4}, radius-thickness, radius, -180, 0, 0, color);
  DrawLineEx({ pos.x, pos.y-size/2 }, { pos.x, pos.y+size/2}, thickness, color);
  DrawLineEx({ pos.x-size/4, pos.y-size/4 }, { pos.x-size/4, pos.y+size/2}, thickness, color);
  DrawLineEx({ pos.x+size/4, pos.y-size/4 }, { pos.x+size/4, pos.y+size/2}, thickness, color);
}

void DrawBarracks(Vector2 pos, float size, Color color, float thickness){
  DrawSquare(pos, size, color, thickness);
  DrawSoldier(pos, size, color, thickness);
}

void DrawTankFactory(Vector2 pos, float size, Color color, float thickness){
  DrawSquare(pos, size, color, thickness);
  DrawTank(pos, size, color, thickness);
}

void DrawRocketLauncherFactory(Vector2 pos, float size, Color color, float thickness){
  DrawSquare(pos, size, color, thickness);
  DrawRocketLauncher(pos, size, color, thickness);
}

void DrawTestPalletPart(Vector2 pos, float size, float padding, Color color){
  float oldX = pos.x;
  float s = size;
  float p = padding;

  DrawCircleV(pos, 2, RED);
  DrawBarracks(pos, s, color, 2);

  pos.x += s+p;
  DrawSoldier(pos, s/2, color, 2);
  DrawCircleV(pos, 2, RED);

  pos.x += s+p;
  DrawTankFactory(pos, s, color, 2);
  DrawCircleV(pos, 2, RED);

  pos.x += s+p;
  DrawTank(pos, s/2, color, 2);
  DrawCircleV(pos, 2, RED);

  pos.x += s+p;
  DrawRocketLauncherFactory(pos, s, color, 2);
  DrawCircleV(pos, 2, RED);

  pos.x += s+p;
  DrawRocketLauncher(pos, s/2, color, 2);
  DrawCircleV(pos, 2, RED);

  pos.x += s+p;
  DrawTower(pos, s, color, 2);
  DrawCircleV(pos, 2, RED);

  pos.x += s+p;
  DrawRocketSilo(pos, s, color, 2);
  DrawCircleV(pos, 2, RED);
}

void DrawTestPallet(Vector2 pos){
  float size = 20.0f;
  float usize = size;
  float padding = 10.0f;
  DrawTestPalletPart(pos, size, padding, WHITE);
  pos.y += usize+padding;
  DrawTestPalletPart(pos, size, padding, BLUE);
  pos.y += usize+padding;
  DrawTestPalletPart(pos, size, padding, RED);
  pos.y += usize+padding;
  DrawTestPalletPart(pos, size, padding, GREEN);
  pos.y += usize+padding;
  DrawTestPalletPart(pos, size, padding, YELLOW);
}
