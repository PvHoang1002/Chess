#pragma once
#include <stdint.h>

#include "Move.hpp"

class StoreMove
{
public:
    StoreMove();

    void init(StoreMove* newStore, Move newMove, bool updateKing, bool updateQueen,
              Box previousPassant, bool took, uint8_t piece);

    StoreMove* getNext();
    void setNext(StoreMove* newNext);

    Move getMove();
    void setMove(Move newMove);

    bool tookPiece();
    void setTookPiece(bool newSet);

    uint8_t getTakenPiece();
    void setPieceTaken(uint8_t newPiece);

    bool getPreviousWhiteKingSide();
    bool getPreviousWhiteQueenSide();
    bool getPreviousBlackKingSide();
    bool getPreviousBlackQueenSide();
    void setPreviousCastles(bool whiteKing, bool whiteQueen, bool blackKing, bool blackQueen);

    Box getPreviousEnPassant();
    void setPreviousEnPassant(Box newBox);

    Box getPreviousWhiteStraightLeftBox();
    Box getPreviousWhiteStraightRightBox();
    Box getPreviousWhiteStraightUpBox();
    Box getPreviousWhiteStraightDownBox();

    Box getPreviousWhiteUpLeftBox();
    Box getPreviousWhiteUpRightBox();
    Box getPreviousWhiteDownRightBox();
    Box getPreviousWhiteDownLeftBox();

    Box getPreviousBlackStraightLeftBox();
    Box getPreviousBlackStraightRightBox();
    Box getPreviousBlackStraightUpBox();
    Box getPreviousBlackStraightDownBox();

    Box getPreviousBlackUpLeftBox();
    Box getPreviousBlackUpRightBox();
    Box getPreviousBlackDownRightBox();
    Box getPreviousBlackDownLeftBox();
    void setThreatBoxes(Box whiteStraightLeftBox, Box whiteUpLeftBox, Box whiteStraightUpBox,
                        Box whiteUpRightBox, Box whiteStraightRightBox, Box whiteDownRightBox,
                        Box whiteStraightDownBox, Box whiteDownLeftBox,
                        Box blackStraightLeftBox, Box blackUpLeftBox, Box blackStraightUpBox,
                        Box blackUpRightBox, Box blackStraightRightBox, Box blackDownRightBox,
                        Box blackStraightDownBox, Box blackDownLeftBox);

    uint8_t getPreviousWhiteThreatenedInfo();
    uint8_t getPreviousBlackThreatenedInfo();
    uint8_t getPreviousWhiteAttackedInfo();
    uint8_t getPreviousBlackAttackedInfo();
    void setThreatInfo(uint8_t whiteThreatened, uint8_t blackThreatened,
                       uint8_t whiteAttacked, uint8_t blackAttacked,
                       int amountAttacked, bool attackedByKnight, Box attackedFromBox);

    int getPreviousAmountAttacked();

    bool getPreviousAttackedByKnight();

    Box getPreviousAttackedFromBox();

private:
    StoreMove* next;
    Move move;
    bool previousWhiteKingSide;
    bool previousBlackKingSide;
    bool previousWhiteQueenSide;
    bool previousBlackQueenSide;
    Box previousEnPassant;
    bool tookAPiece;
    uint8_t pieceTaken;
    bool previousAttackedByKnight;
    uint8_t previousWhiteThreatenedInfo;
    uint8_t previousBlackThreatenedInfo;
    uint8_t previousWhiteAttackedInfo;
    uint8_t previousBlackAttackedInfo;
    Box previousAttackedfromBox;
    int previousAmountAttacked;

    Box previousWhiteStraightLeftBox, previousWhiteStraightRightBox,
        previousWhiteStraightUpBox, previousWhiteStraightDownBox,
        previousWhiteUpRightBox, previousWhiteUpLeftBox,
        previousWhiteDownRightBox, previousWhiteDownLeftBox;

    Box previousBlackStraightLeftBox, previousBlackStraightRightBox,
        previousBlackStraightUpBox, previousBlackStraightDownBox,
        previousBlackUpRightBox, previousBlackUpLeftBox,
        previousBlackDownRightBox, previousBlackDownLeftBox;
};


