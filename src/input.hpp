
#pragma once

#include <map>

#include "maths_types.hpp"


enum class Control
{
  none,
  toggle_grav,
  toggle_friction,
  new_balls,
  quit,

  move_up,
  move_left,
  move_right,
  move_down,

  shoot
};


struct MovementInput
{
  bool up = false;
  bool left = false;
  bool right = false;
  bool down = false;
};


struct GLFWwindow;


class Input
{
private:
  std::map<int, Control> bind_list;

  class Game &game;

public:
  MovementInput player_move;
  vec2 mouse;

public:
  Input(GLFWwindow *window, class Game &game);

  void AddBind(int key, Control control);

  Control GetControl(int key) const;

  void SetupBinds();

  void HandleCommand(Control control, bool down);

  static Input * GetThis(GLFWwindow *window);
  static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
  static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
  static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

  void SetupCallbacks(GLFWwindow *window);
  void RemoveCallbacks(GLFWwindow *window);

};
