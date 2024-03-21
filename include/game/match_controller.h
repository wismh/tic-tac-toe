#pragma once

#include <game/board.h>

#include <random>

namespace game {

enum class Screen { Menu, Play };

// Owns match/round orchestration: which screen is active, the board for the current round, and
// the post-victory pause before a round clears. Pure game logic — no engine, ECS, UI, or audio
// dependency — so it can be constructed and driven directly in tests.
class MatchController {
public:
    // What happened as a result of on_cell_click()/tick(), so Game can decide which UI/audio
    // side effects to fire (step sfx, HUD refresh) without MatchController knowing UI or audio
    // exist. Moved covers "a mark was placed" (human or bot, win or not); RoundEnded covers the
    // score-award + board-reset transition that fires once the post-victory pause elapses.
    enum class StepResult { NoOp, Moved, RoundEnded };

    [[nodiscard]] Screen screen() const { return screen_; }
    [[nodiscard]] const Board& board() const { return board_; }

    void enter_menu();
    void start_play(bool pve);

    [[nodiscard]] bool can_click(int x, int y) const;
    StepResult on_cell_click(int x, int y);
    StepResult tick(float dt, std::mt19937& rng);

private:
    static constexpr float kVictoryPause = 1.f;
    // Presentation-only pause after it becomes the bot's turn, before it actually calls
    // choose_bot_move()/places its mark, so the bot's move doesn't read as instant/robotic. Does
    // not affect PvP (no bot ever has current_is_bot() true) and does not change which move the
    // bot picks or the RNG sequence it consumes — it only delays when step_bot() is called.
    static constexpr float kBotThinkDelay = 0.4f;

    Board board_{};
    Screen screen_ = Screen::Menu;
    float victory_wait_ = 0.f;
    float bot_think_wait_ = 0.f;
};

}
