#pragma once
#include <stdint.h>

class BoardState
{
public:
	~BoardState();

	void setBoard(uint8_t** newBoard);
	uint8_t** getBoard();

	char getCurrentTurn();
	void setCurrentTurn(char newTurn);

	bool getWhiteCanKingsideCastle();
	void setWhiteCanKingsideCastle(bool newSet);

	bool getWhiteCanQueensideCastle();
	void setWhiteCanQueensideCastle(bool newSet);

	bool getBlackCanKingsideCastle();
	void setBlackCanKingsideCastle(bool newSet);

	bool getBlackCanQueensideCastle();
	void setBlackCanQueensideCastle(bool newSet);

	int getEnPassantX();
	void setEnPassantX(int newSet);

	int getEnPassantY();
	void setEnPassantY(int newSet);

	int getHalfMoveClock();
	void setHalfMoveClock(int newSet);

	int getFullMoveClock();
	void setFullMoveCLock(int newSet);

	static BoardState* copyBoardState(BoardState* currentBoardState);

    int getDepth();

private:
	uint8_t** board;

	char currentTurn;

	bool whiteCanKingsideCastle;
	bool whiteCanQueensideCastle;

	bool blackCanKingsideCastle;
	bool blackCanQueensideCastle;

	int enPassantX;
	int enPassantY;

	int halfMoveClock;
	int fullMoveClock;

	int depth;	//0 for main state, add 1 for every move after it.
};


