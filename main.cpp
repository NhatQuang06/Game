#include "def.h"
#include "Button.h"
#include "Player.cpp"
#include "Obstacle.cpp"
#include <fstream>
#include "Music.h"

AudioManager audioManager;
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* spriteSheet = nullptr;
SDL_Texture* scoreTexture = nullptr;
SDL_Texture* obstacleTexture = nullptr;
SDL_Texture* highScoreTexture = nullptr;
SDL_Texture* backgroundTexture = nullptr;
SDL_Texture* menuBackgroundTexture = nullptr;
SDL_Rect frames[FRAME_COUNT];
TTF_Font* font = nullptr;
SDL_Texture* backgroundDayTexture = nullptr;
SDL_Texture* backgroundNightTexture = nullptr;
SDL_Texture* currentBackgroundTexture = nullptr;

bool isDay = true;
bool isFading = false;
Uint32 fadeStartTime = 0;
const Uint32 fadeDuration = 2000;
Uint32 lastBackgroundSwitch = 0;
const Uint32 backgroundSwitchInterval = 15000;

bool inMainMenu = true;
bool showMusicMenu = false;
bool gameOver = false;
Button* startButton;
Button* musicButton;
Button* exitButton;
Button* restartButton;
Button* backToMenuButton;
Button* musicOnButton;
Button* musicOffButton;
Button* returnToMenuButton;
Uint32 lastUpdate = 0;
Uint32 gameStartTime = 0;
int obstacleSpeed = OBSTACLE_SPEED;
int animationSpeed = ANIMATION_SPEED;

Player* player = nullptr;
Obstacle* obstacle = nullptr;
int volumeLevel = 5;
const int maxVolumeLevel = 10;
int score = 0;
int highScore = 0;
int lastScore = -1;
int lastHighScore = -1;
bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;
    if (TTF_Init() == -1) return false;
    window = SDL_CreateWindow("Sprite Animation", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) return false;
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return false;
    font = TTF_OpenFont("times.ttf", 24);
    if (!audioManager.loadMusic("music.mp3")) return false;

    std::ifstream inFile("highscore.txt");
    if (inFile.is_open()) {
        inFile >> highScore;
        inFile.close();
    }

    SDL_Color textColor = {255, 255, 255, 255};
    startButton = new Button(WIDTH / 2 - 100, HEIGHT / 2 - 50, 200, 40, "Start Game", textColor, font);
    musicButton = new Button(WIDTH / 2 - 100, HEIGHT / 2, 200, 40, "Music", textColor, font);
    exitButton = new Button(WIDTH / 2 - 100, HEIGHT / 2 + 50, 200, 40, "Exit Game", textColor, font);
    musicOnButton = new Button(WIDTH / 2 - 100, HEIGHT / 2 - 100, 200, 40, "On", textColor, font);
    musicOffButton = new Button(WIDTH / 2 - 100, HEIGHT / 2 - 40, 200, 40, "Off", textColor, font);

    returnToMenuButton = new Button(WIDTH / 2 - 100, HEIGHT / 2 + 140, 200, 40, "Return", textColor, font);
    restartButton = new Button(WIDTH / 2 - 100, HEIGHT / 2 - 20, 200, 40, "Restart", textColor, font);
    backToMenuButton = new Button(WIDTH / 2 - 100, HEIGHT / 2 + 30, 200, 40, "Main Menu", textColor, font);
    return true;
}
bool loadMedia() {
    menuBackgroundTexture = IMG_LoadTexture(renderer, "main menu.png");
    backgroundDayTexture = IMG_LoadTexture(renderer, "background.png");
    backgroundNightTexture = IMG_LoadTexture(renderer, "background2.png");
    currentBackgroundTexture = backgroundDayTexture;
    spriteSheet = IMG_LoadTexture(renderer, "player_spritesheet.png");
    obstacleTexture = IMG_LoadTexture(renderer, "obstacle.png");
    if (!menuBackgroundTexture || !spriteSheet || !obstacleTexture) return false;

    int sheetWidth, sheetHeight;
    SDL_QueryTexture(spriteSheet, nullptr, nullptr, &sheetWidth, &sheetHeight);
    int frameWidth = sheetWidth / FRAME_COLS - 10;
    int frameHeight = sheetHeight / FRAME_ROWS;
    for (int i = 0; i < FRAME_COUNT; i++) {
        frames[i] = { (i % FRAME_COLS) * frameWidth, (i / FRAME_COLS) * frameHeight, frameWidth, frameHeight };
    }
    return true;
}
void updateScoreTextures() {
    SDL_Color white = {255, 255, 255, 255};

    if (score != lastScore) {
        if (scoreTexture) SDL_DestroyTexture(scoreTexture);
        std::string scoreText = "Score: " + std::to_string(score);
        SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText.c_str(), white);
        scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
        SDL_FreeSurface(scoreSurface);
        lastScore = score;
    }

    if (highScore != lastHighScore) {
        if (highScoreTexture) SDL_DestroyTexture(highScoreTexture);
        std::string hsText = "High Score: " + std::to_string(highScore);
        SDL_Surface* hsSurface = TTF_RenderText_Solid(font, hsText.c_str(), white);
        highScoreTexture = SDL_CreateTextureFromSurface(renderer, hsSurface);
        SDL_FreeSurface(hsSurface);
        lastHighScore = highScore;
    }
}
void close() {
    SDL_DestroyTexture(spriteSheet);
    SDL_DestroyTexture(obstacleTexture);
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(menuBackgroundTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    delete startButton;
    delete musicButton;
    delete exitButton;
    delete musicOnButton;
    delete musicOffButton;

    delete restartButton;
    delete backToMenuButton;
    delete returnToMenuButton;
    TTF_CloseFont(font);

    std::ofstream outFile("highscore.txt");
    if (outFile.is_open()) {
        outFile << highScore;
        outFile.close();
    }
    TTF_Quit();
    SDL_Quit();
}


void renderMenu() {
    SDL_RenderClear(renderer);
    SDL_Rect bgRect = { 0, 0, WIDTH, HEIGHT };
    SDL_RenderCopy(renderer, menuBackgroundTexture, nullptr, &bgRect);

    if (inMainMenu) {
        if (showMusicMenu) {
            musicOnButton->render(renderer);
            musicOffButton->render(renderer);
            int barX = WIDTH / 2 - 100;
            int barY = HEIGHT / 2 + 20;
            int barWidth = 200;
            int barHeight = 30;


            SDL_Rect outlineRect = { barX, barY, barWidth, barHeight };
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &outlineRect);


            SDL_Rect fillRect = { barX, barY, volumeLevel * (barWidth / maxVolumeLevel), barHeight };
            SDL_SetRenderDrawColor(renderer, 0, 191, 255, 255);
            SDL_RenderFillRect(renderer, &fillRect);
            returnToMenuButton->render(renderer);
        } else {
            startButton->render(renderer);
            musicButton->render(renderer);
            exitButton->render(renderer);
        }
    } else if (gameOver) {
        restartButton->render(renderer);
        backToMenuButton->render(renderer);
    }

    SDL_RenderPresent(renderer);
}

