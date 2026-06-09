#include <raylib.h>
#include <cmath>

// ----------------------------------------------------------------------------
// Вспомогательные геометрические функции (уже были)
// ----------------------------------------------------------------------------
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

// ----------------------------------------------------------------------------
// Отрисовка стрелки (уже была)
// ----------------------------------------------------------------------------
void DrawArrow(Vector2 from, Vector2 to,
               float lineThickness = 3.0f,
               float arrowLen = 20.0f,
               float featherLen = 15.0f,
               float featherAngleDeg = 45.0f) {
    if (Vec2Distance(from, to) < 5.0f) return;
    DrawLineEx(from, to, lineThickness, WHITE);
    Vector2 dir = Vec2Normalize(Vec2Sub(to, from));
    Vector2 perp = { -dir.y, dir.x };
    Vector2 tip = to;
    Vector2 base = Vec2Sub(tip, Vec2Scale(dir, arrowLen));
    Vector2 left  = Vec2Add(base, Vec2Scale(perp, arrowLen * 0.4f));
    Vector2 right = Vec2Sub(base, Vec2Scale(perp, arrowLen * 0.4f));
    DrawTriangle(tip, left, right, WHITE);
    Vector2 mid = Vec2Scale(Vec2Add(from, to), 0.5f);
    float featherRad = featherAngleDeg * 3.14159f / 180.0f;
    Vector2 backward = Vec2Scale(dir, -1.0f);
    Vector2 leftFeatherDir  = Vec2Rotate(backward,  featherRad);
    Vector2 rightFeatherDir = Vec2Rotate(backward, -featherRad);
    Vector2 leftFeatherEnd  = Vec2Add(mid, Vec2Scale(leftFeatherDir, featherLen));
    Vector2 rightFeatherEnd = Vec2Add(mid, Vec2Scale(rightFeatherDir, featherLen));
    DrawLineEx(mid, leftFeatherEnd, lineThickness, WHITE);
    DrawLineEx(mid, rightFeatherEnd, lineThickness, WHITE);
}

// ----------------------------------------------------------------------------
// Отрисовка сетки (уже была)
// ----------------------------------------------------------------------------
void DrawGrid(int x, int y, int width, int height,
              int cellsX, int cellsY,
              Color color, float thickness = 1.0f) {
    float stepX = (float)width / cellsX;
    for (int i = 0; i <= cellsX; ++i) {
        float lineX = x + i * stepX;
        DrawLineEx({ lineX, (float)y }, { lineX, (float)(y + height) },
                   thickness, color);
    }
    float stepY = (float)height / cellsY;
    for (int i = 0; i <= cellsY; ++i) {
        float lineY = y + i * stepY;
        DrawLineEx({ (float)x, lineY }, { (float)(x + width), lineY },
                   thickness, color);
    }
}

// ----------------------------------------------------------------------------
// НОВАЯ ФУНКЦИЯ: отрисовка курсора‑треугольника под углом 45°
// ----------------------------------------------------------------------------
void DrawCustomCursor(Vector2 pos, float size, Color color) {
    const float angleDeg = 45.0f;
    const float angleRad = angleDeg * 3.14159f / 180.0f;

    // Локальные координаты треугольника (вершина вверх, основание внизу)
    Vector2 local[3] = {
        { 0.0f,      -size },          // вершина
        { -size/2.0f,  size },         // левый нижний угол
        {  size/2.0f,  size }          // правый нижний угол
    };

    Vector2 world[3];
    for (int i = 0; i < 3; ++i) {
        // Поворот на 45°
        float x = local[i].x * cosf(angleRad) - local[i].y * sinf(angleRad);
        float y = local[i].x * sinf(angleRad) + local[i].y * cosf(angleRad);
        // Смещение в позицию курсора
        world[i] = { pos.x + x, pos.y + y };
    }

    DrawTriangle(world[0], world[1], world[2], color);
}

