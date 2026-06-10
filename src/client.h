#pragma once
#include <raylib.h>

class Client {
  public:
    Client();
    ~Client();

    bool Start();
    void Run();
    void Stop();

    void Update(float dT);
  private:
    int gameWidth;
    int gameHeight;
    RenderTexture2D target;
    Shader crtShader;
};
