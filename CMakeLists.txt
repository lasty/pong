cmake_minimum_required(VERSION 3.9)


option(OLD_OPENGL "Use OpenGL 3.3 instead of modern 4.5" OFF)


#### Deps

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_SOURCE_DIR}/cmake")

find_package(GLFW 3.2 REQUIRED)

find_package(SDL2 2.0 REQUIRED COMPONENTS main image mixer)

find_package(GLEW 2.0 REQUIRED)

find_package(OpenGL REQUIRED)

#### System dependant shit

if(MINGW)
  set(MINGW32 mingw32)
endif()

##### Main target

add_library(core
  src/game.cpp
  src/gl.cpp
  src/input.cpp
  src/maths.cpp
  src/particles.cpp
  src/renderer.cpp
  src/shader.cpp
  src/sound.cpp)

add_executable(pong WIN32 src/main.cpp)
add_executable(test_pong EXCLUDE_FROM_ALL src/tests.cpp)


if($ENV{FEATURES_OVERRIDE})
  #Somehow homebrew clang/gcc fails compile feature on my macbook
  target_compile_options(core PUBLIC "-std=gnu++1y")
else()
  target_compile_features(core PUBLIC cxx_std_14)
endif()


if(OLD_OPENGL)
  target_compile_definitions(core PUBLIC -DOLD_OPENGL=1)
endif()


target_link_libraries(core PRIVATE
  SDL2::SDL2 SDL2::mixer
  GLFW::GLFW
  OpenGL::GL)

target_link_libraries(core PUBLIC GLEW::GLEW)


target_link_libraries(pong PRIVATE core ${MINGW32} SDL2::main SDL2::SDL2)
target_link_libraries(test_pong PRIVATE core)


#### Extra

#Enable all warnings if debug build
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  message(STATUS "Enabling all warnings")
  target_compile_options(core PUBLIC
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -fmax-errors=1>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -ferror-limit=1 -Wno-missing-braces>
    $<$<CXX_COMPILER_ID:AppleClang>:-Weverything>)
endif()

if(MINGW)
  target_compile_options(core PUBLIC "-fdiagnostics-color=always")
endif()
