#version 330

in vec2 fragTexCoord;
out vec4 finalColor;

uniform sampler2D texture0;
uniform vec2 resolution;
uniform float distortion;
uniform float chromaStrength;
uniform float hexSize;        // размер гексагона (от центра до угла) в пикселях
uniform float hexLineWidth;   // толщина линии в пикселях
uniform float hexBrightness;  // яркость линий (0=чёрные, 1=белые)
uniform sampler2D bloomTex;

// ---------- Геометрическое искажение ----------
vec2 distort(vec2 uv) {
    vec2 p = uv * 2.0 - 1.0;
    float aspect = resolution.x / resolution.y;
    p.x *= aspect;
    float r2 = dot(p, p);
    float factor = 1.0 + distortion * r2;
    p *= factor;
    p.x /= aspect;
    return clamp((p + 1.0) / 2.0, 0.0, 1.0);
}

// ---------- Хроматическая аберрация ----------
vec3 chromaticAberration(vec2 uv) {
    vec2 offset = (uv - 0.5) * chromaStrength;
    vec2 uv_r = distort(uv + offset);
    vec2 uv_g = distort(uv);
    vec2 uv_b = distort(uv - offset);
    float r = texture(texture0, uv_r).r;
    float g = texture(texture0, uv_g).g;
    float b = texture(texture0, uv_b).b;
    return vec3(r, g, b);
}

// ---------- Правильная гексагональная сетка (pointy top) ----------
float hexGrid(vec2 uv) {
    // Пиксельные координаты
    vec2 px = uv * resolution;
    float radius = hexSize;
    float hexW = sqrt(3.0) * radius;   // ширина шестиугольника
    float hexH = 2.0 * radius;         // высота

    // Шаг сетки в пикселях (axial координаты)
    vec2 gridStep = vec2(hexW, hexH * 0.75);
    vec2 p = px / gridStep;

    // Находим ближайший центр в axial координатах
    vec2 ri = round(p);
    vec2 diff = p - ri;

    // Корректировка для правильной гексагональной решётки
    if (abs(diff.x) > abs(diff.y)) {
        ri.x += sign(diff.x);
    } else {
        ri.y += sign(diff.y);
    }

    // Координаты центра в пикселях
    vec2 center = ri * gridStep;
    // Сдвиг чётных строк
    if (mod(ri.y, 2.0) != 0.0) {
        center.x += hexW * 0.5;
    }

    // Локальные координаты относительно центра (в пикселях)
    vec2 delta = px - center;

    // Преобразуем в координаты для pointy top гексагона
    // Нормализуем к радиусу
    vec2 q;
    q.x = delta.x / radius;                     // горизонталь
    q.y = delta.y / radius;                     // вертикаль

    // Расстояния до трёх пар параллельных граней
    float d1 = abs(q.x);
    float d2 = abs(q.x * 0.5 + q.y * 0.8660254);
    float d3 = abs(q.x * 0.5 - q.y * 0.8660254);
    float maxDist = max(d1, max(d2, d3));

    // Расстояние до ближайшей грани в пикселях
    float edgeDist = (1.0 - maxDist) * radius;

    // Рисуем линию, если расстояние до грани меньше заданной толщины
    if (edgeDist < hexLineWidth && edgeDist > 0.0) {
        return 1.0;
    } else {
        return 0.0;
    }
}

void main() {
    vec3 color = chromaticAberration(uv);
    float grid = hexGrid(uv);
    vec3 gridColor = vec3(hexBrightness);
    color = mix(color, gridColor, grid);
    
    vec3 bloom = texture(bloomTex, uv).rgb;
    color += bloom;
    
    finalColor = vec4(color, 1.0);
}
