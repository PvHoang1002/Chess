#include <iostream>
#include <SDL_image.h>

#include "Window.hpp"

int Window::screenHeight = 0;
int Window::screenWidth = 0;
SDL_Renderer* Window::renderer = nullptr;

Window::Window() {
	frozen = true;
}

Window::~Window() {

}

void Window::init(const char* title, int xpos, int ypos, bool fullscreen) {
	int flags = SDL_WINDOW_RESIZABLE;

	if (fullscreen) {
		flags = flags | SDL_WINDOW_FULLSCREEN;
	}
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
		std::cout << "Subsystems Initialized!\n";
		calculateInitialWindowDimensions();
		window = SDL_CreateWindow(title, xpos, ypos, Window::screenWidth, Window::screenHeight , flags);
		if (window) {
			std::cout << "Window Created!\n";
			SDL_SetWindowMinimumSize(window, MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT);
		}
		renderer = SDL_CreateRenderer(window, -1, 0);
		if (renderer) {
			SDL_SetRenderDrawColor(renderer, 121, 121, 121, 255);
			std::cout << "Renderer Created!\n";
			SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		}

		int imgFlags = IMG_INIT_PNG;

		if (!(IMG_Init(imgFlags) & imgFlags))
		{
			printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
			isRunning = false;
		}
		else {
			isRunning = true;

			game = new Game();
			game->init();
		}
	}
	else {
		isRunning = false;
		fprintf(stderr, "Something failed in SDL setup\n");
	}
}

void Window::render() {
	SDL_SetRenderDrawColor(renderer, BACKGROUND_RED, BACKGROUND_GREEN, BACKGROUND_BLUE, 255);
	SDL_RenderClear(renderer);

	game->render();

	SDL_RenderPresent(Window::renderer);
}

void Window::update(float deltaTime) {
	game->update();
}

bool Window::running() {
	return isRunning;
}

//Set the initial height and width to be a square that is 80% of the smallest dimension.
void Window::calculateInitialWindowDimensions() {
	SDL_DisplayMode DM;
	SDL_GetCurrentDisplayMode(0, &DM);
	auto Width = DM.w;
	auto Height = DM.h;

	//If height is smaller.
	int squareWidth;

	if (Width > Height) {
		squareWidth = 0.8 * Height;
	}
	else {
		squareWidth = 0.8 * Width;
	}
	Window::screenHeight = Window::screenWidth = squareWidth;
}

void Window::handleEvents() {
	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type) {
	case SDL_QUIT:
		isRunning = false;
		break;
	case SDL_KEYDOWN:
		handleKeyDown(event.key);
		break;
	case SDL_KEYUP:
		handleKeyUp(event.key);
		break;
	case SDL_WINDOWEVENT:
		if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
			resizeWindow(event.window.data1, event.window.data2);
		}
		break;
	case SDL_MOUSEBUTTONDOWN:
		game->handleMouseButtonDown(event.button);
		break;
    default:
        break;
	}
}

void Window::handleKeyDown(SDL_KeyboardEvent& key) {
	switch (key.keysym.scancode) {
	case SDL_SCANCODE_X:
		game->reset();
		break;
	case SDL_SCANCODE_R:
		game->makeRandomMove();
		break;
	case SDL_SCANCODE_Z:
		game->unmakeMove();
		break;
	default:
		break;
	}
}

void Window::handleKeyUp(SDL_KeyboardEvent& key) {
	switch (key.keysym.scancode) {
	default:
		break;
	}
}

void Window::resizeWindow(int newWidth, int newHeight) {
	Window::screenHeight = newHeight;
	Window::screenWidth = newWidth;
	game->resize();
}

void Window::clean() {
	delete(game);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	std::cout << "Cleaned succesfully!\n";
}

