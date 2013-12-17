#include <cstdlib>
#include <ctime>
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

std::array<std::array<std::array<int, 8>, 256>, 256> value_table;

void InitValueTable() {
  using othello::gamestate::State;
  using othello::gamestate::Pow3;
  using othello::gamestate::Base3ToUint8Pair;
  using othello::gamestate::LineState;
  for (int i = 0; i < Pow3(8); ++i) {
    auto state_pair = Base3ToUint8Pair(i);
    LineState line_state(i);
    for (int j = 0; j < 8; ++j) {
      int value = 0;
      for (int k = 0; k < 8; ++k) {
        if (line_state[k] == State::BLACK)
          value += table[j * 8 + k];
        else if (line_state[k] == State::WHITE)
          value -= table[j * 8 + k];
      }
      value_table[state_pair.first][state_pair.second][j] = value;
    }
  }
}

int func(const othello::gamestate::GameState& game_state) {
  using othello::gamestate::State;
  int value = 0;
  if (game_state.GetState() == State::BLACK) {
    for (int i = 0; i < 8; ++i) {
      value += value_table
          [(game_state.GetBlack() >> (i * 8)) & 0xFF]
          [(game_state.GetWhite() >> (i * 8)) & 0xFF]
          [i];
    }
  } else {
    for (int i = 0; i < 8; ++i) {
      value -= value_table
          [(game_state.GetBlack() >> (i * 8)) & 0xFF]
          [(game_state.GetWhite() >> (i * 8)) & 0xFF]
          [i];
    }
  }
  return value;
}

int main() {
  using othello::gamestate::LineState;
  using othello::gamestate::GameTree;
  using othello::gamestate::Position;
  LineState::Init();
  InitValueTable();
  std::unique_ptr<GameTree> game_tree(new GameTree());
  int tree_depth, search_depth;
  std::cin >> tree_depth >> search_depth;
  bool pass = false;
  while (true) {
    std::cout << "Now State" << std::endl;
    std::cout << game_tree->ToString() << std::endl;
    game_tree->SearchTree(tree_depth, search_depth, func);
    GameTree& optimal_child = *(game_tree->GetChildren().front());
    boost::optional<Position> opt_position = optimal_child.GetRecentPut();
    if (opt_position) {
      game_tree.reset(game_tree->Put(*opt_position).release());
      pass = false;
    } else if (!pass) {
      game_tree.reset(game_tree->Pass().release());
      pass = true;
    } else
      break;
  }
}

