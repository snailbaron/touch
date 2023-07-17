#pragma once

#include "world.hpp"

#include <gfx/resources.hpp>
#include <gfx/scene.hpp>
#include <gfx/sdl-wrapper.hpp>

#include <evening.hpp>

struct KeyboardControl {
    bool left = false;
    bool right = false;
    bool up = false;
    bool down = false;
};

class View : public evening::Subscriber {
public:
    View(World& world);

    bool done() const;

    void processInput();
    void update(double delta);
    void render();

private:
    void processEvent(const SDL_Event& event);

    const World& _world;
    sdl::Window _window;
    sdl::Renderer _renderer;
    gfx::Resources _resources;
    gfx::Scene _scene;
    bool _done = false;
    KeyboardControl _keyboardControl;
    std::map<thing::Entity, size_t> _spriteEntityMap;
    std::map<thing::Entity, size_t> _characterEntityMap;
    thing::Entity _cameraEntity;
};
