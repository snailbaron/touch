#include <gfx/sdl-wrapper.hpp>

namespace sdl {

namespace {

void check(int errorCode)
{
    if (errorCode != 0) {
        throw Error{SDL_GetError()};
    }
}

template <class T>
T* check(T* ptr)
{
    if (ptr == nullptr) {
        throw Error{SDL_GetError()};
    }
    return ptr;
}

} // namespace

Window::Window(
    const std::string& title, int x, int y, int w, int h, uint32_t flags)
    : _ptr(
        check(SDL_CreateWindow(title.c_str(), x, y, w, h, flags)),
        SDL_DestroyWindow)
{ }

Renderer::Renderer(Window& window, int index, uint32_t flags)
    : _ptr(
        check(SDL_CreateRenderer(window._ptr.get(), index, flags)),
        SDL_DestroyRenderer)
{ }

void Renderer::clear()
{
    check(SDL_RenderClear(_ptr.get()));
}

void Renderer::present()
{
    SDL_RenderPresent(_ptr.get());
}

void Renderer::setDrawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    check(SDL_SetRenderDrawColor(_ptr.get(), r, g, b, a));
}

void Renderer::fillRect(const SDL_Rect& rect)
{
    check(SDL_RenderFillRect(_ptr.get(), &rect));
}

void Renderer::fillRectF(const SDL_FRect& rect)
{
    check(SDL_RenderFillRectF(_ptr.get(), &rect));
}

void init(uint32_t flags)
{
    check(SDL_Init(flags));
}

void quit()
{
    SDL_Quit();
}

std::optional<SDL_Event> pollEvent()
{
    auto event = SDL_Event{};
    if (SDL_PollEvent(&event)) {
        return event;
    }
    return std::nullopt;
}

} // namespace sdl
