
#include "renderer.hpp"

#include <vector>

#include "maths.hpp"
#include "game.hpp"
#include "gl.hpp"
#include "maths_utils.hpp"


std::vector<float> MakeCircle(float radius, int segments)
{
  std::vector<float> out;

  for(int i = 0; i < segments; i++)
  {
    float angle = float(2*PI) * (float(i) / float(segments));

    float x = radius * cosf(angle);
    float y = radius * sinf(angle);

    out.push_back(x);
    out.push_back(y);
  }

  return out;
}


std::vector<float> MakeArrow(float radius)
{
  std::vector<float> out;

  float width = (PI / 8);
  for(float angle : { 0.0f, PI - width, PI + width })
  {
    float x = radius * cosf(angle);
    float y = radius * sinf(angle);

    out.push_back(x);
    out.push_back(y);
  }

  return out;
}


std::vector<float> MakeRect(float width, float height)
{
  std::vector<float> out;

  out.push_back(0);
  out.push_back(0);

  out.push_back(width);
  out.push_back(0);

  out.push_back(width);
  out.push_back(height);

  out.push_back(0);
  out.push_back(height);

  return out;
}


Renderer::Renderer()
{
  SetupShapes();

  SetupDynamicVertexData();
}


Renderer::~Renderer()
{
  DeleteShapes();
}


void Renderer::UseProgram(int program_id)
{
  if (state.program == program_id) return;

  state.program = program_id;
  GL::UseProgram(basic_shader);
}


void Renderer::UseVAO(int vao_id)
{
  if (state.vao == vao_id) return;

  state.vao = vao_id;
  GL::UseVAO(vao_id);
}


void Renderer::EnableBlend()
{
  if (state.blending) return;
  state.blending = true;

  glEnable(GL_BLEND);
  //glEnablei(GL_BLEND, 0);
  glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
  glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
}


void Renderer::DisableBlend()
{
  if (state.blending == false) return;
  state.blending = false;

  glDisable(GL_BLEND);
}


void Renderer::Resize(int width, int height)
{
  basic_shader.SetResolution(width, height);
}


shape_def Renderer::AddShape(std::vector<float> const & vertexes)
{
  if (vertexes.size() % attribs_per_vertex != 0)
    { throw std::runtime_error("Uneven vertexes given to AddShape"); }


  shape_def s;
  s.offset = (vertex_data.size() / attribs_per_vertex);
  s.count = vertexes.size() / attribs_per_vertex;

  vertex_data.insert(vertex_data.end(), vertexes.begin(), vertexes.end());

  return s;
}


void Renderer::SetupVertexData()
{
  buf_id = GL::CreateBuffers();

  vao_id = GL::CreateVertexArrays();

  int buffer_index = 0;
  glVertexArrayVertexBuffer(vao_id, buffer_index, buf_id, 0, stride);

  int attrib_id = 0;
  GL::AttachAttribute(vao_id, attrib_id, attribs_per_vertex, GL_FLOAT);
}


void Renderer::UpdateVertexData()
{
  glNamedBufferData(buf_id, sizeof(float) * vertex_data.size(), vertex_data.data(), GL_STATIC_DRAW);
}


void Renderer::SetupDynamicVertexData()
{
  dynamic_buff_id = GL::CreateBuffers();
  dynamic_vao_id = GL::CreateVertexArrays();

  int buffer_index = 0;
  glVertexArrayVertexBuffer(dynamic_vao_id, buffer_index, dynamic_buff_id, 0, stride);

  int attrib_id = 0;
  GL::AttachAttribute(dynamic_vao_id, attrib_id, attribs_per_vertex, GL_FLOAT);

  DynamicLine({0,0}, {10,10});
  UpdateDynamicVertexData();
}


void Renderer::ClearDynamicVertexData()
{
  dynamic_vertex_data.clear();
}


void Renderer::DynamicLine(vec2 const &v1, vec2 const &v2)
{
  dynamic_vertex_data.push_back(v1.x);
  dynamic_vertex_data.push_back(v1.y);

  dynamic_vertex_data.push_back(v2.x);
  dynamic_vertex_data.push_back(v2.y);
}


void Renderer::UpdateDynamicVertexData()
{
  glNamedBufferData(dynamic_buff_id, sizeof(float) * dynamic_vertex_data.size(), dynamic_vertex_data.data(), GL_DYNAMIC_DRAW);
}


void Renderer::DrawDynamic(GLenum draw_type)
{
  UseProgram(basic_shader.GetProgramId());
  UseVAO(dynamic_vao_id);

  basic_shader.SetOffset(0.0f, 0.0f);
  basic_shader.SetRotation(0.0f);
  basic_shader.SetZoom(1.0f);

  glDrawArrays(draw_type, 0, dynamic_vertex_data.size() / attribs_per_vertex);
}


