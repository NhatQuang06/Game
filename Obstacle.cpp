#include "def.h"
class Obstacle {
public:
    int x;
    Obstacle() : x(WIDTH) {}

    void update() {
        extern int obstacleSpeed;
        x -= obstacleSpeed;
        if (x < -50) x = WIDTH;
    }

    SDL_Rect getRect() const {
        return { x + 40, HEIGHT - 150, 100, 100 };
    }

    SDL_Rect getHitbox() const {
        SDL_Rect rect = getRect();
        return { rect.x + 50, rect.y + 50, rect.w - 20, rect.h - 20 };
    }

    void render(SDL_Renderer* renderer, SDL_Texture* texture) const {
        SDL_Rect rect = getRect();
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
    }

    void reset() {
        x = WIDTH;
    }
};
