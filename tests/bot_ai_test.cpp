#include <game/bot_ai.h>

#include <gtest/gtest.h>

#include <random>

namespace game {
namespace {

TEST(BotAiTest, TakesImmediateWinningMoveWhenAvailable) {
    Board board;
    ASSERT_TRUE(board.try_place(2, 2));  // X
    ASSERT_TRUE(board.try_place(0, 0));  // O
    ASSERT_TRUE(board.try_place(1, 2));  // X
    ASSERT_TRUE(board.try_place(1, 0));  // O: row 0 threat at (2, 0)
    ASSERT_EQ(board.current_mark(), Mark::X);

    // X also has its own dangling threat (row 2 at (0, 2)); a win for the mark to move takes
    // priority over blocking, so the winning move should be chosen regardless of seed.
    for (unsigned seed : {1u, 2u, 42u}) {
        std::mt19937 rng(seed);
        const auto [x, y] = choose_bot_move(board, Mark::X, rng);
        EXPECT_EQ(x, 0);
        EXPECT_EQ(y, 2);
    }
}

TEST(BotAiTest, TakesBlockingMoveWhenNoWinButOpponentThreatens) {
    Board board;
    ASSERT_TRUE(board.try_place(2, 2));  // X
    ASSERT_TRUE(board.try_place(0, 0));  // O
    ASSERT_TRUE(board.try_place(0, 1));  // X (neutral, no threat of its own)
    ASSERT_TRUE(board.try_place(1, 0));  // O: row 0 threat at (2, 0)
    ASSERT_EQ(board.current_mark(), Mark::X);

    for (unsigned seed : {1u, 2u, 42u}) {
        std::mt19937 rng(seed);
        const auto [x, y] = choose_bot_move(board, Mark::X, rng);
        EXPECT_EQ(x, 2);
        EXPECT_EQ(y, 0);
    }
}

TEST(BotAiTest, PrefersCenterOnEmptyBoard) {
    Board board;
    std::mt19937 rng(7);
    const auto [x, y] = choose_bot_move(board, Mark::X, rng);
    EXPECT_EQ(x, 1);
    EXPECT_EQ(y, 1);
}

TEST(BotAiTest, RandomFallbackIsValidAndReproducibleUnderFixedSeed) {
    // No win, no block, center already taken, and the last move is a corner (not one of the
    // edge cells that triggers the corner-after-edge heuristic), so choose_bot_move must fall
    // through to the random branch.
    Board first;
    ASSERT_TRUE(first.try_place(0, 0));  // X
    ASSERT_TRUE(first.try_place(1, 1));  // O (center)
    ASSERT_TRUE(first.try_place(2, 1));  // X
    ASSERT_TRUE(first.try_place(2, 2));  // O
    ASSERT_EQ(first.current_mark(), Mark::X);
    ASSERT_EQ(first.outcome(), Outcome::Playing);

    constexpr unsigned kSeed = 42;
    std::mt19937 rng_a(kSeed);
    const auto [x, y] = choose_bot_move(first, Mark::X, rng_a);
    ASSERT_TRUE(first.empty_at(x, y)) << "chosen cell must be empty on the board it was chosen for";

    Board second;
    ASSERT_TRUE(second.try_place(0, 0));  // X
    ASSERT_TRUE(second.try_place(1, 1));  // O (center)
    ASSERT_TRUE(second.try_place(2, 1));  // X
    ASSERT_TRUE(second.try_place(2, 2));  // O

    std::mt19937 rng_b(kSeed);
    const auto [x2, y2] = choose_bot_move(second, Mark::X, rng_b);
    EXPECT_EQ(x, x2);
    EXPECT_EQ(y, y2) << "same seed against the same board state must reproduce the same choice";
}

}  // namespace
}  // namespace game
