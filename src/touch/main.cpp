#include "view.hpp"
#include "world.hpp"

#include <fi.hpp>
#include <gfx.hpp>
#include <tempo.hpp>
#include <ve.hpp>

#include <memory>

int main()
{
    gfx::init();

    auto world = World{};
    auto view = View{world};

    auto timer = tempo::FrameTimer{60};
    for (;;) {
        view.processInput();
        if (view.done()) {
            break;
        }

        if (int framesPassed = timer(); framesPassed > 0) {
            for (int i = 0; i < framesPassed; i++) {
                world.update(timer.delta());
            }

            events.deliver();

            view.update(timer.delta() * framesPassed);
            view.render();
        }

        timer.relax();
    }

    gfx::quit();
}
