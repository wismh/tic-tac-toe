#include <game/bot_ai.h>

#include <array>
#include <tuple>

namespace game {

namespace {

[[nodiscard]] Mark opposite(Mark mark) {
    return mark == Mark::X ? Mark::O : Mark::X;
}

// Returns a cell that completes a line of `mark`, or {-1, -1} if no such cell exists.
[[nodiscard]] std::pair<int, int> threat_cell(const Board& board, Mark mark) {
    constexpr int kSize = Board::kSize;
    int x = -1;
    int y = -1;

    for (int iy = 0; iy < kSize; ++iy) {
        if (board.at(1, iy) == mark && board.at(2, iy) == mark && board.empty_at(0, iy)) {
            x = 0;
            y = iy;
        }
        if (board.at(0, iy) == mark && board.at(2, iy) == mark && board.empty_at(1, iy)) {
            x = 1;
            y = iy;
        }
        if (board.at(0, iy) == mark && board.at(1, iy) == mark && board.empty_at(2, iy)) {
            x = 2;
            y = iy;
        }
    }

    for (int ix = 0; ix < kSize; ++ix) {
        if (board.at(ix, 1) == mark && board.at(ix, 2) == mark && board.empty_at(ix, 0)) {
            x = ix;
            y = 0;
        }
        if (board.at(ix, 0) == mark && board.at(ix, 2) == mark && board.empty_at(ix, 1)) {
            x = ix;
            y = 1;
        }
        if (board.at(ix, 0) == mark && board.at(ix, 1) == mark && board.empty_at(ix, 2)) {
            x = ix;
            y = 2;
        }
    }

    if (board.at(0, 0) == mark && board.at(1, 1) == mark && board.empty_at(2, 2)) {
        x = 2;
        y = 2;
    }
    if (board.at(0, 0) == mark && board.at(2, 2) == mark && board.empty_at(1, 1)) {
        x = 1;
        y = 1;
    }
    if (board.at(1, 1) == mark && board.at(2, 2) == mark && board.empty_at(0, 0)) {
        x = 0;
        y = 0;
    }

    if (board.at(0, 2) == mark && board.at(1, 1) == mark && board.empty_at(2, 0)) {
        x = 2;
        y = 0;
    }
    if (board.at(0, 2) == mark && board.at(2, 0) == mark && board.empty_at(1, 1)) {
        x = 1;
        y = 1;
    }
    if (board.at(1, 1) == mark && board.at(2, 0) == mark && board.empty_at(0, 2)) {
        x = 0;
        y = 2;
    }

    return {x, y};
}

}

std::pair<int, int> choose_bot_move(const Board& board, Mark mark, std::mt19937& rng) {
    constexpr int kSize = Board::kSize;
    const Mark blocking_mark = opposite(mark);

    auto [x, y] = threat_cell(board, mark);
    if (x == -1) {
        std::tie(x, y) = threat_cell(board, blocking_mark);
    }

    if (x == -1) {
        if (board.empty_at(1, 1)) {
            x = 1;
            y = 1;
        } else {
            const auto [last_x, last_y] = board.last_move();
            if ((last_x == 1 && last_y == 0) || (last_x == 1 && last_y == 2) || (last_x == 0 && last_y == 1) ||
                    (last_x == 2 && last_y == 1)) {
                if (board.empty_at(0, 0)) {
                    x = 0;
                    y = 0;
                }
                if (board.empty_at(0, 2)) {
                    x = 0;
                    y = 2;
                }
                if (board.empty_at(2, 0)) {
                    x = 2;
                    y = 0;
                }
                if (board.empty_at(2, 2)) {
                    x = 2;
                    y = 2;
                }
            }
        }
    }

    if (x == -1) {
        std::array<std::pair<int, int>, static_cast<std::size_t>(kSize * kSize)> empties{};
        int count = 0;
        for (int ix = 0; ix < kSize; ++ix) {
            for (int iy = 0; iy < kSize; ++iy) {
                if (board.empty_at(ix, iy)) {
                    empties[static_cast<std::size_t>(count)] = {ix, iy};
                    ++count;
                }
            }
        }
        if (count == 0) {
            return {-1, -1};
        }
        std::uniform_int_distribution<int> dist(0, count - 1);
        std::tie(x, y) = empties[static_cast<std::size_t>(dist(rng))];
    }

    return {x, y};
}

}
