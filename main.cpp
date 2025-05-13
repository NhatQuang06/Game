#include "def.h"
#include "Button.h"
#include "Player.cpp"
#include "Obstacle.cpp"

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* spriteSheet = nullptr;
SDL_Texture* obstacleTexture = nullptr;
SDL_Texture* backgroundTexture = nullptr;
SDL_Rect frames[FRAME_COUNT];
TTF_Font* font = nullptr;

bool inMainMenu = true;
bool gameOver = false;
Button* startButton;
Button* musicButton;
Button* exitButton;
Button* restartButton;
Button* backToMenuButton;
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
    // Khởi tạo các nút
    SDL_Color textColor = {255, 255, 255, 255};
    startButton = new Button(WIDTH / 2 - 100, HEIGHT / 2 - 50, 200, 40, "Start Game", textColor, font);
    musicButton = new Button(WIDTH / 2 - 100, HEIGHT / 2, 200, 40, "Music", textColor, font);
    exitButton = new Button(WIDTH / 2 - 100, HEIGHT / 2 + 50, 200, 40, "Exit Game", textColor, font);

    restartButton = new Button(WIDTH / 2 - 100, HEIGHT / 2 - 20, 200, 40, "Restart", textColor, font);
    backToMenuButton = new Button(WIDTH / 2 - 100, HEIGHT / 2 + 30, 200, 40, "Main Menu", textColor, font);
    return true;
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
    return true;
}

void close() {
    SDL_DestroyTexture(spriteSheet);
    SDL_DestroyTexture(obstacleTexture);
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    delete startButton;
    delete musicButton;
    delete exitButton;
    delete restartButton;
    delete backToMenuButton;
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
}

// ================= MENU & RENDER ===================
void renderMenu() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_Rect menuRect = { WIDTH / 4, HEIGHT / 4, WIDTH / 2, HEIGHT / 2 };
    SDL_RenderFillRect(renderer, &menuRect);
    if (inMainMenu) {
        startButton->render(renderer);
        musicButton->render(renderer);
        exitButton->render(renderer);
    } else if (gameOver) {
        restartButton->render(renderer);
        backToMenuButton->render(renderer);
    }
}

void handleEvents(bool& running) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) running = false;

        else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            int x = e.button.x;
            int y = e.button.y;
            if (inMainMenu) {
                if (startButton->isClicked(x, y)) {
                    inMainMenu = false;
                    gameOver = false;
                } else if (musicButton->isClicked(x, y)) {

                } else if (exitButton->isClicked(x, y)) {
                    running = false;
                }
            } else if (gameOver) {
                if (restartButton->isClicked(x, y)) {
                    gameOver = false;
                    delete player;
                    delete obstacle;
                    player = new Player(frames);
                    obstacle = new Obstacle();
                } else if (backToMenuButton->isClicked(x, y)) {
                    inMainMenu = true;
                }
            }
        }
        else if (e.type == SDL_MOUSEMOTION) {
            int x = e.motion.x;
            int y = e.motion.y;

            startButton->setHovered(false);
            musicButton->setHovered(false);
            exitButton->setHovered(false);
            restartButton->setHovered(false);
            backToMenuButton->setHovered(false);

            if (inMainMenu) {
                if (startButton->isMouseOver(x, y)) startButton->setHovered(true);
                if (musicButton->isMouseOver(x, y)) musicButton->setHovered(true);
                if (exitButton->isMouseOver(x, y)) exitButton->setHovered(true);
            } else if (gameOver) {
                if (restartButton->isMouseOver(x, y)) restartButton->setHovered(true);
                if (backToMenuButton->isMouseOver(x, y)) backToMenuButton->setHovered(true);
            }
        }
        else if (e.type == SDL_KEYDOWN && !inMainMenu && !gameOver) {
            if (e.key.keysym.sym == SDLK_SPACE) {
                player->handleJump();
            }
        }
    }
}

void render(const Player* player, const Obstacle* obstacle) {
    SDL_Rect bgRect = { 0, 0, WIDTH, HEIGHT };
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, backgroundTexture, nullptr, &bgRect);
    player->render(renderer, spriteSheet, gameOver);
    obstacle->render(renderer, obstacleTexture);
    if (gameOver) renderMenu();
    SDL_RenderPresent(renderer);
}

// ================= GAME LOOP ===================
void runGameLoop() {
    bool running = true;
    lastUpdate = SDL_GetTicks();
    player = new Player(frames);
    obstacle = new Obstacle();

    while (running) {
        handleEvents(running);
        if (inMainMenu) {
            // Hiển thị menu
            renderMenu();
        } else {
            // Nếu game chưa kết thúc, tiếp tục cập nhật
            if (!gameOver) {
                player->update(lastUpdate);
                obstacle->update();
                SDL_Rect playerHitbox = player->getHitbox();
                SDL_Rect obstacleHitbox = obstacle->getHitbox();

                if (SDL_HasIntersection(&playerHitbox, &obstacleHitbox)) {
                    gameOver = true;
                }
            }
            render(player, obstacle);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    delete player;
    delete obstacle;
}

// ================= MAIN ===================
int main(int argc, char* args[]) {
    if (!init() || !loadMedia()) return -1;
    runGameLoop();
    close();
    return 0;
}
