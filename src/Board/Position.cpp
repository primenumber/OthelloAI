#include "GameState.hpp"

namespace othello {
namespace gamestate {

std::pair<int, int> Pos2XY(const Position position) {
  return std::pair<int, int>(position % 8, position / 8);
}

int Pos2X(const Position position) {
  return static_cast<int>(position % 8);
}

int Pos2Y(const Position position) {
  return static_cast<int>(position / 8);
}

} // namespace gamestate
} // namespace othello

