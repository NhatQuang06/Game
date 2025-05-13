#pragma once
#include "def.h"

class Obstacle {
public:
    int x;

    Obstacle();
    void update();
    SDL_Rect getRect() const;
    SDL_Rect getHitbox() const;
    void render(SDL_Renderer* renderer, SDL_Texture* texture) const;
    void reset();
};
