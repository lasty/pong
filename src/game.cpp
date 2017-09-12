
#include "game.hpp"

#include <cstdlib>
#include <ctime>

#include <iostream>
#include <algorithm>

#include "input.hpp"
#include "sound.hpp"

#include "maths.hpp"
#include "maths_collisions.hpp"
#include "maths_utils.hpp"



Game::Game(Sound &sound)
:sound(sound)
{
  srand (static_cast <unsigned> (time(0)));

  SetupBlockGeometry();
}



vec2 GetCenter(int width, int height)
{
  return { width / 2.0f, height / 2.0f};
}


vec2 RandomPosition(int width, int height, float border = 50.0f)
{
  return { RandomFloat(border, width - border) , RandomFloat(border, height - border)};
}


vec2 RandomPositionBottom(int width, int height)
{
  float border = 50.0f;
  return { RandomFloat(border, width - border) , RandomFloat(height - 2*border, height - border)};
}


BoundingBox MakeBounds(const Ball & ball)
{
  const vec2 tl {ball.position.x - ball.radius, ball.position.y - ball.radius};
  const vec2 br {ball.position.x + ball.radius, ball.position.y + ball.radius};

  return {tl, br};
}


BoundingBox MakeBounds(const Block &block)
{
  vec2 tl = block.position;
  vec2 br = block.position;

  for (auto line : block.geometry)
  {
    for (auto point : {line.p1, line.p2})
    {
      tl.x = std::min(tl.x, point.x);
      tl.y = std::min(tl.y, point.y);

      br.x = std::max(br.x, point.x);
      br.y = std::max(br.y, point.y);
    }
  }

  return {tl, br};
}


BlockType RandomBlockType()
{
  const std::vector<BlockType> vec {
    BlockType::square,
    BlockType::triangle_left, BlockType::triangle_right,
    BlockType::rectangle,
    BlockType::rect_triangle_left, BlockType::rect_triangle_right
  };
  return vec.at(RandomInt(0, vec.size()));
}


std::vector<Block> NewWorldBorders(float border, int width, int height)
{
  Block b;
  b.type = BlockType::world_border;
  b.position = {0.0f, 0.0f};
  b.colour = {1.0f, 1.0f, 1.0f, 1.0f};
  b.geometry = {};

  Block outofbounds;
  outofbounds.type = BlockType::world_out_of_bounds;
  outofbounds.position = {0.0f, 0.0f};
  outofbounds.colour = {0.0f, 0.0f, 0.0f, 1.0f};
  outofbounds.geometry = {};

  vec2 tl { border, border };
  vec2 tr { width - border, border };
  vec2 bl { border, height + 20.0f };
  vec2 br { width - border, height + 20.0f };

  b.geometry.push_back({tl, tr});
  b.geometry.push_back({tr, br});
  outofbounds.geometry.push_back({br, bl});
  b.geometry.push_back({bl, tl});

  b.bounds = MakeBounds(b);
  outofbounds.bounds = MakeBounds(outofbounds);

  return {b, outofbounds};
}


void Game::SetupBlockGeometry()
{
  vec2 tl {0,0};
  vec2 bl {0, 50};
  vec2 tr {50, 0};
  vec2 br {50, 50};

  block_shapes.emplace(BlockType::square,
    BlockGeometry { {tl, bl}, {bl, br}, {br, tr}, {tr, tl} });

  block_shapes.emplace(BlockType::triangle_left,
    BlockGeometry { {tr, tl}, {tl, br}, {br, tr} });

  block_shapes.emplace(BlockType::triangle_right,
    BlockGeometry { {tr, tl}, {tl, bl}, {bl, tr} });

  vec2 wtr {100, 0};
  vec2 wbr {100, 50};

  block_shapes.emplace(BlockType::rectangle,
    BlockGeometry { {tl, bl}, {bl, wbr}, {wbr, wtr}, {wtr, tl} });

  block_shapes.emplace(BlockType::rect_triangle_left,
    BlockGeometry { {wtr, tl}, {tl, br}, {br, wbr}, {wbr, wtr} });

  block_shapes.emplace(BlockType::rect_triangle_right,
    BlockGeometry { {wtr, tl}, {tl, bl}, {bl, br}, {br, wtr} });


  vec2 ptl {0,0};
  vec2 ptr {100,0};
  vec2 pbl {0, 40};
  vec2 pbr {100, 40};

  block_shapes.emplace(BlockType::paddle,
    BlockGeometry { {ptl, pbl}, {pbl, pbr}, {pbr, ptr}, {ptr, ptl} });
}


