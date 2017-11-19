#pragma once

#include <string>
#include <iostream>
#include <sstream>

#include "game.hpp"
#include "gl.hpp"
#include "maths_types.hpp"


//Defined in to_string.cpp - outputs to window title, cleared per frame
extern std::ostringstream TRACE;


std::string ToString(const State &state);


std::ostream &operator<<(std::ostream &out, vec2 const &v2);
std::ostream &operator<<(std::ostream &out, vec3 const &v3);
// std::ostream &operator<<(std::ostream &out, vec4 const &v4);
std::ostream &operator<<(std::ostream &out, col4 const &v4);
std::ostream &operator<<(std::ostream &out, mat4 const &mat);


std::string ToString(const GLenum &e);
