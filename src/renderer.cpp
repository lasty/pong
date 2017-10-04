
#include "renderer.hpp"

#include <vector>

#include "maths.hpp"
#include "game.hpp"
#include "gl.hpp"
#include "maths_utils.hpp"


VertexData::VertexData(GLenum usage)
: usage(usage)
{
  buffer_id = GL::CreateBuffers();
  vao_id = GL::CreateVertexArrays();

  int buffer_index = 0;
  glVertexArrayVertexBuffer(vao_id, buffer_index, buffer_id, 0, stride);

  GL::AttachAttribute(vao_id, 0, 2, 0, GL_FLOAT); //position location = 0
  GL::AttachAttribute(vao_id, 1, 4, 2, GL_FLOAT); //colour, location = 1
}


VertexData::~VertexData()
{
  GL::DetachAttribute(vao_id, 0); //position location = 0
  GL::DetachAttribute(vao_id, 1); //colour, location = 1

  GL::DeleteBuffers(buffer_id);

  glBindVertexArray(0);
  GL::DeleteVertexArrays(vao_id);
}


void VertexData::Clear()
{
  vertex_data.clear();
}


shape_def VertexData::AddShape(std::vector<float> const &vertexes)
{
  if (vertexes.size() % floats_per_vertex != 0)
  {
    throw std::runtime_error("Uneven vertexes given to AddShape");
  }

  shape_def s;
  s.offset = GetOffset();
  s.count = vertexes.size() / floats_per_vertex;

  vertex_data.insert(vertex_data.end(), vertexes.begin(), vertexes.end());

  return s;
}


void VertexData::AddVertex(const vec2 &position, const vec4 &colour)
{
  vertex_data.push_back(position.x);
  vertex_data.push_back(position.y);
  vertex_data.push_back(colour.r);
  vertex_data.push_back(colour.g);
  vertex_data.push_back(colour.b);
  vertex_data.push_back(colour.a);
}


void VertexData::UpdateVertexes()
{
  glNamedBufferData(buffer_id, sizeof(float) * vertex_data.size(), vertex_data.data(), usage);
}


int VertexData::GetOffset() const
{
  return vertex_data.size() / floats_per_vertex;
}


int VertexData::GetNumVertexes() const
{
  return vertex_data.size() / floats_per_vertex;
}


int VertexData::GetVAO() const
{
  return vao_id;
}


std::vector<float> MakeCircle(float radius, int segments, const vec4 &colour)
{
  std::vector<float> out;

  for (int i = 0; i < segments; i++)
  {
    float angle = float(2 * PI) * (float(i) / float(segments));

    float x = radius * cosf(angle);
    float y = radius * sinf(angle);

    out.push_back(x);
    out.push_back(y);

    out.push_back(colour.r);
    out.push_back(colour.g);
    out.push_back(colour.b);
    out.push_back(colour.a);
  }

  return out;
}


std::vector<float> MakeArrow(float radius, const vec4 &colour)
{
  std::vector<float> out;

  float width = (PI / 8);
  for (float angle : {0.0f, PI - width, PI + width})
  {
    float x = radius * cosf(angle);
    float y = radius * sinf(angle);

    out.push_back(x);
    out.push_back(y);

    out.push_back(colour.r);
    out.push_back(colour.g);
    out.push_back(colour.b);
    out.push_back(colour.a);
  }

  return out;
}


std::vector<float> MakeRect(float width, float height, vec4 colour)
{
  std::vector<float> out;

  out.push_back(0);
  out.push_back(0);
  out.push_back(colour.r);
  out.push_back(colour.g);
  out.push_back(colour.b);
  out.push_back(colour.a);

  out.push_back(width);
  out.push_back(0);
  out.push_back(colour.r);
  out.push_back(colour.g);
  out.push_back(colour.b);
  out.push_back(colour.a);

  out.push_back(width);
  out.push_back(height);
  out.push_back(colour.r);
  out.push_back(colour.g);
  out.push_back(colour.b);
  out.push_back(colour.a);

  out.push_back(0);
  out.push_back(height);
  out.push_back(colour.r);
  out.push_back(colour.g);
  out.push_back(colour.b);
  out.push_back(colour.a);

  return out;
}


