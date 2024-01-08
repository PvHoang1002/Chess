#include <assert.h>
#include <cstddef> //for using NULL

#include "MoveStack.hpp"

void MoveStack::init() {
	top = NULL;
	amountOfMovesInStack = 0;
}

void MoveStack::push(StoreMove* newMove) {
	newMove->setNext(top);
	top = newMove;
	amountOfMovesInStack++;
}

StoreMove* MoveStack::pop() {
    //Check if amount of moves in stack == 0
	assert(amountOfMovesInStack != 0);
	StoreMove* temp = top;
	top = top->getNext();
	--amountOfMovesInStack;
	return temp;
}

int MoveStack::getSize() {
	return amountOfMovesInStack;
}

StoreMove* MoveStack::getTop() {
	return top;
}

void MoveStack::clear() {
	while (top != NULL) {
		StoreMove* temp = top->getNext();
		delete top;
		top = temp;
	}
	amountOfMovesInStack = 0;
}




