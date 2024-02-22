#pragma once

#include <engine/render/material.h>

#include <memory>
#include <utility>

namespace game {

// Minimal IMaterial: one shader, one texture slot, a flat instance color. Enough for the
// wind-host smoke test's bouncing quad; games with real art use `.mat` TOML instead.
class ColorMaterial final : public engine::render::IMaterial {
public:
    ColorMaterial(std::shared_ptr<engine::render::IShader> shader, std::shared_ptr<engine::render::ITexture> texture,
            glm::vec4 color)
        : shader_(std::move(shader)), texture_(std::move(texture)), color_(color) {}

    std::shared_ptr<engine::render::IShader> Shader() const override {
        return shader_;
    }

    std::shared_ptr<engine::render::ITexture> Texture(int slot) const override {
        return slot == 0 ? texture_ : nullptr;
    }

    glm::vec4 Color() const override {
        return color_;
    }

    engine::render::BlendMode Blend() const override {
        return engine::render::BlendMode::Opaque;
    }

private:
    std::shared_ptr<engine::render::IShader> shader_;
    std::shared_ptr<engine::render::ITexture> texture_;
    glm::vec4 color_;
};

}
