#pragma once

#include <SDL.h>

#include <exception>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

namespace sdl {

class Error : public std::exception {
public:
    Error(std::string_view message)
        : _message(message)
    { }

    const char* what() const noexcept override
    {
        return _message.c_str();
    }

private:
    std::string _message;
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
    Renderer(Window& window, int index, uint32_t flags);

    void clear();
    void present();

    void setDrawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

    void fillRect(const SDL_Rect& rect);
    void fillRectF(const SDL_FRect& rect);

private:
    std::unique_ptr<SDL_Renderer, void(*)(SDL_Renderer*)> _ptr;
};

void init(uint32_t flags);
void quit();

std::optional<SDL_Event> pollEvent();

} // namespace sdl
