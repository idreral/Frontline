#pragma once
#include <raylib.h>

inline float Vec2Distance(Vector2 a, Vector2 b);

inline Vector2 Vec2Sub(Vector2 a, Vector2 b);

inline Vector2 Vec2Add(Vector2 a, Vector2 b);

inline Vector2 Vec2Normalize(Vector2 v);

inline Vector2 Vec2Scale(Vector2 v, float s);

inline Vector2 Vec2Rotate(Vector2, float angleRad);

void DrawArrow(Vector2 from, Vector2 to,
               float lineThickness = 3.0f,
               float arrowLen = 20.0f,
               float featherLen = 15.0f,
               float featherAngleDeg = 45.0f,
               Color color = WHITE);

void DrawCursor(Vector2 pos, float size, Color color, float thickness = 2);

void DrawSquare(Vector2 pos, float radius, Color color, float thickness);

void DrawDiamond(Vector2 pos, float radius, Color color, float thickness);

void DrawTestPallet(Vector2 pos = { 0, 0 });
