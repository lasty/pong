#pragma once

#include <vector>
#include <map>

#include "maths_types.hpp"

#include "particles.hpp"


struct BoundingBox
{
  vec2 top_left;
  vec2 bottom_right;
};


struct Ball
{
  int radius = 20;
  bool alive = true;
  vec2 position;
  vec2 velocity;
  vec4 colour;

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
  world_out_of_bounds,

  square,
  triangle_left,
  triangle_right,
  rectangle,
  rect_triangle_left,
  rect_triangle_right
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


struct Paddle
{
  Block block;

  bool alive = true;
  bool sticky_ball = true;

  vec2 sticky_ball_offset;

  std::vector<float> avg_velocity;
};


struct Collision
{
  vec2 position;

  vec2 in_vel;
  vec2 out_vel;

  BlockType block_type;
};


enum class State
{
  new_level,
  ball_launch,
  mid_game,
  ball_reset,
  ball_died,
  game_won,
  main_menu,
  pause_menu
};


struct GameState
{
  bool running = true;
  bool sound_muted = false;
  int width = 0;
  int height = 0;

  bool debug_enabled = false;

  std::vector<Ball> balls;
  std::vector<Block> blocks;
  std::vector<Block> border_lines;

  float state_timer;
  State state = State::new_level;

  Paddle player;
  vec2 mouse_pointer;

  std::vector<Collision> collisions;
  std::vector<Particle> particles;

  int selected_menu_item = -1;
  std::vector<std::string> menu_items;
  int activated_menu_item = -1;
};


class Game
{
private:
  class Sound &sound;

  std::map<BlockType, BlockGeometry> block_shapes;

public:
  Game(Sound &sound);

  void SetupBlockGeometry();
  BlockGeometry MakeGeometry(BlockType bt, const vec2 &offset) const;

  Ball NewBall(const vec2 &position, const vec2 &velocity) const;
  Block NewBlock(const vec2 &position, BlockType bt) const;
  Paddle MakePlayer(const vec2 &position) const;
  Paddle UpdatePlayer(const Paddle &old, const vec2 &position) const;

  GameState NewGame(int width, int height) const;

  GameState Resize(const GameState &state, int width, int height) const;

  void OnHitBlock(Ball &ball, Block &block) const;

  bool CalculateBallCollision(GameState &state, const Ball &old_ball, vec2 &normal_vec, std::vector<Block *> &out_hit_blocks) const;
  Ball UpdatePhysics(GameState &state, float dt, Ball &b, std::vector<Collision> &collisions) const;

  GameState ProcessGameInput(const GameState &state, const struct Intent &intent) const;
  GameState ProcessMenuInput(const GameState &state, const struct Intent &intent) const;

  GameState ProcessIntents(const GameState &state, const std::vector<struct Intent> &intent_stream) const;

  GameState SetState(const GameState &state, State new_state) const;
  GameState ProcessStateGraph(const GameState &state, float dt) const;

  void PlayCollisionSound(const Collision &collision, const GameState &state) const;
  std::vector<Particle> CreateCollisionParticles(const Collision &collision) const;

  GameState Simulate(const GameState &state, float dt) const;
};
