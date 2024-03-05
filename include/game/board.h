#pragma once

#include <array>
#include <random>
#include <utility>

namespace game {

enum class Mark : int { Empty = 0, X = 1, O = 2 };

enum class Outcome : int { Playing = 0, X = 1, O = 2, Draw = 3 };

enum class WinLine : int { None = 0, Row0, Row1, Row2, Col0, Col1, Col2, Diag, AntiDiag };

class Board {
public:
    static constexpr int kSize = 3;

    void reset_round();
    void clear_scores();

    void set_bot_flags(bool x_is_bot, bool o_is_bot);

    [[nodiscard]] bool empty_at(int x, int y) const;
    // Out-of-range coordinates return Mark::Empty rather than asserting, mirroring empty_at's existing
    // bounds-checked-false behavior: it keeps the two query methods consistent and avoids turning a Debug-only
    // assert (which on this toolchain pops a blocking dialog) into a way to hang an automated test run.
    [[nodiscard]] Mark at(int x, int y) const;

    [[nodiscard]] Mark current_mark() const { return current_mark_; }
    [[nodiscard]] Outcome outcome() const { return outcome_; }
    [[nodiscard]] WinLine winning_line() const { return winning_line_; }
    [[nodiscard]] bool current_is_bot() const;

    // The last cell placed on this round (by either mark), or {-1, -1} before any move. Public so
    // bot AI logic can factor in move history without needing access to Board's private state.
    [[nodiscard]] std::pair<int, int> last_move() const { return {last_x_, last_y_}; }

    [[nodiscard]] int score_x() const { return score_x_; }
    [[nodiscard]] int score_o() const { return score_o_; }
    void award_point();

    bool try_place(int x, int y);
    bool step_bot(std::mt19937& rng);

private:
    bool place_current(int x, int y);

    std::array<std::array<Mark, kSize>, kSize> map_{};
    Mark current_mark_ = Mark::X;
    Outcome outcome_ = Outcome::Playing;
    WinLine winning_line_ = WinLine::None;

    int score_x_ = 0;
    int score_o_ = 0;
    bool o_is_bot_ = true;
    bool x_is_bot_ = false;

    int last_x_ = -1;
    int last_y_ = -1;
};

}
