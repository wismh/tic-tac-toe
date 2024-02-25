#include <game/color_material.h>
#include <game/game.h>

#include <engine/core/application_state.h>
#include <engine/ecs/camera.h>
#include <engine/ecs/physics.h>
#include <engine/ecs/transform.h>
#include <engine/render/renderable.h>
#include <engine/ui/canvas.h>
#include <engine/ui/command.h>

#include <asset_ids.h>

#include <cmath>
#include <utility>

namespace game {
namespace {

constexpr const char* kVertexSrc = R"(
#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aUV;
out vec2 vUV;
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
void main() {
    vUV = aUV;
    gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
}
)";

constexpr const char* kFragmentSrc = R"(
#version 330 core
out vec4 FragColor;
in vec2 vUV;
uniform sampler2D uTexture;
uniform vec4 uColor;
void main() {
    FragColor = texture(uTexture, vUV) * uColor;
}
)";

engine::render::MeshDesc unit_quad() {
    engine::render::MeshDesc desc;
    desc.vertices = {
            {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}},
            {{0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}},
            {{0.5f, 0.5f, 0.0f}, {1.0f, 1.0f}},
            {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}},
            {{0.5f, 0.5f, 0.0f}, {1.0f, 1.0f}},
            {{-0.5f, 0.5f, 0.0f}, {0.0f, 1.0f}},
    };
    return desc;
}

}

Game::Game(engine::InputSystem&, engine::IAudioSystem& audio, engine::render::IGraphicFactory& factory) :
    audio_(audio), factory_(factory), hud_(std::make_shared<HudViewModel>()) {}

std::string Game::window_title() const {
    return "Tic Tac Toe";
}

glm::ivec2 Game::window_size() const {
    return {960, 540};
}

void Game::on_start() {
    spawn_camera();
    spawn_quad();
    spawn_hud();
    register_systems();
}

void Game::spawn_camera() {
    const engine::ecs::Entity camera = world_.create();
    world_.emplace<engine::Transform>(camera);
    world_.emplace<engine::Camera>(camera, engine::Camera{.ortho_size = 6.f});
    world_.ctx<engine::ActiveCamera>().entity = camera;
}

void Game::spawn_quad() {
    const auto mesh = factory_.create_mesh(unit_quad());
    const auto shader = factory_.create_shader({kVertexSrc, kFragmentSrc});
    engine::render::TextureDesc white;
    white.width = 1;
    white.height = 1;
    white.rgba = {255, 255, 255, 255};
    const auto texture = factory_.create_texture(white);
    auto material = std::make_shared<ColorMaterial>(shader, texture, glm::vec4{0.95f, 0.45f, 0.35f, 1.0f});

    quad_ = world_.create();
    world_.emplace<engine::Transform>(quad_, engine::Transform{.position = {0.f, 0.f, 0.f}});
    world_.emplace<engine::RigidBody>(quad_, engine::RigidBody{.velocity = {3.2f, 2.4f, 0.f}});
    world_.emplace<engine::BoxCollider>(quad_, engine::BoxCollider{.size = {1.f, 1.f, 1.f}});
    world_.emplace<engine::render::Renderable>(quad_, engine::render::Renderable{
            .mesh = mesh,
            .material = std::move(material),
            .color = {1.f, 1.f, 1.f, 1.f},
            .layer = 0,
            .order_in_layer = 0,
    });
}

void Game::spawn_hud() {
    const engine::ecs::Entity canvas = world_.create();
    world_.emplace<engine::ui::UiCanvas>(canvas, engine::ui::UiCanvas{
            .document = assets::ui::hud,
            .stylesheet = assets::ui::theme,
            .data_context = hud_,
            .fit = engine::ui::UiFit::FillWindow,
            .order = 10,
    });

    hud_->nudge = [this] {
        if (!world_.valid(quad_)) {
            return;
        }
        engine::RigidBody* body = world_.try_get<engine::RigidBody>(quad_);
        if (body == nullptr) {
            return;
        }
        body->velocity.x += (body->velocity.x >= 0.f) ? 1.5f : -1.5f;
        body->velocity.y += (body->velocity.y >= 0.f) ? 1.2f : -1.2f;
    };
    hud_->quit = [this] { world_.ctx<engine::ApplicationState>().quit(); };
}

void Game::register_systems() {
    world_.add_system(engine::ecs::Schedule::Fixed, engine::ecs::Phase::Game, [this](engine::ecs::World& world) {
        if (!world.valid(quad_)) {
            return;
        }
        engine::Transform* transform = world.try_get<engine::Transform>(quad_);
        engine::RigidBody* body = world.try_get<engine::RigidBody>(quad_);
        if (transform == nullptr || body == nullptr) {
            return;
        }

        const engine::ui::WindowSize& window = world.ctx<engine::ui::WindowSize>();
        const engine::ActiveCamera& active = world.ctx<engine::ActiveCamera>();
        const engine::Camera* camera =
                world.valid(active.entity) ? world.try_get<engine::Camera>(active.entity) : nullptr;
        if (camera == nullptr) {
            return;
        }

        const float aspect = engine::camera_aspect(*camera, window);
        const float half_h = camera->ortho_size;
        const float half_w = half_h * aspect;
        const float half = 0.5f;
        bool bounced = false;

        if (transform->position.x + half > half_w) {
            transform->position.x = half_w - half;
            body->velocity.x = -std::abs(body->velocity.x);
            bounced = true;
        } else if (transform->position.x - half < -half_w) {
            transform->position.x = -half_w + half;
            body->velocity.x = std::abs(body->velocity.x);
            bounced = true;
        }
        if (transform->position.y + half > half_h) {
            transform->position.y = half_h - half;
            body->velocity.y = -std::abs(body->velocity.y);
            bounced = true;
        } else if (transform->position.y - half < -half_h) {
            transform->position.y = -half_h + half;
            body->velocity.y = std::abs(body->velocity.y);
            bounced = true;
        }

        if (bounced && hud_) {
            hud_->bounces.set(hud_->bounces.get() + 1);
        }
    });
}

}
