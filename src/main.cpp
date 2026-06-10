#include "client.h"

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

int main() {
  Client c;
  c.Start();
  c.Run();
  c.Stop();
  return 0;
}
