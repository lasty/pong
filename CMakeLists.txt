cmake_minimum_required(VERSION 3.9)

#### Deps

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_SOURCE_DIR}/cmake")

find_package(GLFW 3.2 REQUIRED)

find_package(SDL2 2.0 REQUIRED COMPONENTS main image mixer)

find_package(GLEW 2.0 REQUIRED)

# find_package(GLM 0.9.8.0 REQUIRED)

find_package(OpenGL REQUIRED)

#### System dependant shit

if(MINGW)
  set(MINGW32 mingw32)
endif()

##### Main target

add_executable(pong WIN32
  src/game.cpp
  src/gl.cpp
  src/input.cpp
  src/main.cpp
  src/maths.cpp
  src/particles.cpp
  src/renderer.cpp
  src/shader.cpp
  src/sound.cpp
)

target_compile_features(pong PRIVATE cxx_std_14)

target_link_libraries(pong PRIVATE
  ${MINGW32} SDL2::main SDL2::SDL2 SDL2::mixer
  GLFW::GLFW
  GLEW::GLEW OpenGL::GL)

#### Extra

#Enable all warnings if debug build
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  message(STATUS "Enabling all warnings")
  target_compile_options(pong PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -fmax-errors=1>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -ferror-limit=1
      -Wno-missing-braces
      #-Wno-c++98-compat -Wno-c++1z-extensions -Wno-padded
    >
    $<$<CXX_COMPILER_ID:AppleClang>:-Weverything>)
endif()

if(MINGW)
  target_compile_options(pong PUBLIC "-fdiagnostics-color=always")
endif()
