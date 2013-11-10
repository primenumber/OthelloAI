#include "gameTree.hpp"

namespace othello {
namespace ai {

int GameTree::Search(const int depth, int alpha, const int beta,
                     const bool pass) {
  using board::Position;
  using board::getPuttable;
  using board::invertState;
  if (depth <= 0) {
    value_ = value_func_(board_, state_, stones_);
    return value_;
  }
  if (depth <= 3) {
    value_ = dfs(board_, state_, depth, alpha, beta, false, stones_);
    return value_;
  }
  if (uncalc_) {
    std::vector<Position> pos_list = getPuttable(board_, state_);
    if (pos_list.empty()) {
      pass_ = true;
      if (pass) {
        value_ = win_point_func_(board_, state_, stones_);
      } else {
        auto child = std::unique_ptr<GameTree>(new GameTree(board_,
            invertState(state_), stones_, board::nullpos, value_func_,
            win_point_func_));
        value_ = -child->Search(depth, -beta, -alpha, true);
        children_.emplace_back(std::move(child));
      }
    } else {
      auto itr = pos_list.begin();
      for (; itr != pos_list.end(); ++itr) {
        auto child = std::unique_ptr<GameTree>(new GameTree(
            put(board_, *itr, state_), invertState(state_), stones_ + 1, *itr,
            value_func_, win_point_func_));
        int value = -child->Search(depth-1, -beta, -alpha, false);
        children_.emplace_back(std::move(child));
        alpha = std::max(alpha, value);
        if(alpha >= beta) {
          ++itr;
          break;
        }
      }
      int searched_num = itr - pos_list.begin();
      for(; itr != pos_list.end(); ++itr) {
        auto child = std::unique_ptr<GameTree>();
        children_.emplace_back(new GameTree(
            put(board_, *itr, state_), invertState(state_), stones_, *itr,
            value_func_, win_point_func_));
      }
      sort(children_.begin(), children_.begin() + searched_num,
           Comp_PtrGameTree);
      value_ = alpha;
    }
    uncalc_ = false;
  } else {
    auto itr = children_.begin();
    for (;itr != children_.end(); ++itr) {
      int value = -(*itr)->Search(depth-1, -beta, -alpha, false);
      alpha = std::max(alpha, value);
      if (alpha >= beta) {
        ++itr;
        break;
      }
    }
    sort(children_.begin(), itr, Comp_PtrGameTree);
    value_ = alpha;
  }
  return value_;
}

bool operator<(const GameTree& lhs, const GameTree& rhs) {
  return lhs.GetValue() < rhs.GetValue();
}

bool Comp_PtrGameTree(const std::unique_ptr<GameTree>& lhs,
                      const std::unique_ptr<GameTree>& rhs) {
  return *lhs < *rhs;
}

} // namespace ai
} // namespace othello
