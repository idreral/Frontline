#include game.h

class Building {
public:
  void spawnBuilding
  Vector2 getPos(){
    return position;
  }
  void attack(int unit_team, int damage){
    if(unit_team != team){
      health = health - damage;
      if(health < 1){
        team = unit_team;
        health = 0;
      }
    } else {
      if(health != maxHealth){
        health += 1;
      } else {
        spawnUnit();
      }
    }
  }

  virtual void update(float dT);
  virtual void render();
protected:
  virtual void spawnUnit();
  Vector2 position;
  int health;
  int maxHealth;
  int team;
};

//class Tower : public Building{};

class SoldierFactory : public Building{};

//class TankFactory : public Building();

//class RocketLauncherFactory : public Building();

class BuildingsManager {};

enum GameState{
  WAITING_FOR_PLAYERS,
  COUNTDOWN,
  PLAYING_HOST,
  PLAYING_CLIENT,
  PAUSED,
  DISCONNECTED
};

class GameManager {
public:
  GameManager()
  void Update(float dT){};
private:
  std::string playerName;
  GameState gameState;
};
