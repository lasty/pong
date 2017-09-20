
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <ctime>

// #define TEST_MATHS

constexpr int SWAP_INTERVAL{1};
constexpr int GL_MAJOR{4};
constexpr int GL_MINOR{5};


#include <SDL.h>
#undef main


#include "gl.hpp"

#include <GLFW/glfw3.h>


#include "input.hpp"
#include "game.hpp"
#include "renderer.hpp"
#include "maths.hpp"
#include "maths_utils.hpp"
#include "sound.hpp"


std::ostringstream TRACE;


void glfw_error_callback([[maybe_unused]] int error, const char *description)
{
  std::cout << "GLFW Error: " << description << std::endl;
}


void set_display_title(GLFWwindow *window, const std::string &str)
{
  const std::string title = "GLFW - " + str;

  glfwSetWindowTitle(window, title.c_str());
}


class Timer
{
private:
  int frame_count = 0;
  int last_fps = 0;
  float frame_last_time = 1.0f;
  float time_last = 0.0f;

public:
  Timer()
  {
    time_last = glfwGetTime();
    frame_last_time = time_last + 1.0f;
  }

  float Update()
  {
    float time_now = glfwGetTime();
    float delta_time = time_now - time_last;
    time_last = time_now;

    if (SWAP_INTERVAL > 1)
      delta_time = 1.0f / 60.0f;

    frame_count++;

    if (time_now >= frame_last_time)
    {
      last_fps = frame_count;
      frame_last_time += 1.0f;
      frame_count = 0;
    }

    return delta_time;
  }

  int FPS() const { return last_fps; }
};


void SetTitle(GLFWwindow *window, const Timer &timer, const std::string &msg)
{
  std::stringstream ss;
  ss << std::fixed;
  ss.precision(2);
  TRACE << std::fixed;
  TRACE.precision(2);

  ss << "FPS: " << timer.FPS() << "  "; //seconds: " << frame_timer;

  ss << "   " << msg;

  set_display_title(window, ss.str());
}


void ClearTrace(std::ostringstream &TRACE)
{
  TRACE.str({});
  TRACE.clear();
}


void main_game()
{
  std::cout << "Hello, world" << std::endl;

  SDL_Init(SDL_INIT_AUDIO);

  SDL_version linked;
  SDL_version compiled;

  SDL_VERSION(&compiled);
  SDL_GetVersion(&linked);

  std::cout << "SDL Version: "
            << "(compiled with " << (int)compiled.major << "." << (int)compiled.minor << "." << (int)compiled.patch << ")"
            << "  (linked with " << (int)linked.major << "." << (int)linked.minor << "." << (int)linked.patch << ")"
            << std::endl;


  std::cout << "GLFW Version: " << glfwGetVersionString() << std::endl;
  if (not glfwInit()) throw std::runtime_error("failed to init GLFW");
  glfwSetErrorCallback(glfw_error_callback);


  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_MAJOR);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_MINOR);


  int width = 640;
  int height = 480;
  GLFWwindow *window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);

  if (not window)
  {
    throw std::runtime_error("failed to create window");
  }

  glfwMakeContextCurrent(window);

  //load opengl extension library here
  if (glewInit() != GLEW_OK)
  {
    throw std::runtime_error("failed to init GLEW");
  }

  std::cout << "GLEW Version: " << glewGetString(GLEW_VERSION) << std::endl;
  std::cout << "GL Version String: " << glGetString(GL_VERSION) << std::endl;

  int gl_version_maj = GL::GetInteger(GL_MAJOR_VERSION);
  int gl_version_min = GL::GetInteger(GL_MINOR_VERSION);

  std::cout << "GL Version: " << gl_version_maj << "." << gl_version_min << std::endl;

  GL::Debuging(true);

  glfwSwapInterval(SWAP_INTERVAL);

  Renderer renderer;
  Sound sound;
  Timer timer;

  glfwGetFramebufferSize(window, &width, &height);

  Game game{sound};
  GameState gamestate = game.NewGame(width, height);

  Input input(window);

  // Main Loop
  while ((not glfwWindowShouldClose(window)) and gamestate.running)
  {
    glfwPollEvents();

    float delta_time = timer.Update();


    gamestate = game.ProcessIntents(gamestate, input.GetIntentStream());

    gamestate = game.ProcessStateGraph(gamestate, delta_time);

    gamestate = game.Simulate(gamestate, delta_time);


    SetTitle(window, timer, TRACE.str());
    ClearTrace(TRACE);


    //Check framebuffer size
    glfwGetFramebufferSize(window, &width, &height);
    if (not(width == gamestate.width and height == gamestate.height))
    {
      renderer.Resize(width, height);
      gamestate = game.Resize(gamestate, width, height);
      //float ratio = width / (float) height;
      glViewport(0, 0, width, height);
    }

    // Render
    glClearColor(0.1, 0.2, 0.3, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    renderer.DrawGameState(gamestate);

    glfwSwapBuffers(window);

  } // end main loop


  //Clean up
  sound.Quit();
  SDL_Quit();
  input.RemoveCallbacks(window);

  glfwDestroyWindow(window);
  glfwTerminate();
}


int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
{
#ifdef TEST_MATHS
  TestMaths();
#else
  main_game();
#endif

  return EXIT_SUCCESS;
}
