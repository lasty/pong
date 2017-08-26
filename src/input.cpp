
#include "input.hpp"


#include <iostream>

#include <GLFW/glfw3.h>

#include "game.hpp"


Input::Input(GLFWwindow *window, Game &game)
:game(game)
{
  SetupCallbacks(window);

  SetupBinds();
}


void Input::AddBind(int key, Control control)
{
  bind_list[key] = control;
}



Control Input::GetControl(int key) const
{
  const auto & it = bind_list.find(key);
  if (it != bind_list.end()) return (*it).second;
  return Control::none;
}


void Input::SetupBinds()
{
  AddBind(GLFW_KEY_ESCAPE, Control::quit);
  AddBind(GLFW_KEY_Q, Control::quit);

  AddBind(GLFW_KEY_SPACE, Control::new_balls);
  AddBind(GLFW_KEY_N, Control::new_balls);

  AddBind(GLFW_KEY_G, Control::toggle_grav);
  AddBind(GLFW_KEY_F, Control::toggle_friction);

  AddBind(GLFW_KEY_W, Control::move_up);
  AddBind(GLFW_KEY_UP, Control::move_up);

  AddBind(GLFW_KEY_S, Control::move_down);
  AddBind(GLFW_KEY_DOWN, Control::move_down);

  AddBind(GLFW_KEY_A, Control::move_left);
  AddBind(GLFW_KEY_LEFT, Control::move_left);

  AddBind(GLFW_KEY_D, Control::move_right);
  AddBind(GLFW_KEY_RIGHT, Control::move_right);

  AddBind(GLFW_MOUSE_BUTTON_LEFT, Control::shoot);

}


void Input::HandleCommand(Control control, bool down)
{

  switch (control)
  {
    case Control::none :
      std::cout << "enqueued control none, somehow" << std::endl;
    break;

    case Control::quit :
      if (down) game.running = false;
    break;

    case Control::new_balls :
      if (down) game.NewObjects();
    break;

    case Control::toggle_grav :
      if (down) game.ToggleGravity();
    break;

    case Control::toggle_friction :
      if (down) game.ToggleFriction();
    break;

    case Control::move_up :
      player_move.up = down;  //OK confusing name
    break;

    case Control::move_down :
      player_move.down = down;
    break;

    case Control::move_left :
      player_move.left = down;
    break;

    case Control::move_right :
      player_move.right = down;
    break;

    case Control::shoot:
      if (down) game.Shoot();
    break;
  }
}


Input * Input::GetThis(GLFWwindow *window)
{
  Input* input = static_cast<Input*>(glfwGetWindowUserPointer(window));

  if (not input) throw std::runtime_error("Key callback user pointer not set");

  return input;
}


void Input::key_callback(
  GLFWwindow* window, int key, [[maybe_unused]] int scancode,
  [[maybe_unused]] int action, [[maybe_unused]] int mods)
{
  Input* input = Input::GetThis(window);

  bool down = (action == GLFW_PRESS);  //  : (action == GLFW_RELEASE) ? false

  Control control = input->GetControl(key);

  input->HandleCommand(control, down);
}


void Input::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
  Input* input = Input::GetThis(window);

  input->mouse = { float(xpos), float(ypos) };
}


void Input::mouse_button_callback(GLFWwindow* window, int button, int action, [[maybe_unused]] int mods)
{
  Input* input = Input::GetThis(window);

  bool down = (action == GLFW_PRESS);
  Control control = input->GetControl(button);

  input->HandleCommand(control, down);
}


void Input::SetupCallbacks(GLFWwindow *window)
{
  if (glfwGetWindowUserPointer(window) != nullptr)
    throw std::runtime_error("Already got window user pointer?");

  glfwSetWindowUserPointer(window, this);

  glfwSetKeyCallback(window, Input::key_callback);
  glfwSetMouseButtonCallback(window, Input::mouse_button_callback);
  glfwSetCursorPosCallback(window, Input::cursor_position_callback);
}


void Input::RemoveCallbacks(GLFWwindow *window)
{
  if (glfwGetWindowUserPointer(window) != this)
    throw std::runtime_error("Pointer not set correctly?");

  glfwSetKeyCallback(window, nullptr);
  glfwSetMouseButtonCallback(window, nullptr);
  glfwSetCursorPosCallback(window, nullptr);


  glfwSetWindowUserPointer(window, nullptr);
}
