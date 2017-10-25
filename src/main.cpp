
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <ctime>


constexpr int SWAP_INTERVAL{1};

#if OLD_OPENGL
constexpr int GL_MAJOR{3};
constexpr int GL_MINOR{3};
#else
constexpr int GL_MAJOR{4};
constexpr int GL_MINOR{5};
#endif


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


void glfw_error_callback([[maybe_unused]] int error, const char *description)
{
  std::cout << "GLFW Error: " << description << std::endl;
}


void set_display_title(GLFWwindow *window, const std::string &str)
{
  const std::string gl_ver{'0' + GL_MAJOR, '.', '0' + GL_MINOR};
  const std::string title = "GL " + gl_ver + " - " + str;

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


class TimedLogger
{
public:
  float last_time = glfwGetTime();
  std::string last_msg;

  void BEGIN(const std::string &msg)
  {
    last_msg = msg;
    last_time = glfwGetTime();
  }

  void END()
  {
    float time_now = glfwGetTime();
    float seconds = time_now - last_time;
    last_time = time_now;

    std::cout << "---==[ Time Log ] ==--  "
              << last_msg << "    (" << seconds * 1000 << " ms)"
              << std::endl;
  }
};


void main_game()
{
  std::cout << "Hello, world" << std::endl;
  std::cout.precision(2);
  std::cout << std::fixed;

  TimedLogger TIMELOG;

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

  TIMELOG.BEGIN("glfw creating window");
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_MAJOR);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_MINOR);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

  std::cout << "Requestiong OpenGL Context version " << GL_MAJOR << "." << GL_MINOR
            << std::endl;

  int width = 640;
  int height = 480;
  GLFWwindow *window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);

  if (not window)
  {
    throw std::runtime_error("failed to create window");
  }

  TIMELOG.END();

  TIMELOG.BEGIN("make context current");
  glfwMakeContextCurrent(window);
  TIMELOG.END();

  TIMELOG.BEGIN("glewInit()");
  //load opengl extension library here
  if (glewInit() != GLEW_OK)
  {
    throw std::runtime_error("failed to init GLEW");
  }
  TIMELOG.END();

  TIMELOG.BEGIN("Misc gl stuff");
  std::cout << "GLEW Version: " << glewGetString(GLEW_VERSION) << std::endl;
  std::cout << "GL Version String: " << glGetString(GL_VERSION) << std::endl;

  int gl_version_maj = GL::GetInteger(GL_MAJOR_VERSION);
  int gl_version_min = GL::GetInteger(GL_MINOR_VERSION);

  std::cout << "GL Version: " << gl_version_maj << "." << gl_version_min << std::endl;

  #if !OLD_OPENGL
    GL::Debuging(true);
  #endif

  glfwSwapInterval(SWAP_INTERVAL);
  TIMELOG.END();

  TIMELOG.BEGIN("Renderer, Sound, Timer, Game, etc, inits");
  Renderer renderer;
  Sound sound;
  Timer timer;

  glfwGetFramebufferSize(window, &width, &height);

  Game game{sound};
  GameState gamestate = game.NewGame(width, height);

  Input input(window);

  TIMELOG.END();

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


  TIMELOG.BEGIN("Cleanup");

  //Clean up
  sound.Quit();
  SDL_Quit();
  input.RemoveCallbacks(window);

  glfwDestroyWindow(window);

  TIMELOG.END();

  glfwTerminate();
}


int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
{
  try {

  main_game();
  }
  catch(std::exception &e)
  {
    std::cout << "std::exception thrown -- " << e.what() << std::endl;
  }

  return EXIT_SUCCESS;
}
