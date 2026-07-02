#pragma once

#include <raylib.h>
#include <string>

class UIElement{
  public:
    virtual void Draw();
  protected:
    Vector2 pos;
    Color color;
};

class UIText{
  private:
    float textSize;
    std::string text;
};

class UIButton{
  private:
    float textSize;
    std::string text;
};

class UIManager{
  public:
    void Update(float dT);
    void CreateButton(Vector2 pos1, Vector2 pos2, std::string text, float textSize, Color color);
    void CreateText(Vector2 pos, std::string text, float textSize, Color color);
  private:

};
