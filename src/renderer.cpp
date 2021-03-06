
#include "renderer.hpp"

#include <vector>

#include "game.hpp"
#include "gl.hpp"
#include "maths.hpp"
#include "to_string.hpp"


VertexData::VertexData(GLenum usage)
: usage(usage)
{
  buffer_id = GL::CreateBuffers();
  vao_id = GL::CreateVertexArrays();

#if OLD_OPENGL
  glBindVertexArray(vao_id);
  glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
#else
  int buffer_index = 0;
  glVertexArrayVertexBuffer(vao_id, buffer_index, buffer_id, 0, stride);
#endif

  AttachAttribute(0, 2, 0, GL_FLOAT); //position location = 0
  AttachAttribute(1, 4, 2, GL_FLOAT); //colour, location = 1

#if OLD_OPENGL
#endif
}


VertexData::~VertexData()
{
#if OLD_OPENGL
  glBindVertexArray(vao_id);
#endif

  DetachAttribute(0); //position location = 0
  DetachAttribute(1); //colour, location = 1

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

  shape_def s{};
  s.offset = GetOffset();
  s.count = vertexes.size() / floats_per_vertex;

  vertex_data.insert(vertex_data.end(), vertexes.begin(), vertexes.end());

  return s;
}


void VertexData::AddVertex(const vec2 &position, const col4 &colour)
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
#if OLD_OPENGL
  glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_data.size(), vertex_data.data(), usage);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
#else
  glNamedBufferData(buffer_id, sizeof(float) * vertex_data.size(), vertex_data.data(), usage);
#endif
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


void VertexData::AttachAttribute(int attrib_id, int size, int offset, GLenum type)
{
#if OLD_OPENGL
  const GLvoid *offset_ptr = reinterpret_cast<GLvoid *>(offset * sizeof(float));
  // glBindVertexArray(vao_id);
  // glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
  glVertexAttribPointer(attrib_id, size, type, GL_FALSE, stride, offset_ptr);
  glEnableVertexAttribArray(attrib_id);
// glBindBuffer(GL_ARRAY_BUFFER, 0);
// glBindVertexArray(0);
#else
  constexpr int buffer_index = 0;
  const int relative_offset = offset * sizeof(float);
  glEnableVertexArrayAttrib(vao_id, attrib_id);
  glVertexArrayAttribFormat(vao_id, attrib_id, size, type, GL_FALSE, relative_offset);
  glVertexArrayAttribBinding(vao_id, attrib_id, buffer_index);
#endif
}


void VertexData::DetachAttribute(int attrib_id)
{
#if OLD_OPENGL
  // glBindVertexArray(vao_id);
  glDisableVertexAttribArray(attrib_id);
// glBindVertexArray(0);
#else
  glDisableVertexArrayAttrib(vao_id, attrib_id);
#endif
}


std::vector<float> MakeCircle(float radius, int segments, const col4 &colour)
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


std::vector<float> MakeArrow(float radius, const col4 &colour)
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


std::vector<float> MakeRect(float width, float height, col4 colour)
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
, circle_shape{}
, arrow_shape{}
, lines_data(GL_DYNAMIC_DRAW)
, particle_data(GL_DYNAMIC_DRAW)
{
  SetupShapes();

#if OLD_OPENGL
  GLenum err = glGetError();
  if (err != GL_NO_ERROR)
  {
    std::cout << "Renderer constructor: GL Error: " << err << std::endl;
  }
#endif
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


void Renderer::DynamicLine(vec2 const &v1, vec2 const &v2, const col4 &colour)
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
  col4 colour{1.0f, 1.0f, 1.0f, 1.0f};
  circle_shape = shapes_data.AddShape(MakeCircle(1, 64, colour));

  arrow_shape = shapes_data.AddShape(MakeArrow(20, colour));

  SetupBlockShapes();

  shapes_data.UpdateVertexes();
}


void AddVertex(std::vector<float> &vec, const vec2 &pos, const col4 &col)
{
  vec.push_back(pos.x);
  vec.push_back(pos.y);

  vec.push_back(col.r);
  vec.push_back(col.g);
  vec.push_back(col.b);
  vec.push_back(col.a);
}

