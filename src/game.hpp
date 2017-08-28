

#include <vector>

#include "maths_types.hpp"


using Position = vec2;
using Velocity = vec2;
using Colour = vec4;


struct Rect
{
  Position position;
  Velocity velocity;
  float width;
  float height;
  Colour colour;
};


struct Ball
{
  int radius = 20;
  Position position;
  Velocity velocity;
  Colour colour;
  float rot = 0.0f;
};

struct Line
{
  Position p1;
  Position p2;
  Colour colour;
};


class Game
{
private:
  friend class Renderer;

  int width = 0;
  int height = 0;

  bool gravity_enabled = false;
  bool friction_enabled = false;

public: //XXX
  std::vector<Ball> balls;
  std::vector<Rect> rects;
  std::vector<Line> lines;

  Ball player;
  Ball mouse_pointer;


public:

  Game(int width, int height);
  //~Game();

  bool running = true;

  Position GetCenterScreen() const;
  Position RandomPosition() const;

  Ball NewBall() const;
  Rect NewRect() const;
  Line NewLine() const;

  void NewObjects();
  void ToggleGravity();
  void ToggleFriction();

  void Resize(int width, int height);

  bool Collides_Any(const Ball &ball) const;
  bool Collides_Any(const Rect &rect) const;

  Ball UpdatePhysics(float dt, const Ball & b) const;

  void Update(float dt);

  void PlayerInput(float dt, const class Input & input);

  void Shoot();

};
