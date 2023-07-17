#include <fi.hpp>
#include <gfx.hpp>
#include <tempo.hpp>
#include <ve.hpp>

#include <memory>

struct KeyboardControl {
    bool left = false;
    bool right = false;
    bool up = false;
    bool down = false;
};

template <class T> struct XYModel {
    T x;
    T y;
};
using Vector = ve::Vector<XYModel, float>;

struct Character {
    Vector position;
    Vector velocity;
};

Vector controlDirection(const KeyboardControl& kb)
{
    auto direction = Vector{};
    if (kb.up) {
        direction.y += 1;
    }
    if (kb.down) {
        direction.y -= 1;
    }
    if (kb.left) {
        direction.x -= 1;
    }
    if (kb.right) {
        direction.x += 1;
    }
    return direction;
}

int main()
{
    gfx::init();

    auto keyboardControl = KeyboardControl{};
    auto character = Character{};

    auto view = gfx::View{};

    auto heroSprite =
        view.scene().add(r::Character::Hero, gfx::Position{3.f, 1.f});
    [[maybe_unused]] auto treeSprite =
        view.scene().add(r::Object::Tree, gfx::Position{-1.f, -1.f});

    auto timer = tempo::FrameTimer{60};
    bool done = false;
    while (!done) {
        while (auto event = sdl::pollEvent()) {
            if (event->type == SDL_QUIT || (
                    event->type == SDL_KEYDOWN &&
                    event->key.keysym.sym == SDLK_F4 &&
                    ((event->key.keysym.mod & SDLK_LALT) ||
                        (event->key.keysym.mod & SDLK_RALT)))) {
                done = true;
                break;
            }

            if ((event->type == SDL_KEYDOWN || event->type == SDL_KEYUP) &&
                    event->key.repeat) {
                break;
            }

            if (event->type == SDL_KEYDOWN &&
                    event->key.keysym.sym == SDLK_a) {
                keyboardControl.left = true;
            } else if (event->type == SDL_KEYDOWN &&
                    event->key.keysym.sym == SDLK_d) {
                keyboardControl.right = true;
            } else if (event->type == SDL_KEYDOWN &&
                    event->key.keysym.sym == SDLK_w) {
                keyboardControl.up = true;
            } else if (event->type == SDL_KEYDOWN &&
                    event->key.keysym.sym == SDLK_s) {
                keyboardControl.down = true;
            } else if (event->type == SDL_KEYUP &&
                    event->key.keysym.sym == SDLK_a) {
                keyboardControl.left = false;
            } else if (event->type == SDL_KEYUP &&
                    event->key.keysym.sym == SDLK_d) {
                keyboardControl.right = false;
            } else if (event->type == SDL_KEYUP &&
                    event->key.keysym.sym == SDLK_w) {
                keyboardControl.up = false;
            } else if (event->type == SDL_KEYUP &&
                    event->key.keysym.sym == SDLK_s) {
                keyboardControl.down = false;
            }
        }
        if (done) {
            break;
        }

        if (int framesPassed = timer(); framesPassed > 0) {
            auto control = controlDirection(keyboardControl);

            static constexpr auto Acceleration = 30.f;
            static constexpr auto Deceleration = 50.f;
            static constexpr auto MaxSpeed = 8.f;

            for (int i = 0; i < framesPassed; i++) {
                character.velocity +=
                    control * timer.delta() * (Acceleration + Deceleration);

                if (auto speed = length(character.velocity); speed > 0) {
                    auto newSpeed = std::max<float>(
                        0.f, speed - (float)timer.delta() * Deceleration);
                    character.velocity *= newSpeed / speed;
                }

                if (auto speed = length(character.velocity); speed > MaxSpeed) {
                    character.velocity *= MaxSpeed / speed;
                }

                character.position += character.velocity * timer.delta();

                auto& sprite = view.scene().character(heroSprite);
                sprite.position = {character.position.x, character.position.y};
                if (length(character.velocity) > 10.f) {
                    sprite.speed = gfx::Speed::Run;
                } else if (length(character.velocity) > 1.f) {
                    sprite.speed = gfx::Speed::Walk;
                } else {
                    sprite.speed = gfx::Speed::Stand;
                }

                if (length(character.velocity) > 0) {
                    if (std::abs(character.velocity.x) >
                            std::abs(character.velocity.y)) {
                        if (character.velocity.x > 0) {
                            sprite.direction = gfx::Direction::Right;
                        } else {
                            sprite.direction = gfx::Direction::Left;
                        }
                    } else {
                        if (character.velocity.y > 0) {
                            sprite.direction = gfx::Direction::Up;
                        } else {
                            sprite.direction = gfx::Direction::Down;
                        }
                    }
                }
            }

            view.update(timer.delta() * framesPassed);
            view.render();
        }

        timer.relax();
    }

    gfx::quit();
}
