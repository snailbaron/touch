#include <gfx/init.hpp>

#include <gfx/sdl-wrapper.hpp>

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

} // namespace gfx
