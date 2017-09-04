
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



Game::Game(int width, int height, Sound &sound)
:width(width)
,height(height)
,sound(sound)
{
  srand (static_cast <unsigned> (time(0)));

  SetupBlockGeometry();
  NewObjects();

  player = NewBlock(5, 5, BlockType::paddle);
  player.colour = {1.0f, 1.0f, 1.0f, 1.0f};


  mouse_pointer.radius = 20;
  mouse_pointer.position = GetCenterScreen();
  mouse_pointer.colour = {1.0f, 1.0f, 0.8f, 1.0f};
  mouse_pointer.velocity = {0.0f, 0.0f};

}


//Game::~Game() { }


vec2 Game::GetCenterScreen() const
{
  return { width / 2.0f, height / 2.0f};
}


vec2 Game::RandomPosition() const
{
  float border = 50.0f;
  return { RandomFloat(border, width - border) , RandomFloat(border, height - border)};
}


vec2 Game::RandomPositionBottom() const
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


Ball Game::NewBall() const
{
  Ball b;
  //b.position = GetCenterScreen();
  //b.position = RandomPosition();
  b.position = RandomPositionBottom();

  const float speed = 100.0f;
  b.velocity = { RandomFloat(-speed, speed) , RandomFloat(-speed, speed)};

  b.radius = RandomInt(5, 50);

  b.colour = RandomRGB();

  b.bounds = MakeBounds(b);

  return b;
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


Block NewWorldBorders(float border, int width, int height)
{
  Block b;
  b.type = BlockType::world_border;
  b.position = {0.0f, 0.0f};
  b.colour = {1.0f, 1.0f, 1.0f, 1.0f};

  b.geometry = {};

  vec2 tl { border, border };
  vec2 tr { width - border, border };
  vec2 bl { border, height - border };
  vec2 br { width - border, height - border };

  b.geometry.push_back({tl, tr});
  b.geometry.push_back({tr, br});
  b.geometry.push_back({br, bl});
  b.geometry.push_back({bl, tl});

  b.bounds = MakeBounds(b);

  return b;
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
  vec2 pbl {0, 50};
  vec2 pbr {100, 50};

  block_shapes.emplace(BlockType::paddle,
    BlockGeometry { {ptl, pbl}, {pbl, pbr}, {pbr, ptr}, {ptr, ptl} });
}


const BlockGeometry & Game::GetGeometry(BlockType bt) const
{
  return block_shapes.at(bt);
}


void Game::NewObjects()
{
  balls.clear();
  for(int i=0; i<1; i++)
  {
    balls.push_back(NewBall());
  }
  //std::cout << "Generated " << balls.size() << " new balls." << std::endl;


  blocks.clear();
  for(int x=0; x<5; x++)
  {
    for (int y=0; y<3; y++)
    {
      blocks.push_back(NewBlock(x, y, RandomBlockType()));
    }
  }


  border_lines.clear();
  border_lines.push_back(NewWorldBorders(5, width, height));

}


void Game::ToggleGravity()
{
  gravity_enabled = not gravity_enabled;

  // std::cout << "Gravity " << (gravity_enabled ? "on" : "off") << std::endl;
}


void Game::ToggleFriction()
{
  friction_enabled = not friction_enabled;

  // std::cout << "Friction " << (friction_enabled ? "on" : "off") << std::endl;
}


void Game::Resize(int width, int height)
{
  this->width = width;
  this->height = height;


  border_lines.clear();
  border_lines.push_back(NewWorldBorders(5, width, height));
}


void Game::OnHitBlock([[maybe_unused]] Ball &ball, Block &block)
{
  block.alive = false;

  float balance = (ball.position.x / width);

  if (block.type == BlockType::world_border)
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


bool Game::CalculateBallCollision(const Ball & old_ball, vec2 &out_normal_vec, std::vector<Block*> &out_hit_blocks)
{
  vec2 normal_acc = {};
  int num_normals = 0;
  out_hit_blocks.clear();

  for (auto vec : { BlockIterator(blocks), BlockIterator(border_lines), BlockIterator(player) })
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


Ball Game::UpdatePhysics(float dt, Ball & old_ball)
{
  Ball out = old_ball;
  float orig_speed = get_length(old_ball.velocity);

  out.position = old_ball.position + (old_ball.velocity * dt);
  out.bounds = MakeBounds(out);


  std::vector<Block*> hit_blocks {};
  vec2 normal_avg {};
  if (CalculateBallCollision(old_ball, normal_avg, hit_blocks))
  {
    vec2 refl = reflect(normalize(old_ball.velocity), normalize(normal_avg));

    out.velocity = normalize(refl) * orig_speed;

    for(Block * block : hit_blocks)
    {
      OnHitBlock(old_ball, *block);
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


void Game::Update(float dt)
{
  for(Ball &b : balls)
  {
    b = UpdatePhysics(dt, b);
  }


  for([[maybe_unused]] Block &b : blocks)
  {
    //TODO maybe add sounds or particles when destroyed
  }


  remove_inplace(blocks, [=](auto &b){ return not b.alive; } );


  //remove_inplace(balls, [=](auto &b){ return b.radius == 10 && Collides_Any(b); } );
}


void Game::UpdatePlayer(const vec2 &position)
{
  player.position = position;
  player.geometry = GetGeometry(BlockType::paddle);


  for (auto & line : player.geometry)
  {
    line.p1 += position;
    line.p2 += position;
  }

  player.bounds = MakeBounds(player);
}


void Game::PlayerInput([[maybe_unused]] float dt, Input const &input)
{
  mouse_pointer.position = input.mouse;
  mouse_pointer.bounds = MakeBounds(mouse_pointer);

  [[maybe_unused]] MovementInput move = input.player_move;

  player.position.x = mouse_pointer.position.x;
  player.position.y = mouse_pointer.position.y; //height - 50;

  UpdatePlayer(player.position);

  //player = UpdatePhysics(dt, player);
}


void Game::Shoot()
{
  Ball b = NewBall();

  b.position = player.position;
  b.velocity = {0.0f, -300.0f};

  b.position.x += 50.0f;
  b.position.y -= 11.0f;

  b.colour.r = RandomFloat(0.5f, 1.0f);
  b.colour.g = RandomFloat(0.5f, 1.0f);
  b.colour.b = RandomFloat(0.5f, 1.0f);
  b.radius = 10.0f;

  balls.push_back(b);

  sound.PlaySound("paddle_bounce", player.position.x / width);
}
