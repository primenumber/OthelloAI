#include "dfs.hpp"

namespace othello {
namespace ai {

int CalcWinPoint(const board::Board& board, const board::State state) {
  using board::countBoard;
  auto num = countBoard(board); 
  if (state == board::State::BLACK) {
    return (num.first - num.second) * 1000;
  } else {
    return (num.second - num.first) * 1000;
  }
}

int dfs(const board::Board& board, const board::State state,
        const int depth, int alpha, const int beta,
        const bool pass, const int stones) {
  using othello::board::Position;
  using othello::board::getPuttable;
  using othello::board::invertState;
  using othello::board::State;
  using othello::value::CalcValue;
  if (depth > 0) {
    std::vector<Position> pos_list = getPuttable(board, state);
    if (pos_list.empty()) {
      if (pass) {
        return CalcWinPoint(board, state);
      } else {
        return -dfs(board, invertState(state), depth, -beta, -alpha, true,
                    stones);
      }
    }
    for (Position puttable : pos_list) {
      int value = -dfs(put(board, puttable, state), invertState(state), depth-1,
                       -beta, -alpha, false, stones + 1);
      alpha = std::max(alpha, value);
      if (alpha >= beta)
        return alpha;
    }
    return alpha;
  } else {
    return CalcValue(board, state, stones);
  }
}

} // namespace ai
} // namespace othello
