#include <gfx/view.hpp>

namespace gfx {

void init()
{
    sdl::init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    img::init(IMG_INIT_PNG);
}

void quit()
{
    img::quit();
    sdl::quit();
}

View::View()
    : _window(
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
}

Scene& View::scene()
{
    return _scene;
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

} // namespace gfx
