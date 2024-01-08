#include "Game.hpp"
#include "Window.hpp"

int Game::boardTopLeftX;
int Game::boardTopLeftY;

Game::~Game()
{
    delete(board);
}

void Game::init()
{
    boardTopLeftX = boardTopLeftY = 0;
    board = new Board();
    board->init();
}

void Game::render()
{
    board->render(board->getBoardState());
}

void Game::update()
{

}

void Game::resize()
{
    board->resize();

    boardTopLeftX = (Window::screenWidth - board->getWidth()) / 2;
    boardTopLeftY = (Window::screenHeight - board->getHeight()) / 2;
}

void Game::handleMouseButtonDown(SDL_MouseButtonEvent& event)
{
    board->handleMouseButtonDown(event, board->getBoardState());
}

void Game::reset()
{
    board->reset();
}

void Game::unmakeMove()
{
    board->unMakeMove(board->getBoardState());
    board->nextTurn(board->getBoardState());
}

void Game::makeRandomMove()
{
    board->makeRandomMove(board->getBoardState());
}


