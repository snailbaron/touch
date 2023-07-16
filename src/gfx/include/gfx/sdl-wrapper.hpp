#pragma once

#include <SDL.h>
#include <SDL_image.h>

#include <cstddef>
#include <exception>
#include <filesystem>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>

namespace sdl {

class Texture {
private:
    explicit Texture(SDL_Texture* ptr);

    std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> _ptr;

    friend class Renderer;
};

class Window {
public:
    Window(
        const std::string& title, int x, int y, int w, int h, uint32_t flags);

private:
    std::unique_ptr<SDL_Window, void(*)(SDL_Window*)> _ptr;

    friend class Renderer;
};

class Renderer {
public:
    struct Size {
        int w = 0;
        int h = 0;
    };

    Renderer(Window& window, int index, uint32_t flags);

    Size outputSize() const;

    void clear();
    void present();

    void setDrawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

    void fillRect(const SDL_Rect& rect);
    void fillRect(const SDL_FRect& rect);

    void copy(
        const Texture& texture,
        const SDL_Rect& srcrect,
        const SDL_Rect& dstrect);
    void copy(
        const Texture& texture,
        const SDL_Rect& srcrect,
        const SDL_FRect& dstrect);

    Texture loadTexture(const std::filesystem::path& file) const;
    Texture loadTexture(std::span<const std::byte> data) const;

private:
    std::unique_ptr<SDL_Renderer, void(*)(SDL_Renderer*)> _ptr;
};

void init(uint32_t flags);
void quit();

std::optional<SDL_Event> pollEvent();

} // namespace sdl

namespace img {

void init(int flags);
void quit();

} // namespace img
