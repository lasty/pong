
#include "game.hpp"

#include <cstdlib>
#include <ctime>

#include <algorithm>
#include <iostream>

#include "input.hpp"
#include "sound.hpp"

#include "maths.hpp"
#include "maths_collisions.hpp"
#include "to_string.hpp"



Ball::Ball(const vec2 &position, const vec2 &velocity)
: position(position)
, velocity(velocity)
, colour {}
, bounds{}
{
  colour = RandomRGB();
  UpdateBounds();
}


void Ball::UpdateBounds()
{
  bounds.top_left = {position.x - radius, position.y - radius};
  bounds.bottom_right = {position.x + radius, position.y + radius};
}


void Block::UpdateBounds()
{
  const Block &block = *this;

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

  bounds.top_left = tl;
  bounds.bottom_right = br;
}


Game::Game(Sound &sound)
: sound(sound)
{
  srand(static_cast<unsigned>(time(nullptr)));

  SetupBlockGeometry();
}


BlockType RandomBlockType()
{
  const std::vector<BlockType> vec{
    BlockType::square,
    BlockType::triangle_left, BlockType::triangle_right,
    BlockType::rectangle,
    BlockType::rect_triangle_left, BlockType::rect_triangle_right};
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

  vec2 tl{border, border};
  vec2 tr{width - border, border};
  vec2 bl{border, height + 20.0f};
  vec2 br{width - border, height + 20.0f};

  b.geometry.push_back({tl, tr});
  b.geometry.push_back({tr, br});
  outofbounds.geometry.push_back({br, bl});
  b.geometry.push_back({bl, tl});

  b.UpdateBounds();
  outofbounds.UpdateBounds();

  return {b, outofbounds};
}


void Game::SetupBlockGeometry()
{
  vec2 tl{0, 0};
  vec2 bl{0, 50};
  vec2 tr{50, 0};
  vec2 br{50, 50};

  block_shapes.emplace(BlockType::square,
    BlockGeometry{{tl, bl}, {bl, br}, {br, tr}, {tr, tl}});

  block_shapes.emplace(BlockType::triangle_left,
    BlockGeometry{{tr, tl}, {tl, br}, {br, tr}});

  block_shapes.emplace(BlockType::triangle_right,
    BlockGeometry{{tr, tl}, {tl, bl}, {bl, tr}});

  vec2 wtr{100, 0};
  vec2 wbr{100, 50};

  block_shapes.emplace(BlockType::rectangle,
    BlockGeometry{{tl, bl}, {bl, wbr}, {wbr, wtr}, {wtr, tl}});

  block_shapes.emplace(BlockType::rect_triangle_left,
    BlockGeometry{{wtr, tl}, {tl, br}, {br, wbr}, {wbr, wtr}});

  block_shapes.emplace(BlockType::rect_triangle_right,
    BlockGeometry{{wtr, tl}, {tl, bl}, {bl, br}, {br, wtr}});


  vec2 ptl{-50, 0};
  vec2 ptr{50, 0};
  vec2 pbl{-50, 40};
  vec2 pbr{50, 40};

  block_shapes.emplace(BlockType::paddle,
    BlockGeometry{{ptl, pbl}, {pbl, pbr}, {pbr, ptr}, {ptr, ptl}});
}


BlockGeometry Game::MakeGeometry(BlockType bt, const vec2 &offset) const
{
  BlockGeometry geom = block_shapes.at(bt);

  for (auto &line : geom)
  {
    line.p1 += offset;
    line.p2 += offset;
  }
  return geom;
}


Block Game::NewBlock(const vec2 &position, BlockType bt) const
{
  Block b;
  b.type = bt;

  b.position = position;
  b.colour = RandomRGB();
  b.geometry = MakeGeometry(b.type, position);

  b.UpdateBounds();

  return b;
}


GameState Game::NewGame(int width, int height) const
{
  GameState state;

  state.width = width;
  state.height = height;

  state.player = MakePlayer({width / 2.0f, height - 50.0f});

  state.mouse_pointer = {width / 2.0f, height / 2.0f};

  for (int x = 0; x < 5; x++)
  {
    for (int y = 0; y < 3; y++)
    {
      vec2 position{50.0f + (110.0f * x), 50.0f + (60.0f * y)};
      state.blocks.push_back(NewBlock(position, RandomBlockType()));
    }
  }

  state.border_lines = NewWorldBorders(5, width, height);

  return state;
}


