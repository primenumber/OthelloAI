#pragma once
#include <algorithm>
#include "Board/board.hpp"
#include "value.hpp"

namespace othello {
namespace ai {

int CalcWinPoint(const board::Board& board, const board::State state);

int dfs(const board::Board& board, const board::State state,
        const int depth, int alpha, const int beta,
        const bool pass = false);

} // namespace ai
} // namespace othello
