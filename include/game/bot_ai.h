#pragma once

#include <game/board.h>

#include <random>
#include <utility>

namespace game {

// Chooses a cell for `mark` to play on `board`: take an immediate win if one exists, else block
// the opponent's immediate win, else prefer the center, else prefer a corner right after an edge
// was just played, else fall back to a uniformly random empty cell drawn from `rng`. Returns
// {-1, -1} if no empty cell exists. Reads `board` only through its public API (at/empty_at/
// last_move/kSize), so it needs no special access to Board's internals and can be tested with
// any Board state constructed through the normal public API.
[[nodiscard]] std::pair<int, int> choose_bot_move(const Board& board, Mark mark, std::mt19937& rng);

}
