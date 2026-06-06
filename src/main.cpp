#include <raylib.h>
#include <vector>
#include <list>
#include <cmath>
#include <algorithm>

// ---------- Константы ----------
const int SCREEN_W = 1000;
const int SCREEN_H = 700;
const float UNIT_SPEED = 100.0f;
const float SPAWN_INTERVAL = 1.0f;
const float HOVER_THICK = 5.0f;
const float NORMAL_THICK = 2.0f;
const float ARROW_HEAD_LEN = 15.0f;
const float FEATHER_LEN = 12.0f;   // длина перьев

enum Team { TEAM_NEUTRAL = 0, TEAM_BLUE = 1, TEAM_RED = 2 };

// ---------- Векторные функции ----------
static inline float Vec2Dist(Vector2 a, Vector2 b) {
    float dx = a.x - b.x, dy = a.y - b.y;
    return sqrtf(dx*dx + dy*dy);
}
static inline Vector2 Vec2Sub(Vector2 a, Vector2 b) { return { a.x - b.x, a.y - b.y }; }
static inline Vector2 Vec2Add(Vector2 a, Vector2 b) { return { a.x + b.x, a.y + b.y }; }
static inline Vector2 Vec2Scale(Vector2 v, float s) { return { v.x * s, v.y * s }; }
static inline Vector2 Vec2Normalize(Vector2 v) {
    float len = sqrtf(v.x*v.x + v.y*v.y);
    if (len < 0.0001f) return {0,0};
    return { v.x/len, v.y/len };
}
static inline float Vec2Dot(Vector2 a, Vector2 b) { return a.x*b.x + a.y*b.y; }
static float PointToSegmentDistance(Vector2 p, Vector2 a, Vector2 b) {
    Vector2 ab = Vec2Sub(b, a);
    Vector2 ap = Vec2Sub(p, a);
    float t = Vec2Dot(ap, ab) / Vec2Dot(ab, ab);
    t = fmaxf(0.0f, fminf(1.0f, t));
    Vector2 closest = Vec2Add(a, Vec2Scale(ab, t));
    return Vec2Dist(p, closest);
}

// Рисование стрелки с перпендикулярными перьями посередине (вариант А)
static void DrawArrowLine(Vector2 from, Vector2 to, Color color, float thickness) {
    if (Vec2Dist(from, to) < 5.0f) return;
    // Основная линия
    DrawLineEx(from, to, thickness, color);
    // Наконечник
    Vector2 dir = Vec2Normalize(Vec2Sub(to, from));
    Vector2 perp = { -dir.y, dir.x };
    Vector2 tip = to;
    Vector2 base = Vec2Sub(tip, Vec2Scale(dir, ARROW_HEAD_LEN));
    Vector2 left = Vec2Add(base, Vec2Scale(perp, ARROW_HEAD_LEN*0.5f));
    Vector2 right = Vec2Sub(base, Vec2Scale(perp, ARROW_HEAD_LEN*0.5f));
    DrawTriangle(tip, left, right, color);
    // Перья посередине (перпендикулярно)
    Vector2 mid = Vec2Scale(Vec2Add(from, to), 0.5f);
    Vector2 leftFeather = Vec2Add(mid, Vec2Scale(perp, FEATHER_LEN));
    Vector2 rightFeather = Vec2Sub(mid, Vec2Scale(perp, FEATHER_LEN));
    DrawLineEx(mid, leftFeather, thickness, color);
    DrawLineEx(mid, rightFeather, thickness, color);
}

// ---------- Структуры ----------
struct Building;
struct Route;

struct Unit {
    Route* route;
    float progress;
    Unit(Route* r, float p) : route(r), progress(p) {}
};

struct Route {
    Building* start;
    Building* end;
    Team team;
    std::list<Unit> units;
    float spawnTimer;
    bool hiddenUnits;   // если true, юниты не рисуются (из-за противоположного маршрута)

