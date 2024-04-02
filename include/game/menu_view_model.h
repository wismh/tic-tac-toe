#pragma once

#include <engine/ui/command.h>
#include <engine/ui/view_model.h>

namespace game {

class MenuViewModel final : public engine::ui::ViewModel {
public:
    MenuViewModel();

    engine::ui::RelayCommand play_pvp;
    engine::ui::RelayCommand play_pve;
    engine::ui::RelayCommand exit;
};

}
