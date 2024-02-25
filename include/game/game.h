#pragma once

#include <engine/audio/audio_system.h>
#include <engine/core/input_system.h>
#include <engine/igame.h>
#include <engine/render/graphic_factory.h>

#include <game/hud_view_model.h>

#include <memory>

namespace game {

class Game final : public engine::GameBase {
public:
    Game(engine::InputSystem&, engine::IAudioSystem& audio, engine::render::IGraphicFactory& factory);

    std::string window_title() const override;
    glm::ivec2 window_size() const override;

    void on_start() override;

private:
    void spawn_camera();
    void spawn_quad();
    void spawn_hud();
    void register_systems();

    engine::IAudioSystem& audio_;
    engine::render::IGraphicFactory& factory_;
    std::shared_ptr<HudViewModel> hud_;
    engine::ecs::Entity quad_{};
};

}
