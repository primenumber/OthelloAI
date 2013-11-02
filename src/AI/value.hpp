#pragma once
#include <array>
#include "Board/board.hpp"

namespace othello {
namespace value {

class CalcValue {
 public:
  int operator()(const board::Board& board, board::State state) {
    int value = 0;
    value += calc_table(board, state);
    return value;
  }
//  virtual int operator()(const board::Board& board, board::State state,
//                                int count_turns) {
//  }
 private:
  static int calc_table(const board::Board&, board::State);
};

} // namespace value
} // namespace othello
