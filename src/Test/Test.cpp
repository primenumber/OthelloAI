#include <array>
#include <iostream>
#include <boost/optional.hpp>
#include "Board/GameState.hpp"

constexpr std::array<int, 64> table = {
  100,-50, 15, 15, 15, 15,-50,100,
  -50,-70,-20,-20,-20,-20,-70,-50,
   15,-20,  0, -5, -5,  0,-20, 15,
   15,-20, -5, -5, -5, -5,-20, 15,
   15,-20, -5, -5, -5, -5,-20, 15,
   15,-20,  0, -5, -5,  0,-20, 15,
  -50,-70,-20,-20,-20,-20,-70,-50,
  100,-50, 15, 15, 15, 15,-50,100
};

int func(const othello::gamestate::GameState& game_state) {
  int value = 0;
  for (int i = 0; i < 64; ++i) {
    if (game_state.atPosition(i) == game_state.GetState()) {
      value += table[i];
    } else if (game_state.atPosition(i) == InvertState(game_state.GetState())) {
      value -= table[i];
    }
  }
  return value;
}

int main() {
  using othello::gamestate::GameState;
  using othello::gamestate::Position;
  GameState game_state;
  bool pass = false;
  while (true) {
    std::cout << "Now State" << std::endl;
    std::cout << game_state.ToString() << std::endl;
    boost::optional<Position> opt_position = game_state.OptimalPut(7, func);
    if (opt_position) {
      game_state.Put(*opt_position);
      pass = false;
    } else if (!pass) {
      game_state.Pass();
      pass = true;
    } else
      break;
  }
}

