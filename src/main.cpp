
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <ctime>

// #define TEST_MATHS

const int SWAP_INTERVAL {1};
const bool DRAW_TEST_CASE = false;
const int GL_MAJOR {4};
const int GL_MINOR {5};


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

bool mainloop_running = true;
float test_case_seed = time(0);
int test_case_step = 0;


void glfw_error_callback([[maybe_unused]] int error, const char* description)
{
  std::cout << "GLFW Error: " << description << std::endl;
}


void set_display_title(GLFWwindow* window, const std::string &str)
{
  const std::string title = "GLFW - " + str;

  glfwSetWindowTitle(window, title.c_str());
}



void main_game()
{
  int width = 640;
  int height = 480;



  std::cout << "Hello, world" << std::endl;

  SDL_version linked;
  SDL_version compiled;

  SDL_VERSION(&compiled);
  SDL_GetVersion(&linked);

  std::cout << "SDL Version: "
    << "(compiled with " << (int)compiled.major << "." << (int)compiled.minor << "." << (int)compiled.patch << ")"
    << "  (linked with " << (int)linked.major << "." << (int)linked.minor << "." << (int)linked.patch << ")"
    << std::endl;

  SDL_Init(SDL_INIT_AUDIO);


  std::cout << "GLFW Version: " << glfwGetVersionString() << std::endl;
  if (not glfwInit()) throw std::runtime_error("failed to init GLFW");
  glfwSetErrorCallback(glfw_error_callback);


  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_MAJOR);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_MINOR);

  // Create a windowed mode window and its OpenGL context


  GLFWwindow* window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);

  if (not window)
  {
    throw std::runtime_error("failed to create window");
  }

  // Make the window's context current

  glfwMakeContextCurrent(window);

  //load opengl extension library here
  if(glewInit() != GLEW_OK)
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


  int frame_count = 0;
  int last_fps = 0;
  float frame_timer = glfwGetTime();
  float frame_last_time = frame_timer + 1.0f;
  float time_last = glfwGetTime();


  Renderer renderer;
  Sound sound;

  glfwGetFramebufferSize(window, &width, &height);
  Game game{width, height, sound};

  Input input(window);

  // Loop until the user closes the window
  while (mainloop_running and game.IsRunning())
  {

    glfwPollEvents();
    if (glfwWindowShouldClose(window))
    {
      mainloop_running = false;
    }

    //Input and Update

    auto time_now = glfwGetTime();
    auto delta_time = time_now - time_last;
    time_last = time_now;

    if (SWAP_INTERVAL > 1)
    delta_time = 1.0f / 60.0f;


    if (not DRAW_TEST_CASE)
    {
      auto intent_stream = input.GetIntentStream();

      //game.PlayerInput(delta_time, input);

      game.ProcessIntents(intent_stream);

      game.Update(delta_time);
    }

    // FPS counter
    frame_count++;
    frame_timer = glfwGetTime();


    if (frame_timer >= frame_last_time)
    {
      last_fps = frame_count;
      frame_last_time += 1.0f;
      frame_count = 0;
    }

    //Set title
    {
      std::stringstream ss;  ss << std::fixed;  ss.precision(2);
      TRACE << std::fixed;  TRACE.precision(2);

      ss << "FPS: " << last_fps <<  "  ";//seconds: " << frame_timer;

      ss << "   " << TRACE.str();

      set_display_title(window, ss.str());

      //clear the trace
      TRACE.str({});
      TRACE.clear();
    }


    //Check framebuffer size

    glfwGetFramebufferSize(window, &width, &height);
    renderer.Resize(width, height);
    game.Resize(width, height);
    //float ratio = width / (float) height;


    // Render
    glViewport(0, 0, width, height);
    glClearColor(0.1, 0.2, 0.3, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);


    if (DRAW_TEST_CASE)
    {
      renderer.DrawTestCase(game, test_case_seed, test_case_step);
    }
    else
    {
      renderer.DrawGameState(game);
    }


    // Present framebuffer

    glfwSwapBuffers(window);

  }  // end main loop


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
