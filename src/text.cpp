
#include "text.hpp"


#include "maths.hpp"

#include <string>


Text::Text()
{
  SetupVertexes();
  SetupGlyphs();
}

void Text::SetupVertexes()
{
  //Vertexes are mapped to the 9 points of the keypad, think 7-segment LED display

  //below, for fullstop
  vertices.push_back({0.0f, 25.0f});

  //bottom row
  vertices.push_back({0.0f, 20.0f});
  vertices.push_back({5.0f, 20.0f});
  vertices.push_back({10.0f, 20.0f});

  //middle row
  vertices.push_back({0.0f, 10.0f});
  vertices.push_back({5.0f, 10.0f});
  vertices.push_back({10.0f, 10.0f});

  //top row
  vertices.push_back({0.0f, 0.0f});
  vertices.push_back({5.0f, 0.0f});
  vertices.push_back({10.0f, 0.0f});
}


void Text::SetupGlyphs()
{
  //Sets up list of indexes for drawing GL_LINES (pairs of line segments)

  glyphs[0] = {1, 9, 7, 3, 7, 9, 9, 3, 3, 1, 1, 7}; //error character

  glyphs[' '] = {}; //Space is an empty list
  glyphs['.'] = {1, 0};
  glyphs[':'] = {5, 6, 2, 3};
  glyphs['<'] = {9, 4, 4, 3};
  glyphs['>'] = {7, 6, 6, 1};

  glyphs['A'] = {1, 4, 4, 8, 8, 6, 6, 3, 4, 6};
  glyphs['B'] = {1, 7, 7, 8, 8, 6, 6, 4, 5, 3, 3, 1};
  glyphs['C'] = {9, 8, 8, 4, 4, 2, 2, 3};
  glyphs['D'] = {1, 7, 7, 8, 8, 6, 6, 2, 2, 1};

  glyphs['E'] = {7, 1, 1, 3, 7, 9, 4, 5};
  glyphs['F'] = {7, 1, 7, 9, 4, 5};
  glyphs['G'] = {9, 8, 8, 4, 4, 2, 2, 3, 3, 6, 6, 5};
  glyphs['H'] = {7, 1, 9, 3, 4, 6};

  glyphs['I'] = {8, 2};
  glyphs['J'] = {9, 3, 3, 2, 2, 4};
  glyphs['K'] = {7, 1, 4, 5, 5, 9, 5, 3};
  glyphs['L'] = {7, 1, 1, 3};

  glyphs['M'] = {1, 7, 7, 5, 5, 9, 9, 3};
  glyphs['N'] = {7, 1, 7, 3, 3, 9};
  glyphs['O'] = {8, 6, 6, 2, 2, 4, 4, 8};
  glyphs['P'] = {7, 1, 7, 8, 8, 6, 6, 4};

  glyphs['Q'] = {8, 6, 6, 2, 2, 4, 4, 8, 5, 3};
  glyphs['R'] = {7, 1, 7, 8, 8, 6, 6, 4, 5, 3};
  glyphs['S'] = {9, 8, 8, 4, 4, 6, 6, 2, 2, 1};
  glyphs['T'] = {8, 2, 7, 9};

  glyphs['U'] = {7, 1, 1, 3, 3, 9};
  glyphs['V'] = {7, 2, 2, 9};
  glyphs['W'] = {7, 1, 1, 5, 5, 3, 3, 9};
  glyphs['X'] = {1, 9, 7, 3};

  glyphs['Y'] = {7, 5, 9, 5, 5, 2};
  glyphs['Z'] = {7, 9, 9, 1, 1, 3};

  glyphs['0'] = {7, 1, 1, 3, 3, 9, 9, 7, 9, 1};
  glyphs['1'] = {5, 9, 9, 3};
  glyphs['2'] = {7, 9, 9, 6, 6, 4, 4, 1, 1, 3};
  glyphs['3'] = {7, 9, 9, 3, 3, 1, 4, 6};
  glyphs['4'] = {7, 4, 4, 6, 9, 3};

  glyphs['5'] = {9, 7, 7, 4, 4, 6, 6, 3, 3, 1};
  glyphs['6'] = {9, 7, 7, 1, 1, 3, 3, 6, 6, 4};
  glyphs['7'] = {7, 9, 9, 5, 5, 2};
  glyphs['8'] = {7, 9, 9, 3, 3, 1, 1, 7, 4, 6};
  glyphs['9'] = {1, 3, 3, 9, 9, 7, 7, 4, 4, 6};
}


const std::vector<int> &Text::GetGlyph(char ch) const
{
  auto it = glyphs.find(ch);

  if (it != glyphs.end()) return it->second;

  it = glyphs.find(toupper(ch));
  if (it != glyphs.end()) return it->second;

  return glyphs.at(0);
}


std::vector<float>
Text::MakeGlyph(char ch, const vec2 &offset, const vec4 &colour) const
{
  std::vector<float> out;

  const auto &indexes = GetGlyph(ch);

  for (int i : indexes)
  {
    vec2 v = vertices.at(i);
    v += offset;

    out.push_back(v.x);
    out.push_back(v.y);

    out.push_back(colour.r);
    out.push_back(colour.g);
    out.push_back(colour.b);
    out.push_back(colour.a);
  }

  return out;
}


std::vector<float>
Text::MakeString(const std::string &str, vec2 offset, const vec4 &colour) const
{
  std::vector<float> out;

  for (char ch : str)
  {
    std::vector<float> char_vertexes = MakeGlyph(ch, offset, colour);

    out.insert(out.end(), char_vertexes.begin(), char_vertexes.end());
    offset.x += 15;
  }

  return out;
}
