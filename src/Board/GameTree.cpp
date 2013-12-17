#include "GameState.hpp"
#include <algorithm>

namespace othello {
namespace gamestate {

std::unique_ptr<GameTree> GameTree::Put(const Position position) {
  if(children_.empty())
    MakeChildren();
  if (children_.front()->IsPassed()) {
    std::cerr << "called GameTree::Put but no puttable position" << std::endl;
    return Pass();
  }
  for (auto& child : children_) {
    if (*(child->recent_position_) == position) {
      std::unique_ptr<GameTree> next = std::unique_ptr<GameTree>(child.release());
      return next;
    }
  }
  std::cerr << "called GameTree::Put but designated unputtable position" << std::endl;
  return Pass();
}

std::unique_ptr<GameTree> GameTree::Pass() {
  std::unique_ptr<GameTree> next =
      std::unique_ptr<GameTree>(children_.front().release());
  return next;
}

std::vector<std::unique_ptr<GameTree>>& GameTree::GetChildren() {
  if(children_.empty())
    MakeChildren();
  return children_;
}

int GameTree::SearchTree(const int tree_depth, const int search_depth,
    const std::function<int (const GameState&)>& value_func) {
  return SearchTreeImpl(tree_depth, search_depth, value_func, -kValueMax,
                        kValueMax);
}

void GameTree::MakeChildren() {
  uint64_t puttable_bits = GetPuttable();
  if (puttable_bits != 0) {
    for (int i = 0; i < 64; ++i) {
      if (puttable_bits & ((UINT64_C(1) << i))) {
        GameState child_state = GameState(*this).Put(i);
        children_.emplace_back(new GameTree(child_state));
      }
    }
  } else {
    GameState child_state = GameState(*this).Pass();
    children_.emplace_back(new GameTree(child_state));
  }
}

int GameTree::SearchTreeImpl(const int tree_depth, const int search_depth,
    const std::function<int (const GameState&)>& value_func,
    int alpha, const int beta) {
  if (tree_depth == 0) {
    return value_ = AlphaBeta(search_depth, value_func, alpha, beta);
  } else {
    if(children_.empty()) {
      MakeChildren();
      for (auto& child : children_)
        child->value_ = value_func(*child);
      sort(children_.begin(), children_.end());
    }
    auto itr = children_.begin();
    for (; itr != children_.end(); ++itr) {
      int value = -((*itr)->SearchTreeImpl(tree_depth - 1, search_depth,
          value_func, -beta, -alpha));
      if (value >= alpha) {
        alpha = value;
        if (alpha >= beta)
          break;
      }
    }
    sort(children_.begin(), itr);
    return value_ = alpha;
  }
}

bool operator<(const std::unique_ptr<GameTree>& lhs,
               const std::unique_ptr<GameTree>& rhs) {
  return lhs->GetValue() < rhs->GetValue();
}

} // namespace gamestate
} // namespace othello

