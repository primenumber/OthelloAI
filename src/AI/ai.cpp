#include <algorithm>
#include <array>
#include <iostream>
#include <map>
#include <vector>
#include <utility>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "Board/board.hpp"
#include "value.hpp"

#define SLOW_ENV

namespace othello {
namespace ai {

int CalcWinPoint(const board::Board& board, const board::State state) {
  auto num = countBoard(board); 
  if (state == board::State::BLACK) {
    return (num.first - num.second) * 1000;
  } else {
    return (num.second - num.first) * 1000;
  }
}

int dfs(const board::Board& board, const board::State state, const int depth,
        int alpha, const int beta, const bool pass = false) {
  using othello::board::Position;
  using othello::board::getPuttable;
  using othello::board::invertState;
  if (depth > 0) {
    std::vector<Position> puttable_list = getPuttable(board, state);
    if (puttable_list.empty()) {
      if (pass) {
        return CalcWinPoint(board, state);
      } else {
        return -dfs(board, invertState(state), depth, -beta, -alpha, true);
      }
    }
    for (Position puttable : puttable_list) {
      int value = -dfs(put(board, puttable, state), invertState(state), depth-1,
                       -beta, -alpha);
      alpha = std::max(alpha, value);
      if (alpha >= beta)
        return alpha;
    }
    return alpha;
  } else {
    value::CalcValue cv;
    return cv(board, state);
  }
}

} // namespace ai
} // namespace othello

#ifdef SLOW_ENV

constexpr std::array<int, 64> dfs_depth_array{{
  0, 0, 0, 0,
  0, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
 10, 9, 8, 7, 6, 6, 6, 6, 6, 6
}};

#else

constexpr std::array<int, 64> dfs_depth_array{{
  0, 0, 0, 0,
  0, 0, 8, 7, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6,16,15,14,13,12,11,
 10, 9, 8, 7, 6, 5, 4, 4, 4, 4
}};

#endif

int main() {
  using othello::board::Board;
  using othello::board::toBoard;
  using othello::board::Position;
  using othello::board::State;
  using othello::board::xyToPos;
  using othello::board::posToXY;
  using othello::board::invertState;
  using othello::board::getPuttable;
  using std::cin;
  using std::cout;
  using std::endl;
  using othello::ai::dfs;
  srand(getpid());
  char buf[81];
  sprintf(buf, "ai01_log_%d.log", getpid());
  FILE* fp = fopen(buf, "w");
  std::string player;
  cin >> player;
  State state;
  if (player == "x") {
    fprintf(fp, "black\n");
    state = State::BLACK;
  } else {
    fprintf(fp, "white\n");
    state = State::WHITE;
  }
  while (1) {
    std::string rt_str;
    long long remain_time;
    cin >> rt_str;
    fprintf(fp, "remain:%s\n", rt_str.c_str());
    remain_time = atoll(rt_str.c_str());
    if (remain_time == 0)
      break;
    std::string board_str;
    for (int i = 0; i < 8; ++i) {
      std::string str;
      cin >> str;
      board_str += str + '\n';
    }
    Board board = toBoard(board_str);
    auto stones = countBoard(board);
    int stone_num = stones.first + stones.second;
    Position pos;
    if (stone_num == 4) {
      cout << "f5" << endl;
    } else {
      std::vector<Position> puttable_list = getPuttable(toBoard(board_str), state);
      int alpha = -1000000;
      const int beta = 1000000;
      for (Position puttable : puttable_list) {
        int value = -dfs(put(toBoard(board_str), puttable, state),
                         invertState(state), dfs_depth_array[stone_num],
                         -beta, -alpha);
        if (value > alpha) {
          pos = puttable;
          alpha = value;
        }
      }
      char col = posToXY(pos).first + 'a';
      char row = posToXY(pos).second + '1';
      cout << col << row << endl;
    }
    fprintf(fp, "%c%c\n", col, row);
  }
  fprintf(fp, "--game set--\n");
  fclose(fp);
  return 0;
}

