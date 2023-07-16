#pragma once

#include <gfx/resources.hpp>
#include <gfx/scene.hpp>
#include <gfx/sdl-wrapper.hpp>

namespace gfx {

void init();
void quit();

class View {
public:
    View();

    Scene& scene();

    void update(double delta);
    void render();

private:
    sdl::Window _window;
    sdl::Renderer _renderer;
    Resources _resources;
    Scene _scene;
};

} // namespace gfx
