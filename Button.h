#ifndef BUTTON_H
#define BUTTON_H
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

class Button {
private:
    SDL_Rect rect;
    std::string text;
    SDL_Color textColor;
    TTF_Font* font;
    bool isHovered = false;

public:
    Button(int x, int y, int w, int h, const std::string& text, SDL_Color color, TTF_Font* font);

    void render(SDL_Renderer* renderer);
    bool isClicked(int x, int y);
    bool isMouseOver(int x, int y) const {
        SDL_Point point = { x, y };
        return SDL_PointInRect(&point, &rect);
    }
    void setHovered(bool hovered) {
        isHovered = hovered;
    }
};

#endif
