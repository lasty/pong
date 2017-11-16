#pragma once


struct vec2
{
  float x;
  float y;
};


struct vec3
{
    float x;
    float y;
    float z;
};


/*
struct vec4
{
    float x;
    float y;
    float z;
    float w;
};
*/

struct col4
{
    float r;
    float g;
    float b;
    float a;
};


struct mat4
{
  float elements[4][4];
};
