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

std::string ToString(const boost::optional<Position>& position) {
  if (position) {
    auto xy_pair = Pos2XY(*position);
    std::string result;
    result += static_cast<char>('a' + xy_pair.first);
    result += static_cast<char>('1' + xy_pair.second);
    return result;
  } else {
    return "pass"; 
  }
}

} // namespace gamestate
} // namespace othello

