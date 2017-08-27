

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


class Game
{
private:
  friend class Renderer;

  int width = 0;
  int height = 0;

  bool gravity_enabled = false;
  bool friction_enabled = false;

  std::vector<Ball> balls;
  std::vector<Rect> rects;

  Ball player;
  Ball mouse_pointer;

public:

  Game(int width, int height);
  ~Game();

  bool running = true;

  Position GetCenterScreen() const;
  Position RandomPosition() const;

  Ball NewBall() const;
  Rect NewRect() const;

  void NewObjects();
  void ToggleGravity();
  void ToggleFriction();

  void Resize(int width, int height);

  bool Collides(const Ball &b1, const Ball &b2) const;
  bool Collides(const Rect &r, vec2 point) const;
  bool Collides(const Rect &r1, const Rect &r2) const;
  bool Collides(const Rect &r, const Ball &c) const;

  bool Collides_Any(const Ball &ball) const;
  bool Collides_Any(const Rect &rect) const;


  Ball UpdatePhysics(float dt, const Ball & b) const;

  void Update(float dt);

  void PlayerInput(float dt, const class Input & input);

  void Shoot();

};