    Route(Building* s, Building* e, Team t)
        : start(s), end(e), team(t), spawnTimer(SPAWN_INTERVAL), hiddenUnits(false) {}

    void Update(float dt);
    void Draw(bool hovered) const;
    bool IsHovered(Vector2 mouse) const;
};

struct Building {
    Vector2 pos;
    Team team;
    int hp;
    std::vector<Route*> outgoing;

    Building(Vector2 p, Team t, int h) : pos(p), team(t), hp(h) {}

    void Draw(bool hovered);
    bool IsHovered(Vector2 mouse) const;
    bool AddOutgoing(Route* r);
    void RemoveOutgoing(Route* r);
    void OnUnitArrive(Team unitTeam);
};

// ---------- Реализации ----------
void Route::Update(float dt) {
    for (auto it = units.begin(); it != units.end(); ) {
        it->progress += UNIT_SPEED * dt / Vec2Dist(start->pos, end->pos);
        if (it->progress >= 1.0f) {
            it = units.erase(it);
            end->OnUnitArrive(team);
        } else {
            ++it;
        }
    }
    if (team != TEAM_NEUTRAL) {
        spawnTimer -= dt;
        if (spawnTimer <= 0.0f) {
            spawnTimer += SPAWN_INTERVAL;
            units.emplace_back(this, 0.0f);
        }
    }
}

void Route::Draw(bool hovered) const {
    Color col;
    if (team == TEAM_BLUE) col = SKYBLUE;
    else if (team == TEAM_RED) col = RED;
    else col = GRAY;
    float thick = hovered ? HOVER_THICK : NORMAL_THICK;
    DrawArrowLine(start->pos, end->pos, col, thick);
    // Рисуем юнитов, только если не скрыты
    if (!hiddenUnits) {
        for (auto& u : units) {
            Vector2 pos = {
                start->pos.x + (end->pos.x - start->pos.x) * u.progress,
                start->pos.y + (end->pos.y - start->pos.y) * u.progress
            };
            DrawCircleV(pos, 5, col);
        }
    }
}

bool Route::IsHovered(Vector2 mouse) const {
    return PointToSegmentDistance(mouse, start->pos, end->pos) < 10.0f;
}

void Building::Draw(bool hovered) {
    float radius = 15.0f + (hp / 60.0f) * 20.0f;
    Color col;
    if (team == TEAM_NEUTRAL) col = GRAY;
    else if (team == TEAM_BLUE) col = BLUE;
    else col = RED;
    DrawCircleV(pos, radius, Fade(col, 0.4f));
    DrawCircleLinesV(pos, radius, hovered ? YELLOW : BLACK);
    DrawText(TextFormat("%d", hp), pos.x-12, pos.y-8, 20, BLACK);
}

bool Building::IsHovered(Vector2 mouse) const {
    float rad = 15.0f + (hp/60.0f)*20.0f;
    return Vec2Dist(mouse, pos) < rad;
}

bool Building::AddOutgoing(Route* r) {
    if (outgoing.size() >= 2) return false;
    outgoing.push_back(r);
    return true;
}

void Building::RemoveOutgoing(Route* r) {
    auto it = std::find(outgoing.begin(), outgoing.end(), r);
    if (it != outgoing.end()) outgoing.erase(it);
}

void Building::OnUnitArrive(Team unitTeam) {
    if (unitTeam == team) {
        if (hp < 60) {
            hp++;
            if (hp == 60) {
                for (Route* r : outgoing) {
                    if (r->team == team) {
                        r->units.emplace_back(r, 0.0f);
                    }
                }
            }
        } else {
            for (Route* r : outgoing) {
                if (r->team == team) {
                    r->units.emplace_back(r, 0.0f);
                }
            }
        }
    } else {
        hp--;
        if (hp <= 0) {
            team = unitTeam;
            hp = 1;
            for (Route* r : outgoing) {
                r->start = nullptr;
            }
            outgoing.clear();
        }
    }
}

