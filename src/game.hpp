#pragma once

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


struct Line
{
  vec2 p1;
  vec2 p2;
};


enum class BlockType
{
  none = 0,
  paddle,
  world_border,

  square, triangle_left, triangle_right,
  rectangle, rect_triangle_left, rect_triangle_right
};


using BlockGeometry = std::vector<Line>;

struct Block
{

  BlockType type = BlockType::none;
  bool alive = true;

  vec2 position;
  vec4 colour;

  BlockGeometry geometry;

  BoundingBox bounds;
};



class Game
{
private:
  friend class Renderer;

  int width = 0;
  int height = 0;
  class Sound &sound;

  bool gravity_enabled = false;
  bool friction_enabled = false;

  std::vector<Ball> balls;
  std::vector<Block> blocks;
  std::vector<Block> border_lines;

  std::map<BlockType, BlockGeometry> block_shapes;

  Block player;
  Ball mouse_pointer;


public:

  Game(int width, int height, Sound &sound);
  //~Game();

  bool running = true;

  vec2 GetCenterScreen() const;
  vec2 RandomPosition() const;
  vec2 RandomPositionBottom() const;

  Ball NewBall() const;
  Block NewBlock(int x, int y, BlockType bt) const;

  void SetupBlockGeometry();
  const BlockGeometry & GetGeometry(BlockType bt) const;

  void NewObjects();
  void ToggleGravity();
  void ToggleFriction();

  void Resize(int width, int height);

  void OnHitBlock(Ball &ball, Block &block);

  bool CalculateBallCollision(const Ball & old_ball, vec2 &normal_vec, std::vector<Block*> &out_hit_blocks);
  Ball UpdatePhysics(float dt, Ball & b);

  void Update(float dt);

  void UpdatePlayer(const vec2 &position);
  void PlayerInput(float dt, const class Input & input);

  void Shoot();

};
