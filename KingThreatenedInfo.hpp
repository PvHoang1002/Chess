#pragma once
#include <stdint.h>

#include "Box.hpp"

struct KingThreatenedInfo {
	uint8_t threatenedInfo;
	uint8_t attackedInfo;

	Box straightUpBox;
	Box straightDownBox;
	Box straightLeftBox;
    Box straightRightBox;
	Box upLeftBox;
    Box upRightBox;
	Box downLeftBox;
	Box downRightBox;

	int amountAttacked;

	Box attackedfromBox;

	bool attackedByKnight;

	static const uint8_t straightUpThreatened;
    static const uint8_t straightDownThreatened;
	static const uint8_t straightLeftThreatened;
	static const uint8_t straightRightThreatened;
	static const uint8_t upLeftThreatened;
	static const uint8_t upRightThreatened;
	static const uint8_t downLeftThreatened;
	static const uint8_t downRightThreatened;
};
