#include <game/board.h>

#include <gtest/gtest.h>

#include <random>

namespace game {
namespace {

class BoardTest : public ::testing::Test {
protected:
    Board board;
};

// --- Rows ---------------------------------------------------------------

TEST_F(BoardTest, RowZeroWinsForX) {
    ASSERT_TRUE(board.try_place(0, 0));  // X
    ASSERT_TRUE(board.try_place(1, 1));  // O
    ASSERT_TRUE(board.try_place(1, 0));  // X
    ASSERT_TRUE(board.try_place(2, 1));  // O
    ASSERT_TRUE(board.try_place(2, 0));  // X completes row 0
    EXPECT_EQ(board.outcome(), Outcome::X);
}

TEST_F(BoardTest, RowOneWinsForO) {
    ASSERT_TRUE(board.try_place(0, 0));  // X
    ASSERT_TRUE(board.try_place(0, 1));  // O
    ASSERT_TRUE(board.try_place(1, 0));  // X
    ASSERT_TRUE(board.try_place(1, 1));  // O
    ASSERT_TRUE(board.try_place(2, 2));  // X
    ASSERT_TRUE(board.try_place(2, 1));  // O completes row 1
    EXPECT_EQ(board.outcome(), Outcome::O);
}

TEST_F(BoardTest, RowTwoWinsForX) {
    ASSERT_TRUE(board.try_place(0, 2));  // X
    ASSERT_TRUE(board.try_place(0, 0));  // O
    ASSERT_TRUE(board.try_place(1, 2));  // X
    ASSERT_TRUE(board.try_place(1, 0));  // O
    ASSERT_TRUE(board.try_place(2, 2));  // X completes row 2
    EXPECT_EQ(board.outcome(), Outcome::X);
}

// --- Columns --------------------------------------------------------------

TEST_F(BoardTest, ColumnZeroWinsForO) {
    ASSERT_TRUE(board.try_place(1, 0));  // X
    ASSERT_TRUE(board.try_place(0, 0));  // O
    ASSERT_TRUE(board.try_place(1, 1));  // X
    ASSERT_TRUE(board.try_place(0, 1));  // O
    ASSERT_TRUE(board.try_place(2, 2));  // X
    ASSERT_TRUE(board.try_place(0, 2));  // O completes column 0
    EXPECT_EQ(board.outcome(), Outcome::O);
}

TEST_F(BoardTest, ColumnOneWinsForX) {
    ASSERT_TRUE(board.try_place(1, 0));  // X
    ASSERT_TRUE(board.try_place(0, 0));  // O
    ASSERT_TRUE(board.try_place(1, 1));  // X
    ASSERT_TRUE(board.try_place(0, 1));  // O
    ASSERT_TRUE(board.try_place(1, 2));  // X completes column 1
    EXPECT_EQ(board.outcome(), Outcome::X);
}

TEST_F(BoardTest, ColumnTwoWinsForO) {
    ASSERT_TRUE(board.try_place(0, 0));  // X
    ASSERT_TRUE(board.try_place(2, 0));  // O
    ASSERT_TRUE(board.try_place(0, 1));  // X
    ASSERT_TRUE(board.try_place(2, 1));  // O
    ASSERT_TRUE(board.try_place(1, 2));  // X
    ASSERT_TRUE(board.try_place(2, 2));  // O completes column 2
    EXPECT_EQ(board.outcome(), Outcome::O);
}

// --- Diagonals --------------------------------------------------------------

TEST_F(BoardTest, MainDiagonalWinsForX) {
    ASSERT_TRUE(board.try_place(0, 0));  // X
    ASSERT_TRUE(board.try_place(1, 0));  // O
    ASSERT_TRUE(board.try_place(1, 1));  // X
    ASSERT_TRUE(board.try_place(2, 0));  // O
    ASSERT_TRUE(board.try_place(2, 2));  // X completes main diagonal
    EXPECT_EQ(board.outcome(), Outcome::X);
}

TEST_F(BoardTest, AntiDiagonalWinsForO) {
    ASSERT_TRUE(board.try_place(0, 0));  // X
    ASSERT_TRUE(board.try_place(0, 2));  // O
    ASSERT_TRUE(board.try_place(1, 0));  // X
    ASSERT_TRUE(board.try_place(1, 1));  // O
    ASSERT_TRUE(board.try_place(2, 2));  // X
    ASSERT_TRUE(board.try_place(2, 0));  // O completes anti-diagonal
    EXPECT_EQ(board.outcome(), Outcome::O);
}

// --- Draw / Playing --------------------------------------------------------------

TEST_F(BoardTest, FullBoardWithNoLineIsDraw) {
    // X O X
    // X O O
    // O X X
    ASSERT_TRUE(board.try_place(0, 0));  // X
    ASSERT_TRUE(board.try_place(1, 0));  // O
    ASSERT_TRUE(board.try_place(2, 0));  // X
    ASSERT_TRUE(board.try_place(1, 1));  // O
    ASSERT_TRUE(board.try_place(0, 1));  // X
    ASSERT_TRUE(board.try_place(2, 1));  // O
    ASSERT_TRUE(board.try_place(1, 2));  // X
    ASSERT_TRUE(board.try_place(0, 2));  // O
    ASSERT_TRUE(board.try_place(2, 2));  // X
    EXPECT_EQ(board.outcome(), Outcome::Draw);
}

TEST_F(BoardTest, EmptyBoardIsPlaying) {
    EXPECT_EQ(board.outcome(), Outcome::Playing);
}

TEST_F(BoardTest, PartiallyFilledUnfinishedBoardIsPlaying) {
    ASSERT_TRUE(board.try_place(0, 0));  // X
    ASSERT_TRUE(board.try_place(1, 1));  // O
    EXPECT_EQ(board.outcome(), Outcome::Playing);
}

// --- P0 regression: a genuine win must not be masked by an empty line -----
// checked earlier in the outcome scan's iteration order (rows, then columns,
// then diagonals). Before the original fix, comparing three Mark::Empty
// cells for equality (Empty == Empty == Empty) caused the scan to return
// static_cast<Outcome>(Mark::Empty) == Outcome::Playing on the very first
// empty line it encountered, without ever inspecting the rest of the board.
// These cases are reconstructed here using only legal, turn-alternating
// try_place calls (the new API no longer allows poking the grid directly).

TEST_F(BoardTest, MiddleRowWinDetectedDespiteEmptyTopRow) {
    // Row 0 (checked first) stays completely empty; row 1 is a genuine X win.
    ASSERT_TRUE(board.try_place(0, 1));  // X
    ASSERT_TRUE(board.try_place(0, 2));  // O
    ASSERT_TRUE(board.try_place(1, 1));  // X
    ASSERT_TRUE(board.try_place(1, 2));  // O
    ASSERT_TRUE(board.try_place(2, 1));  // X completes row 1
    EXPECT_EQ(board.outcome(), Outcome::X);
    EXPECT_TRUE(board.empty_at(0, 0));
    EXPECT_TRUE(board.empty_at(1, 0));
    EXPECT_TRUE(board.empty_at(2, 0));
}

TEST_F(BoardTest, LastColumnWinDetectedDespiteEmptyFirstColumn) {
    // Column 0 (checked first among columns) stays completely empty; column
    // 2 (checked last) is a genuine X win.
    ASSERT_TRUE(board.try_place(2, 0));  // X
    ASSERT_TRUE(board.try_place(1, 0));  // O
    ASSERT_TRUE(board.try_place(2, 1));  // X
    ASSERT_TRUE(board.try_place(1, 1));  // O
    ASSERT_TRUE(board.try_place(2, 2));  // X completes column 2
    EXPECT_EQ(board.outcome(), Outcome::X);
    EXPECT_TRUE(board.empty_at(0, 0));
    EXPECT_TRUE(board.empty_at(0, 1));
    EXPECT_TRUE(board.empty_at(0, 2));
}

TEST_F(BoardTest, AntiDiagonalWinDetectedWithNoRowOrColumnComplete) {
    // Neither diagonal shares a cell exclusively: both diagonals cross
    // every row and every column, and share the center cell with each
    // other. So whenever a diagonal is genuinely complete, no row, column,
    // or the other diagonal can be all-Empty (each has one of the winning
    // cells already). This case exercises that the outcome scan still walks
    // past the (necessarily non-uniform, non-winning) rows, columns, and
    // main diagonal to find the real anti-diagonal win, rather than
    // over-fitting the fix to only the row/column cases above.
    ASSERT_TRUE(board.try_place(2, 0));  // X
    ASSERT_TRUE(board.try_place(0, 0));  // O
    ASSERT_TRUE(board.try_place(1, 1));  // X
    ASSERT_TRUE(board.try_place(2, 2));  // O
    ASSERT_TRUE(board.try_place(0, 2));  // X completes anti-diagonal
    EXPECT_EQ(board.outcome(), Outcome::X);
}

// --- P1 safety: no public method can create an invalid state --------------

TEST_F(BoardTest, TryPlaceOnOccupiedCellFailsAndPreservesState) {
    ASSERT_TRUE(board.try_place(0, 0));  // X
    EXPECT_EQ(board.current_mark(), Mark::O);

    EXPECT_FALSE(board.try_place(0, 0));
    EXPECT_EQ(board.at(0, 0), Mark::X);          // unchanged, not overwritten by O
    EXPECT_EQ(board.current_mark(), Mark::O);    // turn did not advance
    EXPECT_EQ(board.outcome(), Outcome::Playing);
}

TEST_F(BoardTest, TryPlaceOutOfBoundsFailsSafely) {
    EXPECT_FALSE(board.try_place(-1, 0));
    EXPECT_FALSE(board.try_place(0, -1));
    EXPECT_FALSE(board.try_place(Board::kSize, 0));
    EXPECT_FALSE(board.try_place(0, Board::kSize));
    EXPECT_EQ(board.current_mark(), Mark::X);
    EXPECT_EQ(board.outcome(), Outcome::Playing);
}

TEST_F(BoardTest, TryPlaceAfterGameOverIsNoOp) {
    ASSERT_TRUE(board.try_place(0, 0));  // X
    ASSERT_TRUE(board.try_place(0, 1));  // O
    ASSERT_TRUE(board.try_place(1, 0));  // X
    ASSERT_TRUE(board.try_place(2, 1));  // O
    ASSERT_TRUE(board.try_place(2, 0));  // X completes row 0
    ASSERT_EQ(board.outcome(), Outcome::X);

    EXPECT_FALSE(board.try_place(1, 1));
    EXPECT_TRUE(board.empty_at(1, 1));
    EXPECT_EQ(board.outcome(), Outcome::X);
}

TEST_F(BoardTest, TurnAlternatesAfterNonWinningMove) {
    EXPECT_EQ(board.current_mark(), Mark::X);
    ASSERT_TRUE(board.try_place(0, 0));
    EXPECT_EQ(board.current_mark(), Mark::O);
    ASSERT_TRUE(board.try_place(1, 1));
    EXPECT_EQ(board.current_mark(), Mark::X);
}

TEST_F(BoardTest, TurnDoesNotAlternateAfterWinningMove) {
    ASSERT_TRUE(board.try_place(0, 0));  // X
    ASSERT_TRUE(board.try_place(0, 1));  // O
    ASSERT_TRUE(board.try_place(1, 0));  // X
    ASSERT_TRUE(board.try_place(2, 1));  // O
    ASSERT_TRUE(board.try_place(2, 0));  // X completes row 0
    ASSERT_EQ(board.outcome(), Outcome::X);
    EXPECT_EQ(board.current_mark(), Mark::X);
}

TEST_F(BoardTest, AtOutOfBoundsReturnsEmpty) {
    // at() mirrors empty_at's bounds-checked behavior instead of asserting: an out-of-range query
    // safely reports Mark::Empty rather than reading out of the grid.
    EXPECT_EQ(board.at(-1, 0), Mark::Empty);
    EXPECT_EQ(board.at(0, -1), Mark::Empty);
    EXPECT_EQ(board.at(Board::kSize, 0), Mark::Empty);
    EXPECT_EQ(board.at(0, Board::kSize), Mark::Empty);
}

// --- P1 safety: bot RNG is fully injected and deterministic ----------------

TEST_F(BoardTest, StepBotNoOpWhenGameAlreadyOver) {
    ASSERT_TRUE(board.try_place(0, 0));  // X
    ASSERT_TRUE(board.try_place(0, 1));  // O
    ASSERT_TRUE(board.try_place(1, 0));  // X
    ASSERT_TRUE(board.try_place(2, 1));  // O
    ASSERT_TRUE(board.try_place(2, 0));  // X completes row 0
    ASSERT_EQ(board.outcome(), Outcome::X);

    std::mt19937 rng(1);
    EXPECT_FALSE(board.step_bot(rng));
}

TEST_F(BoardTest, StepBotTakesWinningMoveRegardlessOfSeed) {
    for (unsigned seed : {1u, 2u, 42u}) {
        Board local;
        ASSERT_TRUE(local.try_place(2, 2));  // X
        ASSERT_TRUE(local.try_place(0, 0));  // O
        ASSERT_TRUE(local.try_place(1, 2));  // X
        ASSERT_TRUE(local.try_place(1, 0));  // O: row 0 threat at (2, 0)
        ASSERT_EQ(local.current_mark(), Mark::X);

        std::mt19937 rng(seed);
        ASSERT_TRUE(local.step_bot(rng));
        // X had its own dangling threat too (row 2 at (0, 2)); a win for the
        // current mark always takes priority over blocking, so X takes it.
        EXPECT_EQ(local.at(0, 2), Mark::X);
        EXPECT_EQ(local.outcome(), Outcome::X);
    }
}

TEST_F(BoardTest, StepBotTakesBlockingMoveRegardlessOfSeed) {
    for (unsigned seed : {1u, 2u, 42u}) {
        Board local;
        ASSERT_TRUE(local.try_place(2, 2));  // X
        ASSERT_TRUE(local.try_place(0, 0));  // O
        ASSERT_TRUE(local.try_place(0, 1));  // X (neutral, no threat of its own)
        ASSERT_TRUE(local.try_place(1, 0));  // O: row 0 threat at (2, 0)
        ASSERT_EQ(local.current_mark(), Mark::X);

        std::mt19937 rng(seed);
        ASSERT_TRUE(local.step_bot(rng));
        EXPECT_EQ(local.at(2, 0), Mark::X);
        EXPECT_EQ(local.outcome(), Outcome::Playing);
    }
}

TEST_F(BoardTest, StepBotRandomFallbackIsValidAndReproducible) {
    // No win, no block, center already taken, and the last move is a corner
    // (not one of the edge cells that triggers the corner-after-edge
    // heuristic), so step_bot must fall through to the random branch.
    Board first;
    ASSERT_TRUE(first.try_place(0, 0));  // X
    ASSERT_TRUE(first.try_place(1, 1));  // O (center)
    ASSERT_TRUE(first.try_place(2, 1));  // X
    ASSERT_TRUE(first.try_place(2, 2));  // O
    ASSERT_EQ(first.current_mark(), Mark::X);
    ASSERT_EQ(first.outcome(), Outcome::Playing);

    constexpr unsigned kSeed = 42;
    std::mt19937 rng_a(kSeed);
    ASSERT_TRUE(first.step_bot(rng_a));

    int chosen_x = -1;
    int chosen_y = -1;
    for (int x = 0; x < Board::kSize; ++x) {
        for (int y = 0; y < Board::kSize; ++y) {
            if (first.at(x, y) == Mark::X && !(x == 0 && y == 0) && !(x == 2 && y == 1)) {
                chosen_x = x;
                chosen_y = y;
            }
        }
    }
    ASSERT_NE(chosen_x, -1) << "step_bot should have placed a mark on an empty cell";

    Board second;
    ASSERT_TRUE(second.try_place(0, 0));  // X
    ASSERT_TRUE(second.try_place(1, 1));  // O (center)
    ASSERT_TRUE(second.try_place(2, 1));  // X
    ASSERT_TRUE(second.try_place(2, 2));  // O

    std::mt19937 rng_b(kSeed);
    ASSERT_TRUE(second.step_bot(rng_b));
    EXPECT_EQ(second.at(chosen_x, chosen_y), Mark::X)
            << "same seed against the same board state must reproduce the same choice";
}

}  // namespace
}  // namespace game
