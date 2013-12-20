#include "GameState.hpp"

namespace othello {
namespace gamestate {

State InvertState(const State state) {
  return (state == State::BLACK) ? State::WHITE : State::BLACK;
}

bool IsBlack(const State state) {
  return state == State::BLACK;
}

bool IsWhite(const State state) {
  return state == State::WHITE;
}

} // namespace gamestate
} // namespace othello

