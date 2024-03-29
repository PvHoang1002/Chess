#include "Piece.hpp"
#include "Window.hpp"

SDL_Texture* Piece::blackKingTexture;
SDL_Texture* Piece::whiteKingTexture;
SDL_Texture* Piece::blackKnightTexture;
SDL_Texture* Piece::whiteKnightTexture;
SDL_Texture* Piece::blackRookTexture;
SDL_Texture* Piece::whiteRookTexture;
SDL_Texture* Piece::blackBishopTexture;
SDL_Texture* Piece::whiteBishopTexture;
SDL_Texture* Piece::whiteQueenTexture;
SDL_Texture* Piece::blackQueenTexture;
SDL_Texture* Piece::whitePawnTexture;
SDL_Texture* Piece::blackPawnTexture;

uint8_t Piece::white;
uint8_t Piece::black;
uint8_t Piece::queen;
uint8_t Piece::king;
uint8_t Piece::bishop;
uint8_t Piece::knight;
uint8_t Piece::rook;
uint8_t Piece::pawn;

void Piece::init() {
	loadImages();
	loadTypes();
}

void Piece::loadImages() {
	SDL_Surface* surface;

	surface = IMG_Load("images/blackKing.png");
	blackKingTexture = SDL_CreateTextureFromSurface(Window::renderer, surface);
	SDL_FreeSurface(surface);

	surface = IMG_Load("images/whiteKing.png");
	whiteKingTexture = SDL_CreateTextureFromSurface(Window::renderer, surface);
	SDL_FreeSurface(surface);

	surface = IMG_Load("images/blackQueen.png");
	blackQueenTexture = SDL_CreateTextureFromSurface(Window::renderer, surface);
	SDL_FreeSurface(surface);

	surface = IMG_Load("images/whiteQueen.png");
	whiteQueenTexture = SDL_CreateTextureFromSurface(Window::renderer, surface);
	SDL_FreeSurface(surface);

	surface = IMG_Load("images/blackBishop.png");
	blackBishopTexture = SDL_CreateTextureFromSurface(Window::renderer, surface);
	SDL_FreeSurface(surface);

	surface = IMG_Load("images/whiteBishop.png");
	whiteBishopTexture = SDL_CreateTextureFromSurface(Window::renderer, surface);
	SDL_FreeSurface(surface);

	surface = IMG_Load("images/blackKnight.png");
	blackKnightTexture = SDL_CreateTextureFromSurface(Window::renderer, surface);
	SDL_FreeSurface(surface);

	surface = IMG_Load("images/whiteKnight.png");
	whiteKnightTexture = SDL_CreateTextureFromSurface(Window::renderer, surface);
	SDL_FreeSurface(surface);

	surface = IMG_Load("images/blackRook.png");
	blackRookTexture = SDL_CreateTextureFromSurface(Window::renderer, surface);
	SDL_FreeSurface(surface);

	surface = IMG_Load("images/whiteRook.png");
	whiteRookTexture = SDL_CreateTextureFromSurface(Window::renderer, surface);
	SDL_FreeSurface(surface);

	surface = IMG_Load("images/blackPawn.png");
	blackPawnTexture = SDL_CreateTextureFromSurface(Window::renderer, surface);
	SDL_FreeSurface(surface);

	surface = IMG_Load("images/whitePawn.png");
	whitePawnTexture = SDL_CreateTextureFromSurface(Window::renderer, surface);
	SDL_FreeSurface(surface);
}

void Piece::destroyImages() {
	SDL_DestroyTexture(whitePawnTexture);
	SDL_DestroyTexture(blackPawnTexture);

	SDL_DestroyTexture(whiteRookTexture);
	SDL_DestroyTexture(blackRookTexture);

	SDL_DestroyTexture(whiteKnightTexture);
	SDL_DestroyTexture(blackKnightTexture);

	SDL_DestroyTexture(whiteBishopTexture);
	SDL_DestroyTexture(blackBishopTexture);

	SDL_DestroyTexture(whiteKingTexture);
	SDL_DestroyTexture(blackKingTexture);

	SDL_DestroyTexture(whiteQueenTexture);
	SDL_DestroyTexture(blackQueenTexture);
}

void Piece::loadTypes() {
	//First 6 bits for type.
	pawn = 1; //pawn = 00000001
	rook = 2; //rook = 00000010
	knight = 4; //knight = 00000100
	bishop = 8; //bishop = 00001000
	queen = 16; //queen = 00010000
	king = 32; // king = 00100000

	//Last 2 bits for color.
	white = 64; // white = 01000000
	black = 128; // black = 10000000
}

