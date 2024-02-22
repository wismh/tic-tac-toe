#pragma once

#include <engine/ui/bindable.h>
#include <engine/ui/command.h>
#include <engine/ui/view_model.h>

namespace game {

class HudViewModel final : public engine::ui::ViewModel {
public:
    HudViewModel();

    engine::ui::Bindable<int> bounces{0};
    engine::ui::RelayCommand nudge;
    engine::ui::RelayCommand quit;
};

}
