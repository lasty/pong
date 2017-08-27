
#include "renderer.hpp"

#include <vector>

#include "maths.hpp"

#include "game.hpp"

#include "gl.hpp"


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
  if (vertexes.size() % 2 != 0)
    { throw std::runtime_error("Uneven vertexes given to AddShape"); }


  shape_def s;
  s.offset = (vertex_data.size() / 2);
  s.count = vertexes.size() / 2;

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


void Renderer::SetupShapes()
{
  circle_shape = AddShape(MakeCircle(1, 64));

  arrow_shape = AddShape(MakeArrow(20));

  SetupVertexData();
  UpdateVertexData();
}


void Renderer::DeleteShapes()
{
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
  UseProgram(basic_shader.GetProgramId());
  UseVAO(vao_id);

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
  basic_shader.SetColour(rect.colour);
  basic_shader.SetZoom(1.0f);

  DrawShape(GL_LINE_LOOP, shape);

  if (draw_outline)
  {
    DrawShape(GL_TRIANGLE_FAN, shape);
  }

  DrawCircle(5, rect.position.x, rect.position.y);
  FillCircle(5, rect.position.x + rect.width, rect.position.y + rect.height);
}


void Renderer::DrawGameState(const Game & game)
{
  EnableBlend();
  glLineWidth(2.0f);

  for(const auto &ball : game.balls)
  {
      RenderBall(ball, game.Collides_Any(ball));
  }

  for(const auto &rect : game.rects)
  {
      RenderRect(rect, game.Collides(rect, game.mouse_pointer.position));
  }

  RenderArrow(game.player);

  RenderBall(game.mouse_pointer);

  basic_shader.SetColour(1.0f, 1.0f, 1.0f, 1.0f);

}
