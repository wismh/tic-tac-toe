#include <game/play_presenter.h>

#include <engine/resources/asset_id.h>

#include <asset_ids.h>

#include <string>
#include <utility>

namespace game {

PlayPresenter::PlayPresenter(std::shared_ptr<PlayViewModel> view_model) : view_model_(std::move(view_model)) {}

void PlayPresenter::sync_marks(const Board& board) {
    for (int x = 0; x < Board::kSize; ++x) {
        for (int y = 0; y < Board::kSize; ++y) {
            const std::size_t index = static_cast<std::size_t>(x * Board::kSize + y);
            const Mark mark = board.at(x, y);
            engine::AssetId source = assets::textures::empty;
            if (mark == Mark::X) {
                source = assets::textures::x;
            } else if (mark == Mark::O) {
                source = assets::textures::o;
            }
            view_model_->cell_mark[index].set(source);
        }
    }
}

void PlayPresenter::refresh_scores(const Board& board) {
    view_model_->score_x.set("X:" + std::to_string(board.score_x()));
    view_model_->score_o.set("O:" + std::to_string(board.score_o()));
}

void PlayPresenter::sync_win_line(const Board& board) {
    const WinLine active = board.winning_line();
    const auto source_for = [active](WinLine line) {
        return active == line ? assets::textures::line : assets::textures::empty;
    };
    view_model_->win_row_0.set(source_for(WinLine::Row0));
    view_model_->win_row_1.set(source_for(WinLine::Row1));
    view_model_->win_row_2.set(source_for(WinLine::Row2));
    view_model_->win_col_0.set(source_for(WinLine::Col0));
    view_model_->win_col_1.set(source_for(WinLine::Col1));
    view_model_->win_col_2.set(source_for(WinLine::Col2));
    view_model_->win_diag.set(source_for(WinLine::Diag));
    view_model_->win_anti_diag.set(source_for(WinLine::AntiDiag));
}

void PlayPresenter::sync_result_message(const Board& board) {
    switch (board.outcome()) {
        case Outcome::X:
            view_model_->result_message.set("X wins!");
            break;
        case Outcome::O:
            view_model_->result_message.set("O wins!");
            break;
        case Outcome::Draw:
            view_model_->result_message.set("Draw!");
            break;
        case Outcome::Playing:
            view_model_->result_message.set("");
            break;
    }
}

}