void AddTri(std::vector<float> &vec, const vec2 &pos1, const vec2 &pos2, const vec2 &pos3, const col4 &col)
{
  AddVertex(vec, pos1, col);
  AddVertex(vec, pos2, col);
  AddVertex(vec, pos3, col);
}


void Renderer::SetupBlockShapes()
{
  vec2 tl{0, 0};
  vec2 bl{0, 50};
  vec2 tr{50, 0};
  vec2 br{50, 50};
  vec2 wtr{100, 0};
  vec2 wbr{100, 50};

  // col4 col1{1.0f, 1.0f, 1.0f, 1.0f};
  col4 col1{0.7f, 0.7f, 0.7f, 0.7f};

  std::vector<float> vec;

  vec.clear();
  AddTri(vec, bl, tr, tl, col1);
  AddTri(vec, br, tr, bl, col1);
  block_shapes.emplace(BlockType::square, shapes_data.AddShape(vec));

  vec.clear();
  AddTri(vec, tr, tl, br, col1);
  block_shapes.emplace(BlockType::triangle_left, shapes_data.AddShape(vec));

  vec.clear();
  AddTri(vec, tr, tl, bl, col1);
  block_shapes.emplace(BlockType::triangle_right, shapes_data.AddShape(vec));

  vec.clear();
  AddTri(vec, bl, wtr, tl, col1);
  AddTri(vec, wbr, wtr, bl, col1);
  block_shapes.emplace(BlockType::rectangle, shapes_data.AddShape(vec));

  vec.clear();
  AddTri(vec, wtr, tl, br, col1);
  AddTri(vec, wtr, br, wbr, col1);
  block_shapes.emplace(BlockType::rect_triangle_left, shapes_data.AddShape(vec));

  vec.clear();
  AddTri(vec, tl, bl, br, col1);
  AddTri(vec, tl, br, wtr, col1);
  block_shapes.emplace(BlockType::rect_triangle_right, shapes_data.AddShape(vec));


  vec2 ptl{-50, 0};
  vec2 ptr{50, 0};
  vec2 pbl{-50, 40};
  vec2 pbr{50, 40};

  vec.clear();
  AddTri(vec, ptl, pbl, ptr, col1);
  AddTri(vec, ptr, pbl, pbr, col1);

  block_shapes.emplace(BlockType::paddle, shapes_data.AddShape(vec));
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
  //glLineWidth(2.0f);

  const auto &radius = ball.radius;

  basic_shader.SetOffset(ball.position);
  basic_shader.SetRotation(0.0f);
  basic_shader.SetZoom(radius);

  basic_shader.SetColour(ball.colour.r, ball.colour.g, ball.colour.b, ball.colour.a * 0.3f);

  glDrawArrays(GL_TRIANGLE_FAN, circle_shape.offset, circle_shape.count);

  if (draw_outline)
  {
    float bright = 1.3f;
    col4 colour_bright = ball.colour * bright;
    basic_shader.SetColour(colour_bright);

    glDrawArrays(GL_LINE_LOOP, circle_shape.offset, circle_shape.count);
  }
}


void Renderer::RenderArrow(const vec2 &position, float rot)
{
  //glLineWidth(2.0f);

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
    col4 colour{1.0f, 1.0f, 1.0f, 1.0f};
    shape = shapes_data.AddShape(MakeRect(w, h, colour));
    shapes_data.UpdateVertexes();
    rect_shapes[w][h] = shape;
  }

  return shape;
}


void Renderer::RenderBlock(const Block &block, bool draw_normals)
{
  auto shape = block_shapes[block.type];

  basic_shader.SetOffset(block.position);
  basic_shader.SetColour(block.colour);
  basic_shader.SetRotation(0.0f);
  basic_shader.SetZoom(1.0f);

  if (shape.offset != 0 and shape.count != 0)
    DrawShape(GL_TRIANGLES, shape);


  if (true)
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
        DynamicLine(center, center + (normal * 4.0f), col4{1.0f, 1.0f, 1.0f, 1.0f});
      }
    }
  }
}


