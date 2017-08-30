
#include "game.hpp"

#include <cstdlib>
#include <ctime>

#include <iostream>
#include <algorithm>

#include "input.hpp"

#include "maths.hpp"
#include "maths_utils.hpp"


BorderLine::BorderLine(const vec2 &p1, const vec2 &p2)
: p1(p1)
, p2(p2)
{
  bounds = MakeBounds(*this);
}


Game::Game(int width, int height)
:width(width)
,height(height)
{
  srand (static_cast <unsigned> (time(0)));

  SetupBlockGeometry();
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


BoundingBox MakeBounds(const BorderLine & line)
{
  const float top = std::min(line.p1.y, line.p2.y);
  const float bottom = std::max(line.p1.y, line.p2.y);

  const float left = std::min(line.p1.x, line.p2.x);
  const float right = std::max(line.p1.x, line.p2.x);

  return { {left, top} , {right, bottom} };
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


Block Game::NewBlock(int x, int y) const
{
  Block b;
  b.type = RandomBlockType();
  b.position = { 50.0f + (110.0f * x) , 50.0f + (60.0f * y) };
  b.colour = RandomRGB();
  b.geometry = GetGeometry(b.type);

  for (auto & line : b.geometry)
  {
    line.p1 += b.position;
    line.p2 += b.position;
    line.bounds = MakeBounds(line);
  }

  b.bounds = MakeBounds(b);

  return b;
}


BorderLine MakeLine(vec2 const &p1, vec2 const &p2)
{
  BorderLine l{p1, p2};
  return l;
}


void Game::SetupBlockGeometry()
{
  vec2 tl {0,0};
  vec2 bl {0, 50};
  vec2 tr {50, 0};
  vec2 br {50, 50};

  BlockGeometry sq { {tl, bl}, {bl, br}, {br, tr}, {tr, tl} };
  block_shapes.emplace(BlockType::square, sq);

  BlockGeometry tril { {tr, tl}, {tl, br}, {br, tr} };
  block_shapes.emplace(BlockType::triangle_left, tril);

  BlockGeometry trir { {tr, tl}, {tl, bl}, {bl, tr} };
  block_shapes.emplace(BlockType::triangle_right, trir);

  vec2 wtr {100, 0};
  vec2 wbr {100, 50};

  BlockGeometry rec { {tl, bl}, {bl, wbr}, {wbr, wtr}, {wtr, tl} };
  block_shapes.emplace(BlockType::rectangle, rec);

  block_shapes.emplace(BlockType::rect_triangle_left,
    BlockGeometry{ {wtr, tl}, {tl, br}, {br, wbr}, {wbr, wtr} });

  block_shapes.emplace(BlockType::rect_triangle_right,
    BlockGeometry{ {wtr, tl}, {tl, bl}, {bl, br}, {br, wtr} });

}


const BlockGeometry & Game::GetGeometry(BlockType bt) const
{
  return block_shapes.at(bt);
}

void Game::NewObjects()
{
  balls.clear();
  for(int i=0; i<5; i++)
  {
    balls.push_back(NewBall());
  }
  //std::cout << "Generated " << balls.size() << " new balls." << std::endl;


  blocks.clear();
  for(int x=0; x<5; x++)
  {
    for (int y=0; y<3; y++)
    {
      blocks.push_back(NewBlock(x, y));

    }
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


bool BoundingBoxCollides(const BoundingBox &a, const BoundingBox &b)
{
  if (&a == &b) return false;

  return
    (a.top_left.x < b.bottom_right.x)
    and
    (a.bottom_right.x > b.top_left.x)
    and
    (a.top_left.y < b.bottom_right.y)
    and
    (a.bottom_right.y > b.top_left.y);
}


bool Collides(const Ball &b1, const vec2 &point)
{
  float dist = distance(b1.position, point);

  return (dist <= b1.radius);
}



bool Collides(const Ball &b1, const Ball &b2)
{
  if (&b1 == &b2) return false;

  float radii = b1.radius + b2.radius;

  float dist = get_length(b2.position - b1.position);

  return (dist <= radii);
}


bool Collides(const Ball &ball, BorderLine const &line)
{
  vec2 collision_point = nearest_point_on_line(line.p1, line.p2, ball.position);

  float dist = distance(collision_point, ball.position);

  return (dist < ball.radius);
}


bool Collides(const Block &r, vec2 point)
{
  //TODO fix this, but is this needed?
  //TODO make BoundingBox collides with point instead?

  BoundingBox pt { point, point };
  return BoundingBoxCollides(r.bounds, pt);

  /*
  return
    in_range(r.position.x, r.position.x + r.width, point.x)
    and
    in_range(r.position.y, r.position.y + r.height, point.y);
  */
}


bool Collides(const Block &r1, const Block &r2)
{
  //TODO does this make sense?

  return BoundingBoxCollides(r1.bounds, r2.bounds);

  /*
  if (&r1 == &r2) return false;

  return
    (r1.position.x < r2.position.x + r2.width)
    and
    (r1.position.x + r1.width > r2.position.x)
    and
    (r1.position.y < r2.position.y + r2.height)
    and
    (r1.position.y + r1.height > r2.position.y);
  */
}


bool Collides([[maybe_unused]] const Block &block, [[maybe_unused]] const BorderLine &line)
{
  //TODO
  return false;
}


bool Collides(const BorderLine &line, const vec2 &point)
{
  //XXX maybe useless?

  vec2 nearest = nearest_point_on_line(line.p1, line.p2, point);

  float dist = distance(nearest, point);

  return (dist < 1.0f);
}


bool Collides([[maybe_unused]] const BorderLine &line1, [[maybe_unused]] const BorderLine &line2)
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
    if (BoundingBoxCollides(o.bounds, item.bounds) and Collides(o, item))
      return true;
  }
  return false;
}


bool Collides(const Ball &ball, const Block &block)
{
  if (not BoundingBoxCollides(ball.bounds, block.bounds)) return false;
  return Collides_List(ball, block.geometry);
}


//Applies Collides() to lists
template <typename OBJ1, typename LIST, typename VECTYPE = std::vector<const typename LIST::value_type *>>
VECTYPE GetVec_Collides_List(const OBJ1 &o, const LIST &other)
{
  VECTYPE vec;

  for(auto const &item : other)
  {
    if (BoundingBoxCollides(o.bounds, item.bounds) and Collides(o, item))
      vec.push_back(&item);
  }
  return vec;
}

/*
bool Collides_Rect_Circle(const Ball &c, const Rect &r)
{
  //Might be broken...

  const vec2 block_center {r.position.x + r.width/2.0f , r.position.y + r.height/2.0f };
  const vec2 circle_distance = vec_abs(c.position - block_center);

  if ((circle_distance.x > (r.width/2.0f + c.radius))
    or (circle_distance.y > (r.height/2.0f + c.radius))) return false;

  if ((circle_distance.x <= (r.width/2.0f))
    or (circle_distance.y <= (r.height/2.0f))) return true;

  vec2 corner { r.width/2.0f, r.height/2.0f };

  vec2 dist_to_corner = circle_distance - corner;

  return (get_length(dist_to_corner) <= c.radius);
}
*/



template <typename OBJ>
bool Game::Collides_Any(const OBJ &obj) const
{
  //Balls
  if (Collides_List(obj, balls)) return true;
  //if (Collides(obj, mouse_pointer)) return true;
  //if (Collides(obj, player)) return true;

  //Blocks
  if (Collides_List(obj, blocks)) return true;

  //Border Lines
  if (Collides_List(obj, border_lines)) return true;

  return false;
}


//Explicit template instantiation
template bool Game::Collides_Any<>(const Ball &obj) const;
template bool Game::Collides_Any<>(const Block &obj) const;


vec2 CalculateReflection(const Ball &b, const BorderLine &line)
{
  vec2 normal = get_normal(line.p1, line.p2);

  if (dot(normal, b.velocity) > 0 )  //if normal is wrong way
  {
    //return {0,0};
    normal = get_normal(line.p2, line.p1);
  }

  return reflect(b.velocity, normal);
}




Ball Game::UpdatePhysics(float dt, const Ball & b) const
{
  Ball out = b;
  float orig_speed = get_length(b.velocity);

  out.position = b.position + (b.velocity * dt);
  out.bounds = MakeBounds(out);


  [[maybe_unused]] const float gravity = 300.0f;
  [[maybe_unused]] const float bounce = -0.8f;
  [[maybe_unused]] const float friction_amount = 1.0f;

  //XXX
  for (auto & block : blocks)
  {
    if (BoundingBoxCollides(out.bounds, block.bounds))
    {
      auto line_list = GetVec_Collides_List(out, block.geometry);
      for (auto line_ptr : line_list)
      {
        auto line = *line_ptr;

        // out.velocity = CalculateReflection(out, line);
        // out.position = b.position;

        //out.velocity = CalculateReflection(b, line);
        vec2 ref = CalculateReflection(b, line);
        vec2 force = ref - b.velocity;
        out.velocity += force;

        //Back up radius amount from line
        try {
          vec2 intersection = get_intersection(line.p1, line.p2, b.position, out.position);
          vec2 back_dir = normalize(intersection - b.position);
          out.position = intersection - back_dir * (b.radius+2);
          throw "xxx";
        }
        catch (...) //get_intersecion throws when lines are parallel
        {
          out.position = b.position;
        }

        out.position = b.position;

        out.bounds = MakeBounds(out);
      }
    }
  }



  //TRACE << " STEP ";
  // if (Collides_List(b,border_lines)) TRACE << "LAST  ";
  // if (Collides_List(out,border_lines)) TRACE << "THIS  ";


  auto line_list = GetVec_Collides_List(out, border_lines);
  //if (line_list.size())
  for (auto line_ptr : line_list)
  {
    auto line = *line_ptr;
    //auto line = *line_list.front();

    vec2 ref = CalculateReflection(b, line);

    vec2 force = ref - b.velocity;

    out.velocity += force;

    //out.velocity = ref;

    //Back up radius amount from line
    try {
      vec2 intersection = get_intersection(line.p1, line.p2, b.position, out.position);
      vec2 back_dir = normalize(intersection - b.position);
      out.position = intersection - back_dir * (b.radius);
      throw "xxx";
    }
    catch (...) //get_intersecion throws when lines are parallel
    {
      out.position = b.position;
    }

    out.bounds = MakeBounds(out);

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


/* //XXX
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
*/

  if (get_length(out.velocity) != orig_speed)
  {
    out.velocity = normalize(out.velocity) * orig_speed;
  }

  out.bounds = MakeBounds(out);
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

  //TODO maybe add sounds or particles when destroyed
  remove_inplace(blocks, [=](auto &r){ return Collides_Any(r); } );


  //remove_inplace(balls, [=](auto &b){ return b.radius == 10 && Collides_Any(b); } );


  //TODO update other objects?


  //XXX Debugging stuff

  // if (lines.size())
  // {
  //   const auto & line = lines.front();
  //   vec2 normal = get_normal(line.p1, line.p2);
  //   vec2 dir = mouse_pointer.position - line.p1;
  //
  //   float dp = dot(normal, dir);
  //
  //   TRACE << "dot(normal, dir) = " << dp ;
  // }


}


void Game::PlayerInput(float dt, Input const &input)
{
  mouse_pointer.position = input.mouse;
  mouse_pointer.bounds = MakeBounds(mouse_pointer);

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
    vec2 v{0.0f, 0.0f};

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