void Renderer::SetupShapes()
{
  circle_shape = AddShape(MakeCircle(1, 64));

  arrow_shape = AddShape(MakeArrow(20));

  SetupVertexData();
  UpdateVertexData();
}


void Renderer::DeleteShapes()
{
  //TODO
  //glDeleteBuffers(fbo);
  //glDeleteFBO(fbo);
}


void Renderer::DrawShape(GLenum draw_type, shape_def const & shape)
{
  glDrawArrays(draw_type, shape.offset, shape.count);
}


void Renderer::DrawCircle(int radius, float x, float y)
{
  UseProgram(basic_shader.GetProgramId());
  UseVAO(vao_id);

  basic_shader.SetOffset(x, y);
  basic_shader.SetZoom(radius);

  glDrawArrays(GL_LINE_LOOP, circle_shape.offset, circle_shape.count);
}


void Renderer::FillCircle(int radius, float x, float y)
{
  UseProgram(basic_shader.GetProgramId());
  UseVAO(vao_id);

  basic_shader.SetOffset(x, y);
  basic_shader.SetZoom(radius);

  glDrawArrays(GL_TRIANGLE_FAN, circle_shape.offset, circle_shape.count);
}


void Renderer::RenderBall(const Ball & ball, bool draw_outline)
{
  const auto & radius = ball.radius;

  UseProgram(basic_shader.GetProgramId());
  UseVAO(vao_id);

  basic_shader.SetOffset(ball.position);
  basic_shader.SetRotation(ball.rot);
  basic_shader.SetZoom(radius);

  basic_shader.SetColour(ball.colour.r, ball.colour.g, ball.colour.b, ball.colour.a * 0.3f);

  glDrawArrays(GL_TRIANGLE_FAN, circle_shape.offset, circle_shape.count);

  if (draw_outline)
  {
    float bright = 1.3f;
    vec4 colour_bright = ball.colour * bright;
    basic_shader.SetColour(colour_bright);

    glDrawArrays(GL_LINE_LOOP, circle_shape.offset, circle_shape.count);
  }
}


void Renderer::RenderArrow(const Ball & arrow)
{
  basic_shader.SetOffset(arrow.position.x, arrow.position.y);
  basic_shader.SetRotation(arrow.rot);
  basic_shader.SetColour(arrow.colour);
  basic_shader.SetZoom(1.0f);

  DrawShape(GL_LINE_LOOP, arrow_shape);
}


shape_def Renderer::GetRectShape(int w, int h)
{
  auto shape = rect_shapes[w][h];
  if (shape.offset == 0)
  {
    shape = AddShape(MakeRect(w, h));
    UpdateVertexData();
    rect_shapes[w][h] = shape;
  }

  return shape;
}


void Renderer::RenderRect(const Rect & rect, bool draw_outline)
{
  auto shape = GetRectShape(rect.width, rect.height);

  basic_shader.SetOffset(rect.position);
  basic_shader.SetRotation(0.0f);
  basic_shader.SetColour(rect.colour.r, rect.colour.g, rect.colour.b, 0.3f);
  basic_shader.SetZoom(1.0f);

  DrawShape(GL_TRIANGLE_FAN, shape);


  if (draw_outline)
  {
    basic_shader.SetColour(rect.colour);
    DrawShape(GL_LINE_LOOP, shape);
  }

  // DrawCircle(5, rect.position.x, rect.position.y);
  // FillCircle(5, rect.position.x + rect.width, rect.position.y + rect.height);
}


void Renderer::DrawGameState(const Game & game)
{
  const bool draw_normals = false;

  EnableBlend();
  glLineWidth(2.0f);

  UseProgram(basic_shader.GetProgramId());
  UseVAO(vao_id);

  for(const auto &ball : game.balls)
  {
    bool collides = game.Collides_Any(ball);

    RenderBall(ball, collides);
  }

  for(const auto &rect : game.rects)
  {
      bool collides = game.Collides_Any(rect);

      RenderRect(rect, collides);
  }

  RenderArrow(game.player);

  RenderBall(game.mouse_pointer, game.Collides_Any(game.mouse_pointer));

  basic_shader.SetColour(1.0f, 1.0f, 1.0f, 1.0f);


  ClearDynamicVertexData();

  for (const auto &vec : {game.lines, game.border_lines})
  {
    for(const auto &line : vec)
    {
      DynamicLine(line.p1, line.p2);

      if (draw_normals)
      {
        vec2 normal = get_normal(line.p1, line.p2);
        vec2 center = (line.p1 + line.p2) / 2.0f;
        DynamicLine(center, center+ (normal * 20.0f));
      }
    }
  }

  if (draw_normals)
  {
    for (const auto& ball : game.balls)
    {
      vec2 dir = (ball.velocity);
      DynamicLine(ball.position, ball.position + dir * 0.5f);
    }
  }

  UpdateDynamicVertexData();
  DrawDynamic(GL_LINES);

}