// ---------- Глобальные данные ----------
std::vector<Building> buildings;
std::vector<Route*> routes;
Team currentController = TEAM_BLUE;
Route* hoveredRoute = nullptr;
Building* hoveredBuilding = nullptr;

// Поиск под курсором
Building* GetBuildingAt(Vector2 mouse) {
    for (auto& b : buildings) if (b.IsHovered(mouse)) return &b;
    return nullptr;
}
Route* GetRouteAt(Vector2 mouse) {
    for (auto* r : routes) if (r->IsHovered(mouse)) return r;
    return nullptr;
}

// Обновление видимости юнитов для противоположных маршрутов
void UpdateOppositeVisibility() {
    // Сначала сбросим все hiddenUnits в false
    for (auto* r : routes) r->hiddenUnits = false;
    // Для каждой пары противоположных маршрутов установим hiddenUnits = true для более нового (по порядку в векторе)
    for (size_t i = 0; i < routes.size(); ++i) {
        for (size_t j = i+1; j < routes.size(); ++j) {
            Route* a = routes[i];
            Route* b = routes[j];
            if (a->start == b->end && a->end == b->start) {
                // Противоположные. Тот, у которого индекс больше (создан позже), будет скрыт
                // Но также нужно учитывать, что если оба уже скрыты? Не нужно.
                // Скрываем юниты у более нового
                if (i < j) b->hiddenUnits = true;
                else a->hiddenUnits = true;
            }
        }
    }
}

// Создание стрелки
bool TryCreateRoute(Building* from, Building* to, Team team) {
    if (from == to) return false;
    if (from->team != team) return false;
    if (from->outgoing.size() >= 2) return false;
    for (Route* r : routes) {
        if (r->start == from && r->end == to && r->team == team) return false;
    }
    Route* newRoute = new Route(from, to, team);
    if (!from->AddOutgoing(newRoute)) {
        delete newRoute;
        return false;
    }
    routes.push_back(newRoute);
    UpdateOppositeVisibility();
    return true;
}

void DeleteRoute(Route* r) {
    if (!r) return;
    if (r->start) r->start->RemoveOutgoing(r);
    auto it = std::find(routes.begin(), routes.end(), r);
    if (it != routes.end()) routes.erase(it);
    delete r;
    UpdateOppositeVisibility();
}

bool TryReverseRoute(Route* r) {
    if (!r) return false;
    Building* a = r->start;
    Building* b = r->end;
    Team team = r->team;
    if (b->team != team) return false;
    if (b->outgoing.size() >= 2) return false;
    DeleteRoute(r);
    return TryCreateRoute(b, a, team);
}

void ResolveCollisions() {
    for (size_t i = 0; i < routes.size(); ++i) {
        Route* r1 = routes[i];
        if (!r1->start || !r1->end) continue;
        for (size_t j = i+1; j < routes.size(); ++j) {
            Route* r2 = routes[j];
            if (!r2->start || !r2->end) continue;
            if (r1->start == r2->end && r1->end == r2->start && r1->team != r2->team) {
                size_t n1 = r1->units.size();
                size_t n2 = r2->units.size();
                size_t kill = std::min(n1, n2);
                for (size_t k = 0; k < kill; ++k) {
                    if (!r1->units.empty()) r1->units.pop_front();
                    if (!r2->units.empty()) r2->units.pop_front();
                }
            }
        }
    }
}

void Update(float dt) {
    for (auto* r : routes) r->Update(dt);
    // Удаление помеченных
    for (auto it = routes.begin(); it != routes.end(); ) {
        if ((*it)->start == nullptr) {
            delete *it;
            it = routes.erase(it);
        } else {
            ++it;
        }
    }
    ResolveCollisions();
    UpdateOppositeVisibility(); // обновить скрытие после удалений/добавлений
}

