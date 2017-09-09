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


struct GameState
{
  bool running = true;
  int width = 0;
  int height = 0;

  bool debug_enabled = false;

  std::vector<Ball> balls;
  std::vector<Block> blocks;
  std::vector<Block> border_lines;

  Block player;
  Ball mouse_pointer;
};


class Game
{
private:
  class Sound &sound;

  std::map<BlockType, BlockGeometry> block_shapes;

public:

  Game(Sound &sound);

  void SetupBlockGeometry();
  const BlockGeometry & GetGeometry(BlockType bt) const;

  Ball NewBall(int width, int height) const;
  Block NewBlock(int x, int y, BlockType bt) const;
  Block MakePlayer(const vec2 &position) const;
  GameState NewGame(int width, int height) const;

  GameState Resize(const GameState & state, int width, int height) const;

  void OnHitBlock(const GameState &state, Ball &ball, Block &block) const;

  bool CalculateBallCollision(GameState &state, const Ball & old_ball, vec2 &normal_vec, std::vector<Block*> &out_hit_blocks) const;
  Ball UpdatePhysics(GameState &state, float dt, Ball & b) const;

  GameState ProcessIntents(const GameState &state, const std::vector<struct Intent> &intent_stream) const;
  GameState Simulate(const GameState &state, float dt) const;

  Ball Shoot(const vec2 & position) const;

};