void handleEvents(bool& running) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) running = false;

        else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            int x = e.button.x;
            int y = e.button.y;
            if (inMainMenu) {
                if (showMusicMenu) {
                    if (musicOnButton->isClicked(x, y)) {
                        audioManager.resumeMusic();
                    } else if (musicOffButton->isClicked(x, y)) {
                        audioManager.pauseMusic();
                    } else if (returnToMenuButton->isClicked(x, y)) {
                        showMusicMenu = false;
                    }
                } else {
                    if (startButton->isClicked(x, y)) {
                        inMainMenu = false;
                        gameOver = false;
                        score = 0;
                        lastScore = -1;
                        delete player;
                        delete obstacle;
                        player = new Player(frames);
                        obstacle = new Obstacle();
                        obstacleSpeed = OBSTACLE_SPEED;
                        animationSpeed = ANIMATION_SPEED;
                        gameStartTime = SDL_GetTicks();
                        isDay = true;
                        isFading = false;
                        currentBackgroundTexture = backgroundDayTexture;
                        lastBackgroundSwitch = SDL_GetTicks();
                    } else if (musicButton->isClicked(x, y)) {
                        showMusicMenu = true;
                    } else if (exitButton->isClicked(x, y)) {
                        running = false;
                    }
                }
            } else if (gameOver) {
                if (restartButton->isClicked(x, y)) {
                    gameOver = false;
                    score = 0;
                    lastScore = -1;
                    delete player;
                    delete obstacle;
                    player = new Player(frames);
                    obstacle = new Obstacle();
                    gameStartTime = SDL_GetTicks();
                    isDay = true;
                    isFading = false;
                    currentBackgroundTexture = backgroundDayTexture;
                    lastBackgroundSwitch = SDL_GetTicks();
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
            musicOnButton->setHovered(false);
            musicOffButton->setHovered(false);
            restartButton->setHovered(false);
            backToMenuButton->setHovered(false);
            returnToMenuButton->setHovered(false);
            if (inMainMenu) {
                if (startButton->isMouseOver(x, y)) startButton->setHovered(true);
                if (musicButton->isMouseOver(x, y)) musicButton->setHovered(true);
                if (exitButton->isMouseOver(x, y)) exitButton->setHovered(true);
                if (showMusicMenu) {
                    if (musicOnButton->isMouseOver(x, y)) musicOnButton->setHovered(true);
                    if (musicOffButton->isMouseOver(x, y)) musicOffButton->setHovered(true);
                    if (returnToMenuButton->isMouseOver(x, y)) returnToMenuButton->setHovered(true);

                }
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
        else if (e.type == SDL_KEYDOWN && inMainMenu && showMusicMenu) {
            if (e.key.keysym.sym == SDLK_PLUS || e.key.keysym.sym == SDLK_EQUALS) {
                if (volumeLevel < maxVolumeLevel) {
                    volumeLevel++;
                    audioManager.setVolume(volumeLevel * (MIX_MAX_VOLUME / maxVolumeLevel));
                }
            }
            if (e.key.keysym.sym == SDLK_MINUS) {
                if (volumeLevel > 0) {
                    volumeLevel--;
                    audioManager.setVolume(volumeLevel * (MIX_MAX_VOLUME / maxVolumeLevel));
                }
            }
        }
    }
}

void render(const Player* player, const Obstacle* obstacle) {

    SDL_RenderClear(renderer);
    SDL_Rect bgRect = { 0, 0, WIDTH, HEIGHT };

    Uint32 now = SDL_GetTicks();

    if (isFading) {
        float t = (float)(now - fadeStartTime) / fadeDuration;
        if (t >= 1.0f) {
            isFading = false;
            isDay = !isDay;
            currentBackgroundTexture = isDay ? backgroundDayTexture : backgroundNightTexture;
            SDL_RenderCopy(renderer, currentBackgroundTexture, nullptr, &bgRect);
        } else {
            SDL_Texture* from = isDay ? backgroundDayTexture : backgroundNightTexture;
            SDL_Texture* to = isDay ? backgroundNightTexture : backgroundDayTexture;

            SDL_SetTextureAlphaMod(from, Uint8((1.0f - t) * 255));
            SDL_SetTextureAlphaMod(to, Uint8(t * 255));

            SDL_RenderCopy(renderer, from, nullptr, &bgRect);
            SDL_RenderCopy(renderer, to, nullptr, &bgRect);

            SDL_SetTextureAlphaMod(from, 255);
            SDL_SetTextureAlphaMod(to, 255);
        }
    } else {
        SDL_RenderCopy(renderer, currentBackgroundTexture, nullptr, &bgRect);
    }
    player->render(renderer, spriteSheet, gameOver);
    obstacle->render(renderer, obstacleTexture);
    if (gameOver) renderMenu();
    if (scoreTexture) {
        SDL_Rect scoreRect = {10, 10, 0, 0};
        SDL_QueryTexture(scoreTexture, nullptr, nullptr, &scoreRect.w, &scoreRect.h);
        SDL_RenderCopy(renderer, scoreTexture, nullptr, &scoreRect);
    }
    if (highScoreTexture) {
        SDL_Rect hsRect = {10, 40, 0, 0};
        SDL_QueryTexture(highScoreTexture, nullptr, nullptr, &hsRect.w, &hsRect.h);
        SDL_RenderCopy(renderer, highScoreTexture, nullptr, &hsRect);
    }
    SDL_RenderPresent(renderer);
}

void runGameLoop() {
    bool running = true;
    audioManager.playMusic();
    lastUpdate = SDL_GetTicks();
    gameStartTime = SDL_GetTicks();
    lastBackgroundSwitch = SDL_GetTicks();
    player = new Player(frames);
    obstacle = new Obstacle();

    while (running) {
        handleEvents(running);
        if (inMainMenu) {
            renderMenu();
        } else {
            if (!gameOver) {

                int elapsed = static_cast<int>((SDL_GetTicks() - gameStartTime) / 1000);


                obstacleSpeed = std::min(30, 15 + elapsed / 10);
                animationSpeed = std::max(30, 70 - elapsed / 2);

                player->update(lastUpdate);
                obstacle->update();
                Uint32 now = SDL_GetTicks();
                if (!isFading && now - lastBackgroundSwitch >= backgroundSwitchInterval) {
                    isFading = true;
                    fadeStartTime = now;
                    lastBackgroundSwitch = now;
                }
                score += 1;
                SDL_Rect playerHitbox = player->getHitbox();
                SDL_Rect obstacleHitbox = obstacle->getHitbox();

                if (SDL_HasIntersection(&playerHitbox, &obstacleHitbox)) {
                    gameOver = true;
                    if (score > highScore) {
                        highScore = score;
                    }
                }
            }
            updateScoreTextures();
            render(player, obstacle);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(10);
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
