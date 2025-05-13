#include "def.h"

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
