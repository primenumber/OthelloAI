#include "GameState.hpp"
#include <algorithm>

namespace othello {
namespace gamestate {

GameState& GameState::Put(const Position position) {
  basic_GameState::Put(position, state_);
  state_ = InvertState(state_);
  recent_position_ = position;
  return *this;
}

GameState& GameState::Pass() {
  state_ = InvertState(state_);
  recent_position_.reset();
  return *this;
}

std::vector<GameState> GameState::NextGameStates() const {
  uint64_t puttable_bits = GetPuttable();
  if (puttable_bits != 0) {
    std::vector<GameState> result;
    for (int i = 0; i < 64; ++i) {
      if (puttable_bits & ((UINT64_C(1) << i))) {
        result.emplace_back(*this);
        result.back().Put(static_cast<Position>(i));
      }
    }
    return result;
  } else {
    return std::vector<GameState>(1, GameState(*this).Pass());
  }
}

int GameState::NegaMax(const int depth,
    const std::function<int (const GameState&)>& value_func) const {
  if (depth == 0) {
    return value_func(*this);
  } else {
    std::vector<GameState> next = NextGameStates();
    if (!recent_position_ && !next.front().recent_position_) {
      return value_func(*this);
    } else {
      int value_max = -kValueMax;
      for (GameState state : next) {
        int value = -state.NegaMax(depth - 1, value_func);
        value_max = std::max(value_max, value);
      }
      return value_max;
    }
  }
}

int GameState::AlphaBeta(const int depth,
    const std::function<int (const GameState&)>& value_func,
    int alpha, const int beta) const {
  if (depth == 0) {
    return value_func(*this);
  } else {
    std::vector<GameState> next = NextGameStates();
    if (!recent_position_ && !next.front().recent_position_) {
      return value_func(*this);
    } else {
      for (GameState state : next) {
        int value = -state.AlphaBeta(depth - 1, value_func, -beta, -alpha);
        alpha = std::max(alpha, value);
        if (alpha >= beta)
          return alpha;
      }
      return alpha;
    }
  }
}

boost::optional<Position> GameState::GetOptimalPut(const int depth,
    const std::function<int (const GameState&)>& value_func) const {
  std::vector<GameState> next = NextGameStates();
  boost::optional<Position> position;
  int value_max = -kValueMax;
  for (GameState state : next) {
    int value = -state.AlphaBeta(depth - 1, value_func, -kValueMax, -value_max);
    if (value > value_max) {
      value_max = value;
      position = state.recent_position_;
    }
  }
  return position;
}

} // namespace gamestate
} // namespace othello

