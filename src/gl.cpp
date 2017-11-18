
#include "gl.hpp"

#include <cstring>
#include <iostream>
#include <map>
#include <vector>


static_assert(sizeof(GLfloat) == sizeof(float), "opengl float wrong size");
static_assert(sizeof(GLint) == sizeof(int), "opengl int wrong size");
static_assert(sizeof(GLenum) == sizeof(int), "opengl enum wrong size");


namespace GL {


//Debugging stuff

const std::map<GLenum, std::string> enum_to_string{
  {GL_DEBUG_SEVERITY_HIGH, "SEVERITY_HIGH"},
  {GL_DEBUG_SEVERITY_MEDIUM, "SEVERITY_MEDIUM"},
  {GL_DEBUG_SEVERITY_LOW, "SEVERITY_LOW"},
  {GL_DEBUG_SEVERITY_NOTIFICATION, "NOTIFICATION"},
  {GL_DEBUG_TYPE_ERROR, "ERROR"},
  {GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR, "DEPRECATED_BEHAVIOR"},
  {GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR, "UNDEFINED_BEHAVIOR"},
  {GL_DEBUG_TYPE_PORTABILITY, "PORTABILITY"},
  {GL_DEBUG_TYPE_PERFORMANCE, "PERFORMANCE"},
  {GL_DEBUG_TYPE_MARKER, "MARKER"},
  {GL_DEBUG_TYPE_PUSH_GROUP, "PUSH_GROUP"},
  {GL_DEBUG_TYPE_POP_GROUP, "POP_GROUP"},
  {GL_DEBUG_TYPE_OTHER, "OTHER"},
  {GL_DEBUG_SOURCE_API, "SOURCE_API"},
  {GL_DEBUG_SOURCE_WINDOW_SYSTEM, "SOURCE_WINDOW_SYSTEM"},
  {GL_DEBUG_SOURCE_SHADER_COMPILER, "SHADER_COMPILER"},
  {GL_DEBUG_SOURCE_THIRD_PARTY, "SOURCE_THIRD_PARTY"},
  {GL_DEBUG_SOURCE_APPLICATION, "SOURCE_APPLICATION"},
  {GL_DEBUG_SOURCE_OTHER, "SOURCE_OTHER"}};


template<typename MAP>
typename MAP::mapped_type const get_map(MAP const &map, typename MAP::key_type const &key, typename MAP::mapped_type const &default_value)
{
  auto const &it = map.find(key);
  if (it == map.end()) return default_value;
  return it->second;
}


//typedef void (APIENTRY *DEBUGPROC)
void opengl_debug_callback(
  [[maybe_unused]] GLenum source,
  [[maybe_unused]] GLenum type,
  [[maybe_unused]] GLuint id,
  [[maybe_unused]] GLenum severity,
  [[maybe_unused]] GLsizei length,
  const GLchar *message,
  [[maybe_unused]] const void *userParam)
{
  //if (severity != GL_DEBUG_SEVERITY_NOTIFICATION)
  if (strstr(message, "Buffer detailed") != message)
  {
    std::cout << "[OPENGL"
              << " " << get_map(enum_to_string, type, "?")
              << " " << get_map(enum_to_string, severity, "?")
              //<< " " << get_map(enum_to_string, source, "?")
              << "]  " << message << std::endl;
  }
}


// void debug_message_insert(msg)
// (
//   glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_OTHER,
//     0, GL_DEBUG_SEVERITY_NOTIFICATION, msg);
// }


void Debuging(bool enable)
{
  if (enable)
  {
    glDebugMessageCallback(opengl_debug_callback, nullptr);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glEnable(GL_DEBUG_OUTPUT);
  }
  else
  {
    glDisable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(nullptr, nullptr);
  }
}


int GetShaderi(int shader_id, GLenum param_name)
{
  int value = 0;
  glGetShaderiv(shader_id, param_name, &value);
  return value;
}


int GetProgrami(int program_id, GLenum param_name)
{
  int value = 0;
  glGetProgramiv(program_id, param_name, &value);
  return value;
}


int GetInteger(GLenum param_name)
{
  int value = 0;
  glGetIntegerv(param_name, &value);
  return value;
}


std::string GetShaderLog(int shader_id)
{
  int log_length = GetShaderi(shader_id, GL_INFO_LOG_LENGTH);
  if (log_length == 0) return "";

  std::vector<GLchar> log_data(log_length);
  glGetShaderInfoLog(shader_id, log_length, nullptr, log_data.data());

  std::string log_str{log_data.begin(), log_data.begin() + log_length};

  return log_str;
}


std::string GetProgramLog(int program_id)
{
  int log_length = GetProgrami(program_id, GL_INFO_LOG_LENGTH);
  if (log_length == 0) return "";

  std::vector<GLchar> log_data(log_length);
  glGetProgramInfoLog(program_id, log_length, nullptr, log_data.data());

  std::string log_str{log_data.begin(), log_data.begin() + log_length};

  return log_str;
}


int CreateShader(int shader_type, const std::string &shader_source)
{
  std::array<const GLchar*,1> source_ptrs {{shader_source.c_str()}};

  int shader_id = glCreateShader(shader_type);
  glShaderSource(shader_id, 1, source_ptrs.data(), nullptr);
  glCompileShader(shader_id);

  int status = GetShaderi(shader_id, GL_COMPILE_STATUS);
  if (status != GL_TRUE)
  {
    std::string log = GetShaderLog(shader_id);
    std::cerr << "Shader log: " << log << std::endl;
    throw std::runtime_error{"Error compiling shader"};
  }

  return shader_id;
}


void LinkProgram(int program_id)
{
  glLinkProgram(program_id);

  int status = GetProgrami(program_id, GL_LINK_STATUS);
  if (status != GL_TRUE)
  {
    std::string log = GetProgramLog(program_id);
    std::cerr << "Program log: " << log << std::endl;
    throw std::runtime_error{"Error linking shader program"};
  }
}


int ValidateProgram(int program_id)
{
  glValidateProgram(program_id);

  int status = GetProgrami(program_id, GL_VALIDATE_STATUS);
  return status;
}


int CreateBuffers()
{
  GLuint buf_id = 0;

  #if OLD_OPENGL
    glGenBuffers(1, &buf_id);
  #else
    glCreateBuffers(1, &buf_id);
  #endif

  return buf_id;
}


void DeleteBuffers(int buffer_id)
{
  GLuint buf_id = buffer_id;
  glDeleteBuffers(1, &buf_id);
}


int CreateVertexArrays()
{
  GLuint vao_id = 0;

  #if OLD_OPENGL
    glGenVertexArrays(1, &vao_id);
  #else
    glCreateVertexArrays(1, &vao_id);
  #endif

  return vao_id;
}


void DeleteVertexArrays(int vao_id)
{
  GLuint vao = vao_id;
  glDeleteVertexArrays(1, &vao);
}




} //namespace GL