Paddle Game::MakePlayer(const vec2 &position) const
{
  Block block = NewBlock({0, 0}, BlockType::paddle);
  block.colour = {1.0f, 1.0f, 1.0f, 1.0f};

  block.position = position;
  block.geometry = MakeGeometry(BlockType::paddle, position);

  block.UpdateBounds();

  Paddle player;

  player.block = block;
  player.alive = true;
  player.sticky_ball = true;
  player.sticky_ball_offset = {0.0f, -11.0f};

  player.avg_velocity = std::vector<float>(10, position.x);

  return player;
}


Paddle Game::UpdatePlayer(const Paddle &old, const vec2 &position) const
{
  Paddle player = old;
  player.block.position.x = position.x;

  player.block.geometry = MakeGeometry(BlockType::paddle, player.block.position);

  player.block.UpdateBounds();

  return player;
}


void UpdatePaddleVelocity(Paddle &player)
{
  player.avg_velocity.push_back(player.block.position.x);

  player.avg_velocity.erase(player.avg_velocity.begin());
}


float GetPaddleVelocity(const Paddle &player)
{
  float vels = 0.0f;
  for (unsigned i = 1; i < player.avg_velocity.size(); i++)
  {
    vels += (player.avg_velocity[i] - player.avg_velocity[i - 1]);
  }

  float avg = vels / float(player.avg_velocity.size() - 1);

  return avg;
}


GameState Game::Resize(const GameState &state, int width, int height) const
{
  GameState out = state;

  out.width = width;
  out.height = height;

  out.border_lines = NewWorldBorders(5, width, height);

  return out;
}


void Game::OnHitBlock(Ball &ball, Block &block) const
{
  if (block.type != BlockType::paddle)
  {
    block.alive = false;
  }

  if (block.type == BlockType::world_out_of_bounds)
  {
    ball.alive = false;
  }
}

//TODO maybe refactor this into a loop taking a lambda, eg, for_all_blocks(f)

struct BlockIterator
{
  Block *i1;
  Block *i2;

  explicit BlockIterator(std::vector<Block> &vec)
  : i1(vec.data())
  , i2(vec.data() + vec.size())
  {
  }

  explicit BlockIterator(Block &single)
  : i1(&single)
  , i2(i1 + 1)
  {
  }

  Block *begin() { return i1; }
  Block *end() { return i2; }
};


