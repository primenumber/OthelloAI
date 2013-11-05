#pragma once
#include <algorithm>
#include <memory>
#include <vector>
#include "Board/board.hpp"
#include "dfs.hpp"

namespace othello {
namespace ai {

class GameTree {
 public:
  typedef std::function<int (const board::Board&, const board::State)> vfunc;
  GameTree(const board::Board& board, const board::State state,
      const board::Position position, const vfunc& value_func,
      const vfunc& win_point_func)
      : children_(), board_(board), state_(state), position_(position),
        value_(), pass_(false), uncalc_(true), value_func_(value_func),
        win_point_func_(win_point_func) {}
  virtual ~GameTree() {}
  int Search(const int /*depth*/, int /*alpha*/, const int /*beta*/,
             const bool pass);
  int GetValue() const { return value_; }
  std::vector<std::unique_ptr<GameTree>> children_;
  board::Board board_;
  board::State state_;
  board::Position position_;
  int value_;
 private:
  bool pass_;
  bool uncalc_;
  vfunc value_func_;
  vfunc win_point_func_;
};

bool operator<(const GameTree&, const GameTree&);

bool Comp_PtrGameTree(const std::unique_ptr<GameTree>&,
                      const std::unique_ptr<GameTree>&);

} // namespace ai
} // namespace othello
