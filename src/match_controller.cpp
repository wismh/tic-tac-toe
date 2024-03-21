#include <game/match_controller.h>

namespace game {

void MatchController::enter_menu() {
    screen_ = Screen::Menu;
    board_.clear_scores();
    board_.reset_round();
    board_.set_bot_flags(false, false);
    victory_wait_ = 0.f;
    bot_think_wait_ = 0.f;
}

void MatchController::start_play(bool pve) {
    screen_ = Screen::Play;
    board_.clear_scores();
    board_.reset_round();
    board_.set_bot_flags(false, pve);
    victory_wait_ = 0.f;
    bot_think_wait_ = 0.f;
}

bool MatchController::can_click(int x, int y) const {
    if (screen_ != Screen::Play || board_.outcome() != Outcome::Playing) {
        return false;
    }
    if (!board_.empty_at(x, y)) {
        return false;
    }
    return !board_.current_is_bot();
}

MatchController::StepResult MatchController::on_cell_click(int x, int y) {
    if (!board_.try_place(x, y)) {
        return StepResult::NoOp;
    }
    if (board_.outcome() != Outcome::Playing) {
        victory_wait_ = kVictoryPause;
    }
    return StepResult::Moved;
}

MatchController::StepResult MatchController::tick(float dt, std::mt19937& rng) {
    if (screen_ != Screen::Play) {
        return StepResult::NoOp;
    }

    if (board_.outcome() != Outcome::Playing) {
        bot_think_wait_ = 0.f;
        victory_wait_ -= dt;
        if (victory_wait_ > 0.f) {
            return StepResult::NoOp;
        }
        board_.award_point();
        board_.reset_round();
        victory_wait_ = 0.f;
        return StepResult::RoundEnded;
    }

    if (!board_.current_is_bot()) {
        bot_think_wait_ = 0.f;
        return StepResult::NoOp;
    }

    bot_think_wait_ += dt;
    if (bot_think_wait_ < kBotThinkDelay) {
        return StepResult::NoOp;
    }
    bot_think_wait_ = 0.f;

    if (!board_.step_bot(rng)) {
        return StepResult::NoOp;
    }
    if (board_.outcome() != Outcome::Playing) {
        victory_wait_ = kVictoryPause;
    }
    return StepResult::Moved;
}

}
