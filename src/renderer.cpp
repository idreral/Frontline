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

void DrawCursor(Vector2 pos, float size, Color color) {
    Vector2 local[3] = {
        { 0, -size },
        { -size * 0.6f, size * 0.4f },
        { size * 0.6f, size * 0.4f }
    };
    DrawTriangle(Vec2Add(pos, local[0]), Vec2Add(pos, local[1]), Vec2Add(pos, local[2]), color);
}