const BlockGeometry & Game::GetGeometry(BlockType bt) const
{
  return block_shapes.at(bt);
}


Ball Game::NewBall(int width, int height) const
{
  Ball b;
  b.position = RandomPositionBottom(width, height);

  const float speed = 100.0f;
  b.velocity = { RandomFloat(-speed, speed) , RandomFloat(-speed, speed)};

  b.radius = RandomInt(5, 50);

  b.colour = RandomRGB();

  b.bounds = MakeBounds(b);

  return b;
}


Block Game::NewBlock(int x, int y, BlockType bt) const
{
  Block b;
  b.type = bt;

  b.position = { 50.0f + (110.0f * x) , 50.0f + (60.0f * y) };
  b.colour = RandomRGB();
  b.geometry = GetGeometry(b.type);

  for (auto & line : b.geometry)
  {
    line.p1 += b.position;
    line.p2 += b.position;
  }

  b.bounds = MakeBounds(b);

  return b;
}


GameState Game::NewGame(int width, int height) const
{
  GameState state;

  state.width = width;
  state.height = height;

  state.player = MakePlayer({width / 2.0f, height - 50.0f});

  state.mouse_pointer.radius = 20;
  state.mouse_pointer.position = GetCenter(width, height);
  state.mouse_pointer.colour = {1.0f, 1.0f, 0.8f, 1.0f};
  state.mouse_pointer.velocity = {0.0f, 0.0f};

  // state.balls.clear();
  for(int i=0; i<1; i++)
  {
    state.balls.push_back(NewBall(width, height));
  }

  // state.blocks.clear();
  for(int x=0; x<5; x++)
  {
    for (int y=0; y<3; y++)
    {
      state.blocks.push_back(NewBlock(x, y, RandomBlockType()));
    }
  }

  state.border_lines = NewWorldBorders(5, width, height);

  return state;
}


Block Game::MakePlayer(const vec2 &position) const
{
  Block player = NewBlock(5, 5, BlockType::paddle);
  player.colour = {1.0f, 1.0f, 1.0f, 1.0f};

  player.position = position;
  player.geometry = GetGeometry(BlockType::paddle);

  for (auto & line : player.geometry)
  {
    line.p1 += position;
    line.p2 += position;
  }

  player.bounds = MakeBounds(player);

  return player;
}



GameState Game::Resize(const GameState &state, int width, int height) const
{
  GameState out = state;

  out.width = width;
  out.height = height;

  out.border_lines = NewWorldBorders(5, width, height);

  return out;
}


void Game::OnHitBlock(const GameState &state, Ball &ball, Block &block) const
{
  block.alive = false;

  float balance = (ball.position.x / state.width);

  if (block.type == BlockType::world_out_of_bounds)
  {
    ball.alive = false;
    sound.PlaySound("lost_ball", balance);
  }
  else if (block.type == BlockType::world_border)
  {
    sound.PlaySound("bounce", balance);
  }
  else
  {
    sound.PlaySound("bounce_hit", balance);
  }

}


struct BlockIterator
{
  Block * i1;
  Block * i2;

  BlockIterator(std::vector<Block> &vec)
  : i1(vec.data())
  , i2(vec.data() + vec.size())
  {}

  BlockIterator(Block &single)
  : i1(&single)
  , i2(i1+1)
  {}

  Block * begin() { return i1; }
  Block * end() { return i2; }
};


