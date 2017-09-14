#pragma once

#include <string>

#include <GL/glew.h>


namespace GL {

void Debuging(bool enable);

//convenience GL wrapper functions

int GetShaderi(int shader_id, GLenum param_name);
int GetProgrami(int program_id, GLenum param_name);
int GetInteger(GLenum param_name);
std::string GetShaderLog(int shader_id);
std::string GetProgramLog(int program_id);

int CreateShader(int shader_type, const std::string &shader_source);

void LinkProgram(int program_id);
int ValidateProgram(int program_id);

int CreateBuffers();
int CreateVertexArrays();

void AttachAttribute(int vao_id, int attrib_id, int size, int offset, GLenum type);

template<typename T>
void UseProgram(T const &prog)
{
  int program_id = prog.GetProgramId();
  glUseProgram(program_id);
}

void UseVAO(int vao_id);


} //namespace GL
