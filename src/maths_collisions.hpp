
#pragma once


#include "maths.hpp"


//TODO make these more generic
#include "game.hpp"



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