void Draw() {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    for (auto& b : buildings) b.Draw(&b == hoveredBuilding);
    for (auto* r : routes) r->Draw(r == hoveredRoute);
    const char* teamText = (currentController == TEAM_BLUE) ? "BLUE (LMB)" : "RED (RMB)";
    DrawText(TextFormat("Current team: %s", teamText), 10, 10, 20, DARKGRAY);
    DrawText("LMB: select BLUE building, then target to create route", 10, 40, 16, DARKGRAY);
    DrawText("RMB: select RED building, then target to create route", 10, 60, 16, DARKGRAY);
    DrawText("Click on your route to delete it", 10, 80, 16, DARKGRAY);
    DrawText("Click opposite direction to flip route", 10, 100, 16, DARKGRAY);
    DrawText("Opposite routes: only older route shows units", 10, 120, 16, DARKGRAY);
    EndDrawing();
}

int main() {
    InitWindow(SCREEN_W, SCREEN_H, "RTS with Feather Arrows & Opposite Visibility");
    SetTargetFPS(60);

    Vector2 center = { (float)SCREEN_W/2, (float)SCREEN_H/2 };
    float offsetX = 150, offsetY = 100;
    buildings.emplace_back(Vector2{ center.x - offsetX, center.y - offsetY }, TEAM_BLUE, 30);
    buildings.emplace_back(Vector2{ center.x + offsetX, center.y - offsetY }, TEAM_RED, 30);
    buildings.emplace_back(Vector2{ center.x - offsetX, center.y + offsetY }, TEAM_NEUTRAL, 30);
    buildings.emplace_back(Vector2{ center.x + offsetX, center.y + offsetY }, TEAM_NEUTRAL, 30);

    Building* selectedStart = nullptr;
    bool waitingForEnd = false;
    Team waitingTeam = TEAM_NEUTRAL;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        Vector2 mouse = GetMousePosition();

        hoveredRoute = GetRouteAt(mouse);
        hoveredBuilding = GetBuildingAt(mouse);

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            currentController = TEAM_BLUE;
            if (waitingForEnd && waitingTeam == TEAM_BLUE) {
                Building* target = GetBuildingAt(mouse);
                if (target && selectedStart) TryCreateRoute(selectedStart, target, TEAM_BLUE);
                waitingForEnd = false;
                selectedStart = nullptr;
            } else {
                Route* clickedRoute = GetRouteAt(mouse);
                if (clickedRoute && clickedRoute->team == TEAM_BLUE) {
                    DeleteRoute(clickedRoute);
                } else {
                    Building* b = GetBuildingAt(mouse);
                    if (b && b->team == TEAM_BLUE) {
                        selectedStart = b;
                        waitingForEnd = true;
                        waitingTeam = TEAM_BLUE;
                    }
                }
            }
        } else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            currentController = TEAM_RED;
            if (waitingForEnd && waitingTeam == TEAM_RED) {
                Building* target = GetBuildingAt(mouse);
                if (target && selectedStart) {
                    Route* opposite = nullptr;
                    for (auto* r : routes) {
                        if (r->start == target && r->end == selectedStart && r->team == TEAM_RED) {
                            opposite = r;
                            break;
                        }
                    }
                    if (opposite) {
                        DeleteRoute(opposite);
                        TryCreateRoute(selectedStart, target, TEAM_RED);
                    } else {
                        TryCreateRoute(selectedStart, target, TEAM_RED);
                    }
                }
                waitingForEnd = false;
                selectedStart = nullptr;
            } else {
                Route* clickedRoute = GetRouteAt(mouse);
                if (clickedRoute && clickedRoute->team == TEAM_RED) {
                    DeleteRoute(clickedRoute);
                } else {
                    Building* b = GetBuildingAt(mouse);
                    if (b && b->team == TEAM_RED) {
                        selectedStart = b;
                        waitingForEnd = true;
                        waitingTeam = TEAM_RED;
                    }
                }
            }
        }

        Update(dt);
        Draw();
    }

    for (auto* r : routes) delete r;
    CloseWindow();
    return 0;
}
