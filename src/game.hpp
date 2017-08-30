

#include <vector>
#include <map>

#include "maths_types.hpp"


struct BoundingBox
{
  vec2 top_left;
  vec2 bottom_right;
};



struct Ball
{
  int radius = 20;
  vec2 position;
  vec2 velocity;
  vec4 colour;
  float rot = 0.0f;

  BoundingBox bounds;
};


struct BorderLine
{
  BorderLine() = default;
  BorderLine(const vec2 &p1, const vec2 &p2);

  vec2 p1;
  vec2 p2;

  BoundingBox bounds;
};


using BlockGeometry = std::vector<BorderLine>;


enum class BlockType
{
  none = 0,
  paddle,

  square, triangle_left, triangle_right,
  rectangle, rect_triangle_left, rect_triangle_right
};


struct Block
{
  BlockType type = BlockType::none;

  vec2 position;
  vec4 colour;

  BlockGeometry geometry;

  BoundingBox bounds;
};


BoundingBox MakeBounds(const Ball & ball);
BoundingBox MakeBounds(const Block &block);
BoundingBox MakeBounds(const BorderLine & line);


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
  std::vector<BorderLine> border_lines;

  std::map<BlockType, BlockGeometry> block_shapes;

  Ball player;
  Ball mouse_pointer;


public:

  Game(int width, int height);
  //~Game();

  bool running = true;

  vec2 GetCenterScreen() const;
  vec2 RandomPosition() const;
  vec2 RandomPositionBottom() const;

  Ball NewBall() const;
  Block NewBlock(int x, int y) const;

  void SetupBlockGeometry();
  const BlockGeometry & GetGeometry(BlockType bt) const;

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