Renderer::Renderer()
: shapes_data(GL_STATIC_DRAW)
, lines_data(GL_DYNAMIC_DRAW)
, particle_data(GL_DYNAMIC_DRAW)
{
  SetupShapes();
}


Renderer::~Renderer()
{
}


void Renderer::UseProgram(int program_id)
{
  if (gl_state.program == program_id) return;

  gl_state.program = program_id;
  glUseProgram(program_id);
}


void Renderer::UseVAO(int vao_id)
{
  if (gl_state.vao == vao_id) return;

  gl_state.vao = vao_id;
  glBindVertexArray(vao_id);
}


void Renderer::EnableBlend()
{
  if (gl_state.blending) return;
  gl_state.blending = true;

  glEnable(GL_BLEND);
  //glEnablei(GL_BLEND, 0);
  glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
  glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
}


void Renderer::DisableBlend()
{
  if (gl_state.blending == false) return;
  gl_state.blending = false;

  glDisable(GL_BLEND);
}


void Renderer::Resize(int width, int height)
{
  basic_shader.SetResolution(width, height);
}


void Renderer::DynamicLine(vec2 const &v1, vec2 const &v2, const vec4 &colour)
{
  lines_data.AddVertex(v1, colour);
  lines_data.AddVertex(v2, colour);
}


void Renderer::DrawVertexData(GLenum draw_type, const VertexData &vertex_data)
{
  UseProgram(basic_shader.GetProgramId());
  UseVAO(vertex_data.GetVAO());

  basic_shader.SetOffset(0.0f, 0.0f);
  basic_shader.SetRotation(0.0f);
  basic_shader.SetZoom(1.0f);
  basic_shader.SetColour(1.0f, 1.0f, 1.0f, 1.0f);

  glDrawArrays(draw_type, 0, vertex_data.GetNumVertexes());
}


void Renderer::SetupShapes()
{
  vec4 colour{1.0f, 1.0f, 1.0f, 1.0f};
  circle_shape = shapes_data.AddShape(MakeCircle(1, 64, colour));

  arrow_shape = shapes_data.AddShape(MakeArrow(20, colour));

  // SetupVertexData();
  shapes_data.UpdateVertexes();
}


void Renderer::DrawShape(GLenum draw_type, shape_def const &shape)
{
  glDrawArrays(draw_type, shape.offset, shape.count);
}


void Renderer::DrawCircle(int radius, float x, float y)
{
  basic_shader.SetOffset(x, y);
  basic_shader.SetZoom(radius);

  glDrawArrays(GL_LINE_LOOP, circle_shape.offset, circle_shape.count);
}


void Renderer::FillCircle(int radius, float x, float y)
{
  basic_shader.SetOffset(x, y);
  basic_shader.SetZoom(radius);

  glDrawArrays(GL_TRIANGLE_FAN, circle_shape.offset, circle_shape.count);
}


