#include "value.hpp"
namespace othello {
namespace value {

constexpr std::array<int, board::kboard_table_size> value_table_ = 
    std::array<int, board::kboard_table_size>{{
       0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
       0, 100, -50,  15,  15,  15,  15, -50, 100,   0,
       0, -50, -80, -10, -10, -10, -10, -80, -50,   0,
       0,  15, -10,   5,  -5,  -5,   5, -10,  15,   0,
       0,  15, -10,  -5,  -5,  -5,  -5, -10,  15,   0,
       0,  15, -10,  -5,  -5,  -5,  -5, -10,  15,   0,
       0,  15, -10,   5,  -5,  -5,   5, -10,  15,   0,
       0, -50, -80, -10, -10, -10, -10, -80, -50,   0,
       0, 100, -50,  15,  15,  15,  15, -50, 100,   0,
       0,   0,   0,   0,   0,   0,   0,   0,   0,   0
    }};

int CalcValue::calc_table(const board::Board& board, board::State state) {
  using board::xyToPos;
  int value = 0;
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      if (board[xyToPos(j, i)] != board::State::NONE) {
        value += ((board[xyToPos(j, i)] == state) ?
            (value_table_[xyToPos(j, i)]) : (-value_table_[xyToPos(j, i)]));
      }
    }
  }
  return value;
}

} // namespace value
} // namespace othello
