#pragma once
#include "StoreMove.hpp"

class MoveStack
{
private:
	int amountOfMovesInStack;
	StoreMove* top;

public:
	void init();
	StoreMove* pop();
	void push(StoreMove* newMove);
	int getSize();
	StoreMove* getTop();
	void clear();
};


