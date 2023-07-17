#include "view.hpp"

#include "world.hpp"

#include <iostream>

View::View(World& world)
    : _world(world)
    , _window(
        "Asset Preview",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        1000,
        800,
        0)
    , _renderer(
        _window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)
    , _resources(_renderer)
    , _scene(_resources)
{
    _scene.setScreenSize(1000, 800);

    subscribe<AppearEvent>(events, [this] (const AppearEvent& e) {
        if (e.type == EntityType::Hero) {
            _characterEntityMap[e.entity] = _scene.add(
                r::Character::Hero, {e.position.x, e.position.y});
        } else if (e.type == EntityType::Tree) {
            _spriteEntityMap[e.entity] = _scene.add(
                r::Object::Tree, {e.position.x, e.position.y});
        }
    });

    subscribe<MoveEvent>(events, [this] (const MoveEvent& e) {
        if (auto it = _spriteEntityMap.find(e.entity);
                it != _spriteEntityMap.end()) {
            auto& sprite = _scene.sprite(it->second);
            sprite.position = {e.position.x, e.position.y};
        } else if (auto it = _characterEntityMap.find(e.entity);
                it != _characterEntityMap.end()) {
            auto& character = _scene.character(it->second);
            character.position = {e.position.x, e.position.y};
            if (length(e.velocity) > 10.f) {
                character.speed = gfx::Speed::Run;
            } else if (length(e.velocity) > 1.f) {
                character.speed = gfx::Speed::Walk;
            } else {
                character.speed = gfx::Speed::Stand;
            }

            if (length(e.velocity) > 0) {
                if (std::abs(e.velocity.x) >
                        std::abs(e.velocity.y)) {
                    if (e.velocity.x > 0) {
                        character.direction = gfx::Direction::Right;
                    } else {
                        character.direction = gfx::Direction::Left;
                    }
                } else {
                    if (e.velocity.y > 0) {
                        character.direction = gfx::Direction::Up;
                    } else {
                        character.direction = gfx::Direction::Down;
                    }
                }
            }
        }
    });
}

bool View::done() const
{
    return _done;
}

void View::processInput()
{
    while (!_done) {
        if (auto event = sdl::pollEvent()) {
            this->processEvent(*event);
        } else {
            break;
        }
    }
}

void View::update(double delta)
{
    _scene.update(delta);
}

void View::render()
{
    _renderer.setDrawColor(50, 50, 50, 255);
    _renderer.clear();

    _scene.render(_renderer);

    _renderer.present();
}

void View::processEvent(const SDL_Event& event)
{
    if (event.type == SDL_QUIT || (
            event.type == SDL_KEYDOWN &&
            event.key.keysym.sym == SDLK_F4 &&
            ((event.key.keysym.mod & SDLK_LALT) ||
                (event.key.keysym.mod & SDLK_RALT)))) {
        _done = true;
        return;
    }

    if ((event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) &&
            event.key.repeat) {
        return;;
    }

    if (event.type == SDL_KEYDOWN &&
            event.key.keysym.sym == SDLK_a) {
        _keyboardControl.left = true;
    } else if (event.type == SDL_KEYDOWN &&
            event.key.keysym.sym == SDLK_d) {
        _keyboardControl.right = true;
    } else if (event.type == SDL_KEYDOWN &&
            event.key.keysym.sym == SDLK_w) {
        _keyboardControl.up = true;
    } else if (event.type == SDL_KEYDOWN &&
            event.key.keysym.sym == SDLK_s) {
        _keyboardControl.down = true;
    } else if (event.type == SDL_KEYUP &&
            event.key.keysym.sym == SDLK_a) {
        _keyboardControl.left = false;
    } else if (event.type == SDL_KEYUP &&
            event.key.keysym.sym == SDLK_d) {
        _keyboardControl.right = false;
    } else if (event.type == SDL_KEYUP &&
            event.key.keysym.sym == SDLK_w) {
        _keyboardControl.up = false;
    } else if (event.type == SDL_KEYUP &&
            event.key.keysym.sym == SDLK_s) {
        _keyboardControl.down = false;
    }

    auto controlDirection = Vector{};
    if (_keyboardControl.up) {
        controlDirection.y += 1;
    }
    if (_keyboardControl.down) {
        controlDirection.y -= 1;
    }
    if (_keyboardControl.left) {
        controlDirection.x -= 1;
    }
    if (_keyboardControl.right) {
        controlDirection.x += 1;
    }

    _world.control()->control = controlDirection;
}
