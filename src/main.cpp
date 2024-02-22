#include <engine/core/engine.h>

#include <game/game.h>

int main() {
    engine::Engine<game::Game> app;
    if (!app.Init()) {
        return 1;
    }
    return app.Run();
}
