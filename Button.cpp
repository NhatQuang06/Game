#include "Button.h"
#include <SDL.h>
#include <SDL_ttf.h>

Button::Button(int x, int y, int w, int h, const std::string& text, SDL_Color color, TTF_Font* font) : rect{ x, y, w, h }, text(text), textColor(color), font(font) {}

void Button::render(SDL_Renderer* renderer) {
    if (isHovered) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 70, 70, 70, 255);
    }

    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &rect);

    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), textColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect textRect = {
        rect.x + (rect.w - surface->w) / 2,
        rect.y + (rect.h - surface->h) / 2,
        surface->w,
        surface->h
    };

    SDL_RenderCopy(renderer, texture, NULL, &textRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

bool Button::isClicked(int x, int y) {
    SDL_Point point = {x, y};
    return SDL_PointInRect(&point, &rect);
}