bool Game::CalculateBallCollision(GameState &state, const Ball &old_ball, vec2 &out_normal_vec, std::vector<Block *> &out_hit_blocks) const
{
  vec2 normal_acc = {};
  int num_normals = 0;
  out_hit_blocks.clear();

  for (auto vec : {BlockIterator(state.blocks), BlockIterator(state.border_lines), BlockIterator(state.player.block)})
  {
    for (Block &block : vec)
    {
      if (not BoundingBoxCollides(old_ball.bounds, block.bounds)) continue;

      for (auto &line : block.geometry)
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


Ball Game::UpdatePhysics(GameState &state, float dt, Ball &old_ball, std::vector<Collision> &collisions) const
{
  Ball out = old_ball;
  float orig_speed = get_length(old_ball.velocity);

  out.position = old_ball.position + (old_ball.velocity * dt);
  out.UpdateBounds();


  std::vector<Block *> hit_blocks{};
  vec2 normal_avg{};
  if (CalculateBallCollision(state, old_ball, normal_avg, hit_blocks))
  {
    vec2 refl = reflect(normalize(old_ball.velocity), normalize(normal_avg));

    out.velocity = normalize(refl) * orig_speed;

    for (Block *block : hit_blocks)
    {
      if (block->type == BlockType::paddle)
      {
        out.velocity.x += GetPaddleVelocity(state.player) * 30.0f;
      }
      OnHitBlock(out, *block);
      collisions.push_back({out.position, old_ball.velocity, out.velocity, block->type});
    }

    out.position = old_ball.position;
    out.UpdateBounds();
  }

  return out;
}


template<typename CONT, typename PRED>
void remove_inplace(CONT &container, const PRED &pred)
{
  container.erase(
    std::remove_if(container.begin(), container.end(), pred), container.end());
}


void Game::ProcessGameInput(GameState &state, const Intent &intent) const
{
  switch (intent.type)
  {
    case IntentType::quit:
    case IntentType::menu:
      SetState(state, State::pause_menu);
      if (not state.sound_muted) sound.PlaySound("menu_beep", 0.5f);
      break;

    case IntentType::toggle_debug:
      state.debug_enabled = not state.debug_enabled;
      break;

    case IntentType::new_game:
      state = NewGame(state.width, state.height);
      break;

    case IntentType::reset_ball:
      state.balls.clear();
      if (not state.sound_muted) sound.PlaySound("lost_ball", 0.5f);
      break;

    case IntentType::player_input:
      switch (intent.player_input)
      {
        case PlayerInput::move_left:
          break;
        case PlayerInput::move_right:
          break;

        case PlayerInput::mouse_position:
        {
          state.mouse_pointer = intent.position;

          float b = 50 + 10;
          vec2 pos = {clamp(b, state.width - b, intent.position.x), intent.position.y};
          state.player = UpdatePlayer(state.player, pos);
        }
        break;

        case PlayerInput::shoot:
          if (intent.down)
          {
            if (state.player.sticky_ball)
            {
              auto b = Ball(state.player.block.position + state.player.sticky_ball_offset, {0.0f, -300.0f});
              b.velocity.x += GetPaddleVelocity(state.player) * 30.0f;
              state.balls.push_back(b);
              if (not state.sound_muted) sound.PlaySound("paddle_bounce", state.player.block.position.x / state.width);
              state.player.sticky_ball = false;
            }
            else
            {
              if (not state.sound_muted) sound.PlaySound("error", state.player.block.position.x / state.width);
            }

            const vec2 particle_vel{0.0f, -1.0f};
            const col4 particle_col{1.0f, 1.0f, 0.5f, 1.0f};

            for (int i = 0; i < 10; i++)
            {
              state.particles.emplace_back(
                Particle(state.player.block.position, particle_vel,
                  1.0f, particle_col, 1.0f));
            }
          }
          break;
      }
      break;

    default:
      break;
  }
}


void Game::ProcessMenuInput(GameState &state, const Intent &intent) const
{
  switch (intent.type)
  {
    case IntentType::quit:
      state.running = false;
      break;

    case IntentType::menu:
      if (state.state == State::pause_menu)
      {
        SetState(state, State::mid_game);
        if (not state.sound_muted) sound.PlaySound("menu_beep", 0.5f);
      }
      break;

    case IntentType::menu_up:
      state.selected_menu_item--;
      if (state.selected_menu_item < 0) state.selected_menu_item = state.menu_items.size() - 1;
      if (not state.sound_muted) sound.PlaySound("menu_beep", 0.5f);
      break;

    case IntentType::menu_down:
      state.selected_menu_item = (state.selected_menu_item + 1) % state.menu_items.size();
      if (not state.sound_muted) sound.PlaySound("menu_beep", 0.5f);
      break;

    case IntentType::menu_activate:
      state.activated_menu_item = state.selected_menu_item;
      break;


    default:
      break;
  }
}


void Game::ProcessIntents(GameState &state,
  const std::vector<struct Intent> &intent_stream) const
{
  for (auto &intent : intent_stream)
  {
    if (state.state == State::main_menu or state.state == State::pause_menu)
    {
      ProcessMenuInput(state, intent);
    }
    else
    {
      ProcessGameInput(state, intent);
    }
  }
}



void Game::SetState(GameState &state, State new_state) const
{
  std::cout << "SetState(" << ToString(new_state) << ")" << std::endl;

  state.state_timer = 0.0f;
  state.state = new_state;

  switch (new_state)
  {
    case State::main_menu:
      state.menu_items = {"New Game", "Toggle Sound", "Quit"};
      state.selected_menu_item = 0;
      state.activated_menu_item = -1;
      break;

    case State::pause_menu:
      state.menu_items = {"Resume", "Reset Ball", "Toggle Sound", "Main Menu", "Quit"};
      state.selected_menu_item = 0;
      state.activated_menu_item = -1;
      break;

    case State::new_level:
      state = NewGame(state.width, state.height);
      break;

    case State::ball_launch:
      state.player.sticky_ball = true;
      break;

    case State::mid_game:
      break;

    case State::ball_reset:
      state.balls.clear();
      if (not state.sound_muted) sound.PlaySound("lost_ball", 0.5f);
      break;

    case State::ball_died:
      break;

    case State::game_won:
      if (state.state_timer > 1.0f)
      {
        state.state = State::new_level;
        state.state_timer = 0.0f;
      }
      break;
  }
}


void Game::ProcessStateGraph(GameState &state, float dt) const
{
  state.state_timer += dt;

  switch (state.state)
  {
    case State::main_menu:
      if (state.activated_menu_item >= 0)
      {
        const auto &item = state.menu_items.at(state.activated_menu_item);

        if (item == "New Game") SetState(state, State::new_level);
        if (item == "Toggle Sound")
        {
          state.activated_menu_item = -1;
          state.sound_muted = not state.sound_muted;
        }
        if (item == "Quit") state.running = false;

        if (not state.sound_muted) sound.PlaySound("menu_activated", 0.5f);
      }
      break;

    case State::pause_menu:
      if (state.activated_menu_item >= 0)
      {
        const auto &item = state.menu_items.at(state.activated_menu_item);

        if (item == "Resume") SetState(state, State::mid_game);
        if (item == "Reset Ball") SetState(state, State::ball_reset);
        if (item == "Main Menu") SetState(state, State::main_menu);
        if (item == "Toggle Sound")
        {
          state.activated_menu_item = -1;
          state.sound_muted = not state.sound_muted;
        }
        if (item == "Quit") state.running = false;

        if (not state.sound_muted) sound.PlaySound("menu_activated", 0.5f);
      }
      break;

    case State::new_level:
      SetState(state, State::ball_launch);
      break;

    case State::ball_launch:
      if (not state.player.sticky_ball) SetState(state, State::mid_game);
      break;

    case State::mid_game:
      if (state.balls.empty()) SetState(state, State::ball_died);

      if (state.blocks.empty()) SetState(state, State::game_won);

      break;

    case State::ball_reset:
      SetState(state, State::ball_died);
      break;

    case State::ball_died:
      if (state.state_timer > 1.0f)
      {
        SetState(state, State::ball_launch);
      }
      break;

    case State::game_won:
      if (state.state_timer > 1.0f)
      {
        SetState(state, State::main_menu);
      }
      break;
  }

  TRACE << "State: " << ToString(state.state) << "  ";
}


void Game::PlayCollisionSound(const Collision &collision, const GameState &state) const
{
  if (state.sound_muted) return;

  float balance = (collision.position.x / state.width);

  if (collision.block_type == BlockType::world_out_of_bounds)
  {
    sound.PlaySound("lost_ball", balance);
  }
  else if (collision.block_type == BlockType::world_border)
  {
    sound.PlaySound("bounce", balance);
  }
  else if (collision.block_type == BlockType::paddle)
  {
    sound.PlaySound("paddle_bounce", balance);
  }
  else
  {
    sound.PlaySound("bounce_hit", balance);
  }
}


std::vector<Particle> Game::CreateCollisionParticles(const Collision &collision) const
{
  std::vector<Particle> particles;

  // const vec2 particle_vel = collision.in_vel / 50.0f;
  const col4 particle_col{1.0f, 1.0f, 0.5f, 1.0f};

  for (int i = 0; i < 20; i++)
  {
    vec2 particle_vel {};
    const int r = RandomInt(0, 3);
    if (r == 0)
      particle_vel = collision.in_vel / 150.0f;
    else if (r == 1)
      particle_vel = collision.out_vel / 150.0f;
    else
      particle_vel = {0.0, 0.0};

    particles.emplace_back(
      Particle(collision.position, particle_vel, 0.5f, particle_col, 1.0f));
  }

  return particles;
}


GameState Game::Simulate(const GameState &state, float dt) const
{
  if (state.state == State::pause_menu or state.state == State::main_menu)
    return state;

  GameState out = state;

  for (Ball &b : out.balls)
  {
    b = UpdatePhysics(out, dt, b, out.collisions);
  }


  for (Collision &collision : out.collisions)
  {
    PlayCollisionSound(collision, out);

    std::vector<Particle> new_particles = CreateCollisionParticles(collision);
    out.particles.insert(out.particles.end(), new_particles.begin(), new_particles.end());
  }
  out.collisions.clear();


  for (Particle &p : out.particles)
  {
    p = UpdateParticle(p, dt);
  }


  auto destroyed_blocks = std::stable_partition(out.blocks.begin(), out.blocks.end(),
    [](Block &b) { return b.alive; });

  for (auto it = destroyed_blocks; it != out.blocks.end(); it++)
  {
    Block &block = *it;
    // block.alive = true;
    for (int i = 0; i < 100; i++)
    {
      int whichline = RandomInt(-1, block.geometry.size());
      vec2 pos{0.0f, 0.0f};
      if (whichline >= 0)
      {
        const Line &line = block.geometry[whichline];
        pos = (line.p1 + line.p2) / 2.0f;
      }
      else
      {
        for (const auto &line : block.geometry)
        {
          pos += line.p1 + line.p2;
        }
        pos /= (block.geometry.size() * 2.0f);
      }

      vec2 vel = {0.0, 0.0f};
      auto particle = Particle(pos, vel, 4.0f, block.colour, 1.0f);
      out.particles.push_back(particle);
    }
  }
  out.blocks.erase(destroyed_blocks, out.blocks.end());


  remove_inplace(out.balls, [=](auto &b) { return not b.alive; });


  remove_inplace(out.particles, [=](auto &p) { return p.ttl < 0.0f; });


  UpdatePaddleVelocity(out.player);

  TRACE << "blocks: " << out.blocks.size() << " balls:" << out.balls.size()
        << "particles: " << out.particles.size() << "  ";

  return out;
}