// ----------------------------------------------------------------------------
// Главная функция
// ----------------------------------------------------------------------------
int main() {
    const int gameWidth = 1280;
    const int gameHeight = 720;

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(gameWidth, gameHeight, "Frontline");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);

    // Скрываем системный курсор
    HideCursor();

    // Загрузка шейдера (файл должен существовать)
    Shader crtShader = LoadShader(0, "resources/crt_effect.fs");
    if (crtShader.id != 0) {
        Vector2 texSize = { (float)gameWidth, (float)gameHeight };
        float distortion = 0.07f;
        float chromaStrength = 0.008f;
        float hexSize = 1.6f;
        float hexLineWidth = 0.1f;
        float hexBrightness = 0.0f;

        SetShaderValue(crtShader, GetShaderLocation(crtShader, "resolution"), &texSize, SHADER_UNIFORM_VEC2);
        SetShaderValue(crtShader, GetShaderLocation(crtShader, "distortion"), &distortion, SHADER_UNIFORM_FLOAT);
        SetShaderValue(crtShader, GetShaderLocation(crtShader, "chromaStrength"), &chromaStrength, SHADER_UNIFORM_FLOAT);
    }

    RenderTexture2D target = LoadRenderTexture(gameWidth, gameHeight);

    Vector2 start = { 0, 0 };
    Vector2 end   = { 0, 0 };
    bool hasStart = false;
    bool hasEnd   = false;

    int lastWinW = gameWidth, lastWinH = gameHeight;

    while (!WindowShouldClose()) {
        // --- Масштабирование и преобразование координат мыши ---
        float scale = fminf((float)GetScreenWidth()  / gameWidth,
                            (float)GetScreenHeight() / gameHeight);
        float dstX = (GetScreenWidth()  - gameWidth  * scale) * 0.5f;
        float dstY = (GetScreenHeight() - gameHeight * scale) * 0.5f;

        auto GetGameMousePos = [&]() -> Vector2 {
            Vector2 mouse = GetMousePosition();
            float gx = (mouse.x - dstX) / scale;
            float gy = (mouse.y - dstY) / scale;
            if (gx < 0) gx = 0; if (gx > gameWidth)  gx = gameWidth;
            if (gy < 0) gy = 0; if (gy > gameHeight) gy = gameHeight;
            return { gx, gy };
        };

        // --- Обработка кликов ---
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 gameMouse = GetGameMousePos();
            if (hasStart && hasEnd) {
                start = gameMouse;
                hasStart = true;
                hasEnd = false;
            } else if (hasStart && !hasEnd) {
                end = gameMouse;
                hasEnd = true;
            } else {
                start = gameMouse;
                hasStart = true;
            }
        }

        if (IsKeyPressed(KEY_F11)) ToggleFullscreen();

        // --- Рендеринг на текстуру (игровой мир) ---
        BeginTextureMode(target);
            ClearBackground(BLACK);
            DrawGrid(-10, -10, gameWidth, gameHeight, 8, 8, WHITE, 1.0f);
            if (hasStart) DrawCircleV(start, 7, RED);
            if (hasEnd)   DrawCircleV(end,   7, BLUE);

            Vector2 currentGameMouse = GetGameMousePos();
            if (hasStart && !hasEnd)
                DrawArrow(start, currentGameMouse);
            else if (hasStart && hasEnd)
                DrawArrow(start, end);

            DrawText("Left click: set points", 10, 10, 20, DARKGRAY);

            // --- Отрисовка курсора поверх всего остального ---
            DrawCustomCursor(currentGameMouse, 12.0f, WHITE);

        EndTextureMode();

        // --- Отрисовка на экран с шейдером ---
        BeginDrawing();
            ClearBackground(BLACK);
            BeginShaderMode(crtShader);
                DrawTexturePro(target.texture,
                    (Rectangle){ 0, 0, (float)target.texture.width, (float)-target.texture.height },
                    (Rectangle){ dstX, dstY, gameWidth * scale, gameHeight * scale },
                    (Vector2){ 0, 0 }, 0.0f, WHITE);
            EndShaderMode();
        EndDrawing();
    }

    // Показать курсор перед выходом (опционально)
    ShowCursor();

    UnloadShader(crtShader);
    UnloadRenderTexture(target);
    CloseWindow();

    return 0;
}
