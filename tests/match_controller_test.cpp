#include <game/match_controller.h>

#include <gtest/gtest.h>

#include <random>

namespace game {
namespace {

// --- Bot "thinking" delay ---------------------------------------------------
// MatchController::tick delays kBotThinkDelay seconds (a presentation-only pause) after it
// becomes the bot's turn before it actually calls step_bot()/places the bot's mark. These tests
// exercise that delay by driving tick() directly with small dt increments, the same way Phase 3's
// design intended MatchController to be testable without any engine/UI/audio dependency.

TEST(MatchControllerTest, BotDoesNotMoveBeforeThinkDelayElapses) {
    MatchController match;
    match.start_play(/*pve=*/true);  // X is human, O is the bot.
    std::mt19937 rng(1);

    // Human plays first; this hands the turn to the bot (O).
    ASSERT_TRUE(match.can_click(0, 0));
    ASSERT_EQ(match.on_cell_click(0, 0), MatchController::StepResult::Moved);
    ASSERT_TRUE(match.board().current_is_bot());

    // Advance in small increments that add up to less than the think delay (chosen to stay safely
    // under the 0.3-0.6s range regardless of the exact constant picked): the bot must not have
    // moved yet, so it must still be the bot's turn on every one of these ticks.
    constexpr float kStep = 0.1f;
    for (int i = 0; i < 3; ++i) {
        EXPECT_EQ(match.tick(kStep, rng), MatchController::StepResult::NoOp);
        EXPECT_TRUE(match.board().current_is_bot()) << "bot should not have moved yet at tick " << i;
        EXPECT_EQ(match.board().outcome(), Outcome::Playing);
    }
}

TEST(MatchControllerTest, BotMovesOnceThinkDelayElapses) {
    MatchController match;
    match.start_play(/*pve=*/true);  // X is human, O is the bot.
    std::mt19937 rng(1);

    ASSERT_TRUE(match.can_click(0, 0));
    ASSERT_EQ(match.on_cell_click(0, 0), MatchController::StepResult::Moved);
    ASSERT_TRUE(match.board().current_is_bot());

    // Same sub-threshold ticks as above...
    constexpr float kStep = 0.1f;
    for (int i = 0; i < 3; ++i) {
        ASSERT_EQ(match.tick(kStep, rng), MatchController::StepResult::NoOp);
    }
    ASSERT_TRUE(match.board().current_is_bot()) << "still the bot's turn just before the delay elapses";

    // ...then a bigger tick that pushes cumulative elapsed time comfortably past the delay: the
    // bot must move on exactly this tick.
    EXPECT_EQ(match.tick(0.2f, rng), MatchController::StepResult::Moved);
    // The bot placed its mark and (having not won) handed the turn back to the human (X), so it is
    // no longer the bot's turn.
    EXPECT_FALSE(match.board().current_is_bot());
    EXPECT_EQ(match.board().current_mark(), Mark::X);
}

TEST(MatchControllerTest, ThinkDelayDoesNotAffectPvp) {
    MatchController match;
    match.start_play(/*pve=*/false);  // Neither mark is a bot.
    std::mt19937 rng(1);

    ASSERT_TRUE(match.can_click(0, 0));
    ASSERT_EQ(match.on_cell_click(0, 0), MatchController::StepResult::Moved);

    // No bot is ever "thinking" in PvP: tick() must be a no-op regardless of dt, and must never
    // place a move on either player's behalf.
    EXPECT_EQ(match.tick(1.f, rng), MatchController::StepResult::NoOp);
    EXPECT_EQ(match.board().current_mark(), Mark::O);
    EXPECT_TRUE(match.board().empty_at(1, 1));
}

TEST(MatchControllerTest, ThinkDelayResetsBetweenBotTurns) {
    // A fresh MatchController accumulates no bot-think time before the bot's first turn even
    // starts: ticking while it is still the human's turn must never itself trigger a bot move.
    MatchController match;
    match.start_play(/*pve=*/true);
    std::mt19937 rng(1);

    ASSERT_FALSE(match.board().current_is_bot());
    // A long idle tick while it is the human's turn should do nothing at all.
    EXPECT_EQ(match.tick(10.f, rng), MatchController::StepResult::NoOp);
    EXPECT_TRUE(match.board().empty_at(0, 0));
}

}  // namespace
}  // namespace game
