#pragma once

#include <vector>
#include <map>


#include "maths_types.hpp"


class Text
{
private:
  std::vector<vec2> vertices;
  std::map<char, std::vector<int>> glyphs;

public:
  Text();

private:
  void SetupVertexes();
  void SetupGlyphs();

public:
  const std::vector<int> &GetGlyph(char ch) const;
  std::vector<float> MakeGlyph(char ch, const vec2 &offset, const vec4 &colour) const;
  std::vector<float> MakeString(const std::string &str, vec2 offset, const vec4 &colour) const;
};
