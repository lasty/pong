
#include "shader.hpp"

#include <string>
#include <stdexcept>

#include "gl.hpp"

#include "maths.hpp"


namespace Shader {


Basic::Basic()
{
  vertex_shader_id = GL::CreateShader(GL_VERTEX_SHADER, vertex_src);
  fragment_shader_id = GL::CreateShader(GL_FRAGMENT_SHADER, fragment_src);

  program_id = glCreateProgram();

  glAttachShader(program_id, vertex_shader_id);
  glAttachShader(program_id, fragment_shader_id);

  GL::LinkProgram(program_id);

  uniforms.screen_resolution = glGetUniformLocation(program_id, "screen_resolution");
  uniforms.offset = glGetUniformLocation(program_id, "offset");
  uniforms.rotation = glGetUniformLocation(program_id, "rotation");
  uniforms.colour = glGetUniformLocation(program_id, "colour");
  uniforms.zoom = glGetUniformLocation(program_id, "zoom");

  for (auto& u :
    {uniforms.screen_resolution, uniforms.offset, uniforms.rotation, uniforms.colour, uniforms.zoom})
  {
    if (u == -1) throw std::runtime_error("uniform is not valid");
  }

  //Set some sane defaults
  SetResolution(640, 480);
  SetColour(1.0f, 1.0f, 1.0f, 1.0f);
  SetOffset(0.0f, 0.0f);
  SetRotation(0.0f);
  SetZoom(1.0f);
}


Basic::~Basic()
{
  glDetachShader(program_id, vertex_shader_id);
  glDetachShader(program_id, fragment_shader_id);

  glDeleteShader(vertex_shader_id);
  glDeleteShader(fragment_shader_id);

  glDeleteProgram(program_id);
}


void Basic::SetResolution(int width, int height)
{
  glProgramUniform2i(program_id, uniforms.screen_resolution, width, height);
}


void Basic::SetOffset(int x, int y)
{
  glProgramUniform2f(program_id, uniforms.offset, x, y);
}


void Basic::SetOffset(vec2 const& offset)
{
  glProgramUniform2fv(program_id, uniforms.offset, 1, gl_data(offset));
}


void Basic::SetRotation(float rot)
{
  glProgramUniform1f(program_id, uniforms.rotation, rot);
}


void Basic::SetZoom(float zoom)
{
  glProgramUniform1f(program_id, uniforms.zoom, zoom);
}


void Basic::SetColour(float r, float g, float b, float a)
{
  glProgramUniform4f(program_id, uniforms.colour, r, g, b, a);
}


void Basic::SetColour(vec4 const& colour)
{
  glProgramUniform4fv(program_id, uniforms.colour, 1, reinterpret_cast<const GLfloat*>(&colour));
}


const std::string Basic::vertex_src =
  R"(#version 330

layout(location=0) in vec2 v;
layout(location=1) in vec4 col;

out vec4 vertex_colour;

uniform ivec2 screen_resolution;

uniform vec2 offset;
uniform float rotation;
uniform float zoom;

vec2 ScreenToClip(const vec2 screen)
{
  float x = ((screen.x / float(screen_resolution.x)) * 2.0) - 1.0;
  float y = ((1.0 - (screen.y / float(screen_resolution.y))) * 2.0) - 1.0;
  return vec2(x,y);
}

mat2 RotateMatrix(float angle)
{
  return mat2(cos(angle), sin(angle), -sin(angle), cos(angle));
}

void main(void)
{
  vec2 v_rotated = RotateMatrix(rotation) * v;
  vec2 v_zoomed = v_rotated * zoom;
  vec2 screen_pos = v_zoomed + offset;

  gl_Position = vec4(ScreenToClip(screen_pos), 0.0, 1.0);
  vertex_colour = col;
}
)";


const std::string Basic::fragment_src =
  R"(#version 330

uniform vec4 colour;

in vec4 vertex_colour;
out vec4 out_colour;

void main(void)
{
  out_colour = colour * vertex_colour;
  //out_colour.a += 0.5;
}

)";


} //namespace Shader
