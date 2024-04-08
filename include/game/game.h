#pragma once

#include <engine/audio/audio_system.h>
#include <engine/core/input_system.h>
#include <engine/ecs/entity.h>
#include <engine/igame.h>

#include <game/match_controller.h>
#include <game/menu_view_model.h>
#include <game/play_presenter.h>
#include <game/play_view_model.h>

#include <memory>
#include <random>
#include <string>

namespace game {

class Game final : public engine::GameBase {
public:
    Game(engine::InputSystem&, engine::IAudioSystem& audio);

    std::string window_title() const override;
    glm::ivec2 window_size() const override;

    void on_start() override;
    void on_quit() override;

private:
    void spawn_camera();
    void spawn_ui();
    void register_systems();
    void apply_screen();
    void play_step_sfx();
    void enter_menu();
    void start_play(bool pve);
    bool can_click(int x, int y) const;
    void on_cell_click(int x, int y);
    void tick_round(float dt);

    engine::IAudioSystem& audio_;
    std::shared_ptr<MenuViewModel> menu_vm_;
    std::shared_ptr<PlayViewModel> play_vm_;
    PlayPresenter presenter_;
    MatchController match_;
    engine::ecs::Entity canvas_{};
    std::mt19937 rng_{std::random_device{}()};
};

}
