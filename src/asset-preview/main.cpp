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

enum class Speed {
    Stand,
    Walk,
    Run,
};

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
    auto keyboardControl = KeyboardControl{};
    auto character = Character{};

    sdl::init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    auto window = sdl::Window{
        "Asset Preview",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        1000,
        800,
        0};

    auto renderer = sdl::Renderer{
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC};

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
            }

            renderer.setDrawColor(50, 50, 50, 255);
            renderer.clear();

            auto screenPosition = Vector{
                500.f + character.position.x * 32.f,
                400.f - character.position.y * 32.f,
            };

            renderer.setDrawColor(200, 200, 200, 255);
            renderer.fillRectF({
                .x = screenPosition.x - 10.f,
                .y = screenPosition.y - 10.f,
                .w = 20.f,
                .h = 20.f,
            });

            renderer.present();
        }

        timer.relax();
    }

    sdl::quit();
}
