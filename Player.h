#pragma once
#include "def.h"

class Player {
public:
    int x, y, startY;
    bool jumping, falling;
    SDL_Rect* frames;
    int frameIndex;

    Player(SDL_Rect* spriteFrames);
    void handleJump();
    void update(Uint32& lastUpdate);
    void reset();
    SDL_Rect getDestRect() const;
    SDL_Rect getHitbox() const;
    void render(SDL_Renderer* renderer, SDL_Texture* texture, bool isGameOver) const;
};
