#include <game/hud_view_model.h>

namespace game {

HudViewModel::HudViewModel() {
    Property("bounces", bounces);
    Command("nudge", nudge);
    Command("quit", quit);
}

}
