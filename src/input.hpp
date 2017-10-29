
#pragma once

#include <map>
#include <vector>

#include "maths_types.hpp"


enum class PlayerInput
{
  move_left,
  move_right,
  mouse_position,
  shoot
};


enum class IntentType
{
  quit,
  toggle_debug,
  new_game,
  reset_ball,
  player_input,
  menu,
  menu_up,
  menu_down,
  menu_activate
};


struct Intent
{
  IntentType type;

  union {
    PlayerInput player_input = PlayerInput::move_left;
    float delta_time;
  };

  union {
    vec2 position;
    bool down;
  };
};


class Input
{
private:
  std::map<int, IntentType> intent_bind_list;
  std::map<int, PlayerInput> player_input_bind_list;

  std::vector<Intent> intent_stream;

public:
  Input(struct GLFWwindow *window); //, class Game &game);

  void AddBind(int key, IntentType control);
  void AddBind(int key, PlayerInput control);

  // IntentType GetIntent(int key) const;

  void SetupBinds();

  std::vector<Intent> GetIntentStream();


  // void HandleCommand(IntentType control, bool down);

  static Input *GetThis(GLFWwindow *window);
  static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
  static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);
  static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);

  void SetupCallbacks(GLFWwindow *window);
  void RemoveCallbacks(GLFWwindow *window);
};
