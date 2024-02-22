#pragma once

#include <engine/igame.h>
#include <engine/render/graphic_factory.h>
#include <engine/resources/assets_db.h>

#include <game/hud_view_model.h>

#include <memory>

namespace game {

class Game final : public engine::GameBase {
public:
    Game(engine::AssetsDb& assets, engine::render::IGraphicFactory& factory);

    std::string WindowTitle() const override;
    glm::ivec2 WindowSize() const override;

    void OnStart() override;

private:
    void load_catalog();
    void spawn_camera();
    void spawn_quad();
    void spawn_hud();
    void register_systems();

    engine::AssetsDb& assets_;
    engine::render::IGraphicFactory& factory_;
    std::shared_ptr<HudViewModel> hud_;
    engine::ecs::Entity quad_{};
};

}
