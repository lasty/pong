#pragma once


struct vec2
{
  union {
    float x;
  };
  union {
    float y;
  };
};


struct vec3
{
  union {
    float x;
    float r;
  };
  union {
    float y;
    float g;
  };
  union {
    float z;
    float b;
  };
};


struct vec4
{
  union {
    float x;
    float r;
  };
  union {
    float y;
    float g;
  };
  union {
    float z;
    float b;
  };
  union {
    float w;
    float a;
  };
};


struct mat4
{
  float elements[4][4];
};