bool Game::CalculateBallCollision(GameState &state, const Ball & old_ball, vec2 &out_normal_vec, std::vector<Block*> &out_hit_blocks) const
{
  vec2 normal_acc = {};
  int num_normals = 0;
  out_hit_blocks.clear();

  for (auto vec : { BlockIterator(state.blocks), BlockIterator(state.border_lines), BlockIterator(state.player) })
  {
    for (Block & block : vec)
    {
      if (not BoundingBoxCollides(old_ball.bounds, block.bounds)) continue;

      for(auto & line : block.geometry)
      {
        if (Collides(old_ball, line))
        {
          vec2 line_normal = get_normal(line.p1, line.p2);

          vec2 contact_point = nearest_point_on_line_segment(line.p1, line.p2, old_ball.position);
          vec2 contact_angle = normalize(old_ball.position - contact_point);
          contact_angle = old_ball.velocity * -1.0f;
          float line_dot = dot(normalize(line_normal), normalize(contact_angle));

          if (line_dot > 0)
          {
            normal_acc += line_normal;
            num_normals++;

            out_hit_blocks.push_back(&block);
          }
        }
      }
    }
  }

  if (num_normals)
  {
    out_normal_vec = normal_acc / float(num_normals);
    return true;
  }

  return false;
}


Ball Game::UpdatePhysics(GameState &state, float dt, Ball & old_ball) const
{
  Ball out = old_ball;
  float orig_speed = get_length(old_ball.velocity);

  out.position = old_ball.position + (old_ball.velocity * dt);
  out.bounds = MakeBounds(out);


  std::vector<Block*> hit_blocks {};
  vec2 normal_avg {};
  if (CalculateBallCollision(state, old_ball, normal_avg, hit_blocks))
  {
    vec2 refl = reflect(normalize(old_ball.velocity), normalize(normal_avg));

    out.velocity = normalize(refl) * orig_speed;

    for(Block * block : hit_blocks)
    {
      OnHitBlock(state, out, *block);
    }

    out.position = old_ball.position;
    out.bounds = MakeBounds(out);
  }

  return out;
}


template <typename CONT, typename PRED>
void remove_inplace(CONT &container, const PRED &pred)
{
  container.erase(
    std::remove_if(container.begin(), container.end(), pred)
    , container.end());
}


GameState Game::ProcessIntents(const GameState &state,
  const std::vector<struct Intent> &intent_stream) const
{
  GameState out = state;

  for(auto & intent : intent_stream)
  {
    switch(intent.type)
    {
      case IntentType::quit:
        out.running = false;
      break;

      case IntentType::toggle_debug:
        out.debug_enabled = not out.debug_enabled;
      break;

      case IntentType::new_balls:
        out = NewGame(state.width, state.height);
      break;

      case IntentType::time_passed:
        //TODO
      break;

      case IntentType::player_input:
        switch (intent.player_input)
        {
          case PlayerInput::move_left:
          break;
          case PlayerInput::move_right:
          break;

          case PlayerInput::mouse_position:
            out.mouse_pointer.position = intent.position;
            out.player = MakePlayer({intent.position.x, out.player.position.y});
          break;

          case PlayerInput::shoot:
            if (intent.down)
            {
              out.balls.push_back(Shoot(state.player.position));

              sound.PlaySound("paddle_bounce", state.player.position.x / state.width);
            }
          break;

        }
      break;
    }
  }

  return out;
}

GameState Game::Simulate(const GameState &state, float dt) const
{
  GameState out = state;

  for(Ball &b : out.balls)
  {
    b = UpdatePhysics(out, dt, b);
  }


  remove_inplace(out.blocks, [=](auto &b){ return not b.alive; } );
  remove_inplace(out.balls, [=](auto &b){ return not b.alive; } );


  TRACE << "blocks: " << out.blocks.size() << " balls:" << out.balls.size() << "  ";

  return out;
}



Ball Game::Shoot(const vec2 &position) const
{
  Ball b = NewBall(0, 0);

  b.position = position;
  b.velocity = {0.0f, -300.0f};

  b.position.x += 50.0f;
  b.position.y -= 11.0f;

  b.colour.r = RandomFloat(0.5f, 1.0f);
  b.colour.g = RandomFloat(0.5f, 1.0f);
  b.colour.b = RandomFloat(0.5f, 1.0f);
  b.radius = 10.0f;

  return b;
}
