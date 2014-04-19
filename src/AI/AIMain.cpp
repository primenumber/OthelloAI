#include <algorithm>
#include <array>
#include <chrono>
#include <iostream>
#include <map>
#include <vector>
#include <utility>
#include <boost/optional.hpp>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
//#include "Board/board.hpp"
//#include "value.hpp"
//#include "gameTree.hpp"
//#include "dfs.hpp"
#include "Board/GameState.hpp"

// primeOthello2
//
// OthelloGameServer2 に対応
// primeOthello2 in_pipe out_pipe
// 第一引数に入力用名前付きパイプ
// 第二引数に出力用名前付きパイプ
// 入力には現在の盤面の状態、残り持ち時間(ns)とどちらが何を打ったか(パスも含めて)が入力される
// 例)
// x d3
// 125000000000
// ........
// ........
// ...x....
// ...xx...
// ...xo...
// ........
// ........
// ........
// 出力:どこに置くか(自分の番でない/パスのときは何もしない)
// 例)
// f7

namespace othello {
namespace ai {

constexpr std::array<int, 64> table = {
  100,-50, 15, 15, 15, 15,-50,100,
  -50,-70,-10,-10,-10,-10,-70,-50,
   15,-10,  0, -5, -5,  0,-10, 15,
   15,-10, -5, -5, -5, -5,-10, 15,
   15,-10, -5, -5, -5, -5,-10, 15,
   15,-10,  0, -5, -5,  0,-10, 15,
  -50,-70,-10,-10,-10,-10,-70,-50,
  100,-50, 15, 15, 15, 15,-50,100
};

// TODO : GameTreeの適当なラッパと評価関数を書く
class GameSearch {
 public:
  GameSearch() : tree_depth(7), search_depth(2), game_tree_(new GameTree()) {
    if (!initialized) InitValueTable();
  }
  void Search(const std::chrono::nanoseconds& search_time);
  Enemy_Put(const boost::optional<Position>& position);
  My_Put();
 private:
  int tree_depth_;
  int search_depth_;
  std::unique_ptr<GameTree> game_tree_;
  static bool initialized;
  static std::array<std::array<std::array<int, 8>, 256>, 256> value_table;
  static void InitValueTable()
  static int value_func(const gamestate::GameState& game_state) const;
  static int value_func_count(const gamestate::GameState& game_state) const;
};

std::array<std::array<std::array<int, 8>, 256>, 256> GameSearch::value_table;
bool GameSearch::initialized = false;

GameSearch::Search(const std::chrono::nanoseconds& search_limit) {
  auto search_begin_time = std::chrono::system_clock::now();
  for (tree_depth_ += 2; tree_depth_ <= 20; tree_depth_ += 2) {
    std::pair<int, int> stones_num = game_tree_->CountStones();
    if (tree_depth_ + search_depth_
        + stones_num.first + stones_num.second < 64)
      game_tree_->SearchTree(tree_depth_, search_depth_, value_func);
    else
      game_tree_->SearchTree(tree_depth_, search_depth_, value_func_count);
    auto now_time = std::chrono::system_clock::now();
    if (now_time - search_begin_time >= search_limit)
      break;
  }
}

GameSearch::Enemy_Put(const boost::optional<Position>& opt_position) {
  if (opt_position) {
    game_tree_.reset(game_tree_->Put(*opt_position).release());
    pass = false;
  } else if (!pass) {
    game_tree_.reset(game_tree_->Pass().release());
    pass = true;
  }
  --tree_depth;
}

GameSearch::My_Put() {
  Position opt_position = *(game_tree_->GetChildren().front());
  if (opt_position) {
    game_tree_.reset(game_tree_->Put(*opt_position).release());
    pass = false;
  } else if (!pass) {
    game_tree_.reset(game_tree_->Pass().release());
    pass = true;
  }
  --tree_depth;
}

int GameSearch::value_func(const gamestate::GameState& game_state) {
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
  value += othello::bitwise::CountBits(game_state.GetPuttable())*6;
  value -= othello::bitwise::CountBits(game_state.GetPuttable(
      othello::gamestate::InvertState(game_state.GetState())))*6;
  return value;
}

int GameSearch::value_func_count(const gamestate::GameState& game_state) {
  using othello::gamestate::State;
  std::pair<int, int> stones = game_state.CountStones();
  if (game_state.GetState() == State::BLACK)
    return (stones.first - stones.second) * 40;
  else
    return (stones.second - stones.first) * 40;
}

void GameSearch::InitValueTable() {
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

} // namespace ai
} // namespace othello

int main() {
  othello::ai::GameSearch game_search;
  othello::gamestate::State = state;
  std::string player;
  cin >> player;
  State state;
  if (player == "x") {
    state = State::BLACK;
  } else {
    state = State::WHITE;
  }
  bool pass = false;
  while (true) {
  }
  return 0;
}

