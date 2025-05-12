#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <SDL_image.h>
const int WIDTH = 1200;
const int HEIGHT = 700;
const int FRAME_COUNT = 6;
const int FRAME_COLS = 3;
const int FRAME_ROWS = 2;
const int ANIMATION_SPEED = 70;
const int JUMP_HEIGHT = 200;
const int GRAVITY = 5;
const int OBSTACLE_SPEED = 15;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* spriteSheet = nullptr;
SDL_Texture* obstacleTexture = nullptr;
SDL_Texture* backgroundTexture = nullptr;
SDL_Rect frames[FRAME_COUNT];
TTF_Font* font = nullptr;

bool gameOver = false;
bool showMenu = false;

struct Button {
    SDL_Rect rect;
    std::string text;
    SDL_Color color;
    SDL_Texture* texture;

    Button(int x, int y, int w, int h, const std::string& t)
        : rect{ x, y, w, h }, text(t), color({255, 255, 255, 255}), texture(nullptr) {}

    void createTexture(SDL_Renderer* renderer, TTF_Font* font) {
        SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }

    void render(SDL_Renderer* renderer) {
        SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
        if (texture) {
            int texW, texH;
            SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
            SDL_Rect dst = { rect.x + (rect.w - texW) / 2, rect.y + (rect.h - texH) / 2, texW, texH };
            SDL_RenderCopy(renderer, texture, NULL, &dst);
        }
    }

    bool isClicked(int x, int y) {
        return x > rect.x && x < rect.x + rect.w && y > rect.y && y < rect.y + rect.h;
    }
};

std::vector<Button> buttons;

class Player {
public:
    int x, y, startY;
    bool jumping = false, falling = false;
    SDL_Rect* frames;
    int frameIndex = 0;

    Player(SDL_Rect* spriteFrames) : x(50), frames(spriteFrames) {
        y = HEIGHT - frames[0].h * 0.5 - 50;
    }

    void handleJump() {
        if (!jumping && !falling) {
            jumping = true;
            startY = y;
        }
    }

    void update(Uint32& lastUpdate) {
        Uint32 now = SDL_GetTicks();
        if (now - lastUpdate > ANIMATION_SPEED) {
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
        return { rect.x + 10, rect.y + 20, rect.w - 10, rect.h - 10 };
    }

    void render(SDL_Renderer* renderer, SDL_Texture* texture, bool isGameOver) const {
        SDL_Rect dest = getDestRect();
        SDL_RenderCopy(renderer, texture, isGameOver ? &frames[0] : &frames[frameIndex], &dest);
    }
};

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

Uint32 lastUpdate = 0;
Player* player = nullptr;
Obstacle* obstacle = nullptr;

bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;
    if (TTF_Init() == -1) return false;
    window = SDL_CreateWindow("Sprite Animation", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) return false;
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return false;
    font = TTF_OpenFont("times.ttf", 24);
    return font != nullptr;
}

bool loadMedia() {
    backgroundTexture = IMG_LoadTexture(renderer, "background.png");
    spriteSheet = IMG_LoadTexture(renderer, "player_spritesheet.png");
    obstacleTexture = IMG_LoadTexture(renderer, "obstacle.png");
    if (!backgroundTexture || !spriteSheet || !obstacleTexture) return false;

    int sheetWidth, sheetHeight;
    SDL_QueryTexture(spriteSheet, nullptr, nullptr, &sheetWidth, &sheetHeight);
    int frameWidth = sheetWidth / FRAME_COLS;
    int frameHeight = sheetHeight / FRAME_ROWS;
    for (int i = 0; i < FRAME_COUNT; i++) {
        frames[i] = { (i % FRAME_COLS) * frameWidth, (i / FRAME_COLS) * frameHeight, frameWidth, frameHeight };
    }

    buttons.push_back(Button(WIDTH / 2 - 100, 270, 200, 50, "Retry"));
    buttons.push_back(Button(WIDTH / 2 - 100, 340, 200, 50, "Exit"));

    for (auto& btn : buttons) {
        btn.createTexture(renderer, font);
    }

    return true;
}

void close() {
    for (auto& btn : buttons) {
        SDL_DestroyTexture(btn.texture);
    }
    SDL_DestroyTexture(spriteSheet);
    SDL_DestroyTexture(obstacleTexture);
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
}

void renderMenu() {
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
    for (auto& btn : buttons) {
        btn.render(renderer);
    }
    SDL_RenderPresent(renderer);
}

void handleEvents(bool& running) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) running = false;
        else if (e.type == SDL_KEYDOWN && !showMenu) {
            if (!gameOver && e.key.keysym.sym == SDLK_SPACE) {
                player->handleJump();
            } else if (gameOver && e.key.keysym.sym == SDLK_r) {
                gameOver = false;
                showMenu = false;
                player->reset();
                obstacle->reset();
                lastUpdate = SDL_GetTicks();
            } else if (e.key.keysym.sym == SDLK_ESCAPE) running = false;
        } else if (e.type == SDL_MOUSEBUTTONDOWN && showMenu) {
            int mx = e.button.x;
            int my = e.button.y;
            if (buttons[0].isClicked(mx, my)) {                    // Retry
                gameOver = false;
                showMenu = false;
                player->reset();
                obstacle->reset();
                lastUpdate = SDL_GetTicks();
            } else if (buttons[1].isClicked(mx, my)) running = false;   // Exit
        }
    }
}

void render(const Player* player, const Obstacle* obstacle) {
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
    player->render(renderer, spriteSheet, gameOver);
    obstacle->render(renderer, obstacleTexture);
    SDL_RenderPresent(renderer);
}

void runGameLoop() {
    bool running = true;
    lastUpdate = SDL_GetTicks();
    player = new Player(frames);
    obstacle = new Obstacle();

    while (running) {
        handleEvents(running);
        if (showMenu) {
            renderMenu();
            SDL_Delay(16);
            continue;
        }

        if (!gameOver) {
            player->update(lastUpdate);
            obstacle->update();
            SDL_Rect playerHitbox = player->getHitbox();
            SDL_Rect obstacleHitbox = obstacle->getHitbox();
            if (SDL_HasIntersection(&playerHitbox, &obstacleHitbox)) {
                gameOver = true;
                showMenu = true;
            }
        }

        render(player, obstacle);
        SDL_Delay(16);
    }

    delete player;
    delete obstacle;
}

int main(int argc, char* args[]) {
    if (!init() || !loadMedia()) return -1;
    runGameLoop();
    close();
    return 0;
}


