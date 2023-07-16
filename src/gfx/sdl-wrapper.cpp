#include <gfx/sdl-wrapper.hpp>

#include <stdexcept>

namespace sdl {

namespace {

void check(int errorCode)
{
    if (errorCode != 0) {
        throw std::runtime_error{SDL_GetError()};
    }
}

template <class T>
T* check(T* ptr)
{
    if (ptr == nullptr) {
        throw std::runtime_error{SDL_GetError()};
    }
    return ptr;
}

} // namespace

Texture::Texture(SDL_Texture* ptr)
    : _ptr(ptr, SDL_DestroyTexture)
{ }

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

Renderer::Size Renderer::outputSize() const
{
    auto size = Size{};
    check(SDL_GetRendererOutputSize(_ptr.get(), &size.w, &size.h));
    return size;
}

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

void Renderer::fillRect(const SDL_FRect& rect)
{
    check(SDL_RenderFillRectF(_ptr.get(), &rect));
}

void Renderer::copy(
    const Texture& texture, const SDL_Rect& srcrect, const SDL_Rect& dstrect)
{
    check(SDL_RenderCopy(_ptr.get(), texture._ptr.get(), &srcrect, &dstrect));
}

void Renderer::copy(
    const Texture& texture, const SDL_Rect& srcrect, const SDL_FRect& dstrect)
{
    check(SDL_RenderCopyF(_ptr.get(), texture._ptr.get(), &srcrect, &dstrect));
}

Texture Renderer::loadTexture(const std::filesystem::path& file) const
{
    return Texture{check(IMG_LoadTexture(_ptr.get(), file.string().c_str()))};
}

Texture Renderer::loadTexture(std::span<const std::byte> data) const
{
    auto* io = check(SDL_RWFromConstMem(data.data(), (int)data.size()));
    return Texture{check(IMG_LoadTexture_RW(_ptr.get(), io, 1))};
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

namespace img {

void init(int flags)
{
    if (IMG_Init(flags) != flags) {
        throw std::runtime_error{"failed to initialize SDL_image"};
    }
}

void quit()
{
    IMG_Quit();
}

} // namespace img
