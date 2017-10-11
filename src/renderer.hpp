#pragma once

#include <vector>
#include <map>

#include "shader.hpp"
#include "text.hpp"
#include "game.hpp"

typedef uint32_t GLenum;

struct GLState
{
  int program = 0;
  int vao = 0;
  bool blending = false;
};


struct shape_def
{
  int offset;
  int count;
};


class VertexData
{
private:
  std::vector<float> vertex_data;
  int buffer_id = 0;
  int vao_id = 0;
  GLenum usage{};

  const int floats_per_vertex = 2 + 4;
  const int stride = floats_per_vertex * sizeof(float);


public:
  VertexData(GLenum usage);
  ~VertexData();

  void Clear();

  shape_def AddShape(std::vector<float> const &vertexes);

  void AddVertex(const vec2 &position, const vec4 &colour);

  void UpdateVertexes();

  int GetOffset() const;
  int GetNumVertexes() const;

  int GetVAO() const;

  void AttachAttribute(int attrib_id, int size, int offset, GLenum type);
  void DetachAttribute(int attrib_id);
};


class Renderer
{
private:
  GLState gl_state;

  Shader::Basic basic_shader;

  VertexData shapes_data;

  shape_def circle_shape;
  shape_def arrow_shape;
  std::map<int, std::map<int, shape_def>> rect_shapes;
  std::map<BlockType, shape_def> block_shapes;

  Text text;

  VertexData lines_data;
  VertexData particle_data;

public:
  Renderer();
  ~Renderer();

  void UseProgram(int program_id);
  void UseVAO(int vao_id);
  void EnableBlend();
  void DisableBlend();

  void Resize(int width, int height);

  void DynamicLine(vec2 const &v1, vec2 const &v2, const vec4 &colour);
  void DrawVertexData(GLenum draw_type, const VertexData &vertex_data);

  void SetupShapes();
  void SetupBlockShapes();

  void DrawShape(GLenum draw_type, shape_def const &shape);

  void DrawCircle(int radius, float x, float y);
  void FillCircle(int radius, float x, float y);
  void RenderBall(const Ball &ball, bool draw_outline = true);

  void RenderArrow(const vec2 &position, float rot);

  shape_def GetRectShape(int w, int h);
  void RenderBlock(const Block &block, bool draw_normals = false);
  void RenderBounds(const BoundingBox &bounds);

  void DrawGameState(const GameState &state);
};
