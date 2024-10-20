#include <SDL2/SDL.h>   //The SDL2/SDL.h is for my environment
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <cmath>

// Base Game Class
class Game {
public:
    virtual ~Game() {}
    virtual void init() = 0; // Pure virtual function
    virtual void run() = 0;  // Pure virtual function
    virtual void handleEvents() = 0;
    virtual void render() = 0;
};

// Constants
const int WIDTH = 1000;
const int HEIGHT = 800;
const int BLOCK_SIZE = 20;
const int INITIAL_LENGTH = 3;

enum Direction { UP, DOWN, LEFT, RIGHT };

struct SnakeSegment {
    int x, y;
};

// Derived SnakeGame Class
class SnakeGame : public Game { // Inheritance from Game
public:
    SnakeGame();
    ~SnakeGame();
    void init() override;    // Override base class methods
    void run() override;
    void handleEvents() override;
    void render() override;

private:
    void close();
    void update();
    void generateFood();
    bool collision(int x, int y);
    void updateScore();
    void checkLevelUp();
    void setSnakeColor(int level);
    void drawCircle(int centerX, int centerY, int radius, SDL_Color color);
    void drawEye(int centerX, int centerY, int radius, SDL_Color color);

    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;

    std::vector<SnakeSegment> snake;
    Direction dir;
    int foodX, foodY;
    bool foodEaten;
    int score;
    int level;
    SDL_Color snakeColor;
};

// SnakeGame Class Definitions
SnakeGame::SnakeGame() : running(true), dir(RIGHT), foodEaten(true), score(0), level(1) {
    snake.push_back({WIDTH / 2, HEIGHT / 2});
    setSnakeColor(level);
}

SnakeGame::~SnakeGame() {
    close();
}

void SnakeGame::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        exit(1);
    }
    window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        exit(1);
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        exit(1);
    }
}

void SnakeGame::close() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void SnakeGame::generateFood() {
    foodX = (rand() % (WIDTH / BLOCK_SIZE)) * BLOCK_SIZE;
    foodY = (rand() % (HEIGHT / BLOCK_SIZE)) * BLOCK_SIZE;
}

bool SnakeGame::collision(int x, int y) {
    for (const auto& segment : snake) {
        if (segment.x == x && segment.y == y) {
            return true;
        }
    }
    return false;
}

void SnakeGame::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        } else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_UP:
                    if (dir != DOWN) dir = UP;
                    break;
                case SDLK_DOWN:
                    if (dir != UP) dir = DOWN;
                    break;
                case SDLK_LEFT:
                    if (dir != RIGHT) dir = LEFT;
                    break;
                case SDLK_RIGHT:
                    if (dir != LEFT) dir = RIGHT;
                    break;
            }
        }
    }
}

void SnakeGame::update() {
    int newX = snake.front().x;
    int newY = snake.front().y;

    switch (dir) {
        case UP:    newY -= BLOCK_SIZE; break;
        case DOWN:  newY += BLOCK_SIZE; break;
        case LEFT:  newX -= BLOCK_SIZE; break;
        case RIGHT: newX += BLOCK_SIZE; break;
    }

    if (newX < 0) newX = WIDTH - BLOCK_SIZE;
    if (newX >= WIDTH) newX = 0;
    if (newY < 0) newY = HEIGHT - BLOCK_SIZE;
    if (newY >= HEIGHT) newY = 0;

    if (collision(newX, newY)) {
        running = false;
    }

    if (newX == foodX && newY == foodY) {
        foodEaten = true;
        score++;
        checkLevelUp();
    } else {
        snake.pop_back();
    }

    snake.insert(snake.begin(), {newX, newY});

    if (foodEaten) {
        generateFood();
        foodEaten = false;
    }

    updateScore();
}

void SnakeGame::updateScore() {
    std::string title = "Snake Game - Score: " + std::to_string(score) + " | Level: " + std::to_string(level);
    SDL_SetWindowTitle(window, title.c_str());
}

void SnakeGame::checkLevelUp() {
    if (score % 10 == 0) {
        level++;
        setSnakeColor(level);
    }
}

void SnakeGame::setSnakeColor(int level) {
    switch (level) {
        case 1: snakeColor = {0, 255, 0, 255}; break;
        case 2: snakeColor = {0, 0, 255, 255}; break;
        case 3: snakeColor = {255, 255, 0, 255}; break;
        case 4: snakeColor = {255, 0, 255, 255}; break;
        case 5: snakeColor = {255, 165, 0, 255}; break;
        default: snakeColor = {255, 0, 0, 255}; break;
    }
}

void SnakeGame::drawCircle(int centerX, int centerY, int radius, SDL_Color color) {
    int offsetX, offsetY, d;
    offsetX = 0;
    offsetY = radius;
    d = 1 - radius;

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    while (offsetX <= offsetY) {
        SDL_RenderDrawPoint(renderer, centerX + offsetX, centerY + offsetY);
        SDL_RenderDrawPoint(renderer, centerX + offsetY, centerY + offsetX);
        SDL_RenderDrawPoint(renderer, centerX - offsetX, centerY + offsetY);
        SDL_RenderDrawPoint(renderer, centerX - offsetY, centerY + offsetX);
        SDL_RenderDrawPoint(renderer, centerX + offsetX, centerY - offsetY);
        SDL_RenderDrawPoint(renderer, centerX + offsetY, centerY - offsetX);
        SDL_RenderDrawPoint(renderer, centerX - offsetX, centerY - offsetY);
        SDL_RenderDrawPoint(renderer, centerX - offsetY, centerY - offsetX);

        if (d < 0) {
            d += 2 * offsetX + 3;
        } else {
            d += 2 * (offsetX - offsetY) + 5;
            offsetY--;
        }
        offsetX++;
    }
}

void SnakeGame::drawEye(int centerX, int centerY, int radius, SDL_Color color) {
    drawCircle(centerX + radius / 2, centerY - radius / 2, radius / 4, color);
}

void SnakeGame::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (size_t i = 0; i < snake.size(); ++i) {
        const auto& segment = snake[i];
        drawCircle(segment.x + BLOCK_SIZE / 2, segment.y + BLOCK_SIZE / 2, BLOCK_SIZE / 2, snakeColor);

        if (i == 0) {
            int eyeX = segment.x + BLOCK_SIZE / 2;
            int eyeY = segment.y + BLOCK_SIZE / 2;

            switch (dir) {
                case UP:    drawEye(eyeX, eyeY, BLOCK_SIZE / 2, {255, 255, 255, 255}); break;
                case DOWN:  drawEye(eyeX, eyeY, BLOCK_SIZE / 2, {255, 255, 255, 255}); break;
                case LEFT:  drawEye(eyeX, eyeY, BLOCK_SIZE / 2, {255, 255, 255, 255}); break;
                case RIGHT: drawEye(eyeX, eyeY, BLOCK_SIZE / 2, {255, 255, 255, 255}); break;
            }
        }
    }

    drawCircle(foodX + BLOCK_SIZE / 2, foodY + BLOCK_SIZE / 2, BLOCK_SIZE / 2, {255, 0, 0, 255});
    SDL_RenderPresent(renderer);
}

void SnakeGame::run() {
    while (running) {
        handleEvents();
        update();
        render();
        SDL_Delay(100);
    }
}

int main(int argc, char* argv[]) {
    srand(static_cast<unsigned int>(time(nullptr)));

    Game* game = new SnakeGame();  // Polymorphism: Treat SnakeGame as a Game object
    game->init();
    game->run();
    delete game;  // Clean up

    return 0;
}
