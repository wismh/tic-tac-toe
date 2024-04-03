#pragma once

#include <game/board.h>
#include <game/play_view_model.h>

#include <memory>

namespace game {

// Projects Board state onto PlayViewModel's bindable properties. This is the only place that knows
// how game state maps to play-screen UI text/images; Game just decides *when* to call it.
class PlayPresenter {
public:
    explicit PlayPresenter(std::shared_ptr<PlayViewModel> view_model);

    void sync_marks(const Board& board);
    void refresh_scores(const Board& board);
    void sync_result_message(const Board& board);
    void sync_win_line(const Board& board);

private:
    std::shared_ptr<PlayViewModel> view_model_;
};

}
