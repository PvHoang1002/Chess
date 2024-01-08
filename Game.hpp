#pragma once
#include <SDL.h>

#include "Board.hpp"

class Game
{
private:
	Board* board;

public:
	static int boardTopLeftX;
	static int boardTopLeftY;

    ~Game();

	void init();
	void render();

	void update();

	void resize();

	void handleMouseButtonDown(SDL_MouseButtonEvent& event);

	void reset();
	void unmakeMove();
	void makeRandomMove();
};


