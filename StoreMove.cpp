#include "StoreMove.hpp"

StoreMove::StoreMove()
{

}

void StoreMove::init(StoreMove* nextStore, Move newMove, bool updatedKing, bool updatedQueen,
                     Box previousPassant, bool took, uint8_t piece)
{
    next = nextStore;
    move =
    {
        {
            newMove.startBox.x,
            newMove.startBox.y
        },
        {
            newMove.endBox.x,
            newMove.startBox.y
        },
        newMove.kingSideCastle,
        newMove.queenSideCastle,
        newMove.enPassant,
        newMove.isPromotion,
        newMove.promotionType
    };

    previousWhiteKingSide = previousBlackKingSide = previousWhiteQueenSide = previousBlackQueenSide = false;
    previousEnPassant = {previousPassant.x, previousPassant.y};
    tookAPiece = took;
    pieceTaken = piece;
}

StoreMove* StoreMove::getNext()
{
    return next;
}

void StoreMove::setNext(StoreMove* newNext)
{
    next = newNext;
}

Move StoreMove::getMove()
{
    return move;
}

void StoreMove::setMove(Move newMove)
{
    move =
    {
        {
            newMove.startBox.x,
            newMove.startBox.y
        },
        {
            newMove.endBox.x,
            newMove.endBox.y
        },
        newMove.kingSideCastle,
        newMove.queenSideCastle,
        newMove.enPassant,
        newMove.isPromotion,
        newMove.promotionType
    };
}

bool StoreMove::tookPiece()
{
    return tookAPiece;
}

void StoreMove::setTookPiece(bool newSet)
{
    tookAPiece = newSet;
}

uint8_t StoreMove::getTakenPiece()
{
    return pieceTaken;
}

void StoreMove::setPieceTaken(uint8_t newPiece)
{
    pieceTaken = newPiece;
}

bool StoreMove::getPreviousWhiteKingSide()
{
    return previousWhiteKingSide;
}

bool StoreMove::getPreviousBlackKingSide()
{
    return previousBlackKingSide;
}

bool StoreMove::getPreviousWhiteQueenSide()
{
    return previousWhiteQueenSide;
}

bool StoreMove::getPreviousBlackQueenSide()
{
    return previousBlackQueenSide;
}

void StoreMove::setPreviousCastles(bool whiteKing, bool whiteQueen, bool blackKing, bool blackQueen)
{
    previousWhiteKingSide = whiteKing;
    previousWhiteQueenSide = whiteQueen;
    previousBlackKingSide = blackKing;
    previousBlackQueenSide = blackQueen;
}

Box StoreMove::getPreviousEnPassant()
{
    return previousEnPassant;
}

void StoreMove::setPreviousEnPassant(Box newBox)
{
    previousEnPassant = {newBox.x, newBox.y};
}

Box StoreMove::getPreviousWhiteStraightUpBox()
{
    return previousWhiteStraightUpBox;
}

Box StoreMove::getPreviousWhiteStraightDownBox()
{
    return previousWhiteStraightDownBox;
}

Box StoreMove::getPreviousWhiteStraightLeftBox()
{
    return previousWhiteStraightLeftBox;
}

Box StoreMove::getPreviousWhiteStraightRightBox()
{
    return previousWhiteStraightRightBox;
}

Box StoreMove::getPreviousWhiteUpLeftBox()
{
    return previousWhiteUpLeftBox;
}

Box StoreMove::getPreviousWhiteUpRightBox()
{
    return previousWhiteUpRightBox;
}

Box StoreMove::getPreviousWhiteDownLeftBox()
{
    return previousWhiteDownLeftBox;
}

Box StoreMove::getPreviousWhiteDownRightBox()
{
    return previousWhiteDownRightBox;
}

Box StoreMove::getPreviousBlackStraightUpBox()
{
    return previousBlackStraightUpBox;
}

Box StoreMove::getPreviousBlackStraightDownBox()
{
    return previousBlackStraightDownBox;
}

Box StoreMove::getPreviousBlackStraightLeftBox()
{
    return previousBlackStraightLeftBox;
}

Box StoreMove::getPreviousBlackStraightRightBox()
{
    return previousBlackStraightRightBox;
}

