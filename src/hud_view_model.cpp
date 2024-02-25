#include <game/hud_view_model.h>

namespace game {

HudViewModel::HudViewModel() {
    property("bounces", bounces);
    command("nudge", nudge);
    command("quit", quit);
}

}
