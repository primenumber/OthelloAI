#include <algorithm>
#include <array>
#include <chrono>
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
#include "gameTree.hpp"
#include "dfs.hpp"

FILE* fp;

namespace othello {
namespace ai {

class Search {
 public:
  Search(board::State player_state)
      : board_(), player_state_(player_state), now_state_(board::State::BLACK),
        optimal_(),
        game_tree_root_(new GameTree(board_, board::State::BLACK, board::nullpos,
                                     CalcValue, CalcWinPoint)),
        depth(0) { Calc(std::chrono::milliseconds(100)); }
  void Put(board::Position);
  void Calc(std::chrono::milliseconds calc_time);
  board::Position GetOptimalPosition() { return optimal_; }
  const board::Board& GetBoard() const { return board_; }
 private:
  int dfs(const board::Board& board, const board::State state, const int depth,
          int alpha, const int beta, const bool pass = false);
  static int CalcWinPoint(const board::Board& board, const board::State state);
  static int CalcValue(const board::Board& board, const board::State state) {
    value::CalcValue cv;
    return cv(board, state);
  }
  board::Board board_;
  board::State player_state_;
  board::State now_state_;
  board::Position optimal_;
  std::unique_ptr<GameTree> game_tree_root_;
  int depth;
};

void Search::Put(board::Position position) {
  using board::toStr;
  if(position != board::nullpos)
    board_ = board::put(board_, position, now_state_);
  now_state_ = board::invertState(now_state_);
  --depth;
  for(int i = 0; i < game_tree_root_->children_.size(); ++i) {
    if(game_tree_root_->children_[i]->position_ == position) {
      GameTree* tmp = game_tree_root_->children_[i].release();
      game_tree_root_.reset(tmp);
      break;
    }
  }
}

int Search::CalcWinPoint(const board::Board& board, const board::State state) {
  auto num = countBoard(board); 
  if (state == board::State::BLACK) {
    return (num.first - num.second) * 1000;
  } else {
    return (num.second - num.first) * 1000;
  }
}

void Search::Calc(std::chrono::milliseconds calc_time) {
  using board::Position;
  using board::xyToPos;
  std::vector<Position> pos_list = getPuttable(board_, player_state_);
  auto begin_search = std::chrono::system_clock::now();
  auto stones = countBoard(board_);
  int stone_nums = stones.first + stones.second;
  if (stone_nums <= 50) {
    for (; depth <= 20; ++depth) {
      int alpha = -1000000;
      const int beta = 1000000;
      game_tree_root_->Search(depth, -beta, -alpha, false);
      auto now = std::chrono::system_clock::now();
      if (now - begin_search > calc_time / 10)
        break;
    }
  } else {
    int alpha = -1000000;
    const int beta = 1000000;
    game_tree_root_->Search(14, -beta, -alpha, false);
  }
  fprintf(fp ,"search:calc depth:%d\n", depth);
  if (player_state_ == now_state_)
    optimal_ = game_tree_root_->children_[0]->position_;
  else
    optimal_ = game_tree_root_->children_[0]->children_[0]->position_;
}

} // namespace ai
} // namespace othello

othello::board::Position GetPut(const othello::board::Board& board_new,
                                const othello::board::Board& board_old) {
  using othello::board::Position;
  using othello::board::State;
  using othello::board::xyToPos;
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      Position pos = xyToPos(j, i);
      if (board_old[pos] == State::NONE && board_new[pos] != State::NONE)
        return pos;
    }
  }
  return othello::board::nullpos;
}

int main() {
  using othello::board::Board;
  using othello::board::toBoard;
  using othello::board::Position;
  using othello::board::State;
  using othello::board::xyToPos;
  using othello::board::posToXY;
  using othello::board::invertState;
  using othello::board::getPuttable;
  using othello::board::toStr;
  using std::cin;
  using std::cout;
  using std::endl;
  using std::chrono::duration_cast;
  using std::chrono::milliseconds;
  using std::chrono::nanoseconds;
  using othello::ai::Search;
  srand(getpid());
  char buf[81];
  sprintf(buf, "ai01_log_%d.log", getpid());
  fp = fopen(buf, "w");
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
  Search search(state);
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
      fprintf(fp, "f5\n");
      search.Put(xyToPos(5, 4));
    } else {
      std::vector<Position> pos_list = getPuttable(toBoard(board_str), state);
      Position enemy_put = GetPut(board, search.GetBoard());
      search.Put(enemy_put);
      int remain_num = 64 - stone_num;
      search.Calc(duration_cast<milliseconds>(
          nanoseconds(remain_time)/remain_num));
      pos = search.GetOptimalPosition();
      cout << toStr(pos) << endl;
      fprintf(fp, "%s\n", toStr(pos).c_str());
      search.Put(pos);
    }
  }
  fprintf(fp, "--game set--\n");
  fclose(fp);
  return 0;
}

