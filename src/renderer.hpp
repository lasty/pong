#pragma once

#include <vector>
#include <map>

#include "shader.hpp"

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


class Renderer
{
private:
  GLState state;

  Shader::Basic basic_shader;

  int attribs_per_vertex = 2;
  int stride = attribs_per_vertex * sizeof(float);
  std::vector<float> vertex_data;

  int buf_id = 0;
  int vao_id = 0;

  shape_def circle_shape;
  shape_def arrow_shape;
  std::map<int, std::map<int, shape_def>> rect_shapes;


  int dynamic_buff_id = 0;
  int dynamic_vao_id = 0;
  std::vector<float> dynamic_vertex_data;


public:

  Renderer();
  ~Renderer();

  void UseProgram(int program_id);
  void UseVAO(int vao_id);
  void EnableBlend();
  void DisableBlend();

  void Resize(int width, int height);

  void SetupVertexData();
  shape_def AddShape(std::vector<float> const & vertexes);
  void UpdateVertexData();

  void SetupDynamicVertexData();
  void ClearDynamicVertexData();
  void DynamicLine(vec2 const &v1, vec2 const &v2);
  void UpdateDynamicVertexData();
  void DrawDynamic(GLenum draw_type);

  void SetupShapes();
  void DeleteShapes();

  void DrawShape(GLenum draw_type, shape_def const & shape);

  void DrawCircle(int radius, float x, float y);
  void FillCircle(int radius, float x, float y);
  void RenderBall(const struct Ball & ball, bool draw_outline = true);

  void RenderArrow(const Ball & ball);

  shape_def GetRectShape(int w, int h);
  void RenderBlock(const struct Block & block);
  void RenderBounds(const struct BoundingBox & bounds, bool draw_outline = false);


  void DrawGameState(const class GameState & state);

};
