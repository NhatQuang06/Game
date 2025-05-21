#include "def.h"
class Player {
public:
    int x, y, startY;
    bool jumping = false, falling = false;
    SDL_Rect* frames;
    int frameIndex = 0;

    Player(SDL_Rect* spriteFrames) : x(50), frames(spriteFrames) {
        y = HEIGHT - frames[0].h * 0.5 - 70;
    }

    void handleJump() {
        if (!jumping && !falling) {
            jumping = true;
            startY = y;
        }
    }

    void update(Uint32& lastUpdate) {
        Uint32 now = SDL_GetTicks();
        extern int animationSpeed;
        if (now - lastUpdate > animationSpeed) {
            frameIndex = (frameIndex + 1) % FRAME_COUNT;
            lastUpdate = now;
        }

        if (jumping) {
            y -= GRAVITY * 2;
            if (startY - y >= JUMP_HEIGHT) {
                jumping = false;
                falling = true;
            }
        } else if (falling) {
            y += GRAVITY;
            if (y >= HEIGHT - frames[0].h * 0.5 - 50) {
                y = HEIGHT - frames[0].h * 0.5 - 50;
                falling = false;
            }
        }
    }

    void reset() {
        x = 50;
        y = HEIGHT - frames[0].h * 0.5 - 50;
        frameIndex = 0;
        jumping = false;
        falling = false;
    }

    SDL_Rect getDestRect() const {
        return { x, y + 120, static_cast<int>(frames[0].w * 0.3), static_cast<int>(frames[0].h * 0.3) };
    }

    SDL_Rect getHitbox() const {
        SDL_Rect rect = getDestRect();
        return { rect.x , rect.y + 20, rect.w - 10, rect.h - 10 };
    }

    void render(SDL_Renderer* renderer, SDL_Texture* texture, bool isGameOver) const {
        SDL_Rect dest = getDestRect();
        SDL_RenderCopy(renderer, texture, isGameOver ? &frames[0] : &frames[frameIndex], &dest);
    }
};
