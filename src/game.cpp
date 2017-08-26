
#include "game.hpp"

#include <cstdlib>
#include <ctime>

#include <iostream>

#include "input.hpp"

#include "maths.hpp"


float RandomFloat()
{
  return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}


float RandomFloat(const float r1, const float r2)
{
  const float range = r2 - r1;
  return (r1 + (RandomFloat() * range));
}


int RandomInt(int r1, int r2)
{
  const int range = r2 - r1;
  return (r1 + (rand() % range));
}


Colour RandomRGB()
{
  return { RandomFloat(), RandomFloat(), RandomFloat(), 1.0f };
}


Colour RandomRGBA()
{
  return { RandomFloat(), RandomFloat(), RandomFloat(), RandomFloat() };
}


Game::Game(int width, int height)
:width(width)
,height(height)
{
  srand (static_cast <unsigned> (time(0)));

  NewBalls();

  player = NewBall();
  player.radius = 20;
  player.colour = {1.0f, 1.0f, 1.0f, 1.0f};
  player.velocity = {0.0f, 0.0f};


  mouse_pointer.radius = 10;
  mouse_pointer.position = GetCenterScreen();
  mouse_pointer.colour = {1.0f, 1.0f, 0.8f, 1.0f};
  mouse_pointer.velocity = {0.0f, 0.0f};

}


Game::~Game()
{

}


Position Game::GetCenterScreen() const
{
  return { width / 2.0f, height / 2.0f};
}


Position Game::RandomPosition() const
{
  return { RandomFloat(0.0f, width) , RandomFloat(0.0f, height)};
}


Ball Game::NewBall() const
{
  Ball b;
  b.position = GetCenterScreen();
  //b.position = RandomPosition();

  const float speed = 100.0f;
  b.velocity = { RandomFloat(-speed, speed) , RandomFloat(-speed, speed)};

  b.radius = RandomInt(1, 100);

  b.colour = RandomRGB();

  return b;
}


void Game::NewBalls()
{
  balls.clear();

  for(int i=0; i<10; i++)
  {
    Ball ball = NewBall();
    balls.push_back(ball);
  }

  std::cout << "Generated " << balls.size() << " new balls." << std::endl;
}


void Game::ToggleGravity()
{
  gravity_enabled = not gravity_enabled;

  std::cout << "Gravity " << (gravity_enabled ? "on" : "off") << std::endl;
}


void Game::ToggleFriction()
{
  friction_enabled = not friction_enabled;

  std::cout << "Friction " << (friction_enabled ? "on" : "off") << std::endl;
}


void Game::Resize(int width, int height)
{
  this->width = width;
  this->height = height;
}


bool Game::Collides(const Ball &b1, const Ball &b2) const
{
  float radii = b1.radius + b2.radius;

  float dist = get_length(b2.position - b1.position);

  return dist <= radii;
}


bool Game::Collides_Any(const Ball &ball) const
{
  for (const Ball & b : balls)
  {
    if (&b == &ball) continue;  //Don't test self

    if (Collides(ball, b))
      return true;
  }
  return false;
}


Ball Game::UpdatePhysics(float dt, const Ball & b) const
{
  Ball out = b;
  out.position.x = b.position.x + b.velocity.x * dt;
  out.position.y = b.position.y + b.velocity.y * dt;

  const float gravity = 300.0f;
  const float bounce = -0.8f;
  const float friction_amount = 1.0f;

  //Simple boundary collision
  if (out.position.x < b.radius)
  {
    out.position.x = b.radius;
    out.velocity.x *= bounce;
  }

  if (out.position.x > (width - b.radius))
  {
    out.position.x = (width - b.radius);
    out.velocity.x *= bounce;
  }

  if (out.position.y < b.radius)
  {
    out.position.y = b.radius;
    out.velocity.y *= bounce;
  }

  if (out.position.y > (height - b.radius))
  {
    out.position.y = (height - b.radius);
    out.velocity.y *= bounce;
  }

  if (gravity_enabled)
  {
    out.velocity.y += gravity * dt;
  }

  if (friction_enabled)
  {
    float angle = get_angle(out.velocity);
    vec2 friction = angle_to_vec2(angle, friction_amount);

    if (get_length(out.velocity) > get_length(friction))
    {
      out.velocity -= friction;
    }
    else
    {
      out.velocity = {0.0f, 0.0f};
    }
  }

  return out;
}


void Game::Update(float dt)
{
  for(Ball &b : balls)
  {
    b = UpdatePhysics(dt, b);
    b.rot += dt * TWO_PI / 8.0f;

  }

  //player.rot += dt * PI;
}

// #include "maths_utils.hpp"



void Game::PlayerInput(float dt, Input const &input)
{
  mouse_pointer.position = input.mouse ;

  MovementInput move = input.player_move;

  bool MOUSE_rotation = true;
  bool THRUST_movement = false;

  if (THRUST_movement)
  {
    const float thrust = 10.0f;

    if (move.up or move.down)
    {
      float angle = player.rot;

      vec2 v = angle_to_vec2(angle, thrust);

      if (move.up)
      {
         player.velocity += v;
      }
      if (move.down)
      {
        player.velocity -= v;
      }
    }

  }
  else // WASD movement
  {
    Velocity v{0.0f, 0.0f};

    if (move.up) v.y -= 1.0f;
    if (move.down) v.y += 1.0f;
    if (move.left) v.x -= 1.0f;
    if (move.right) v.x += 1.0f;

    float speed = 300.0f;
    v.x *= dt * speed;
    v.y *= dt * speed;

    player.velocity += v;

    player = UpdatePhysics(dt, player);

  }





  if (MOUSE_rotation)
  {
    vec2 angle_to_mouse = mouse_pointer.position - player.position;

    float a = get_angle(angle_to_mouse);
    player.rot = a;
  }
  else // left/right rotation
  {
    const float rot_speed = TWO_PI * 1.0f;

    if (move.left)
    {
      player.rot -= rot_speed * dt;
    }

    if (move.right)
    {
      player.rot += rot_speed * dt;
    }
  }




  player = UpdatePhysics(dt, player);



  // TRACE
  //   << " player " << player.position
  //   << "  " <<  "angle is " << degrees(player.rot) << "deg";

}


void Game::Shoot()
{
  Ball b = NewBall();

  b.position = player.position;
  b.velocity = angle_to_vec2(player.rot, 1000.0f);
  b.colour.r = RandomFloat(0.5f, 1.0f);
  b.colour.g = RandomFloat(0.5f, 1.0f);
  b.colour.b = RandomFloat(0.5f, 1.0f);
  b.radius = 10.0f;

  balls.push_back(b);
}
