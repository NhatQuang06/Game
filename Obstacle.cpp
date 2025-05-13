#include "def.h"
class Obstacle {
public:
    int x;
    Obstacle() : x(WIDTH) {}

    void update() {
        x -= OBSTACLE_SPEED;
        if (x < -50) x = WIDTH;
    }

    SDL_Rect getRect() const {
        return { x + 40, HEIGHT - 150, 100, 100 };
    }

    SDL_Rect getHitbox() const {
        SDL_Rect rect = getRect();
        return { rect.x + 5, rect.y + 5, rect.w - 10, rect.h - 10 };
    }

    void render(SDL_Renderer* renderer, SDL_Texture* texture) const {
        SDL_Rect rect = getRect();
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
    }

    void reset() {
        x = WIDTH;
    }
};
