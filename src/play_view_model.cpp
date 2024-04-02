#include <game/play_view_model.h>

#include <asset_ids.h>

#include <string>

namespace game {

// assets::ui::Play::bind() (codegen from play.xml) can't be used here: it assumes one individually
// named field per binding, but cell_mark/cell_click are arrays for clean iteration in PlayPresenter
// and Game — so the cell bindings are registered by hand, matching the naming convention codegen
// would otherwise use ("cell_{x}_{y}_mark" / "cell_{x}_{y}_click").
PlayViewModel::PlayViewModel() {
    property(engine::ui::intern("score_x"), score_x);
    property(engine::ui::intern("score_o"), score_o);
    property(engine::ui::intern("result_message"), result_message);
    command(engine::ui::intern("back"), back);

    property(engine::ui::intern("win_row_0"), win_row_0);
    property(engine::ui::intern("win_row_1"), win_row_1);
    property(engine::ui::intern("win_row_2"), win_row_2);
    property(engine::ui::intern("win_col_0"), win_col_0);
    property(engine::ui::intern("win_col_1"), win_col_1);
    property(engine::ui::intern("win_col_2"), win_col_2);
    property(engine::ui::intern("win_diag"), win_diag);
    property(engine::ui::intern("win_anti_diag"), win_anti_diag);

    for (int x = 0; x < Board::kSize; ++x) {
        for (int y = 0; y < Board::kSize; ++y) {
            const std::size_t index = static_cast<std::size_t>(x * Board::kSize + y);
            const std::string prefix = "cell_" + std::to_string(x) + "_" + std::to_string(y);
            property(engine::ui::intern(prefix + "_mark"), cell_mark[index]);
            command(engine::ui::intern(prefix + "_click"), cell_click[index]);
        }
    }
}

}
