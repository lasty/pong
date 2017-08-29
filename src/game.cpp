
#include "game.hpp"

#include <cstdlib>
#include <ctime>

#include <iostream>
#include <algorithm>

#include "input.hpp"

#include "maths.hpp"


const int num_balls = 10;
const int num_rects = 10;


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


//Game::~Game() { }


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
  //b.position = GetCenterScreen();
  b.position = RandomPosition();

  const float speed = 100.0f;
  b.velocity = { RandomFloat(-speed, speed) , RandomFloat(-speed, speed)};

  b.radius = RandomInt(5, 50);

  b.colour = RandomRGB();

  return b;
}


Rect Game::NewRect() const
{
  Rect r;
  r.position = RandomPosition();

  const float speed = 100.0f;
  r.velocity = { RandomFloat(-speed, speed) , RandomFloat(-speed, speed)};

  r.width = RandomInt(50, 100);
  r.height = RandomInt(50, 100);

  r.colour = RandomRGB();

  return r;
}


Line Game::NewLine() const
{
  Line l;

  l.p1 = RandomPosition();
  l.p2 = RandomPosition();

  l.colour = RandomRGB();

  return l;
}


Line MakeLine(vec2 const &p1, vec2 const &p2)
{
  return {p1, p2, RandomRGB()};
}


void Game::NewObjects()
{
  balls.clear();
  for(int i=0; i<num_balls; i++)
  {
    balls.push_back(NewBall());
  }
  //std::cout << "Generated " << balls.size() << " new balls." << std::endl;


  rects.clear();
  for(int i=0; i<num_rects; i++)
  {
    rects.push_back(NewRect());
  }
  //std::cout << "Generated " << rects.size() << " new rects." << std::endl;


  lines.clear();
  for(int i=0; i<2; i++)
  {
    lines.push_back(NewLine());
  }


  border_lines.clear();

  float b = 5.0f;
  vec2 tl { b, b };
  vec2 tr { width - b, b };
  vec2 bl { b, height - b };
  vec2 br { width - b, height - b };

  border_lines.push_back(MakeLine(tl, tr));
  border_lines.push_back(MakeLine(tr, br));
  border_lines.push_back(MakeLine(br, bl));
  border_lines.push_back(MakeLine(bl, tl));

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


bool Collides(const Ball &b1, const vec2 &point)
{
  float dist = distance(b1.position, point);

  return (dist <= b1.radius);
}


bool Collides(const Ball &c, const Rect &r)
{
  const vec2 rect_center {r.position.x + r.width/2.0f , r.position.y + r.height/2.0f };
  const vec2 circle_distance = vec_abs(c.position - rect_center);

  if ((circle_distance.x > (r.width/2.0f + c.radius))
    or (circle_distance.y > (r.height/2.0f + c.radius))) return false;

  if ((circle_distance.x <= (r.width/2.0f))
    or (circle_distance.y <= (r.height/2.0f))) return true;

  vec2 corner { r.width/2.0f, r.height/2.0f };

  vec2 dist_to_corner = circle_distance - corner;

  return (get_length(dist_to_corner) <= c.radius);
}


bool Collides(const Ball &b1, const Ball &b2)
{
  if (&b1 == &b2) return false;

  float radii = b1.radius + b2.radius;

  float dist = get_length(b2.position - b1.position);

  return (dist <= radii);
}


bool Collides(const Ball &ball, Line const &line)
{
  vec2 collision_point = nearest_point_on_line(line.p1, line.p2, ball.position);

  float dist = distance(collision_point, ball.position);

  return (dist < ball.radius);
}


bool Collides(const Rect &r, vec2 point)
{
  return
    in_range(r.position.x, r.position.x + r.width, point.x)
    and
    in_range(r.position.y, r.position.y + r.height, point.y);
}


bool Collides(const Rect &r1, const Rect &r2)
{
  if (&r1 == &r2) return false;

  return
    (r1.position.x < r2.position.x + r2.width)
    and
    (r1.position.x + r1.width > r2.position.x)
    and
    (r1.position.y < r2.position.y + r2.height)
    and
    (r1.position.y + r1.height > r2.position.y);
}


bool Collides([[maybe_unused]] const Rect &rect, [[maybe_unused]] const Line &line)
{
  //TODO
  return false;
}


bool Collides(const Line &line, const vec2 &point)
{
  //XXX maybe useless?

  vec2 nearest = nearest_point_on_line(line.p1, line.p2, point);

  float dist = distance(nearest, point);

  return (dist < 1.0f);
}


bool Collides([[maybe_unused]] const Line &line1, [[maybe_unused]] const Line &line2)
{
  if (&line1 == &line2) return false;

  //TODO
  return false;
}


//Try swapping the order of the arguments
template <typename OBJ1, typename OBJ2>
inline bool Collides(const OBJ1 &o1, const OBJ2 &o2)
{
  return Collides(o2, o1);
}


//Applies Collides() to lists
template <typename OBJ1, typename LIST>
bool Collides_List(const OBJ1 &o, const LIST &other)
{
  for(auto const &item : other)
  {
    if (Collides(o, item))
      return true;
  }
  return false;
}

//Applies Collides() to lists
template <typename OBJ1, typename LIST, typename VECTYPE = std::vector<const typename LIST::value_type *>>
VECTYPE GetVec_Collides_List(const OBJ1 &o, const LIST &other)
{
  VECTYPE vec;

  for(auto const &item : other)
  {
    if (Collides(o, item))
    vec.push_back(&item);
  }
  return vec;
}



template <typename OBJ>
bool Game::Collides_Any(const OBJ &obj) const
{
  //Balls
  if (Collides_List(obj, balls)) return true;
  //if (Collides(obj, mouse_pointer)) return true;
  //if (Collides(obj, player)) return true;

  //Rects
  if (Collides_List(obj, rects)) return true;

  //Lines
  if (Collides_List(obj, lines)) return true;
  if (Collides_List(obj, border_lines)) return true;

  return false;
}

//Explicit template instantiation
template bool Game::Collides_Any<>(const Ball &obj) const;
template bool Game::Collides_Any<>(const Rect &obj) const;


Ball Game::UpdatePhysics(float dt, const Ball & b) const
{
  Ball out = b;
  out.position.x = b.position.x + b.velocity.x * dt;
  out.position.y = b.position.y + b.velocity.y * dt;

  const float gravity = 300.0f;
  const float bounce = -0.8f;
  const float friction_amount = 1.0f;

  //Lines
  for (auto & vec : {lines, border_lines})
  {
    auto line_list = GetVec_Collides_List(b, vec);
    if (line_list.size())
    {
      auto line = *line_list.front();

      //reflect bounce
      vec2 normal = get_normal(line.p1, line.p2);
      out.velocity = reflect(out.velocity, normal);


      //back off position by radius
      vec2 contact_pos = nearest_point_on_line(line.p1, line.p2, out.position);

      out.position = contact_pos + (normal * (b.radius + 5));
      //vec2 towards_last = normalize(contact_pos - b.position);
      //out.position = contact_pos + (towards_last * (b.radius + 5));

    }
  }

/*
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
*/

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

  remove_inplace(rects, [=](auto &r){ return Collides_Any(r); } );


  //remove_inplace(balls, [=](auto &b){ return b.radius == 10 && Collides_Any(b); } );


  //TODO update other objects?
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
  b.position += b.velocity * 0.03f;

  b.colour.r = RandomFloat(0.5f, 1.0f);
  b.colour.g = RandomFloat(0.5f, 1.0f);
  b.colour.b = RandomFloat(0.5f, 1.0f);
  b.radius = 10.0f;

  balls.push_back(b);
}
