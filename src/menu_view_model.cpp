#include <game/menu_view_model.h>

#include <asset_ids.h>

namespace game {

MenuViewModel::MenuViewModel() {
    assets::ui::Menu::bind(*this);
}

}
