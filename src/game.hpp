

#include <vector>

#include "maths_types.hpp"


using Position = vec2;
using Velocity = vec2;
using Colour = vec4;


struct BoundingBox
{
  vec2 top_left;
  vec2 bottom_right;
};


struct Block
{
  Position position;
  Velocity velocity;
  float width;
  float height;
  Colour colour;

  BoundingBox bounds;
};


struct Ball
{
  int radius = 20;
  Position position;
  Velocity velocity;
  Colour colour;
  float rot = 0.0f;

  BoundingBox bounds;
};

struct Line
{
  Position p1;
  Position p2;
  Colour colour;

  BoundingBox bounds;
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
  std::vector<Block> blocks;
  std::vector<Line> lines;
  std::vector<Line> border_lines;

  Ball player;
  Ball mouse_pointer;


public:

  Game(int width, int height);
  //~Game();

  bool running = true;

  Position GetCenterScreen() const;
  Position RandomPosition() const;

  Ball NewBall() const;
  Block NewBlock() const;
  Line NewLine() const;

  void NewObjects();
  void ToggleGravity();
  void ToggleFriction();

  void Resize(int width, int height);

  template <typename OBJ>
  bool Collides_Any(const OBJ &obj) const;

  Ball UpdatePhysics(float dt, const Ball & b) const;

  void Update(float dt);

  void PlayerInput(float dt, const class Input & input);

  void Shoot();

};
