
#include "to_string.hpp"


std::ostringstream TRACE;


std::string ToString(const State &state)
{
  switch (state)
  {
    case State::new_level:
      return "new_level";
    case State::ball_launch:
      return "ball_launch";
    case State::mid_game:
      return "mid_game";
    case State::ball_reset:
      return "ball_reset";
    case State::ball_died:
      return "ball_died";
    case State::game_won:
      return "game_won";
    case State::main_menu:
      return "main_menu";
    case State::pause_menu:
      return "pause_menu";
  }
  return "[ERROR: State not found]";
}



std::ostream &operator<<(std::ostream &out, vec2 const &v2)
{
  return out << "vec2{" << v2.x << ", " << v2.y << "}";
}


std::ostream &operator<<(std::ostream &out, vec3 const &v3)
{
  return out << "vec3{" << v3.x << ", " << v3.y << ", " << v3.z << "}";
}


// std::ostream &operator<<(std::ostream &out, vec4 const &v4)
// {
//   return out << "vec4{" << v4.x << ", " << v4.y
//              << ", " << v4.z << ", " << v4.w << "}";
// }


std::ostream &operator<<(std::ostream &out, col4 const &v4)
{
  return out << "col4{" << v4.r << ", " << v4.g
             << ", " << v4.b << ", " << v4.a << "}";
}


std::ostream &operator<<(std::ostream &out, mat4 const &mat)
{
  out << "mat4 [";
  for (int x = 0; x < 4; x++)
  {
    out << "\n";
    for (int y = 0; y < 4; y++)
    {
      out << "\t" << mat.elements[y][x];
    }
  }
  out << " ]";
  return out;
}


std::string ToString(const GLenum &e)
{
    switch(e)
    {

        case GL_DEBUG_SEVERITY_HIGH: return "SEVERITY_HIGH";
        case GL_DEBUG_SEVERITY_MEDIUM: return "SEVERITY_MEDIUM";
        case GL_DEBUG_SEVERITY_LOW: return "SEVERITY_LOW";
        case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
        case GL_DEBUG_TYPE_ERROR: return "ERROR";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
        case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
        case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
        case GL_DEBUG_TYPE_MARKER: return "MARKER";
        case GL_DEBUG_TYPE_PUSH_GROUP: return "PUSH_GROUP";
        case GL_DEBUG_TYPE_POP_GROUP: return "POP_GROUP";
        case GL_DEBUG_TYPE_OTHER: return "OTHER";
        case GL_DEBUG_SOURCE_API: return "SOURCE_API";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "SOURCE_WINDOW_SYSTEM";
        case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER_COMPILER";
        case GL_DEBUG_SOURCE_THIRD_PARTY: return "SOURCE_THIRD_PARTY";
        case GL_DEBUG_SOURCE_APPLICATION: return "SOURCE_APPLICATION";
        case GL_DEBUG_SOURCE_OTHER: return "SOURCE_OTHER";

        default: return "[ERROR: Unknown GLenum]";
    }
}



