
#include "input.hpp"


#include <iostream>

#include <GLFW/glfw3.h>

#include "game.hpp"


Input::Input(GLFWwindow* window) //, Game &game)
//:game(game)
{
  SetupCallbacks(window);

  SetupBinds();
}


void Input::AddBind(int key, IntentType control)
{
  intent_bind_list[key] = control;
}

void Input::AddBind(int key, PlayerInput control)
{
  player_input_bind_list[key] = control;
}


void Input::SetupBinds()
{
  AddBind(GLFW_KEY_ESCAPE, IntentType::quit);
  AddBind(GLFW_KEY_Q, IntentType::quit);

  AddBind(GLFW_KEY_SPACE, IntentType::new_game);
  AddBind(GLFW_KEY_N, IntentType::new_game);

  AddBind(GLFW_KEY_G, IntentType::toggle_debug);
  AddBind(GLFW_KEY_F1, IntentType::toggle_debug);

  AddBind(GLFW_KEY_F5, IntentType::reset_ball);
  AddBind(GLFW_KEY_R, IntentType::reset_ball);

  AddBind(GLFW_KEY_W, IntentType::menu_up);
  AddBind(GLFW_KEY_UP, IntentType::menu_up);

  AddBind(GLFW_KEY_S, IntentType::menu_down);
  AddBind(GLFW_KEY_DOWN, IntentType::menu_down);

  AddBind(GLFW_KEY_ENTER, IntentType::menu_activate);
  AddBind(GLFW_KEY_SPACE, IntentType::menu_activate);


  AddBind(GLFW_KEY_A, PlayerInput::move_left);
  AddBind(GLFW_KEY_LEFT, PlayerInput::move_left);

  AddBind(GLFW_KEY_D, PlayerInput::move_right);
  AddBind(GLFW_KEY_RIGHT, PlayerInput::move_right);
  AddBind(GLFW_MOUSE_BUTTON_LEFT, PlayerInput::shoot);
  AddBind(GLFW_MOUSE_BUTTON_RIGHT, PlayerInput::shoot);
}


Input* Input::GetThis(GLFWwindow* window)
{
  Input* input = static_cast<Input*>(glfwGetWindowUserPointer(window));

  if (not input) throw std::runtime_error("Key callback user pointer not set");

  return input;
}


void Input::key_callback(
  GLFWwindow* window, int key, [[maybe_unused]] int scancode,
  int action, [[maybe_unused]] int mods)
{
  Input* input = Input::GetThis(window);

  auto it = input->intent_bind_list.find(key);
  if (it != input->intent_bind_list.end())
  {
    Intent i;
    i.type = it->second;

    if (action == GLFW_PRESS)
      input->intent_stream.push_back(i);
  }


  auto it2 = input->player_input_bind_list.find(key);
  if (it2 != input->player_input_bind_list.end())
  {
    Intent i{IntentType::player_input, {}, {}};
    i.player_input = it2->second;
    i.down = (action == GLFW_PRESS);

    input->intent_stream.push_back(i);
  }
}


std::vector<Intent> Input::GetIntentStream()
{
  auto out = intent_stream;
  intent_stream.clear();
  return out;
}


void Input::cursor_position_callback(GLFWwindow* window, double xpos, [[maybe_unused]] double ypos)
{
  Input* input = Input::GetThis(window);


  Intent i{IntentType::player_input, PlayerInput::mouse_position, {}};
  i.position = vec2{float(xpos), float(ypos)};

  input->intent_stream.push_back(i);
}


void Input::mouse_button_callback(GLFWwindow* window, int button, int action, [[maybe_unused]] int mods)
{
  Input* input = Input::GetThis(window);

  auto it2 = input->player_input_bind_list.find(button);
  if (it2 != input->player_input_bind_list.end())
  {
    Intent i;
    i.type = IntentType::player_input;
    i.player_input = it2->second;
    i.down = (action == GLFW_PRESS);

    input->intent_stream.push_back(i);
  }
}


void Input::SetupCallbacks(GLFWwindow* window)
{
  if (glfwGetWindowUserPointer(window) != nullptr)
    throw std::runtime_error("Already got window user pointer?");

  glfwSetWindowUserPointer(window, this);

  glfwSetKeyCallback(window, Input::key_callback);
  glfwSetMouseButtonCallback(window, Input::mouse_button_callback);
  glfwSetCursorPosCallback(window, Input::cursor_position_callback);
}


void Input::RemoveCallbacks(GLFWwindow* window)
{
  if (glfwGetWindowUserPointer(window) != this)
    throw std::runtime_error("Pointer not set correctly?");

  glfwSetKeyCallback(window, nullptr);
  glfwSetMouseButtonCallback(window, nullptr);
  glfwSetCursorPosCallback(window, nullptr);

  glfwSetWindowUserPointer(window, nullptr);
}
