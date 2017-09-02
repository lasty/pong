
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


Block Game::NewBlock(int x, int y, BlockType bt) const
{
  Block b;
  b.type = bt;
  //b.type = (bt == BlockType::none) ? RandomBlockType() : bt;

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
      blocks.push_back(NewBlock(x, y, RandomBlockType()));
    }
  }


  border_lines.clear();
  border_lines.push_back(NewWorldBorders(5, width, height));

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


bool Collides(const Ball &ball, BorderLine const &line)
{
  vec2 collision_point = nearest_point_on_line_segment(line.p1, line.p2, ball.position);

  float dist = distance(collision_point, ball.position);

  return (dist < ball.radius);
}


/*
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
*/


bool Collides(const Ball &ball, const Block &block)
{
  if (not BoundingBoxCollides(ball.bounds, block.bounds)) return false;

  for(auto const &line : block.geometry)
  {
    if (Collides(ball, line)) return true;
  }
  return false;
}


std::vector<Collision> Game::GetAllCollisions(Ball &old_ball, Ball &ball)
{
  std::vector<Collision> vec_out;

  for (auto vec : { &blocks, &border_lines })
  {
    for (Block & block : *vec)
    {
      if (Collides(ball, block))
      {
        std::vector<BorderLine> collided_lines;
        for(auto & line : block.geometry)
        {
          if (Collides(ball, line)) collided_lines.push_back(line);
        }

        vec_out.push_back( Collision{ball, block, collided_lines, old_ball.position, old_ball.velocity} );
      }
    }
  }

  return vec_out;

}


/*
bool Game::Collides_Any(const Ball &obj) const
{
  //Blocks
  if (Collides_List(obj, blocks)) return true;

  //Border Lines
  if (Collides_List(obj, border_lines)) return true;

  return false;
}
*/

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



Ball Game::UpdatePhysics(float dt, Ball & old_ball)
{
  Ball out = old_ball;
  float orig_speed = get_length(old_ball.velocity);

  out.position = old_ball.position + (old_ball.velocity * dt);
  out.bounds = MakeBounds(out);


  [[maybe_unused]] const float gravity = 300.0f;
  [[maybe_unused]] const float bounce = -0.8f;
  [[maybe_unused]] const float friction_amount = 1.0f;


  auto collisions = GetAllCollisions(old_ball, out);

  std::vector<BorderLine> collision_lines;

  vec2 normal_acc = {};
  int num_normals = 0;

  for (auto & coll : collisions)
  {
    for (auto & line : coll.lines)
    {
      collision_lines.push_back(line);
      vec2 normal = get_normal(line.p1, line.p2);

      vec2 contact_point = nearest_point_on_line_segment(line.p1, line.p2, old_ball.position);
      vec2 contact_angle = normalize(old_ball.position - contact_point);
      contact_angle = old_ball.velocity * -1.0f;
      float line_dot = dot(normalize(normal), normalize(contact_angle));

      if (line_dot > 0)
      {
        normal_acc += normal;
        num_normals++;
      }
    }
  }

  //TODO
  // Filter list to be lines facing the ball
  // Average normals of filtered list
  // Apply bounce


  if (num_normals)
  {

  // if (collision_lines.size())
  // {
  //   TRACE << "coll: " << collision_lines.size() << "  ";
  //   auto best_line = collision_lines.front();
  //   float best_dot = -1.0f;


    // for (auto horiz_line : collision_lines)
    // {
    //   // BorderLine horiz_line = collision_lines.front();
    //
    //   vec2 contact_horiz = nearest_point_on_line_segment(horiz_line.p1, horiz_line.p2, old_ball.position);
    //   //
    //   // TRACE << "distance -  horiz: " << distance(old_ball.position, contact_horiz)
    //   //   << "  diag: " << distance(old_ball.position, contact_diag) << "  ";
    //
    //   vec2 horiz_normal = get_normal(horiz_line.p1, horiz_line.p2);
    //   // vec2 diag_normal = get_normal(diag_line.p1, diag_line.p2);
    //   vec2 horiz_tangent = normalize(old_ball.position - contact_horiz);
    //   // vec2 diag_tangent = normalize(old_ball.position - contact_diag);
    //
    //   TRACE << "line: " << horiz_normal << "  " << horiz_tangent << "  ";
    //   // TRACE << "diag : " << diag_normal << "  " << diag_tangent << "  ";
    //
    //   float horiz_dot = dot(normalize(horiz_normal), normalize(horiz_tangent));
    //   // float diag_dot = dot(normalize(diag_normal), normalize(diag_tangent));
    //
    //   TRACE << " dot: " << horiz_dot << "  ";
    //   // TRACE << "diag_dot: " << diag_dot << "  ";
    //
    //   if (horiz_dot > best_dot)
    //   {
    //     best_dot = horiz_dot;
    //     best_line = horiz_line;
    //   }
    // }


    vec2 normal_avg = normal_acc / float(num_normals);

    //vec2 refl = CalculateReflection(old_ball, best_line);
    vec2 refl = reflect(normalize(old_ball.velocity), normalize(normal_avg));

    //vec2 force = refl - old_ball.velocity;
    //out.velocity = force;

    out.velocity = normalize(refl) * orig_speed;


        // //Back up radius amount from line
        // try {
        //   vec2 intersection = get_intersection(line.p1, line.p2, old_ball.position, out.position);
        //   vec2 back_dir = normalize(intersection - old_ball.position);
        //   out.position = intersection - back_dir * (old_ball.radius+2);
        //   throw "xxx";
        // }
        // catch (...) //get_intersecion throws when lines are parallel
        // {
        //   out.position = old_ball.position;
        // }

    out.position = old_ball.position;
    out.bounds = MakeBounds(out);

    //out.velocity = normalize(out.velocity) * orig_speed;



  }


  //TRACE << " STEP ";
  // if (Collides_List(b,border_lines)) TRACE << "LAST  ";
  // if (Collides_List(out,border_lines)) TRACE << "THIS  ";


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
  //remove_inplace(blocks, [=](auto &r){ return Collides_Any(r); } );


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
  b.velocity = angle_to_vec2(player.rot, 100.0f);
  b.position += b.velocity * 0.03f;

  b.colour.r = RandomFloat(0.5f, 1.0f);
  b.colour.g = RandomFloat(0.5f, 1.0f);
  b.colour.b = RandomFloat(0.5f, 1.0f);
  b.radius = 10.0f;

  balls.push_back(b);
}
