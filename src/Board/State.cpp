#include "GameState.hpp"

namespace othello {
namespace gamestate {

State InvertState(const State state) {
  return (state == State::BLACK) ? State::WHITE : State::BLACK;
}

} // namespace gamestate
} // namespace othello

