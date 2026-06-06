#include <raylib.h>
#include <cmath>

static inline float Vec2Distance(Vector2 a, Vector2 b) {
    float dx = a.x - b.x, dy = a.y - b.y;
    return sqrtf(dx*dx + dy*dy);
}
static inline Vector2 Vec2Sub(Vector2 a, Vector2 b) {
    return { a.x - b.x, a.y - b.y };
}
static inline Vector2 Vec2Normalize(Vector2 v) {
    float len = sqrtf(v.x*v.x + v.y*v.y);
    if (len == 0) return { 0, 0 };
    return { v.x / len, v.y / len };
}
static inline Vector2 Vec2Scale(Vector2 v, float s) {
    return { v.x * s, v.y * s };
}
static inline Vector2 Vec2Add(Vector2 a, Vector2 b) {
    return { a.x + b.x, a.y + b.y };
}
static inline Vector2 Vec2Rotate(Vector2 v, float angleRad) {
    float c = cosf(angleRad), s = sinf(angleRad);
    return { v.x * c - v.y * s, v.x * s + v.y * c };
}

// Стрелка с наклонными перьями посередине (направлены назад)
void DrawArrow(Vector2 from, Vector2 to,
               float lineThickness = 3.0f,
               float arrowLen = 20.0f,
               float featherLen = 15.0f,
               float featherAngleDeg = 45.0f) {
    if (Vec2Distance(from, to) < 5.0f) return;

    // Основная линия
    DrawLineEx(from, to, lineThickness, WHITE);

    // Направление стрелки (от from к to)
    Vector2 dir = Vec2Normalize(Vec2Sub(to, from));
    Vector2 perp = { -dir.y, dir.x };

    // 1. Наконечник (треугольник на конце)
    Vector2 tip = to;
    Vector2 base = Vec2Sub(tip, Vec2Scale(dir, arrowLen));
    Vector2 left  = Vec2Add(base, Vec2Scale(perp, arrowLen * 0.4f));
    Vector2 right = Vec2Sub(base, Vec2Scale(perp, arrowLen * 0.4f));
    DrawTriangle(tip, left, right, WHITE);

    // 2. Перья на середине — наклонены назад (к началу стрелки)
    Vector2 mid = Vec2Scale(Vec2Add(from, to), 0.5f); // средняя точка
    float featherRad = featherAngleDeg * 3.14159f / 180.0f;

    // Направление "назад" (от средней точки к началу)
    Vector2 backward = Vec2Scale(dir, -1.0f);
    // Поворачиваем назад влево и вправо
    Vector2 leftFeatherDir  = Vec2Rotate(backward,  featherRad);
    Vector2 rightFeatherDir = Vec2Rotate(backward, -featherRad);

    Vector2 leftFeatherEnd  = Vec2Add(mid, Vec2Scale(leftFeatherDir, featherLen));
    Vector2 rightFeatherEnd = Vec2Add(mid, Vec2Scale(rightFeatherDir, featherLen));

    DrawLineEx(mid, leftFeatherEnd, lineThickness, WHITE);
    DrawLineEx(mid, rightFeatherEnd, lineThickness, WHITE);
}

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "Frontline");
    SetTargetFPS(60);

    Vector2 start = { 0, 0 };
    Vector2 end   = { 0, 0 };
    bool hasStart = false;
    bool hasEnd   = false;

    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (hasStart && hasEnd) {
                start = mousePos;
                hasStart = true;
                hasEnd = false;
            } else if (hasStart && !hasEnd) {
                end = mousePos;
                hasEnd = true;
            } else {
                start = mousePos;
                hasStart = true;
            }
        }

        BeginDrawing();
        ClearBackground(DARKGRAY);

        if (hasStart) DrawCircleV(start, 7, RED);
        if (hasEnd)   DrawCircleV(end,   7, BLUE);

        if (hasStart && !hasEnd)
            DrawArrow(start, mousePos);
        else if (hasStart && hasEnd)
            DrawArrow(start, end);

        DrawText("Left click: set points", 10, 10, 20, DARKGRAY);
        if (!hasStart)
            DrawText("Click to set start point", 10, 40, 20, BLACK);
        else if (hasStart && !hasEnd)
            DrawText("Click to set end point (arrow fixed)", 10, 40, 20, BLACK);
        else if (hasStart && hasEnd)
            DrawText("Click to set new start point (old end removed)", 10, 40, 20, BLACK);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