void Renderer::RenderBounds(const BoundingBox &bounds)
{
  //glLineWidth(1.0f);
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

void Renderer::RenderMenu(const GameState &state)
{
  const auto &items = state.menu_items;
  const auto &selected = state.selected_menu_item;


  lines_data.Clear();

  UseProgram(basic_shader.GetProgramId());
  UseVAO(shapes_data.GetVAO());


  for (int i = 0; i < static_cast<int>(items.size()); i++)
  {
    const auto &str = items.at(i);
    vec2 pos = {100.0f, 100.0f + (30.0f * i)};
    col4 col{1.0f, 1.0f, 0.7f, 1.0f};

    lines_data.AddShape(text.MakeString(str, pos, col));

    if (i == selected)
    {
      pos.x -= 30.0f;
      lines_data.AddShape(text.MakeString(">", pos, col));
      pos.x += 45.0f + (15 * str.size());
      lines_data.AddShape(text.MakeString("<", pos, col));
    }
  }

  lines_data.UpdateVertexes();
  basic_shader.SetColour(1.0f, 1.0f, 1.0f, 1.0f);
  DrawVertexData(GL_LINES, lines_data);
}


void Renderer::RenderGame(const GameState &state)
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

  TRACE << "Player.block.type = " << static_cast<int>(state.player.block.type) << "  ";

  RenderBlock(state.player.block, draw_normals);
  if (draw_bounds) RenderBounds(state.player.block.bounds);
  if (state.player.sticky_ball)
  {
    vec2 pos = state.player.block.position + state.player.sticky_ball_offset;
    DrawCircle(10.0f, pos.x, pos.y);
  }


  if (not state.particles.empty())
  {
    particle_data.Clear();
    particle_data.AddShape(MakeParticleVertexes(state.particles));
    particle_data.UpdateVertexes();

    DrawVertexData(GL_TRIANGLES, particle_data);
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
          DynamicLine(center, center + (normal * 20.0f), col4{1.0f, 1.0f, 1.0f, 1.0f});
        }
      }
    }
  }


  if (draw_velocity)
  {
    for (const auto &ball : state.balls)
    {
      vec2 dir = (ball.velocity);
      DynamicLine(ball.position, ball.position + dir * 1.0f, col4{1.0f, 1.0f, 1.0f, 1.0f});
    }
  }

  //Draw HUD text
  col4 col{1.0f, 1.0f, 0.7f, 1.0f};

  std::ostringstream status; //, status2, status3;
  status << "Balls: " << state.balls.size() << "                       "
         << "Blocks: " << state.blocks.size();
  //status3 << "Particles: " << state.particles.size();

  lines_data.AddShape(text.MakeString(status.str(), vec2{10.0f, 10.0f}, col));
  // lines_data.AddShape(text.MakeString(status2.str(), vec2{10.0f, 40.0f}, col));
  // lines_data.AddShape(text.MakeString(status3.str(), vec2{10.0f, 70.0f}, col));

  // lines_data.AddShape(text.MakeString("The Quick Brown Fox Jumps", vec2{10.0f, 400.0f}, col));
  // lines_data.AddShape(text.MakeString("Over The Lazy Dog.", vec2{10.0f, 430.0f}, col));


  lines_data.UpdateVertexes();
  basic_shader.SetColour(1.0f, 1.0f, 1.0f, 1.0f);
  DrawVertexData(GL_LINES, lines_data);
}


void Renderer::DrawGameState(const GameState &state)
{
  if (state.state == State::main_menu or state.state == State::pause_menu)
  {
    RenderMenu(state);
  }
  else
  {
    RenderGame(state);
  }


  UseProgram(0);
  UseVAO(0);


#if OLD_OPENGL
  GLenum err = glGetError();
  if (err != GL_NO_ERROR)
  {
    std::cout << "DrawGameState() - GL Error: " << err << std::endl;
  }
#endif
}
