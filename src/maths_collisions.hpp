
#pragma once

#include "game.hpp"

bool BoundingBoxCollides(const BoundingBox &a, const BoundingBox &b);
bool Collides(const Ball &ball, Line const &line);
bool Collides(const Ball &ball, const Block &block);
bool Collides(const Ball &b1, const vec2 &point);
bool Collides(const Ball &b1, const Ball &b2);
