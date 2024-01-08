#pragma once
#include <vector>
#include <stdint.h>

#include "Box.hpp"

class PieceLocations
{
public:
	Box& getKingLocation();
    void setKingLocation(Box newLocation);

	std::vector<Box>& getPawnLocations();
	std::vector<Box>& getKnightLocations();
	std::vector<Box>& getRookLocations();
	std::vector<Box>& getBishopLocations();
	std::vector<Box>& getQueenLocations();

	void clear();

    void addPiece(uint8_t piece, Box newBox);

	void removePiece(uint8_t piece, Box box);
	void removePawn(Box removeBox);
	void removeKnight(Box removeBox);
	void removeRook(Box removeBox);
	void removeBishop(Box removeBox);
	void removeQueen(Box removeBox);

	void updatePiece(uint8_t piece, Box oldBox, Box newBox);
	void updatePawn(Box oldBox, Box newBox);
	void updateRook(Box oldBox, Box newBox);
	void updateKnight(Box oldBox, Box newBox);
	void updateBishop(Box oldBox, Box newBox);
	void updateQueen(Box oldBox, Box newBox);

private:
	Box kingLocation;
	std::vector<Box> pawnLocations;
	std::vector<Box> knightLocations;
	std::vector<Box> rookLocations;
	std::vector<Box> bishopLocations;
	std::vector<Box> queenLocations;
};


