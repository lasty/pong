
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

  NewObjects();

  player = NewBall();
  player.radius = 20;
  player.colour = {1.0f, 1.0f, 1.0f, 1.0f};
  player.velocity = {0.0f, 0.0f};


  mouse_pointer.radius = 20;
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


Rect Game::NewRect() const
{
  Rect r;
  r.position = RandomPosition();

  const float speed = 100.0f;
  r.velocity = { RandomFloat(-speed, speed) , RandomFloat(-speed, speed)};

  r.width = RandomInt(50, 200);
  r.height = RandomInt(50, 200);

  r.colour = RandomRGB();

  return r;
}


void Game::NewObjects()
{
  balls.clear();

  for(int i=0; i<0; i++)
  {
    Ball ball = NewBall();
    balls.push_back(ball);
  }

  //std::cout << "Generated " << balls.size() << " new balls." << std::endl;


  rects.clear();

  for(int i=0; i<1; i++)
  {
    Rect rect = NewRect();
    rects.push_back(rect);
  }

  //std::cout << "Generated " << rects.size() << " new rects." << std::endl;
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


bool in_range(float beg, float end, float p)
{
  return (p >= beg and p <= end);
}


bool Game::Collides(const Rect &r, vec2 point) const
{
  return
    in_range(r.position.x, r.position.x + r.width, point.x)
    and
    in_range(r.position.y, r.position.y + r.height, point.y);
}


bool Game::Collides(const Rect &r1, const Rect &r2) const
{
  return
    (r1.position.x < r2.position.x + r2.width)
    and
    (r1.position.x + r1.width > r2.position.x)
    and
    (r1.position.y < r2.position.y + r2.height)
    and
    (r1.position.y + r1.height > r2.position.y);
}

#include "maths_utils.hpp"

bool Game::Collides(const Rect &r, const Ball &c) const
{
  const vec2 rect_center {r.position.x + r.width/2.0f , r.position.y + r.height/2.0f };
  const vec2 circle_distance = vec_abs(c.position - rect_center);

  TRACE << "circle_distance " << circle_distance << "  ";

  if ((circle_distance.x > (r.width/2.0f + c.radius))
    or (circle_distance.y > (r.height/2.0f + c.radius))) return false;

  if ((circle_distance.x <= (r.width/2.0f))
    or (circle_distance.y <= (r.height/2.0f))) return true;

  vec2 corner { r.width/2.0f, r.height/2.0f };

  vec2 dist_to_corner = circle_distance - corner;

  return (get_length(dist_to_corner) <= c.radius);
}

//
// float point_point_distance(const vec2& p1, const vec2 &p2)
// {
//   vec2 dist = p1 - p2;
//   return get_length(dist);
// }
//
//
// float point_line_distance(const vec2 &l1, const vec2 &l2, const vec2 &p1)
// {
//   float x1 = l1.x;
//   float x2 = l2.x;
//   float y1 = l1.y;
//   float y2 = l2.y;
//
//   float px = p1.x;
//   float py = p1.y;
//   float dist = abs( (l2.x - l1.x)*(l1.y-p1.y) - (l1.x-p1.x)*(l2.y-l1.y)) /
//
// //  float dist = abs( (x2-x1)*(y1-py)-(x1-px)*(y2-y1) ) /
//     point_point_distance(l1, l2);
//   return dist;
// }
//
//
// bool Game::Collides(const Rect &r, const Ball &c) const
// {
//   const vec2 v1 = r.position;
//   const vec2 v2 = {r.position.x + r.width, r.position.y};
//   const vec2 v3 = {r.position.x + r.width, r.position.y + r.height};
//   const vec2 v4 = {r.position.x, r.position.y + r.height};
//
//   const float radius = c.radius;
//   const vec2 circle = c.position;
//   //
//   // if (
//   //   (point_point_distance(circle, v1) < radius)
//   //   or
//   //   (point_point_distance(circle, v2) < radius)
//   //   or
//   //   (point_point_distance(circle, v3) < radius)
//   //   or
//   //   (point_point_distance(circle, v4) < radius)
//   // ) return true;
//
//   if (
//     (point_line_distance(v1, v2, circle) < radius)
//     // or
//     // (point_line_distance(v2, v3, circle) < radius)
//     // or
//     // (point_line_distance(v3, v4, circle) < radius)
//     // or
//     // (point_line_distance(v4, v1, circle) < radius)
//   ) return true;
//
//
//   //return false;
//   return Collides(r, circle);
// }

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


bool Game::Collides_Any(const Rect &rect) const
{
  for (const Rect &r : rects)
  {
    if (&r == &rect) continue;  //Don't test self

    if (Collides(rect, r))
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
    //b.rot += dt * TWO_PI / 8.0f;
  }

}



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
