#include <game/board.h>

#include <game/bot_ai.h>

namespace game {

namespace {

[[nodiscard]] Mark opposite(Mark mark) {
    return mark == Mark::X ? Mark::O : Mark::X;
}

}

void Board::reset_round() {
    for (auto& col : map_) {
        col.fill(Mark::Empty);
    }
    current_mark_ = Mark::X;
    outcome_ = Outcome::Playing;
    winning_line_ = WinLine::None;
    last_x_ = -1;
    last_y_ = -1;
}

void Board::clear_scores() {
    score_x_ = 0;
    score_o_ = 0;
}

void Board::set_bot_flags(bool x_is_bot, bool o_is_bot) {
    x_is_bot_ = x_is_bot;
    o_is_bot_ = o_is_bot;
}

bool Board::empty_at(int x, int y) const {
    if (x < 0 || y < 0 || x >= kSize || y >= kSize) {
        return false;
    }
    return map_[static_cast<std::size_t>(x)][static_cast<std::size_t>(y)] == Mark::Empty;
}

Mark Board::at(int x, int y) const {
    if (x < 0 || y < 0 || x >= kSize || y >= kSize) {
        return Mark::Empty;
    }
    return map_[static_cast<std::size_t>(x)][static_cast<std::size_t>(y)];
}

bool Board::current_is_bot() const {
    return (current_mark_ == Mark::X && x_is_bot_) || (current_mark_ == Mark::O && o_is_bot_);
}

void Board::award_point() {
    if (outcome_ == Outcome::X) {
        ++score_x_;
    } else if (outcome_ == Outcome::O) {
        ++score_o_;
    }
}

bool Board::place_current(int x, int y) {
    map_[static_cast<std::size_t>(x)][static_cast<std::size_t>(y)] = current_mark_;
    last_x_ = x;
    last_y_ = y;

    static constexpr WinLine kRowLines[kSize] = {WinLine::Row0, WinLine::Row1, WinLine::Row2};
    for (int y2 = 0; y2 < kSize; ++y2) {
        if (map_[0][static_cast<std::size_t>(y2)] != Mark::Empty &&
                map_[0][static_cast<std::size_t>(y2)] == map_[1][static_cast<std::size_t>(y2)] &&
                map_[1][static_cast<std::size_t>(y2)] == map_[2][static_cast<std::size_t>(y2)]) {
            outcome_ = static_cast<Outcome>(map_[0][static_cast<std::size_t>(y2)]);
            winning_line_ = kRowLines[y2];
            return true;
        }
    }
    static constexpr WinLine kColLines[kSize] = {WinLine::Col0, WinLine::Col1, WinLine::Col2};
    for (int x2 = 0; x2 < kSize; ++x2) {
        if (map_[static_cast<std::size_t>(x2)][0] != Mark::Empty &&
                map_[static_cast<std::size_t>(x2)][0] == map_[static_cast<std::size_t>(x2)][1] &&
                map_[static_cast<std::size_t>(x2)][1] == map_[static_cast<std::size_t>(x2)][2]) {
            outcome_ = static_cast<Outcome>(map_[static_cast<std::size_t>(x2)][0]);
            winning_line_ = kColLines[x2];
            return true;
        }
    }
    if (map_[0][0] != Mark::Empty && map_[0][0] == map_[1][1] && map_[1][1] == map_[2][2]) {
        outcome_ = static_cast<Outcome>(map_[0][0]);
        winning_line_ = WinLine::Diag;
        return true;
    }
    if (map_[0][2] != Mark::Empty && map_[0][2] == map_[1][1] && map_[1][1] == map_[2][0]) {
        outcome_ = static_cast<Outcome>(map_[0][2]);
        winning_line_ = WinLine::AntiDiag;
        return true;
    }

    bool full = true;
    for (int x2 = 0; x2 < kSize && full; ++x2) {
        for (int y2 = 0; y2 < kSize; ++y2) {
            if (map_[static_cast<std::size_t>(x2)][static_cast<std::size_t>(y2)] == Mark::Empty) {
                full = false;
                break;
            }
        }
    }

    if (full) {
        outcome_ = Outcome::Draw;
    } else {
        outcome_ = Outcome::Playing;
        current_mark_ = opposite(current_mark_);
    }
    return true;
}

bool Board::try_place(int x, int y) {
    if (outcome_ != Outcome::Playing) {
        return false;
    }
    if (!empty_at(x, y)) {
        return false;
    }
    return place_current(x, y);
}

bool Board::step_bot(std::mt19937& rng) {
    if (outcome_ != Outcome::Playing) {
        return false;
    }

    const auto [x, y] = choose_bot_move(*this, current_mark_, rng);
    if (x == -1) {
        return false;
    }
    return place_current(x, y);
}

}
