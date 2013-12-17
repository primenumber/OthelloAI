#include "GameState.hpp"

namespace othello {
namespace gamestate {

std::pair<uint8_t, uint8_t> Base3ToUint8Pair(int base3) {
  uint8_t black_state = 0;
  uint8_t white_state = 0;
  for (int i = 0; i < 8; ++i) {
    int r = base3 % 3;
    if (r == 1)
      black_state |= 1 << i;
    else if (r == 2)
      white_state |= 1 << i;
    base3 /= 3;
  }
  return std::make_pair(black_state, white_state);
}

} // namespace gamestate
} // namespace othello