Box StoreMove::getPreviousBlackUpLeftBox()
{
    return previousBlackUpLeftBox;
}

Box StoreMove::getPreviousBlackUpRightBox()
{
    return previousBlackUpRightBox;
}

Box StoreMove::getPreviousBlackDownLeftBox()
{
    return previousBlackDownLeftBox;
}

Box StoreMove::getPreviousBlackDownRightBox()
{
    return previousBlackDownRightBox;
}

void StoreMove::setThreatBoxes(Box whiteStraightLeftBox, Box whiteUpLeftBox, Box whiteStraightUpBox,
                               Box whiteUpRightBox, Box whiteStraightRightBox, Box whiteDownRightBox,
                               Box whiteStraightDownBox, Box whiteDownLeftBox,
                               Box blackStraightLeftBox, Box blackUpLeftBox, Box blackStraightUpBox,
                               Box blackUpRightBox, Box blackStraightRightBox, Box blackDownRightBox,
                               Box blackStraightDownBox, Box blackDownLeftBox)
{
    previousWhiteStraightUpBox = {whiteStraightUpBox.x, whiteStraightUpBox.y};
    previousWhiteStraightDownBox = {whiteStraightDownBox.x, whiteStraightDownBox.y};
    previousWhiteStraightLeftBox = {whiteStraightLeftBox.x, whiteStraightLeftBox.y};
    previousWhiteStraightRightBox = {whiteStraightRightBox.x, whiteStraightRightBox.y};
    previousWhiteUpLeftBox = {whiteUpLeftBox.x, whiteUpLeftBox.y};
    previousWhiteUpRightBox = {whiteUpRightBox.x, whiteUpRightBox.y};
    previousWhiteDownLeftBox = {whiteDownLeftBox.x, whiteDownLeftBox.y};
    previousWhiteDownRightBox = {whiteDownRightBox.x, whiteDownRightBox.y};

    previousBlackStraightUpBox = {blackStraightUpBox.x, blackStraightUpBox.y};
    previousBlackStraightDownBox = {blackStraightDownBox.x, blackStraightDownBox.y};
    previousBlackStraightLeftBox = {blackStraightLeftBox.x, blackStraightLeftBox.y};
    previousBlackStraightRightBox = {blackStraightRightBox.x, blackStraightRightBox.y};
    previousBlackUpLeftBox = {blackUpLeftBox.x, blackUpLeftBox.y};
    previousBlackUpRightBox = {blackUpRightBox.x, blackUpRightBox.y};
    previousBlackDownLeftBox = {blackDownLeftBox.x, blackDownLeftBox.y};
    previousBlackDownRightBox = {blackDownRightBox.x, blackDownRightBox.y};
}

uint8_t StoreMove::getPreviousWhiteThreatenedInfo()
{
    return previousWhiteThreatenedInfo;
}

uint8_t StoreMove::getPreviousBlackThreatenedInfo()
{
    return previousBlackThreatenedInfo;
}

uint8_t StoreMove::getPreviousWhiteAttackedInfo()
{
    return previousWhiteAttackedInfo;
}

uint8_t StoreMove::getPreviousBlackAttackedInfo()
{
    return previousBlackAttackedInfo;
}

void StoreMove::setThreatInfo(uint8_t whiteThreatened, uint8_t blackThreatened,
                              uint8_t whiteAttacked, uint8_t blackAttacked,
                              int amountAttacked, bool attackedByKnight, Box attackedFromBox)
{
    previousWhiteThreatenedInfo = whiteThreatened;
    previousBlackThreatenedInfo = blackThreatened;
    previousWhiteAttackedInfo = whiteAttacked;
    previousBlackAttackedInfo = blackAttacked;
    previousAmountAttacked = amountAttacked;
    previousAttackedByKnight = attackedByKnight;
    previousAttackedfromBox = {attackedFromBox.x, attackedFromBox.y};
}

int StoreMove::getPreviousAmountAttacked()
{
    return previousAmountAttacked;
}

bool StoreMove::getPreviousAttackedByKnight()
{
    return previousAttackedByKnight;
}

Box StoreMove::getPreviousAttackedFromBox()
{
    return previousAttackedfromBox;
}


