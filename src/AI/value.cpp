#include "value.hpp"
namespace othello {
namespace value {

constexpr std::array<int, board::kboard_table_size> value_table_ = 
    std::array<int, board::kboard_table_size>{{
       0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
       0, 100, -50,  15,  15,  15,  15, -50, 100,   0,
       0, -50, -50, -10, -10, -10, -10, -50, -50,   0,
       0,  15, -10,   5,  -5,  -5,   5, -10,  15,   0,
       0,  15, -10,  -5,  -5,  -5,  -5, -10,  15,   0,
       0,  15, -10,  -5,  -5,  -5,  -5, -10,  15,   0,
       0,  15, -10,   5,  -5,  -5,   5, -10,  15,   0,
       0, -50, -50, -10, -10, -10, -10, -50, -50,   0,
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

constexpr int settlements_value = 15;
constexpr int edge_value[] = {0,0,0,0,15,-15,15};

std::array<int, 2> stone_num_line(const board::Board& board,
    const int sx, const int sy, const int dx, const int dy) {
  using board::xyToPos;
  using board::State;
  std::array<int, 2> stone_num = {{0, 0}};
  for (int j = 1; j < 7; ++j) {
    switch (board[xyToPos(sx+dx*j, sy+dy*j)]) {
     case State::BLACK:
      ++stone_num[0];
      break;
     case State::WHITE:
      ++stone_num[1];
      break;
     default:;
    }
  }
  return stone_num;
}

int CalcValue::calc_edge(const board::Board& board, const board::State state,
                         const int stones) {
  using board::xyToPos;
  using board::State;
  int value = 0;
  int sx[4] = {0,7,7,0};
  int sy[4] = {0,0,7,7};
  int dx[4] = {1,0,-1,0};
  int dy[4] = {0,1,0,-1};
  for (int i = 0; i < 4; ++i) {
    bool continuity_none[2] = {false, false};
    board::Position pos = xyToPos(sx[i], sy[i]);
    
    std::array<int, 2> stone_num;
    if(stones < 48)
      stone_num = stone_num_line(board, sx[i], sy[i], dx[i], dy[i]);
    int continuity_num[2] = {1, 1};
    for (int j = 1; j < 7; ++j) {
      if (board[pos] != board[xyToPos(sx[i]+dx[i]*j, sy[i]+dy[i]*j)])
        break;
      ++continuity_num[0];
    }
    if (board[pos] == State::NONE) {
      continuity_none[0] = true;
    } else {
      int addvalue = continuity_num[0] * settlements_value;
      if(state == State::WHITE)
        addvalue = -addvalue;
      if(board[pos] == State::WHITE)
        addvalue = -addvalue;
      value += addvalue;
    }
    pos = xyToPos(sx[i]+dx[i]*7, sy[i]+dy[i]*7);
    for (int j = 1; j < 7; ++j) {
      if (board[pos] != board[xyToPos(sx[i]+dx[i]*(7-j), sy[i]+dy[i]*(7-j))])
        break;
      ++continuity_num[1];
    }
    if (board[pos] == State::NONE) {
      continuity_none[1] = true;
    } else {
      int addvalue = continuity_num[1] * settlements_value;
      if(state == State::WHITE)
        addvalue = -addvalue;
      if(board[pos] == State::WHITE)
        addvalue = -addvalue;
      value += addvalue;
    }
    if (stones < 48 && continuity_none[0] && continuity_none[1]) {
      if(continuity_num[0] <= 2 && continuity_num[1] <= 2) {
        int addvalue = 1;
        int sum_continuity = continuity_num[0] + continuity_num[1];
        int sum = 0;
        if (sum_continuity + stone_num[0] == 8) {
          sum = stone_num[0];
        } else if (sum_continuity + stone_num[1] == 8) {
          sum = stone_num[1];
          addvalue = -1;
        }
        if (state == State::WHITE)
          addvalue *= -1;
        addvalue *= edge_value[sum];
        value += addvalue;
      }
    }
  }
  return value;
}

constexpr int value_puttable = 3;

int CalcValue::calc_puttable(const board::Board& board,
                             const board::State state) {
  return getPuttable(board, state).size() * value_puttable;
}

} // namespace value
} // namespace othello