void Renderer::RenderBall(const Ball &ball, bool draw_outline)
{
  glLineWidth(2.0f);

  const auto &radius = ball.radius;

  basic_shader.SetOffset(ball.position);
  basic_shader.SetRotation(0.0f);
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


void Renderer::RenderArrow(const vec2 &position, float rot)
{
  glLineWidth(2.0f);

  basic_shader.SetOffset(position);
  basic_shader.SetRotation(rot);
  basic_shader.SetZoom(1.0f);

  DrawShape(GL_LINE_LOOP, arrow_shape);
}


shape_def Renderer::GetRectShape(int w, int h)
{
  auto shape = rect_shapes[w][h];
  if (shape.offset == 0)
  {
    vec4 colour{1.0f, 1.0f, 1.0f, 1.0f};
    shape = shapes_data.AddShape(MakeRect(w, h, colour));
    shapes_data.UpdateVertexes();
    rect_shapes[w][h] = shape;
  }

  return shape;
}


void Renderer::RenderBlock(const Block &block, bool draw_normals)
{
  for (auto line : block.geometry)
  {
    const auto p1 = line.p1; // + block.position;
    const auto p2 = line.p2; // + block.position;

    DynamicLine(p1, p2, block.colour);

    if (draw_normals)
    {
      vec2 normal = get_normal(p1, p2);
      vec2 center = (p1 + p2) / 2.0f;
      DynamicLine(center, center + (normal * 4.0f), vec4{1.0f, 1.0f, 1.0f, 1.0f});
    }
  }
}


void Renderer::RenderBounds(const BoundingBox &bounds)
{
  glLineWidth(1.0f);
  const vec2 size = bounds.bottom_right - bounds.top_left;
  auto shape = GetRectShape(size.x, size.y);

  basic_shader.SetOffset(bounds.top_left);
  basic_shader.SetRotation(0.0f);
  basic_shader.SetColour(0.9f, 0.1f, 0.9f, 0.05f);
  basic_shader.SetZoom(1.0f);

  DrawShape(GL_TRIANGLE_FAN, shape);

  bool draw_outline = true;
  if (draw_outline)
  {
    basic_shader.SetColour(0.9f, 0.1f, 0.9f, 0.9f);
    DrawShape(GL_LINE_LOOP, shape);
  }
}


void Renderer::DrawGameState(const GameState &state)
{
  const bool draw_normals = state.debug_enabled;
  const bool draw_velocity = state.debug_enabled;
  const bool draw_bounds = state.debug_enabled;

  EnableBlend();

  lines_data.Clear();

  UseProgram(basic_shader.GetProgramId());
  UseVAO(shapes_data.GetVAO());


  for (const auto &ball : state.balls)
  {
    if (draw_bounds) RenderBounds(ball.bounds);

    RenderBall(ball, true);
  }

  for (const auto &block : state.blocks)
  {
    if (draw_bounds) RenderBounds(block.bounds);

    RenderBlock(block, draw_normals);
  }

  if (state.particles.size())
  {
    particle_data.Clear();
    particle_data.AddShape(MakeParticleVertexes(state.particles));
    particle_data.UpdateVertexes();

    DrawVertexData(GL_TRIANGLES, particle_data);
  }


  RenderBlock(state.player.block, draw_normals);
  if (draw_bounds) RenderBounds(state.player.block.bounds);
  if (state.player.sticky_ball)
  {
    vec2 pos = state.player.block.position + state.player.sticky_ball_offset;
    DrawCircle(10.0f, pos.x, pos.y);
  }


  if (draw_bounds)
  {
    DrawCircle(10.0f, state.mouse_pointer.x, state.mouse_pointer.y);
  }


  basic_shader.SetColour(1.0f, 1.0f, 1.0f, 1.0f);


  for (const auto &vec : {state.border_lines})
  {
    for (const auto &block : vec)
    {
      for (const auto &line : block.geometry)
      {
        DynamicLine(line.p1, line.p2, block.colour);

        if (draw_normals)
        {
          vec2 normal = get_normal(line.p1, line.p2);
          vec2 center = (line.p1 + line.p2) / 2.0f;
          DynamicLine(center, center + (normal * 20.0f), vec4{1.0f, 1.0f, 1.0f, 1.0f});
        }
      }
    }
  }


  if (draw_velocity)
  {
    for (const auto &ball : state.balls)
    {
      vec2 dir = (ball.velocity);
      DynamicLine(ball.position, ball.position + dir * 1.0f, vec4{1.0f, 1.0f, 1.0f, 1.0f});
    }
  }


  lines_data.UpdateVertexes();

  basic_shader.SetColour(1.0f, 1.0f, 1.0f, 1.0f);
  glLineWidth(2.0f);

  DrawVertexData(GL_LINES, lines_data);
}
