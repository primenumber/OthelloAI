#pragma once
#include <array>
#include "Board/board.hpp"

namespace othello {
namespace value {

int CalcValue(const board::Board& board, const board::State state,
                const int stones);

} // namespace value
} // namespace othello
