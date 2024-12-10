#include <game/game.h>

#include <engine/audio/events.h>
#include <engine/core/application_state.h>
#include <engine/core/time.h>
#include <engine/ecs/camera.h>
#include <engine/ecs/events.h>
#include <engine/ecs/schedule.h>
#include <engine/ecs/transform.h>
#include <engine/ui/canvas.h>
#include <engine/ui/command.h>

#include <asset_ids.h>

#include <game/board.h>

#include <string>

namespace game {
namespace {

constexpr int kScale = 3;
constexpr int kLogicalW = 192;
constexpr int kLogicalH = 232;
constexpr int kWindowW = kLogicalW * kScale;
constexpr int kWindowH = kLogicalH * kScale;
constexpr float kOrthoHalf = 116.f;

constexpr float kTapVibrationSeconds = 0.02f;
constexpr float kTapVibrationIntensity = 0.4f;
constexpr float kMarkPlacedVibrationSeconds = 0.03f;
constexpr float kMarkPlacedVibrationIntensity = 0.7f;
constexpr float kRoundOutcomeVibrationSeconds = 0.25f;
constexpr float kRoundOutcomeVibrationIntensity = 1.f;

}

Game::Game(engine::InputSystem&, engine::IAudioSystem& audio, engine::IHaptics& haptics) :
    audio_(audio),
    haptics_(haptics),
    menu_vm_(std::make_shared<MenuViewModel>()),
    play_vm_(std::make_shared<PlayViewModel>()),
    presenter_(play_vm_) {}

std::string Game::window_title() const {
    return "Tic Tac Toe";
}

glm::ivec2 Game::window_size() const {
    return {kWindowW, kWindowH};
}

void Game::on_start() {
    menu_vm_->play_pvp = [this] { tap_haptic(); start_play(false); };
    menu_vm_->play_pve = [this] { tap_haptic(); start_play(true); };
    menu_vm_->exit = [this] { tap_haptic(); world_.ctx<engine::ApplicationState>().quit(); };
    play_vm_->back = [this] { tap_haptic(); enter_menu(); };

    for (int x = 0; x < Board::kSize; ++x) {
        for (int y = 0; y < Board::kSize; ++y) {
            const std::size_t index = static_cast<std::size_t>(x * Board::kSize + y);
            play_vm_->cell_click[index] = engine::ui::RelayCommand([this, x, y] { on_cell_click(x, y); },
                    [this, x, y] { return can_click(x, y); });
        }
    }

    spawn_camera();
    spawn_ui();
    register_systems();
    enter_menu();

    engine::ecs::EventWriter<engine::PlayMusicEvent>{world_}.send(engine::PlayMusicEvent{
            .id = assets::sound::music,
            .loop = true,
    });
}

void Game::on_quit() {
    audio_.stop_music(0.f);
}

void Game::spawn_camera() {
    const engine::ecs::Entity camera = world_.create();
    world_.emplace<engine::Transform>(camera);
    world_.emplace<engine::Camera>(camera, engine::Camera{.ortho_size = kOrthoHalf});
    world_.ctx<engine::ActiveCamera>().entity = camera;
}

void Game::spawn_ui() {
    canvas_ = world_.create();
    world_.emplace<engine::ui::UiCanvas>(canvas_, engine::ui::UiCanvas{
            .document = assets::ui::menu,
            .stylesheet = assets::ui::theme,
            .data_context = menu_vm_,
            .reference_size = {static_cast<float>(kWindowW), static_cast<float>(kWindowH)},
            .fit = engine::ui::UiFit::ScaleWithScreenSize,
            .order = 10,
    });
}

void Game::apply_screen() {
    auto& canvas = world_.get<engine::ui::UiCanvas>(canvas_);
    if (match_.screen() == Screen::Menu) {
        canvas.document = assets::ui::menu;
        canvas.data_context = menu_vm_;
    } else {
        canvas.document = assets::ui::play;
        canvas.data_context = play_vm_;
    }
}

void Game::play_step_sfx() {
    engine::ecs::EventWriter<engine::PlaySfxEvent>{world_}.send(engine::PlaySfxEvent{.id = assets::sound::step});
}

void Game::enter_menu() {
    match_.enter_menu();
    presenter_.refresh_scores(match_.board());
    presenter_.sync_marks(match_.board());
    presenter_.sync_result_message(match_.board());
    apply_screen();
}

void Game::start_play(bool pve) {
    match_.start_play(pve);
    presenter_.refresh_scores(match_.board());
    presenter_.sync_marks(match_.board());
    presenter_.sync_result_message(match_.board());
    apply_screen();
}

bool Game::can_click(int x, int y) const {
    return match_.can_click(x, y);
}

void Game::on_cell_click(int x, int y) {
    if (match_.on_cell_click(x, y) != MatchController::StepResult::Moved) {
        return;
    }
    play_step_sfx();
    presenter_.sync_marks(match_.board());
    presenter_.sync_result_message(match_.board());
    if (match_.board().outcome() == Outcome::Playing) {
        mark_placed_haptic();
    } else {
        round_outcome_haptic();
    }
}

void Game::tick_round(float dt) {
    switch (match_.tick(dt, rng_)) {
        case MatchController::StepResult::Moved:
            play_step_sfx();
            presenter_.sync_marks(match_.board());
            presenter_.sync_result_message(match_.board());
            if (match_.board().outcome() == Outcome::Playing) {
                mark_placed_haptic();
            } else {
                round_outcome_haptic();
            }
            break;
        case MatchController::StepResult::RoundEnded:
            presenter_.refresh_scores(match_.board());
            presenter_.sync_marks(match_.board());
            presenter_.sync_result_message(match_.board());
            break;
        case MatchController::StepResult::NoOp:
            break;
    }
}

void Game::tap_haptic() {
    haptics_.vibrate(kTapVibrationSeconds, kTapVibrationIntensity);
}

void Game::mark_placed_haptic() {
    haptics_.vibrate(kMarkPlacedVibrationSeconds, kMarkPlacedVibrationIntensity);
}

void Game::round_outcome_haptic() {
    haptics_.vibrate(kRoundOutcomeVibrationSeconds, kRoundOutcomeVibrationIntensity);
}

void Game::register_systems() {
    world_.add_system(engine::ecs::Schedule::Frame, engine::ecs::Phase::Game, [this](engine::ecs::World& world) {
        tick_round(world.ctx<engine::Time>().delta_time);
    });
}

}
