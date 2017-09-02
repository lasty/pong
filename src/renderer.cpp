
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
  glLineWidth(2.0f);

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
  glLineWidth(2.0f);

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


void Renderer::RenderBlock(const Block & block)
{
  //TODO update this for blocks
  RenderBounds(block.bounds);

  //const auto & vec = game.GetGeometry(block.type);

  for (auto line : block.geometry)
  {
    const auto p1 = line.p1;// + block.position;
    const auto p2 = line.p2;// + block.position;

    DynamicLine(p1, p2);

    bool draw_normals = true;
    if (draw_normals)
    {
      vec2 normal = get_normal(p1, p2);
      vec2 center = (p1 + p2) / 2.0f;
      DynamicLine(center, center + (normal * 4.0f));
    }
  }

}


void Renderer::RenderBounds(const BoundingBox & bounds, [[maybe_unused]] bool draw_outline)
{
  glLineWidth(1.0f);
  const vec2 size = bounds.bottom_right - bounds.top_left;
  auto shape = GetRectShape(size.x, size.y);

  basic_shader.SetOffset(bounds.top_left);
  basic_shader.SetRotation(0.0f);
  basic_shader.SetColour(0.9f, 0.1f, 0.9f, 0.05f);
  basic_shader.SetZoom(1.0f);

  DrawShape(GL_TRIANGLE_FAN, shape);

  if (draw_outline)
  {
    basic_shader.SetColour(0.9f, 0.1f, 0.9f, 0.9f);
    DrawShape(GL_LINE_LOOP, shape);
  }

}



void Renderer::DrawGameState(const Game & game)
{
  const bool draw_normals = true;
  const bool draw_velocity = true;
  const bool draw_bounds = true;

  EnableBlend();

  UseProgram(basic_shader.GetProgramId());
  UseVAO(vao_id);


  for(const auto &ball : game.balls)
  {
    // bool collides = game.Collides_Any(ball);

    if (draw_bounds) RenderBounds(ball.bounds, true);

    RenderBall(ball, true); //collides);
  }

  ClearDynamicVertexData();

  for(const auto &block : game.blocks)
  {
      //bool collides = game.Collides_Any(block);

      if (draw_bounds) RenderBounds(block.bounds, true);

      RenderBlock(block);
  }


  RenderArrow(game.player);
  if (draw_bounds) RenderBounds(game.player.bounds, true);

  RenderBall(game.mouse_pointer, true);//game.Collides_Any(game.mouse_pointer));
  if (draw_bounds) RenderBounds(game.mouse_pointer.bounds, true);

  basic_shader.SetColour(1.0f, 1.0f, 1.0f, 1.0f);


  for (const auto &vec : {game.border_lines})
  {
    for(const auto &block : vec)
    {
      for(const auto &line : block.geometry)
      {
        DynamicLine(line.p1, line.p2);
        if (draw_bounds) RenderBounds(line.bounds, true);

        if (draw_normals)
        {
          vec2 normal = get_normal(line.p1, line.p2);
          vec2 center = (line.p1 + line.p2) / 2.0f;
          DynamicLine(center, center+ (normal * 20.0f));
        }
      }
    }
  }


  if (draw_velocity)
  {
    for (const auto& ball : game.balls)
    {
      vec2 dir = (ball.velocity);
      DynamicLine(ball.position, ball.position + dir * 1.0f);
    }
  }


  UpdateDynamicVertexData();

  basic_shader.SetColour(1.0f, 1.0f, 1.0f, 1.0f);
  glLineWidth(2.0f);

  DrawDynamic(GL_LINES);

}


void Renderer::DrawTestCase(Game &game, [[maybe_unused]] float seed, int step)
{
  srand(2);

  Ball b;
  b.radius = 30;
  b.position = {40, 95};
  b.velocity = {30, 21};
  b.colour = {0.5, 0.5, 0.5, 1.0};
  b.rot = 0.0f;
  b.bounds = MakeBounds(b);

  BorderLine l;
  l.p2 = {50, 115};
  l.p1 = {300, 15};
  l.bounds = MakeBounds(l);

  //Game game { xxx.width, xxx.height };

  game.blocks.clear();

  game.blocks.push_back(game.NewBlock(1, 2, BlockType::rect_triangle_left));

  //game.border_lines.erase(game.border_lines.begin() + 1, game.border_lines.end());
  //game.border_lines.push_back(l);

  game.balls.clear();
  game.balls.push_back(b);

  game.player.position = {-100, -100};
  game.player.bounds = MakeBounds(game.player);

  game.mouse_pointer.position = {-100, -100};
  game.mouse_pointer.bounds = MakeBounds(game.mouse_pointer);


  TRACE << "step " << step << "  ";

  for (int i=0; i<step; i++)
  {
    float dt = 1.0f;
    game.Update(dt);
  }



  DrawGameState(game);



  vec2 ofs { 400.0f, 300.0f };

  //outside triangle
  //auto l1 = BorderLine( {100.0f, 0.0f}, {0.0f, 0.0f});
  //auto l2 = BorderLine( {0.0f, 0.0f}, {50.0f, 50.0f});


  //outside square
  // auto l1 = BorderLine( {100.0f, 0.0f}, {0.0f, 0.0f});
  // auto l2 = BorderLine( {0.0f, 0.0f}, {0.0f, 100.0f});

  //inside square
  auto l1 = BorderLine( {0.0f, -100.0f}, {0.0f, 0.0f});
  auto l2 = BorderLine( {0.0f, 0.0f}, {-100.0f, 0.0f});


  auto n1 = get_normal(l1.p1, l1.p2);
  auto n2 = get_normal(l2.p1, l2.p2);

  auto ln1 = BorderLine( {0.0f,0.0f} , n1 * 10.0f);
  auto ln2 = BorderLine( {0.0f,0.0f} , n2 * 10.0f);

  auto avgnorm = normalize((n1 + n2) / 2.0f);

  vec2 ofs2 { -20.0f, -20.0f };
  auto avgnorm_l = BorderLine( ofs2, ofs2 + (avgnorm * 10.0f) );

  for(const auto &line : {l1, l2, ln1, ln2, avgnorm_l})
  {
    DynamicLine(line.p1 + ofs, line.p2 + ofs);

    if (distance(line.p1, line.p2) > 50) //(draw_normals)
    {
      vec2 normal = get_normal(line.p1, line.p2);
      vec2 center = (line.p1 + line.p2) / 2.0f;
      DynamicLine(center + ofs, center + (normal * 20.0f) + ofs);
    }
  }

  UpdateDynamicVertexData();
  DrawDynamic(GL_LINES);

  /*
  EnableBlend();
  UseProgram(basic_shader.GetProgramId());
  UseVAO(vao_id);
  ClearDynamicVertexData();


  RenderBall(b, true);





  DynamicLine(l.p1, l.p2);

  if (draw_normals)
  {
    vec2 normal = get_normal(l.p1, l.p2);
    vec2 center = (l.p1 + l.p2) / 2.0f;
    DynamicLine(center, center+ (normal * 20.0f));
  }



  UpdateDynamicVertexData();
  basic_shader.SetColour(1.0f, 1.0f, 1.0f, 1.0f);
  glLineWidth(2.0f);
  DrawDynamic(GL_LINES);
  */


}
