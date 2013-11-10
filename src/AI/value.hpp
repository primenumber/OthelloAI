#pragma once
#include <array>
#include "Board/board.hpp"

namespace othello {
namespace value {

class CalcValue {
 public:
  int operator()(const board::Board& board, const board::State state,
                 const int stones) {
    int value = 0;
    value += calc_table(board, state);
    value += calc_edge(board, state, stones);
    value += calc_puttable(board, state);
    return value;
  }
 private:
  static int calc_table(const board::Board&, const board::State);
  static int calc_edge(const board::Board&, const board::State, const int);
  static int calc_puttable(const board::Board&, board::State);
};

} // namespace value
} // namespace othello
