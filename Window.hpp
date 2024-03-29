#pragma once
#include <vector>
#include <SDL.h>

#undef main

#include "Game.hpp"

#define MIN_WINDOW_WIDTH 400
#define MIN_WINDOW_HEIGHT 400

//Define the default background color to the window.
#define BACKGROUND_RED 122
#define BACKGROUND_GREEN 122
#define BACKGROUND_BLUE 122

class Window
{
public:
    Window();
    ~Window();

    void init(const char* title, int xpos, int ypos, bool fullscreen);
    void render();

    void update(float deltaTime);

    bool running();

    void calculateInitialWindowDimensions();

    void handleEvents();
    void handleKeyDown(SDL_KeyboardEvent& key);
    void handleKeyUp(SDL_KeyboardEvent& key);

    void resizeWindow(int windowWidth, int windowHeight);

    void clean();

    static SDL_Renderer* renderer;
    static int screenHeight;
    static int screenWidth;

private:
    bool isRunning;
    SDL_Window* window;
    bool frozen;
    Game* game;
};


