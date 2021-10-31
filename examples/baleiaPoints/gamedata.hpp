#ifndef GAMEDATA_HPP_
#define GAMEDATA_HPP_

#include <bitset>

enum class Input { Right, Left, Down, Up, Fire };
enum class State { Menu, Playing, GameOver, Win };

struct GameData {
  State m_state{State::Menu};
  std::bitset<5> m_input;  // [fire, up, down, left, right]
};

#endif