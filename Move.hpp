#pragma once
#include "Box.hpp"

struct Move {
	Box startBox;
	Box endBox;

	bool kingSideCastle;
	bool queenSideCastle;

	bool enPassant;

	bool isPromotion;
	char promotionType;
};
