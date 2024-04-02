#pragma once

#include <engine/resources/asset_id.h>
#include <engine/ui/bindable.h>
#include <engine/ui/command.h>
#include <engine/ui/view_model.h>

#include <game/board.h>

#include <array>
#include <string>

namespace game {

class PlayViewModel final : public engine::ui::ViewModel {
public:
    static constexpr int kCells = Board::kSize * Board::kSize;

    PlayViewModel();

    engine::ui::Bindable<std::string> score_x{"X:0"};
    engine::ui::Bindable<std::string> score_o{"O:0"};
    engine::ui::Bindable<std::string> result_message{""};
    engine::ui::RelayCommand back;

    std::array<engine::ui::Bindable<engine::AssetId>, kCells> cell_mark;
    std::array<engine::ui::RelayCommand, kCells> cell_click;

    engine::ui::Bindable<engine::AssetId> win_row_0;
    engine::ui::Bindable<engine::AssetId> win_row_1;
    engine::ui::Bindable<engine::AssetId> win_row_2;
    engine::ui::Bindable<engine::AssetId> win_col_0;
    engine::ui::Bindable<engine::AssetId> win_col_1;
    engine::ui::Bindable<engine::AssetId> win_col_2;
    engine::ui::Bindable<engine::AssetId> win_diag;
    engine::ui::Bindable<engine::AssetId> win_anti_diag;
};

}
