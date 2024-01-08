#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "Board.hpp"
#include "Game.hpp"
#include "Window.hpp"
#include "KingThreatenedInfo.hpp"
#include "BoardState.hpp"
#include "Piece.hpp"

#define HIGHLIGHT_COLOR {0, 255, 0, 50}
#define ATTACK_COLOR {255, 0, 0, 100}
#define DANGER_COLOR {255, 128, 0, 100}
#define WIN_COLOR {255, 215, 0, 200}
#define LAST_MOVE_COLOR {0, 0, 255, 100}
#define AMOUNT_OF_BOX 0.8
#define STARTING_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

int Board::boxXWidth;
int Board::boxYHeight;
int Board::boardXBoxes;
int Board::boardYBoxes;

const uint8_t KingThreatenedInfo::straightLeftThreatened = 0b00000001;
const uint8_t KingThreatenedInfo::upLeftThreatened = 0b00000010;
const uint8_t KingThreatenedInfo::straightUpThreatened = 0b00000100;
const uint8_t KingThreatenedInfo::upRightThreatened = 0b00001000;
const uint8_t KingThreatenedInfo::straightRightThreatened = 0b00010000;
const uint8_t KingThreatenedInfo::downRightThreatened = 0b00100000;
const uint8_t KingThreatenedInfo::straightDownThreatened = 0b01000000;
const uint8_t KingThreatenedInfo::downLeftThreatened = 0b10000000;

//INITIALIZE.
//Initialize everything.
void Board::init()
{
    Piece::init();
    boardXBoxes = 8;
    boardYBoxes = 8;
    boxXWidth = Window::screenWidth / boardXBoxes;
    boxYHeight = Window::screenHeight / boardYBoxes;

    boardColor1 = {234, 233, 210, 255};

    boardColor2 = {75, 115, 153, 255};

    boardState = new BoardState();

    uint8_t** board = new uint8_t* [boardXBoxes];
    for (int i = 0; i < boardXBoxes; ++i)
    {
        board[i] = new uint8_t[boardXBoxes];
    }

    for (int x = 0; x < boardXBoxes; ++x)
    {
        for (int y = 0; y < boardYBoxes; ++y)
        {

            board[x][y] = 0;
        }
    }

    boardState->setBoard(board);

    draggingPiece = false;
    draggingPieceBox.x = -1;
    draggingPieceBox.y = -1;

    loadBoardFromFen(STARTING_FEN, boardState);
    initializePieceLocations(boardState);
    initializeKingsThreatened(boardState);

    legalMoves = calculateLegalMoves(boardState);

    highlightKingBox.x = -1;
    highlightKingBox.y = -1;
    winnerKing.x = winnerKing.y = -1;

    gameOver = false;

    moveStack.init();
}

//Initialize piece locations.
void Board::initializePieceLocations(BoardState* currentBoardState)
{
    blackLocations.clear();
    whiteLocations.clear();

    for (int x = 0; x < boardXBoxes; x++)
    {
        for (int y = 0; y < boardYBoxes; y++)
        {
            if (currentBoardState->getBoard()[x][y] != 0)
            {
                uint8_t currentPiece = currentBoardState->getBoard()[x][y];

                if (currentPiece == (Piece::black | Piece::king))
                {
                    blackLocations.setKingLocation({x, y});
                }
                else if (currentPiece == (Piece::black | Piece::queen))
                {
                    blackLocations.getQueenLocations().push_back({x, y});
                }
                else if (currentPiece == (Piece::black | Piece::bishop))
                {
                    blackLocations.getBishopLocations().push_back({x, y});
                }
                else if (currentPiece == (Piece::black | Piece::knight))
                {
                    blackLocations.getKnightLocations().push_back({x, y});
                }
                else if (currentPiece == (Piece::black | Piece::pawn))
                {
                    blackLocations.getPawnLocations().push_back({x, y});
                }
                else if (currentPiece == (Piece::black | Piece::rook))
                {
                    blackLocations.getRookLocations().push_back({x, y});
                }
                else if (currentPiece == (Piece::white | Piece::king))
                {
                    whiteLocations.setKingLocation({x, y});
                }
                else if (currentPiece == (Piece::white | Piece::queen))
                {
                    whiteLocations.getQueenLocations().push_back({x, y});
                }
                else if (currentPiece == (Piece::white | Piece::bishop))
                {
                    whiteLocations.getBishopLocations().push_back({x, y});
                }
                else if (currentPiece == (Piece::white | Piece::knight))
                {
                    whiteLocations.getKnightLocations().push_back({x, y});
                }
                else if (currentPiece == (Piece::white | Piece::pawn))
                {
                    whiteLocations.getPawnLocations().push_back({x, y});
                }
                else if (currentPiece == (Piece::white | Piece::rook))
                {
                    whiteLocations.getRookLocations().push_back({x, y});
                }
            }
        }
    }
}

//Initialize king threatened.
void Board::initializeKingsThreatened(BoardState* currentBoardState)
{
    std::cout << "Threatened at beginning of initialize: "
              << int(blackThreatened.threatenedInfo) << std::endl;

    whiteThreatened.threatenedInfo = 0;
    blackThreatened.threatenedInfo = 0;

    updateStraightUpThreats('w', currentBoardState);
    updateStraightDownThreats('w', currentBoardState);
    updateStraightLeftThreats('w', currentBoardState);
    updateStraightRightThreats('w', currentBoardState);
    updateUpLeftThreats('w', currentBoardState);
    updateUpRightThreats('w', currentBoardState);
    updateDownLeftThreats('w', currentBoardState);
    updateDownRightThreats('w', currentBoardState);

    updateStraightUpThreats('b', currentBoardState);
    updateStraightDownThreats('b', currentBoardState);
    updateStraightLeftThreats('b', currentBoardState);
    updateStraightRightThreats('b', currentBoardState);
    updateUpLeftThreats('b', currentBoardState);
    updateUpRightThreats('b', currentBoardState);
    updateDownLeftThreats('b', currentBoardState);
    updateDownRightThreats('b', currentBoardState);

    std::cout << "Threatened at end of initialize: "
              << int(blackThreatened.threatenedInfo) << std::endl;
}
//-------------------------------------------------------------//

//DELETE.
//Delete board.
Board::~Board()
{
    Piece::destroyImages();
    delete(boardState);
    moveStack.clear();
}

//Clear moves.
void Board::clearMoves()
{
    pseudoLegalMoves.clear();
    legalMoves.clear();
}
//-------------------------------------------------------------//

//GETTER.
int Board::getWidth()
{
    return boardXBoxes * boxXWidth;
}

int Board::getHeight()
{
    return boardYBoxes * boxYHeight;
}

BoardState* Board::getBoardState()
{
    return boardState;
}

SDL_Texture* Board::getTextureAtLocation(int x, int y, BoardState* currentBoardState)
{
    uint8_t currentPiece = currentBoardState->getBoard()[x][y];

    if (currentPiece == (Piece::black | Piece::king))
    {
        return Piece::blackKingTexture;
    }
    else if (currentPiece == (Piece::black | Piece::queen))
    {
        return Piece::blackQueenTexture;
    }
    else if (currentPiece == (Piece::black | Piece::bishop))
    {
        return Piece::blackBishopTexture;
    }
    else if (currentPiece == (Piece::black | Piece::knight))
    {
        return Piece::blackKnightTexture;
    }
    else if (currentPiece == (Piece::black | Piece::pawn))
    {
        return Piece::blackPawnTexture;
    }
    else if (currentPiece == (Piece::black | Piece::rook))
    {
        return Piece::blackRookTexture;
    }
    else if (currentPiece == (Piece::white | Piece::king))
    {
        return Piece::whiteKingTexture;
    }
    else if (currentPiece == (Piece::white | Piece::queen))
    {
        return Piece::whiteQueenTexture;
    }
    else if (currentPiece == (Piece::white | Piece::bishop))
    {
        return Piece::whiteBishopTexture;
    }
    else if (currentPiece == (Piece::white | Piece::knight))
    {
        return Piece::whiteKnightTexture;
    }
    else if (currentPiece == (Piece::white | Piece::pawn))
    {
        return Piece::whitePawnTexture;
    }
    else if (currentPiece == (Piece::white | Piece::rook))
    {
        return Piece::whiteRookTexture;
    }
}
//-------------------------------------------------------------//

//FUNCTIONS IN GAME.
//Reset game (Press X)
void Board::reset()
{
    for (int x = 0; x < boardXBoxes; ++x)
    {
        for (int y = 0; y < boardYBoxes; ++y)
        {
            boardState->getBoard()[x][y] = 0;
        }
    }

    draggingPiece = false;
    draggingPieceBox.x = -1;
    draggingPieceBox.y = -1;

    loadBoardFromFen(STARTING_FEN, boardState);
    initializePieceLocations(boardState);
    updateAllThreats('w', boardState);
    updateAllThreats('b', boardState);

    moveStack.clear();

    legalMoves = calculateLegalMoves(boardState);

    highlightKingBox.x = -1;
    highlightKingBox.y = -1;
    winnerKing.x = winnerKing.y = -1;

    gameOver = false;
}

//Redo move (Press Z)
void Board::unMakeMove(BoardState* currentBoardState)
{
    if (moveStack.getSize() == 0)
    {
        std::cout << "There are no moves to unmake" << std::endl;
        return;
    }
    switchTurns(currentBoardState);

    uint8_t** board = currentBoardState->getBoard();
    StoreMove* previousMove = moveStack.pop();
    Move move = previousMove->getMove();
    char currentTurn = currentBoardState->getCurrentTurn();

    //If it's a promotion we move the piece back and transform it into a pawn.
    if (move.isPromotion)
    {
        if (currentTurn == 'w')
        {
            board[move.startBox.x][move.startBox.y] = Piece::white | Piece::pawn;
            whiteLocations.removePiece(board[move.endBox.x][move.endBox.y], move.endBox);
            whiteLocations.getPawnLocations().push_back(move.startBox);
        }
        else
        {
            board[move.startBox.x][move.startBox.y] = Piece::black | Piece::pawn;
            blackLocations.removePiece(board[move.endBox.x][move.endBox.y], move.endBox);
            blackLocations.getPawnLocations().push_back(move.startBox);
        }
    }

    //Otherwise we can just put the piece back normally.
    else
    {
        board[move.startBox.x][move.startBox.y] = board[move.endBox.x][move.endBox.y];
        if (currentTurn == 'w')
        {
            whiteLocations.updatePiece(board[move.startBox.x][move.startBox.y],
                                       move.endBox, move.startBox);
        }
        else
        {
            blackLocations.updatePiece(board[move.startBox.x][move.startBox.y],
                                       move.endBox, move.startBox);
        }
    }

    //If a piece was taken we put it back.
    if (previousMove->tookPiece())
    {
        board[move.endBox.x][move.endBox.y] = previousMove->getTakenPiece();
        if (currentTurn == 'w')
        {
            blackLocations.addPiece(previousMove->getTakenPiece(), move.endBox);
        }
        else
        {
            whiteLocations.addPiece(previousMove->getTakenPiece(), move.endBox);
        }
    }
    else
    {
        board[move.endBox.x][move.endBox.y] = 0;
    }

    currentBoardState->setBlackCanKingsideCastle(previousMove->getPreviousBlackKingSide());
    currentBoardState->setWhiteCanKingsideCastle(previousMove->getPreviousWhiteKingSide());
    currentBoardState->setWhiteCanQueensideCastle(previousMove->getPreviousWhiteQueenSide());
    currentBoardState->setBlackCanQueensideCastle(previousMove->getPreviousBlackQueenSide());

    Box enPassant = previousMove->getPreviousEnPassant();
    currentBoardState->setEnPassantX(enPassant.x);
    currentBoardState->setEnPassantY(enPassant.y);

    if (currentTurn == 'w')
    {
        whiteThreatened =
        {
            previousMove->getPreviousWhiteThreatenedInfo(),
            previousMove->getPreviousWhiteAttackedInfo(),
            {
                previousMove->getPreviousWhiteStraightLeftBox().x,
                previousMove->getPreviousWhiteStraightLeftBox().y
            },
            {
                previousMove->getPreviousWhiteUpLeftBox().x,
                previousMove->getPreviousWhiteUpLeftBox().y
            },
            {
                previousMove->getPreviousWhiteStraightUpBox().x,
                previousMove->getPreviousWhiteStraightUpBox().y
            },
            {
                previousMove->getPreviousWhiteUpRightBox().x,
                previousMove->getPreviousWhiteUpRightBox().y
            },
            {
                previousMove->getPreviousWhiteStraightRightBox().x,
                previousMove->getPreviousWhiteStraightRightBox().y
            },
            {
                previousMove->getPreviousWhiteDownRightBox().x,
                previousMove->getPreviousWhiteDownRightBox().y
            },
            {
                previousMove->getPreviousWhiteStraightDownBox().x,
                previousMove->getPreviousWhiteStraightDownBox().y
            },
            {
                previousMove->getPreviousWhiteDownLeftBox().x,
                previousMove->getPreviousWhiteDownLeftBox().y
            },
            previousMove->getPreviousAmountAttacked(),
            {
                previousMove->getPreviousAttackedFromBox().x,
                previousMove->getPreviousAttackedFromBox().y
            },
            previousMove->getPreviousAttackedByKnight()

        };

        blackThreatened =
        {
            previousMove->getPreviousBlackThreatenedInfo(),
            previousMove->getPreviousBlackAttackedInfo(),
            {
                previousMove->getPreviousBlackStraightLeftBox().x,
                previousMove->getPreviousBlackStraightLeftBox().y
            },
            {
                previousMove->getPreviousBlackUpLeftBox().x,
                previousMove->getPreviousBlackUpLeftBox().y
            },
            {
                previousMove->getPreviousBlackStraightUpBox().x,
                previousMove->getPreviousBlackStraightUpBox().y
            },
            {
                previousMove->getPreviousBlackUpRightBox().x,
                previousMove->getPreviousBlackUpRightBox().y
            },
            {
                previousMove->getPreviousBlackStraightRightBox().x,
                previousMove->getPreviousBlackStraightRightBox().y
            },
            {
                previousMove->getPreviousBlackDownRightBox().x,
                previousMove->getPreviousBlackDownRightBox().y
            },
            {
                previousMove->getPreviousBlackStraightDownBox().x,
                previousMove->getPreviousBlackStraightDownBox().y
            },
            {
                previousMove->getPreviousBlackDownLeftBox().x,
                previousMove->getPreviousBlackDownLeftBox().y
            },
            0, {-1,-1}, false
        };
    }
    else
    {
        whiteThreatened =
        {
            previousMove->getPreviousWhiteThreatenedInfo(),
            previousMove->getPreviousWhiteAttackedInfo(),
            {
                previousMove->getPreviousWhiteStraightLeftBox().x,
                previousMove->getPreviousWhiteStraightLeftBox().y
            },
            {
                previousMove->getPreviousWhiteUpLeftBox().x,
                previousMove->getPreviousWhiteUpLeftBox().y
            },
            {
                previousMove->getPreviousWhiteStraightUpBox().x,
                previousMove->getPreviousWhiteStraightUpBox().y
            },
            {
                previousMove->getPreviousWhiteUpRightBox().x,
                previousMove->getPreviousWhiteUpRightBox().y
            },
            {
                previousMove->getPreviousWhiteStraightRightBox().x,
                previousMove->getPreviousWhiteStraightRightBox().y
            },
            {
                previousMove->getPreviousWhiteDownRightBox().x,
                previousMove->getPreviousWhiteDownRightBox().y
            },
            {
                previousMove->getPreviousWhiteStraightDownBox().x,
                previousMove->getPreviousWhiteStraightDownBox().y
            },
            {
                previousMove->getPreviousWhiteDownLeftBox().x,
                previousMove->getPreviousWhiteDownLeftBox().y
            },
            0, {-1,-1}, false
        };

        blackThreatened =
        {
            previousMove->getPreviousBlackThreatenedInfo(),
            previousMove->getPreviousBlackAttackedInfo(),
            {
                previousMove->getPreviousBlackStraightLeftBox().x,
                previousMove->getPreviousBlackStraightLeftBox().y
            },
            {
                previousMove->getPreviousBlackUpLeftBox().x,
                previousMove->getPreviousBlackUpLeftBox().y
            },
            {
                previousMove->getPreviousBlackStraightUpBox().x,
                previousMove->getPreviousBlackStraightUpBox().y
            },
            {
                previousMove->getPreviousBlackUpRightBox().x,
                previousMove->getPreviousBlackUpRightBox().y
            },
            {
                previousMove->getPreviousBlackStraightRightBox().x,
                previousMove->getPreviousBlackStraightRightBox().y
            },
            {
                previousMove->getPreviousBlackDownRightBox().x,
                previousMove->getPreviousBlackDownRightBox().y
            },
            {
                previousMove->getPreviousBlackStraightDownBox().x,
                previousMove->getPreviousBlackStraightDownBox().y
            },
            {
                previousMove->getPreviousBlackDownLeftBox().x,
                previousMove->getPreviousBlackDownLeftBox().y
            },
            previousMove->getPreviousAmountAttacked(),
            {
                previousMove->getPreviousAttackedFromBox().x,
                previousMove->getPreviousAttackedFromBox().y
            },
            previousMove->getPreviousAttackedByKnight()
        };
    }
    //Castling and en passant.
    if (move.kingSideCastle)
    {
        board[boardXBoxes - 1][move.startBox.y] = board[move.startBox.x + 1][move.startBox.y];
        board[move.startBox.x + 1][move.startBox.y] = 0;
        if (currentTurn == 'w')
        {
            whiteLocations.updateRook({move.startBox.x + 1, move.startBox.y},
            {boardXBoxes - 1, move.startBox.y});
        }
        else
        {
            blackLocations.updateRook({move.startBox.x + 1, move.startBox.y},
            {boardXBoxes - 1, move.startBox.y});
        }
    }
    else if (move.queenSideCastle)
    {
        board[0][move.startBox.y] = board[move.endBox.x + 1][move.startBox.y];
        board[move.endBox.x + 1][move.startBox.y] = 0;
        if (currentTurn == 'w')
        {
            whiteLocations.updateRook({move.endBox.x + 1, move.startBox.y}, {0, move.startBox.y});
        }
        else
        {
            blackLocations.updateRook({move.endBox.x + 1, move.startBox.y}, {0, move.startBox.y});
        }
    }
    else if (move.enPassant)
    {
        if (currentTurn == 'w')
        {
            board[move.endBox.x][move.endBox.y] = Piece::black | Piece::pawn;
            blackLocations.getPawnLocations().push_back({move.endBox.x, move.startBox.y});
        }
        else
        {
            board[move.endBox.x][move.startBox.y] = Piece::white | Piece::pawn;
            whiteLocations.getPawnLocations().push_back({move.endBox.x, move.startBox.y});
        }
    }

    delete previousMove;
}

//Make random move (Press R)
void Board::makeRandomMove(BoardState* currentBoardState)
{
    legalMoves = calculateLegalMoves(currentBoardState);
    int choice = rand() % legalMoves.size();
    makeMove(legalMoves.at(choice), currentBoardState);
    nextTurn(currentBoardState);
}
//-------------------------------------------------------------//

//RENDER.
//Render everything.
void Board::render(BoardState* currentBoardState)
{
    renderBoard();

    if (draggingPiece)
    {
        renderHighlightMoves();
    }
    else if (moveStack.getSize() != 0)
    {
        renderPreviousMove();
    }
    if (highlightKingBox.x != -1 || winnerKing.x != -1)
    {
        renderKingBox();
    }

    //renderAttackedSquares();

    renderPieces(currentBoardState);

    if (draggingPiece)
    {
        renderDraggedPiece();
    }

    if (waitingForPromotionChoice)
    {
        renderPromotionOptions();
    }
}

//Render board.
void Board::renderBoard()
{
    for (int x = 0; x < boardXBoxes; ++x)
    {
        for (int y = 0; y < boardYBoxes; ++y)
        {
            SDL_Color currentColor = (x + y) % 2 == 0 ? boardColor1 : boardColor2;
            renderBox({x, y}, currentColor);
        }
    }
}

//Render piece.
void Board::renderPieceTexture(SDL_Texture* texture, int x, int y)
{
    int w, h;

    SDL_QueryTexture(texture, NULL, NULL, &w, &h);

    SDL_Rect fromRect, toRect;
    fromRect.w = w;
    fromRect.h = h;
    fromRect.x = fromRect.y = 0;

    toRect.w = boxXWidth * AMOUNT_OF_BOX;
    toRect.h = boxYHeight * AMOUNT_OF_BOX;
    toRect.x = Game::boardTopLeftX + x * boxXWidth + (boxXWidth - toRect.w) / 2;
    toRect.y = Game::boardTopLeftY + y * boxYHeight + (boxYHeight - toRect.h) / 2;

    SDL_RenderCopy(Window::renderer, texture, &fromRect, &toRect);
}

void Board::renderPiece(Box pieceBox, BoardState* currentBoardState)
{
    SDL_Texture* currentTexture = getTextureAtLocation(pieceBox.x, pieceBox.y, currentBoardState);
    renderPieceTexture(currentTexture, pieceBox.x, pieceBox.y);
}

void Board::renderPieces(BoardState* currentBoardState)
{
    if (draggingPiece)
    {
        if (currentBoardState->getBoard()[draggingPieceBox.x][draggingPieceBox.y]
                != (Piece::white | Piece::king))
        {
            renderPiece(whiteLocations.getKingLocation(), currentBoardState);
        }
        if (currentBoardState->getBoard()[draggingPieceBox.x][draggingPieceBox.y]
                != (Piece::black | Piece::king))
        {
            renderPiece(blackLocations.getKingLocation(), currentBoardState);
        }
    }
    else
    {
        renderPiece(whiteLocations.getKingLocation(), currentBoardState);
        renderPiece(blackLocations.getKingLocation(), currentBoardState);
    }

    //White pieces.
    for (unsigned i = 0; i < whiteLocations.getBishopLocations().size(); i++)
    {
        renderPiece(whiteLocations.getBishopLocations().at(i), boardState);
    }
    for (unsigned i = 0; i < whiteLocations.getQueenLocations().size(); i++)
    {
        renderPiece(whiteLocations.getQueenLocations().at(i), boardState);
    }
    for (unsigned i = 0; i < whiteLocations.getPawnLocations().size(); i++)
    {
        renderPiece(whiteLocations.getPawnLocations().at(i), boardState);
    }
    for (unsigned i = 0; i < whiteLocations.getRookLocations().size(); i++)
    {
        renderPiece(whiteLocations.getRookLocations().at(i), boardState);
    }
    for (unsigned i = 0; i < whiteLocations.getKnightLocations().size(); i++)
    {
        renderPiece(whiteLocations.getKnightLocations().at(i), boardState);
    }

    //Black pieces.
    for (unsigned i = 0; i < blackLocations.getBishopLocations().size(); i++)
    {
        renderPiece(blackLocations.getBishopLocations().at(i), boardState);
    }
    for (unsigned i = 0; i < blackLocations.getQueenLocations().size(); i++)
    {
        renderPiece(blackLocations.getQueenLocations().at(i), boardState);
    }
    for (unsigned i = 0; i < blackLocations.getPawnLocations().size(); i++)
    {
        renderPiece(blackLocations.getPawnLocations().at(i), boardState);
    }
    for (unsigned i = 0; i < blackLocations.getRookLocations().size(); i++)
    {
        renderPiece(blackLocations.getRookLocations().at(i), boardState);
    }
    for (unsigned i = 0; i < blackLocations.getKnightLocations().size(); i++)
    {
        renderPiece(blackLocations.getKnightLocations().at(i), boardState);
    }
}

void Board::renderDraggedPiece()
{
    int w, h, mouseX, mouseY;

    SDL_QueryTexture(draggingPieceTexture, NULL, NULL, &w, &h);
    SDL_GetMouseState(&mouseX, &mouseY);

    SDL_Rect fromRect, toRect;
    fromRect.w = w;
    fromRect.h = h;
    fromRect.x = fromRect.y = 0;

    toRect.w = boxXWidth * AMOUNT_OF_BOX;
    toRect.h = boxYHeight * AMOUNT_OF_BOX;
    toRect.x = mouseX - (toRect.w / 2);
    toRect.y = mouseY - (toRect.h / 2);

    SDL_RenderCopy(Window::renderer, draggingPieceTexture, &fromRect, &toRect);
}

//Render box.
void Board::renderAttackedSquares()
{
    for (int x = 0; x < boardXBoxes; x++)
    {
        for (int y = 0; y < boardYBoxes; y++)
        {
            if (squareAttacked({x, y}, boardState))
            {
                renderBox({x, y}, DANGER_COLOR);
            }
        }
    }
}

void Board::renderBox(Box box, SDL_Color color)
{
    SDL_Rect highlightRect;
    highlightRect.w = boxXWidth;
    highlightRect.h = boxYHeight;

    SDL_SetRenderDrawColor(Window::renderer, color.r, color.g, color.b, color.a);

    highlightRect.x = Game::boardTopLeftX + box.x * boxXWidth;
    highlightRect.y = Game::boardTopLeftY + box.y * boxYHeight;
    SDL_RenderFillRect(Window::renderer, &highlightRect);
}

void Board::renderKingBox()
{
    if (highlightKingBox.x != -1)
    {
        renderBox(highlightKingBox, ATTACK_COLOR);
    }
    if (winnerKing.x != -1)
    {
        renderBox(winnerKing, WIN_COLOR);
    }
}

//Render moves.
void Board::renderHighlightMoves()
{
    for (unsigned i = 0; i < highlightBoxes.size(); i++)
    {
        renderBox(highlightBoxes.at(i), HIGHLIGHT_COLOR);
    }
}

void Board::renderPreviousMove()
{
    StoreMove* prevMoveHistory = moveStack.getTop();
    Move prevMove = prevMoveHistory->getMove();

    renderBox(prevMove.startBox, LAST_MOVE_COLOR);
    renderBox(prevMove.endBox, LAST_MOVE_COLOR);
}

//Render promotion options
void Board::renderPromotionOptions()
{
    SDL_Rect renderRect;
    renderRect.w = boxXWidth * 2;
    renderRect.h = boxYHeight * 2;
    renderRect.x = Game::boardTopLeftX;
    renderRect.y = Game::boardTopLeftY + (getHeight() - renderRect.h) / 2;

    int w, h;

    SDL_Rect fromRect;
    fromRect.x = fromRect.y = 0;

    //White turn.
    if (boardState->getCurrentTurn() == 'w')
    {
        SDL_QueryTexture(Piece::whiteQueenTexture, NULL, NULL, &w, &h);
        fromRect.w = w;
        fromRect.h = h;
        SDL_RenderCopy(Window::renderer, Piece::whiteQueenTexture, &fromRect, &renderRect);
        renderRect.x += renderRect.w;

        SDL_QueryTexture(Piece::whiteRookTexture, NULL, NULL, &w, &h);
        fromRect.w = w;
        fromRect.h = h;
        SDL_RenderCopy(Window::renderer, Piece::whiteRookTexture, &fromRect, &renderRect);
        renderRect.x += renderRect.w;

        SDL_QueryTexture(Piece::whiteBishopTexture, NULL, NULL, &w, &h);
        fromRect.w = w;
        fromRect.h = h;
        SDL_RenderCopy(Window::renderer, Piece::whiteBishopTexture, &fromRect, &renderRect);
        renderRect.x += renderRect.w;

        SDL_QueryTexture(Piece::whiteKnightTexture, NULL, NULL, &w, &h);
        fromRect.w = w;
        fromRect.h = h;
        SDL_RenderCopy(Window::renderer, Piece::whiteKnightTexture, &fromRect, &renderRect);
    }
    //Black turn.
    else
    {
        SDL_QueryTexture(Piece::blackQueenTexture, NULL, NULL, &w, &h);
        fromRect.w = w;
        fromRect.h = h;
        SDL_RenderCopy(Window::renderer, Piece::blackQueenTexture, &fromRect, &renderRect);
        renderRect.x += renderRect.w;

        SDL_QueryTexture(Piece::blackRookTexture, NULL, NULL, &w, &h);
        fromRect.w = w;
        fromRect.h = h;
        SDL_RenderCopy(Window::renderer, Piece::blackRookTexture, &fromRect, &renderRect);
        renderRect.x += renderRect.w;

        SDL_QueryTexture(Piece::blackBishopTexture, NULL, NULL, &w, &h);
        fromRect.w = w;
        fromRect.h = h;
        SDL_RenderCopy(Window::renderer, Piece::blackBishopTexture, &fromRect, &renderRect);
        renderRect.x += renderRect.w;

        SDL_QueryTexture(Piece::blackKnightTexture, NULL, NULL, &w, &h);
        fromRect.w = w;
        fromRect.h = h;
        SDL_RenderCopy(Window::renderer, Piece::blackKnightTexture, &fromRect, &renderRect);
    }
}
//-------------------------------------------------------------//

//CALCULATING.
//Calculate pseudo legal moves.
std::vector<Move> Board::calculatePseudoLegalMoves(BoardState* currentBoardState)
{
    std::vector<Move> returnVec;
    std::vector<Box> moves;

    if (currentBoardState->getCurrentTurn() == 'w')
    {
        calculateKingMoves(whiteLocations.getKingLocation(), currentBoardState, returnVec);

        moves = whiteLocations.getBishopLocations();
        for (unsigned i = 0; i < moves.size(); i++)
        {
            calculateBishopMoves(moves.at(i), currentBoardState, returnVec);
        }

        moves = whiteLocations.getRookLocations();
        for (unsigned i = 0; i < moves.size(); i++)
        {
            calculateRookMoves(moves.at(i), currentBoardState, returnVec);
        }

        moves = whiteLocations.getQueenLocations();
        for (unsigned i = 0; i < moves.size(); i++)
        {
            calculateQueenMoves(moves.at(i), currentBoardState, returnVec);
        }

        moves = whiteLocations.getKnightLocations();
        for (unsigned i = 0; i < moves.size(); i++)
        {
            calculateKnightMoves(moves.at(i), currentBoardState, returnVec);
        }

        moves = whiteLocations.getPawnLocations();
        for (unsigned i = 0; i < moves.size(); i++)
        {
            calculatePawnUpMoves(moves.at(i), currentBoardState, returnVec);
        }
    }
    else
    {
        calculateKingMoves(blackLocations.getKingLocation(), currentBoardState, returnVec);

        moves = blackLocations.getBishopLocations();
        for (unsigned i = 0; i < moves.size(); i++)
        {
            calculateBishopMoves(moves.at(i), currentBoardState, returnVec);
        }

        moves = blackLocations.getRookLocations();
        for (unsigned i = 0; i < moves.size(); i++)
        {
            calculateRookMoves(moves.at(i), currentBoardState, returnVec);
        }

        moves = blackLocations.getQueenLocations();
        for (unsigned i = 0; i < moves.size(); i++)
        {
            calculateQueenMoves(moves.at(i), currentBoardState, returnVec);
        }

        moves = blackLocations.getKnightLocations();
        for (unsigned i = 0; i < moves.size(); i++)
        {
            calculateKnightMoves(moves.at(i), currentBoardState, returnVec);
        }

        moves = blackLocations.getPawnLocations();
        for (unsigned i = 0; i < moves.size(); i++)
        {
            calculatePawnDownMoves(moves.at(i), currentBoardState, returnVec);
        }
    }

    return returnVec;
}

void Board::calculateRookMoves(Box box, BoardState* currentBoardState, std::vector<Move>& currentPseudo)
{
    //Going to the right of the board.
    addStraightRightMoves(box, currentBoardState, currentPseudo);

    //Going to the left of the board.
    addStraightLeftMoves(box, currentBoardState, currentPseudo);

    //Going up the board.
    addStraightUpMoves(box, currentBoardState, currentPseudo);

    //Going down the board.
    addStraightDownMoves(box, currentBoardState, currentPseudo);
}

void Board::calculateKnightMoves(Box box, BoardState* currentBoardState, std::vector<Move>& moves)
{
    int x = box.x;
    int y = box.y;

    //Moving down.
    uint8_t** board = currentBoardState->getBoard();
    if (y - 2 >= 0)
    {
        if (x + 1 < boardXBoxes)
        {
            if (board[x + 1][y - 2] == 0 || !pieceIsCurrentPlayersPiece(x + 1, y - 2, currentBoardState))
            {
                attemptAddMove({{x, y}, {x + 1, y - 2}, false, false, false,false, ' ' },
                currentBoardState, moves);
            }
        }
        if (x - 1 >= 0)
        {
            if (board[x - 1][y - 2] == 0 || !pieceIsCurrentPlayersPiece(x - 1, y - 2, currentBoardState))
            {
                attemptAddMove({{x, y}, {x - 1, y - 2}, false, false, false, false, ' ' },
                currentBoardState, moves);
            }
        }
    }
    //Moving up.
    if (y + 2 < boardYBoxes)
    {
        if (x + 1 < boardXBoxes)
        {
            if (board[x + 1][y + 2] == 0 || !pieceIsCurrentPlayersPiece(x + 1, y + 2, currentBoardState))
            {
                attemptAddMove({{x, y}, {x + 1, y + 2}, false, false, false, false, ' ' },
                currentBoardState, moves);
            }
        }
        if (x - 1 >= 0)
        {
            if (board[x - 1][y + 2] == 0 || !pieceIsCurrentPlayersPiece(x - 1, y + 2, currentBoardState))
            {
                attemptAddMove({ {x, y}, {x - 1, y + 2}, false, false, false, false, ' ' },
                currentBoardState, moves);
            }
        }
    }
    //Moving to the left.
    if (x - 2 >= 0)
    {
        if (y + 1 < boardYBoxes)
        {
            if (board[x - 2][y + 1] == 0 || !pieceIsCurrentPlayersPiece(x - 2, y + 1, currentBoardState))
            {
                attemptAddMove({{x, y}, {x - 2, y + 1}, false, false, false,false, ' ' },
                currentBoardState, moves);
            }
        }
        if (y - 1 >= 0)
        {
            if (board[x - 2][y - 1] == 0 || !pieceIsCurrentPlayersPiece(x - 2, y - 1, currentBoardState))
            {
                attemptAddMove({{x, y}, {x - 2, y - 1}, false, false, false, false, ' ' },
                currentBoardState, moves);
            }
        }
    }
    //Moving to the right.
    if (x + 2 < boardXBoxes)
    {
        if (y + 1 < boardYBoxes)
        {
            if (board[x + 2][y + 1] == 0 || !pieceIsCurrentPlayersPiece(x + 2, y + 1, currentBoardState))
            {
                attemptAddMove({{x, y}, {x + 2, y + 1}, false, false, false, false, ' ' },
                currentBoardState, moves);
            }
        }
        if (y - 1 >= 0)
        {
            if (board[x + 2][y - 1] == 0 || !pieceIsCurrentPlayersPiece(x + 2, y - 1, currentBoardState))
            {
                attemptAddMove({{x, y}, {x + 2, y - 1}, false, false, false, false, ' ' },
                currentBoardState, moves);
            }
        }
    }
}

void Board::calculateBishopMoves(Box box, BoardState* currentBoardState, std::vector<Move>& currentPseudo)
{
    //Going down to the right.
    addDownRightMoves(box, currentBoardState, currentPseudo);

    //Going down to the left.
    addDownLeftMoves(box, currentBoardState, currentPseudo);

    //Going up to the right.
    addUpRightMoves(box, currentBoardState, currentPseudo);

    //Going up to the left.
    addUpLeftMoves(box, currentBoardState, currentPseudo);
}

void Board::calculateKingMoves(Box box, BoardState* currentBoardState, std::vector<Move>& currentPseudo)
{
    int x = box.x;
    int y = box.y;
    uint8_t** board = currentBoardState->getBoard();

    //Going up.
    if (y + 1 < boardYBoxes)
    {
        for (int xChange = -1; xChange <= 1; xChange++)
        {
            if (x + xChange < 0 || x + xChange >= boardXBoxes)
            {
                continue;
            }
            if (board[x + xChange][y + 1] == 0 || !pieceIsCurrentPlayersPiece(x + xChange, y + 1, currentBoardState))
            {
                currentPseudo.push_back({ {x,y},{x + xChange,y + 1}, false, false, false });
            }
        }
    }
    //Going down.
    if (y - 1 >= 0)
    {
        for (int xChange = -1; xChange <= 1; xChange++)
        {
            if (x + xChange < 0 || x + xChange >= boardXBoxes)
            {
                continue;
            }
            if (board[x + xChange][y - 1] == 0 || !pieceIsCurrentPlayersPiece(x + xChange, y - 1, currentBoardState))
            {
                currentPseudo.push_back({{x, y}, {x + xChange, y - 1}, false, false, false});
            }
        }
    }
    //Going to the left.
    if (x - 1 >= 0)
    {
        if (board[x - 1][y] == 0 || !pieceIsCurrentPlayersPiece(x - 1, y, currentBoardState))
        {
            currentPseudo.push_back({{x, y}, {x - 1, y}, false, false, false});
        }
    }
    //Going to the right.
    if (x + 1 < boardXBoxes)
    {
        if (board[x + 1][y] == 0 || !pieceIsCurrentPlayersPiece(x + 1, y, currentBoardState))
        {
            currentPseudo.push_back({{x, y}, {x + 1, y}, false, false, false});
        }
    }

    //Calculate castling.
    calculateCastlingMoves(box, currentBoardState, currentPseudo); //Check up later.
}

void Board::calculateQueenMoves(Box box, BoardState* currentBoardState, std::vector<Move>& currentPseudo)
{
    //Going down right.
    addDownRightMoves(box, currentBoardState, currentPseudo);

    //Going down left.
    addDownLeftMoves(box, currentBoardState, currentPseudo);

    //Going up right.
    addUpRightMoves(box, currentBoardState, currentPseudo);

    //Going up left.
    addUpLeftMoves(box, currentBoardState, currentPseudo);

    //Going to the right.
    addStraightRightMoves(box, currentBoardState, currentPseudo);

    //Going to the left.
    addStraightLeftMoves(box, currentBoardState, currentPseudo);

    //Going up the board.
    addStraightUpMoves(box, currentBoardState, currentPseudo);

    //Going down the board.
    addStraightDownMoves(box, currentBoardState, currentPseudo);
}

void Board::calculatePawnUpMoves(Box box, BoardState* currentBoardState, std::vector<Move>& moves)
{
    addStraightUpPawnMoves(box, currentBoardState, moves);
    addUpLeftPawnMoves(box, currentBoardState, moves);
    addUpRightPawnMoves(box, currentBoardState, moves);
}

void Board::calculatePawnDownMoves(Box box, BoardState* currentBoardState, std::vector<Move>& moves)
{
    addStraightDownPawnMoves(box, currentBoardState, moves);
    addDownLeftPawnMoves(box, currentBoardState, moves);
    addDownRightPawnMoves(box, currentBoardState, moves);
}

void Board::calculateCastlingMoves(Box box, BoardState* currentBoardState, std::vector<Move>& currentPseudo)
{

}

//Calculate legal moves.
std::vector<Move> Board::calculateLegalMoves(BoardState* currentBoardState)
{
    std::vector<Move> currentLegal;

    pseudoLegalMoves = calculatePseudoLegalMoves(currentBoardState);
    char currentTurn = currentBoardState->getCurrentTurn();

    Box kingLocation;

    if (currentTurn == 'w')
    {
        kingLocation = whiteLocations.getKingLocation();
        calculateKingLegalMoves(kingLocation, currentBoardState, currentLegal, whiteThreatened);
        if (whiteThreatened.amountAttacked != 2)
        {
            for (unsigned i = 0; i < whiteLocations.getRookLocations().size(); i++)
            {
                calculateRookLegalMoves(whiteLocations.getRookLocations().at(i), kingLocation,
                                        currentBoardState, currentLegal, whiteThreatened);
            }

            for (unsigned i = 0; i < whiteLocations.getKnightLocations().size(); i++)
            {
                calculateKnightLegalMoves(whiteLocations.getKnightLocations().at(i), kingLocation,
                                          currentBoardState, currentLegal, whiteThreatened);
            }

            for (unsigned i = 0; i < whiteLocations.getQueenLocations().size(); i++)
            {
                calculateQueenLegalMoves(whiteLocations.getQueenLocations().at(i), kingLocation,
                                         currentBoardState, currentLegal, whiteThreatened);
            }

            for (unsigned i = 0; i < whiteLocations.getPawnLocations().size(); i++)
            {
                calculatePawnLegalMoves(whiteLocations.getPawnLocations().at(i), kingLocation,
                                        currentBoardState, currentLegal, whiteThreatened);
            }

            for (unsigned i = 0; i < whiteLocations.getBishopLocations().size(); i++)
            {
                calculateBishopLegalMoves(whiteLocations.getBishopLocations().at(i), kingLocation,
                                          currentBoardState, currentLegal, whiteThreatened);
            }
        }
    }
    else
    {
        kingLocation = blackLocations.getKingLocation();
        calculateKingLegalMoves(kingLocation, currentBoardState, currentLegal, blackThreatened);
        if (blackThreatened.amountAttacked != 2)
        {
            for (unsigned i = 0; i < blackLocations.getRookLocations().size(); i++)
            {
                calculateRookLegalMoves(blackLocations.getRookLocations().at(i), kingLocation,
                                        currentBoardState, currentLegal, blackThreatened);
            }

            for (unsigned i = 0; i < blackLocations.getKnightLocations().size(); i++)
            {
                calculateKnightLegalMoves(blackLocations.getKnightLocations().at(i), kingLocation,
                                          currentBoardState, currentLegal, blackThreatened);
            }

            for (unsigned i = 0; i < blackLocations.getQueenLocations().size(); i++)
            {
                calculateQueenLegalMoves(blackLocations.getQueenLocations().at(i), kingLocation,
                                         currentBoardState, currentLegal, blackThreatened);
            }

            for (unsigned i = 0; i < blackLocations.getPawnLocations().size(); i++)
            {
                calculatePawnLegalMoves(blackLocations.getPawnLocations().at(i), kingLocation,
                                        currentBoardState, currentLegal, blackThreatened);
            }

            for (unsigned i = 0; i < blackLocations.getBishopLocations().size(); i++)
            {
                calculateBishopLegalMoves(blackLocations.getBishopLocations().at(i), kingLocation,
                                          currentBoardState, currentLegal, blackThreatened);
            }
        }
    }

    return currentLegal;
}

void Board::calculateRookLegalMoves(Box rookBox, Box kingBox, BoardState* currentBoardState,
                                    std::vector<Move>& currentLegalMoves,
                                    KingThreatenedInfo currentKingInfo)
{
    //Assume the king is not in check.
    bool addAll = true;
    if (inSameCol(rookBox, kingBox))
    {
        //If the rook is below the king.
        if (rookBox.y < kingBox.y)
        {
            //If the king is threatened from above
            //we only add the rook moves that are straight up and down.
            if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::straightUpThreatened)
                    == KingThreatenedInfo::straightUpThreatened)
            {
                if (currentKingInfo.straightUpBox.y < rookBox.y)
                {
                    addStraightDownMoves(rookBox, currentBoardState, currentLegalMoves);
                    addStraightUpMoves(rookBox, currentBoardState, currentLegalMoves);
                    addAll = false;
                }
            }
        }
        //The rook is above the king.
        else if (rookBox.y > kingBox.y)
        {
            //If the king is threatened from below
            //we only add moves that are straight up and down.
            if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::straightDownThreatened)
                    == KingThreatenedInfo::straightDownThreatened)
            {
                if (currentKingInfo.straightDownBox.y > rookBox.y)
                {
                    addStraightDownMoves(rookBox, currentBoardState, currentLegalMoves);
                    addStraightUpMoves(rookBox, currentBoardState, currentLegalMoves);
                    addAll = false;
                }
            }
        }
    }
    else if (inSameRow(rookBox, kingBox))
    {
        //Rook is to the right.
        if (rookBox.x > kingBox.x)
        {
            //If the king is threatened from right
            //we only add the rook moves that are straight right and left.
            if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::straightRightThreatened)
                    == KingThreatenedInfo::straightRightThreatened)
            {
                if (currentKingInfo.straightRightBox.x > rookBox.x)
                {
                    addStraightRightMoves(rookBox, currentBoardState, currentLegalMoves);
                    addStraightLeftMoves(rookBox, currentBoardState, currentLegalMoves);
                    addAll = false;
                }
            }

        }
        //Rook is to the left.
        else if (rookBox.x < kingBox.x)
        {
            //If the king is threatened from left
            //we only add the rook moves that are straight right and left.
            if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::straightLeftThreatened)
                    == KingThreatenedInfo::straightLeftThreatened)
            {
                if (currentKingInfo.straightLeftBox.x < rookBox.x)
                {
                    addStraightRightMoves(rookBox, currentBoardState, currentLegalMoves);
                    addStraightLeftMoves(rookBox, currentBoardState, currentLegalMoves);
                    addAll = false;
                }
            }
        }
    }
    else if (inSameDiagonal(rookBox, kingBox))
    {
        //If the rook is below the king.
        if (rookBox.y < kingBox.y)
        {
            //If the king is threatened we can't move the rook.
            if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::upRightThreatened)
                    == KingThreatenedInfo::upRightThreatened)
            {
                if (currentKingInfo.upRightBox.x > rookBox.x)
                {
                    addAll = false;
                }
            }
        }
        //If the rook is above the king.
        if (rookBox.y > kingBox.y)
        {
            //If the king is threatened we can't move the rook.
            if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::downLeftThreatened)
                    == KingThreatenedInfo::downLeftThreatened)
            {
                if (currentKingInfo.downLeftBox.x < rookBox.x)
                {
                    addAll = false;
                }
            }
        }
    }
    else if (inSameReverseDiagonal(rookBox, kingBox))
    {
        //If the rook is below the king.
        if (rookBox.y < kingBox.y)
        {
            //If the king is threatened we can't move the rook.
            if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::upLeftThreatened)
                    == KingThreatenedInfo::upLeftThreatened)
            {
                if (currentKingInfo.upLeftBox.x < rookBox.x)
                {
                    addAll = false;
                }
            }
        }
        //If the rook is above the king.
        if (rookBox.y > kingBox.y)
        {
            //If the king is threatened we can't move the rook.
            if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::downRightThreatened)
                    == KingThreatenedInfo::downRightThreatened)
            {
                if (currentKingInfo.downRightBox.x > rookBox.x)
                {
                    addAll = false;
                }
            }
        }
    }

    //Add all the possible rook moves.
    if (addAll)
    {
        calculateRookMoves(rookBox, currentBoardState, currentLegalMoves);
    }
}

void Board::calculateKnightLegalMoves(Box knightBox, Box kingBox, BoardState* currentBoardState,
                                      std::vector<Move>& currentLegalMoves,
                                      KingThreatenedInfo currentKingInfo)
{
    //Assume the king is not in check.
    bool addAll = true;
    if (inSameCol(knightBox, kingBox))
    {
        //If the knight is below the king.
        if (knightBox.y < kingBox.y)
        {
            if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::straightUpThreatened)
                    == KingThreatenedInfo::straightUpThreatened)
            {
                if (currentKingInfo.straightUpBox.y < knightBox.y)
                {
                    addAll = false;
                }
            }
        }
        //If the knight is above the king.
        else if (knightBox.y > kingBox.y)
        {
            if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::straightDownThreatened)
                    == KingThreatenedInfo::straightDownThreatened)
            {
                if (currentKingInfo.straightDownBox.y > knightBox.y)
                {
                    addAll = false;
                }
            }
        }
    }
    else if (inSameRow(knightBox, kingBox))
    {
        //Knight is to the right.
        if (knightBox.x > kingBox.x)
        {
            if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::straightRightThreatened)
                    == KingThreatenedInfo::straightRightThreatened)
            {
                if (currentKingInfo.straightRightBox.x > knightBox.x)
                {
                    addAll = false;
                }
            }
        }
        else if (knightBox.x < kingBox.x)
        {
            if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::straightLeftThreatened)
                    == KingThreatenedInfo::straightLeftThreatened)
            {
                if (currentKingInfo.straightLeftBox.x < knightBox.x)
                {
                    addAll = false;
                }
            }
        }
    }
    else if (inSameDiagonal(knightBox, kingBox))
    {
        //If the knight is above the king.
        if (knightBox.y < kingBox.y)
        {
            if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::upRightThreatened)
                    == KingThreatenedInfo::upRightThreatened)
            {
                if (currentKingInfo.upRightBox.x > knightBox.x)
                {
                    addAll = false;
                }
            }
        }
        //If the knight is below the king.
        if (knightBox.y > kingBox.y)
        {
            if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::downLeftThreatened)
                    == KingThreatenedInfo::downLeftThreatened)
            {
                if (currentKingInfo.downLeftBox.x < knightBox.x)
                {
                    addAll = false;
                }
            }
        }
    }
    else if (inSameReverseDiagonal(knightBox, kingBox))
    {
        //If the knight is above the king.
        if (knightBox.y < kingBox.y)
        {
            if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::upLeftThreatened)
                    == KingThreatenedInfo::upLeftThreatened)
            {
                if (currentKingInfo.upLeftBox.x < knightBox.x)
                {
                    addAll = false;
                }
            }
        }
        //If the knight is below the king.
        if (knightBox.y > kingBox.y)
        {
            if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::downRightThreatened)
                    == KingThreatenedInfo::downRightThreatened)
            {
                if (currentKingInfo.downRightBox.x > knightBox.x)
                {
                    addAll = false;
                }
            }
        }
    }

    //Add all the possible knight moves.
    if (addAll)
    {
        calculateKnightMoves(knightBox, currentBoardState, currentLegalMoves);
    }
}

void Board::calculateBishopLegalMoves(Box bishopBox, Box kingBox, BoardState* currentBoardState,
                                      std::vector<Move>& currentLegalMoves,
                                      KingThreatenedInfo currentKingInfo)
{
    //Assume the king is not in check.
    bool addAll = true;
    if (inSameCol(bishopBox, kingBox))
    {
        //If the bishop is below the king.
        if (bishopBox.y < kingBox.y)
        {
            //If the king is threatened we don't add bishop move.
            if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::straightUpThreatened)
                    == KingThreatenedInfo::straightUpThreatened)
            {
                if (currentKingInfo.straightUpBox.y < bishopBox.y)
                {
                    addAll = false;
                }
            }
        }
        //If the bishop is above the king.
        else if (bishopBox.y > kingBox.y)
        {
            if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::straightDownThreatened)
                    == KingThreatenedInfo::straightDownThreatened)
            {
                if (currentKingInfo.straightDownBox.y > bishopBox.y)
                {
                    addAll = false;
                }
            }
        }
    }
    else if (inSameRow(bishopBox, kingBox))
    {
        //Bishop is to the right.
        if (bishopBox.x > kingBox.x)
        {
            if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::straightRightThreatened)
                    == KingThreatenedInfo::straightRightThreatened)
            {
                if (currentKingInfo.straightRightBox.x > bishopBox.x)
                {
                    addAll = false;
                }
            }
        }
        //Bishop is to the left.
        else if (bishopBox.x < kingBox.x)
        {
            if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::straightLeftThreatened)
                    == KingThreatenedInfo::straightLeftThreatened)
            {
                if (currentKingInfo.straightLeftBox.x < bishopBox.x)
                {
                    addAll = false;
                }
            }
        }
    }
    else if (inSameDiagonal(bishopBox, kingBox))
    {
        //If the bishop is below the king.
        if (bishopBox.y < kingBox.y)
        {
            if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::upRightThreatened)
                    == KingThreatenedInfo::upRightThreatened)
            {
                if (currentKingInfo.upRightBox.x > bishopBox.x)
                {
                    addUpRightMoves(bishopBox, currentBoardState, currentLegalMoves);
                    addDownLeftMoves(bishopBox, currentBoardState, currentLegalMoves);
                    addAll = false;
                }
            }
        }
        //If the bishop is above the king.
        if (bishopBox.y > kingBox.y)
        {
            if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::downLeftThreatened)
                    == KingThreatenedInfo::downLeftThreatened)
            {
                if (currentKingInfo.downLeftBox.x < bishopBox.x)
                {
                    addUpRightMoves(bishopBox, currentBoardState, currentLegalMoves);
                    addDownLeftMoves(bishopBox, currentBoardState, currentLegalMoves);
                    addAll = false;
                }
            }
        }
    }
    else if (inSameReverseDiagonal(bishopBox, kingBox))
    {
        //If the bishop is below the king.
        if (bishopBox.y < kingBox.y)
        {
            if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::upLeftThreatened)
                    == KingThreatenedInfo::upLeftThreatened)
            {
                if (currentKingInfo.upLeftBox.x < bishopBox.x)
                {
                    addUpLeftMoves(bishopBox, currentBoardState, currentLegalMoves);
                    addDownRightMoves(bishopBox, currentBoardState, currentLegalMoves);
                    addAll = false;
                }
            }
        }
        //If the bishop is above the king.
        if (bishopBox.y > kingBox.y)
        {
            if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::downRightThreatened)
                    == KingThreatenedInfo::downRightThreatened)
            {
                if (currentKingInfo.downRightBox.x > bishopBox.x)
                {
                    addUpLeftMoves(bishopBox, currentBoardState, currentLegalMoves);
                    addDownRightMoves(bishopBox, currentBoardState, currentLegalMoves);
                    addAll = false;
                }
            }
        }
    }

    //Add all the possible bishop moves.
    if (addAll)
    {
        calculateBishopMoves(bishopBox, currentBoardState, currentLegalMoves);
    }
}

void Board::calculateKingLegalMoves(Box kingBox, BoardState* currentBoardState,
                                    std::vector<Move>& currentLegalMoves,
                                    KingThreatenedInfo currentKingInfo)
{
    int x = kingBox.x;
    int y = kingBox.y;
    uint8_t** board = currentBoardState->getBoard();
    uint8_t currentPlayerPieceType = (currentBoardState->getCurrentTurn() == 'w') ? Piece::white : Piece::black;

    for (int change = -1; change <= 1; ++change)
    {
        if (y + 1 < boardYBoxes)
        {
            if (x + change < boardXBoxes && x + change >= 0)
            {
                if ((board[x + change][y + 1] & currentPlayerPieceType) != currentPlayerPieceType)
                {
                    if (!squareAttacked({ x + change,y + 1 }, currentBoardState))
                    {
                        currentLegalMoves.push_back({{x, y}, {x + change, y + 1},
                            false, false, false, false, ' '});
                    }
                }
            }
        }
        if (y - 1 >= 0)
        {
            if (x + change < boardXBoxes && x + change >= 0)
            {
                if ((board[x + change][y - 1] & currentPlayerPieceType) != currentPlayerPieceType)
                {
                    if (!squareAttacked({ x + change,y - 1 }, currentBoardState))
                    {
                        currentLegalMoves.push_back({{x, y}, {x + change, y - 1},
                            false, false, false, false, ' ' });
                    }
                }
            }
        }
    }
    if (x + 1 < boardXBoxes)
    {
        if (!squareAttacked({x + 1, y}, currentBoardState))
        {
            if ((board[x + 1][y] & currentPlayerPieceType) != currentPlayerPieceType)
            {
                currentLegalMoves.push_back({{x, y}, {x + 1, y}, false, false, false, false, ' '});
            }
        }
    }
    if (x - 1 >= 0)
    {
        if ((board[x - 1][y] & currentPlayerPieceType) != currentPlayerPieceType)
        {
            if (!squareAttacked({x - 1, y}, currentBoardState))
            {
                currentLegalMoves.push_back({{x, y}, {x - 1, y}, false, false, false, false, ' '});
            }
        }
    }

    calculateCastlingLegalMoves(kingBox, currentBoardState, currentLegalMoves);
}

void Board::calculateQueenLegalMoves(Box queenBox, Box kingBox, BoardState* currentBoardState,
                                     std::vector<Move>& currentLegalMoves,
                                     KingThreatenedInfo currentKingInfo)
{
    //Assume the king is not in check.
    bool addAll = true;
    if (inSameCol(queenBox, kingBox))
    {
        //If the queen is below the king.
        if (queenBox.y < kingBox.y)
        {
            if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::straightUpThreatened)
                    == KingThreatenedInfo::straightUpThreatened)
            {
                if (currentKingInfo.straightUpBox.y < queenBox.y)
                {
                    addStraightUpMoves(queenBox, currentBoardState, currentLegalMoves);
                    addStraightDownMoves(queenBox, currentBoardState, currentLegalMoves);
                    addAll = false;
                }
            }
        }
        //If the queen is above the king.
        else if (queenBox.y > kingBox.y)
        {
            if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::straightDownThreatened)
                    == KingThreatenedInfo::straightDownThreatened)
            {
                if (currentKingInfo.straightDownBox.y > queenBox.y)
                {
                    addStraightUpMoves(queenBox, currentBoardState, currentLegalMoves);
                    addStraightDownMoves(queenBox, currentBoardState, currentLegalMoves);
                    addAll = false;
                }
            }
        }
    }
    else if (inSameRow(queenBox, kingBox))
    {
        //Queen is to the right.
        if (queenBox.x > kingBox.x)
        {
            if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::straightRightThreatened)
                    == KingThreatenedInfo::straightRightThreatened)
            {
                if (currentKingInfo.straightRightBox.x > queenBox.x)
                {
                    addStraightLeftMoves(queenBox, currentBoardState, currentLegalMoves);
                    addStraightRightMoves(queenBox, currentBoardState, currentLegalMoves);
                    addAll = false;
                }
            }
        }
        //Queen is to the left.
        else if (queenBox.x < kingBox.x)
        {
            if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::straightLeftThreatened)
                    == KingThreatenedInfo::straightLeftThreatened)
            {
                if (currentKingInfo.straightLeftBox.x < queenBox.x)
                {
                    addStraightLeftMoves(queenBox, currentBoardState, currentLegalMoves);
                    addStraightRightMoves(queenBox, currentBoardState, currentLegalMoves);
                    addAll = false;
                }
            }
        }
    }
    else if (inSameDiagonal(queenBox, kingBox))
    {
        //If the queen is above the king.
        if (queenBox.y > kingBox.y)
        {
            if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::upRightThreatened)
                    == KingThreatenedInfo::upRightThreatened)
            {
                if (currentKingInfo.upRightBox.x > queenBox.x)
                {
                    addUpRightMoves(queenBox, currentBoardState, currentLegalMoves);
                    addDownLeftMoves(queenBox, currentBoardState, currentLegalMoves);
                    addAll = false;
                }
            }
        }
        //If the queen is below the king.
        if (queenBox.y < kingBox.y)
        {
            if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::downLeftThreatened)
                    == KingThreatenedInfo::downLeftThreatened)
            {
                if (currentKingInfo.downLeftBox.x < queenBox.x)
                {
                    addUpRightMoves(queenBox, currentBoardState, currentLegalMoves);
                    addDownLeftMoves(queenBox, currentBoardState, currentLegalMoves);
                    addAll = false;
                }
            }
        }
    }
    else if (inSameReverseDiagonal(queenBox, kingBox))
    {
        //If the queen is below the king.
        if (queenBox.y < kingBox.y)
        {
            if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::upLeftThreatened) == KingThreatenedInfo::upLeftThreatened)
            {
                if (currentKingInfo.upLeftBox.x < queenBox.x)
                {
                    addUpLeftMoves(queenBox, currentBoardState, currentLegalMoves);
                    addDownRightMoves(queenBox, currentBoardState, currentLegalMoves);
                    addAll = false;
                }
            }
        }
        //If the queen is above the king.
        if (queenBox.y > kingBox.y)
        {
            if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::downRightThreatened)
                    == KingThreatenedInfo::downRightThreatened)
            {
                if (currentKingInfo.downRightBox.x > queenBox.x)
                {
                    addUpLeftMoves(queenBox, currentBoardState, currentLegalMoves);
                    addDownRightMoves(queenBox, currentBoardState, currentLegalMoves);
                    addAll = false;
                }
            }
        }
    }

    //Add all the possible queen moves.
    if (addAll)
    {
        calculateQueenMoves(queenBox, currentBoardState, currentLegalMoves);
    }
}

void Board::calculatePawnLegalMoves(Box pawnBox, Box kingBox, BoardState* currentBoardState,
                                    std::vector<Move>& currentLegalMoves,
                                    KingThreatenedInfo currentKingInfo)
{
    char currentTurn = currentBoardState->getCurrentTurn();
    bool addAll = true;

    if (currentTurn == 'w')
    {
        if (inSameCol(pawnBox, kingBox))
        {
            //If the pawn is below the king.
            if (pawnBox.y < kingBox.y)
            {
                //If the king is threatened from above
                //we only add the pawn moves that are straight up and down.
                if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::straightUpThreatened)
                        == KingThreatenedInfo::straightUpThreatened)
                {
                    if (currentKingInfo.straightUpBox.y < pawnBox.y)
                    {
                        addStraightUpPawnMoves(pawnBox, currentBoardState, currentLegalMoves);
                        addAll = false;
                    }
                }
            }
            //If the pawn is above the king.
            else if (pawnBox.y > kingBox.y)
            {
                //If the king is threatened from below
                //we only add moves that are straight up and down.
                if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::straightDownThreatened)
                        == KingThreatenedInfo::straightDownThreatened)
                {
                    if (currentKingInfo.straightDownBox.y > pawnBox.y)
                    {
                        addStraightDownPawnMoves(pawnBox, currentBoardState, currentLegalMoves);
                        addAll = false;
                    }
                }
            }
        }
        else if (inSameRow(pawnBox, kingBox))
        {
            //Pawn is to the right.
            if (pawnBox.x > kingBox.x)
            {
                //If the king is threatened from right
                //we only add the pawn moves that are straight up and down.
                if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::straightRightThreatened)
                        == KingThreatenedInfo::straightRightThreatened)
                {
                    if (currentKingInfo.straightRightBox.x > pawnBox.x)
                    {
                        addAll = false;
                    }
                }
            }
            //Pawn is to the left.
            else if (pawnBox.x < kingBox.x)
            {
                //If the king is threatened from left
                //we only add the pawn moves that are straight up and down.
                if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::straightLeftThreatened)
                        == KingThreatenedInfo::straightLeftThreatened)
                {
                    if (currentKingInfo.straightLeftBox.x < pawnBox.x)
                    {
                        addAll = false;
                    }
                }
            }
        }
        else if (inSameDiagonal(pawnBox, kingBox))
        {
            //If the pawn is above the king.
            if (pawnBox.y < kingBox.y)
            {
                //If the king is attacked diagonally
                //we can only move the pawn to the top right.
                if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::upRightThreatened)
                        == KingThreatenedInfo::upRightThreatened)
                {
                    if (currentKingInfo.upRightBox.x > pawnBox.x)
                    {
                        addUpRightPawnMoves(pawnBox, currentBoardState, currentLegalMoves);
                        addAll = false;
                    }
                }
            }
            //If the pawn is above the king.
            if (pawnBox.y > kingBox.y)
            {
                //If the king is attacked diagonally from below we can't move the pawn.
                if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::downLeftThreatened)
                        == KingThreatenedInfo::downLeftThreatened)
                {
                    if (currentKingInfo.downLeftBox.x < pawnBox.x)
                    {
                        addAll = false;
                    }
                }
            }
        }
        else if (inSameReverseDiagonal(pawnBox, kingBox))
        {
            //If the pawn is below the king.
            if (pawnBox.y < kingBox.y)
            {
                //If the king is attacked diagonally we can't move the pawn.
                if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::upLeftThreatened)
                        == KingThreatenedInfo::upLeftThreatened)
                {
                    if (currentKingInfo.upLeftBox.x < pawnBox.x)
                    {
                        addUpLeftPawnMoves(pawnBox, currentBoardState, currentLegalMoves);
                        addAll = false;
                    }
                }
                else
                {
                    //std::cout << "up left doesn't match" << std::endl;
                }
            }
            //If the pawn is above the king.
            if (pawnBox.y > kingBox.y)
            {
                //If the king is attacked diagonally from below we can't move the pawn.`
                if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::downRightThreatened)
                        == KingThreatenedInfo::downRightThreatened)
                {
                    if (currentKingInfo.downRightBox.x > pawnBox.x)
                    {
                        addAll = false;
                    }
                }
            }
        }
        else
        {
            //std::cout << "Pawn is good: " << pawnBox.x << " " << pawnBox.y << std::endl;
        }

        //Add all the possible pawn moves.
        if (addAll)
        {
            calculatePawnUpMoves(pawnBox, currentBoardState, currentLegalMoves);
        }
    }
    //If it's blacks turn.
    else
    {
        if (inSameCol(pawnBox, kingBox))
        {
            //If the pawn is below the king.
            if (pawnBox.y < kingBox.y)
            {
                //If the king is threatened from above
                //we only add the pawn moves that are straight up and down.
                if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::straightUpThreatened)
                        == KingThreatenedInfo::straightUpThreatened)
                {
                    if (currentKingInfo.straightUpBox.y < pawnBox.y)
                    {
                        addStraightDownPawnMoves(pawnBox, currentBoardState, currentLegalMoves);
                        addAll = false;
                    }
                }
            }
            //If the pawn is above the king.
            else if (pawnBox.y > kingBox.y)
            {
                //If the king is threatened from below
                //we only add moves that are straight up and down.
                if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::straightDownThreatened)
                        == KingThreatenedInfo::straightDownThreatened)
                {
                    if (currentKingInfo.straightDownBox.y > pawnBox.y)
                    {
                        addStraightDownPawnMoves(pawnBox, currentBoardState, currentLegalMoves);
                        addAll = false;
                    }
                }
            }
        }
        else if (inSameRow(pawnBox, kingBox))
        {
            //Pawn is to the right.
            if (pawnBox.x > kingBox.x)
            {
                //If the king is threatened from right
                //we only add the pawn moves that are straight up and down.
                if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::straightRightThreatened)
                        == KingThreatenedInfo::straightRightThreatened)
                {
                    if (currentKingInfo.straightRightBox.x > pawnBox.x)
                    {
                        addAll = false;
                    }
                }
            }
            //Pawn is to the left.
            else if (pawnBox.x < kingBox.x)
            {
                //If the king is threatened from left
                //we only add the pawn moves that are straight up and down.
                if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::straightLeftThreatened)
                        == KingThreatenedInfo::straightLeftThreatened)
                {
                    if (currentKingInfo.straightLeftBox.x < pawnBox.x)
                    {
                        addAll = false;
                    }
                }
            }
        }
        else if (inSameDiagonal(pawnBox, kingBox))
        {
            //If the pawn is below the king.
            if (pawnBox.y < kingBox.y)
            {
                //If the king is attacked diagonally we can only move the pawn to the bottom left.
                if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::upRightThreatened)
                        == KingThreatenedInfo::upRightThreatened)
                {
                    if (currentKingInfo.upRightBox.x > pawnBox.x)
                    {
                        addAll = false;
                    }
                }
            }
            //If the pawn is above the king.
            if (pawnBox.y > kingBox.y)
            {
                //if the king is attacked diagonally from below we can't move the pawn.
                if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::downLeftThreatened)
                        == KingThreatenedInfo::downLeftThreatened)
                {
                    if (currentKingInfo.downLeftBox.x < pawnBox.x)
                    {
                        addDownLeftPawnMoves(pawnBox, currentBoardState, currentLegalMoves);
                        addAll = false;
                    }
                }
            }
        }
        else if (inSameReverseDiagonal(pawnBox, kingBox))
        {
            //If the pawn is below the king.
            if (pawnBox.y < kingBox.y)
            {
                //If the king is attacked diagonally we can't move the pawn.
                if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::upLeftThreatened)
                        == KingThreatenedInfo::upLeftThreatened)
                {
                    if (currentKingInfo.upLeftBox.x < pawnBox.x)
                    {
                        addAll = false;
                    }
                }
            }
            //If the pawn is above the king.
            if (pawnBox.y > kingBox.y)
            {
                //If the king is attacked diagonally from below we can't move the pawn.
                if ((currentKingInfo.threatenedInfo & KingThreatenedInfo::downRightThreatened)
                        == KingThreatenedInfo::downRightThreatened)
                {
                    if (currentKingInfo.downRightBox.x > pawnBox.x)
                    {
                        addDownRightPawnMoves(pawnBox, currentBoardState, currentLegalMoves);
                        addAll = false;
                    }
                }
            }
        }

        //Add all the possible pawn moves.
        if (addAll)
        {
            calculatePawnDownMoves(pawnBox, currentBoardState, currentLegalMoves);
        }
    }
}

void Board::calculateCastlingLegalMoves(Box kingBox, BoardState* currentBoardState,
                                        std::vector<Move>& currentLegalMoves)
{
    int x = kingBox.x;
    int y = kingBox.y;
    uint8_t** board = currentBoardState->getBoard();
    int attackedAmount;
    char currentTurn = currentBoardState->getCurrentTurn();

    if (currentTurn == 'w')
    {
        attackedAmount = whiteThreatened.amountAttacked;
    }
    else
    {
        attackedAmount = blackThreatened.amountAttacked;
    }
    //If the king is attacked you can't castle.
    if (attackedAmount != 0)
    {
        return;
    }
    if ((board[x][y] & Piece::white) == Piece::white)
    {
        if (currentBoardState->getWhiteCanKingsideCastle())
        {
            if (board[x + 1][y] == 0 && board[x + 2][y] == 0)
            {
                if (!squareAttacked({x + 1, y}, currentBoardState) && !squareAttacked({x + 2, y}, currentBoardState))
                {
                    currentLegalMoves.push_back({{x, y}, {x + 2, y}, true, false, false});
                }
            }
        }
        if (currentBoardState->getWhiteCanQueensideCastle())
        {
            if (board[x - 1][y] == 0 && board[x - 2][y] == 0 && board[x-3][y] == 0)
            {
                if (!squareAttacked({x - 1, y}, currentBoardState) && !squareAttacked({x - 2, y}, currentBoardState))
                {
                    currentLegalMoves.push_back({{x, y}, {x - 2, y}, false, true, false});
                }
            }
        }
    }
    else
    {
        if (currentBoardState->getBlackCanKingsideCastle())
        {
            if (board[x + 1][y] == 0 && board[x + 2][y] == 0)
            {
                if (!squareAttacked({x + 1, y}, currentBoardState) && !squareAttacked({x + 2, y}, currentBoardState))
                {
                    currentLegalMoves.push_back({{x, y}, {x + 2, y}, true, false, false});
                }
            }
        }
        if (currentBoardState->getBlackCanQueensideCastle())
        {
            if (board[x - 1][y] == 0 && board[x - 2][y] == 0 && board[x - 3][y] == 0)
            {
                if (!squareAttacked({x - 1, y}, currentBoardState) && !squareAttacked({x - 2, y}, currentBoardState))
                {
                    currentLegalMoves.push_back({{x, y}, {x - 2, y}, false, true, false});
                }
            }
        }
    }
}
//-------------------------------------------------------------//

//UPDATING.
//Update loop.
void Board::update()
{

}

//Update threats.
void Board::updateStraightUpThreats(char currentTurn, BoardState* currentBoardState)
{
    int defense;
    uint8_t** board = currentBoardState->getBoard();
    Box kingSquare;
    bool foundAThreat = false;

    //White turn.
    if (currentTurn == 'w')
    {
        defense = 0;
        kingSquare = whiteLocations.getKingLocation();
        for (int y = kingSquare.y - 1; y >= 0; --y)
        {
            if (board[kingSquare.x][y] == 0)
            {
                continue;
            }
            else if ((board[kingSquare.x][y] & Piece::white) == Piece::white
                     || ((board[kingSquare.x][y] & Piece::rook) != Piece::rook
                         && (board[kingSquare.x][y] & Piece::queen) != Piece::queen))
            {
                defense++;
                if (defense == 2)
                {
                    break;
                }
            }
            else
            {
                if ((board[kingSquare.x][y] & Piece::rook) == Piece::rook
                        || (board[kingSquare.x][y] & Piece::queen) == Piece::queen)
                {
                    whiteThreatened.straightUpBox = {kingSquare.x, y};
                    if (defense == 0)
                    {
                        whiteThreatened.attackedInfo |= KingThreatenedInfo::straightUpThreatened;
                        whiteThreatened.threatenedInfo |= KingThreatenedInfo::straightUpThreatened;
                        whiteThreatened.amountAttacked++;
                        whiteThreatened.attackedfromBox = {kingSquare.x, y};
                        foundAThreat = true;
                        break;
                    }
                    else
                    {
                        whiteThreatened.threatenedInfo |= KingThreatenedInfo::straightUpThreatened;
                        foundAThreat = true;
                        break;
                    }
                }
            }
        }
        if (!foundAThreat)
        {
            whiteThreatened.threatenedInfo &= ~(1UL << 2);
        }

    }
    //Black turn.
    else
    {
        defense = 0;
        kingSquare = blackLocations.getKingLocation();
        for (int y = kingSquare.y - 1; y >= 0; --y)
        {
            if (board[kingSquare.x][y] == 0)
            {
                continue;
            }
            else if ((board[kingSquare.x][y] & Piece::black) == Piece::black
                     || ((board[kingSquare.x][y] & Piece::rook) != Piece::rook
                         && (board[kingSquare.x][y] & Piece::queen) != Piece::queen))
            {
                defense++;
                if (defense == 2)
                {
                    break;
                }
            }
            else
            {
                if ((board[kingSquare.x][y] & Piece::rook) == Piece::rook
                        || (board[kingSquare.x][y] & Piece::queen) == Piece::queen)
                {
                    blackThreatened.straightUpBox = {kingSquare.x, y};
                    if (defense == 0)
                    {
                        blackThreatened.attackedInfo |= KingThreatenedInfo::straightUpThreatened;
                        blackThreatened.threatenedInfo |= KingThreatenedInfo::straightUpThreatened;
                        blackThreatened.amountAttacked++;
                        blackThreatened.attackedfromBox = {kingSquare.x, y};
                        foundAThreat = true;
                        break;
                    }
                    else
                    {
                        blackThreatened.threatenedInfo |= KingThreatenedInfo::straightUpThreatened;
                        foundAThreat = true;
                        break;
                    }
                }
            }
        }
        if (!foundAThreat)
        {
            blackThreatened.threatenedInfo &= ~(1UL << 2);
        }
    }
}

void Board::updateStraightDownThreats(char currentTurn, BoardState* currentBoardState)
{
    int defense;
    uint8_t** board = currentBoardState->getBoard();
    Box kingSquare;
    bool foundAThreat = false;

    //White turn.
    if (currentTurn == 'w')
    {
        defense = 0;
        kingSquare = whiteLocations.getKingLocation();
        for (int y = kingSquare.y + 1; y < boardYBoxes; ++y)
        {
            if (board[kingSquare.x][y] == 0)
            {
                continue;
            }
            else if ((board[kingSquare.x][y] & Piece::white) == Piece::white
                     || ((board[kingSquare.x][y] & Piece::rook) != Piece::rook
                         && (board[kingSquare.x][y] & Piece::queen) != Piece::queen))
            {
                defense++;
                if (defense == 2)
                {
                    break;
                }
            }
            else
            {
                if ((board[kingSquare.x][y] & Piece::rook) == Piece::rook
                        || (board[kingSquare.x][y] & Piece::queen) == Piece::queen)
                {
                    whiteThreatened.straightDownBox = {kingSquare.x, y};
                    if (defense == 0)
                    {
                        whiteThreatened.attackedInfo |= KingThreatenedInfo::straightDownThreatened;
                        whiteThreatened.threatenedInfo |= KingThreatenedInfo::straightDownThreatened;
                        whiteThreatened.amountAttacked++;
                        whiteThreatened.attackedfromBox = {kingSquare.x, y};
                        foundAThreat = true;
                        break;
                    }
                    else
                    {
                        whiteThreatened.threatenedInfo |= KingThreatenedInfo::straightDownThreatened;
                        foundAThreat = true;
                        break;
                    }
                }
            }
        }
        if (!foundAThreat)
        {
            whiteThreatened.threatenedInfo &= ~(1UL << 6);
        }
        foundAThreat = false;
    }
    //Black turn.
    else
    {
        defense = 0;
        kingSquare = blackLocations.getKingLocation();
        for (int y = kingSquare.y + 1; y < boardYBoxes; ++y)
        {
            if (board[kingSquare.x][y] == 0)
            {
                continue;
            }
            else if ((board[kingSquare.x][y] & Piece::black) == Piece::black
                     || ((board[kingSquare.x][y] & Piece::rook) != Piece::rook
                         && (board[kingSquare.x][y] & Piece::queen) != Piece::queen))
            {
                defense++;
                if (defense == 2)
                {
                    break;
                }
            }
            else
            {

                if ((board[kingSquare.x][y] & Piece::rook) == Piece::rook
                        || (board[kingSquare.x][y] & Piece::queen) == Piece::queen)
                {
                    blackThreatened.straightDownBox = {kingSquare.x, y};
                    if (defense == 0)
                    {
                        blackThreatened.attackedInfo |= KingThreatenedInfo::straightDownThreatened;
                        blackThreatened.threatenedInfo |= KingThreatenedInfo::straightDownThreatened;
                        blackThreatened.amountAttacked++;
                        blackThreatened.attackedfromBox = {kingSquare.x, y};
                        foundAThreat = true;
                        break;
                    }
                    else
                    {
                        blackThreatened.threatenedInfo |= KingThreatenedInfo::straightDownThreatened;
                        foundAThreat = true;
                        break;
                    }
                }
            }
        }
        if (!foundAThreat)
        {
            blackThreatened.threatenedInfo &= ~(1UL << 6);
        }
    }
}

void Board::updateStraightLeftThreats(char currentTurn, BoardState* currentBoardState)
{
    int defense;
    uint8_t** board = currentBoardState->getBoard();
    Box kingSquare;
    bool foundAThreat = false;

    //White turn.
    if (currentTurn == 'w')
    {
        defense = 0;
        kingSquare = whiteLocations.getKingLocation();
        for (int x = kingSquare.x - 1; x >= 0; --x)
        {
            if (board[x][kingSquare.y] == 0)
            {
                continue;
            }
            else if ((board[x][kingSquare.y] & Piece::white) == Piece::white
                     || ((board[x][kingSquare.y] & Piece::rook) != Piece::rook
                         && (board[x][kingSquare.y] & Piece::queen) != Piece::queen))
            {
                defense++;
                if (defense == 2)
                {
                    break;
                }
            }
            else
            {
                if ((board[x][kingSquare.y] & Piece::rook) == Piece::rook
                        || (board[x][kingSquare.y] & Piece::queen) == Piece::queen)
                {
                    whiteThreatened.straightLeftBox = {x, kingSquare.y};
                    if (defense == 0)
                    {
                        whiteThreatened.attackedInfo |= KingThreatenedInfo::straightLeftThreatened;
                        whiteThreatened.threatenedInfo |= KingThreatenedInfo::straightLeftThreatened;
                        whiteThreatened.amountAttacked++;
                        whiteThreatened.attackedfromBox = {x, kingSquare.y};
                        foundAThreat = true;
                        break;
                    }
                    else
                    {
                        whiteThreatened.threatenedInfo |= KingThreatenedInfo::straightLeftThreatened;
                        foundAThreat = true;
                        break;
                    }
                }
            }
        }
        if (!foundAThreat)
        {
            whiteThreatened.threatenedInfo &= ~(1UL << 0);
        }
    }
    //Black turn.
    else
    {
        defense = 0;
        kingSquare = blackLocations.getKingLocation();
        for (int x = kingSquare.x - 1; x >= 0; --x)
        {
            if (board[x][kingSquare.y] == 0)
            {
                continue;
            }
            else if ((board[x][kingSquare.y] & Piece::black) == Piece::black
                     || ((board[x][kingSquare.y] & Piece::rook) != Piece::rook
                         && (board[x][kingSquare.y] & Piece::queen) != Piece::queen))
            {
                defense++;
                if (defense == 2)
                {
                    break;
                }
            }
            else
            {
                if ((board[x][kingSquare.y] & Piece::rook) == Piece::rook
                        || (board[x][kingSquare.y] & Piece::queen) == Piece::queen)
                {
                    blackThreatened.straightLeftBox = {x, kingSquare.y};
                    if (defense == 0)
                    {
                        blackThreatened.attackedInfo |= KingThreatenedInfo::straightLeftThreatened;
                        blackThreatened.threatenedInfo |= KingThreatenedInfo::straightLeftThreatened;
                        blackThreatened.amountAttacked++;
                        blackThreatened.attackedfromBox = {x, kingSquare.y};
                        foundAThreat = true;
                        break;
                    }
                    else
                    {
                        blackThreatened.threatenedInfo |= KingThreatenedInfo::straightLeftThreatened;
                        foundAThreat = true;
                        break;
                    }
                }
            }
        }
        if (!foundAThreat)
        {
            blackThreatened.threatenedInfo &= ~(1UL << 0);
        }
    }
}

void Board::updateStraightRightThreats(char currentTurn, BoardState* currentBoardState)
{
    int defense;
    uint8_t** board = currentBoardState->getBoard();
    Box kingSquare;
    bool foundAThreat = false;

    //White turn.
    if (currentTurn == 'w')
    {
        defense = 0;
        kingSquare = whiteLocations.getKingLocation();
        for (int x = kingSquare.x + 1; x < boardXBoxes; ++x)
        {
            if (board[x][kingSquare.y] == 0)
            {
                continue;
            }
            else if ((board[x][kingSquare.y] & Piece::white) == Piece::white
                     || ((board[x][kingSquare.y] & Piece::rook) != Piece::rook
                         && (board[x][kingSquare.y] & Piece::queen) != Piece::queen))
            {
                defense++;
                if (defense == 2)
                {
                    break;
                }
            }
            else
            {
                if ((board[x][kingSquare.y] & Piece::rook) == Piece::rook
                        || (board[x][kingSquare.y] & Piece::queen) == Piece::queen)
                {
                    whiteThreatened.straightRightBox = {x, kingSquare.y};
                    if (defense == 0)
                    {
                        whiteThreatened.attackedInfo |= KingThreatenedInfo::straightRightThreatened;
                        whiteThreatened.threatenedInfo |= KingThreatenedInfo::straightRightThreatened;
                        whiteThreatened.amountAttacked++;
                        whiteThreatened.attackedfromBox = {x, kingSquare.y};
                        foundAThreat = true;
                        break;
                    }
                    else
                    {
                        whiteThreatened.threatenedInfo |= KingThreatenedInfo::straightRightThreatened;
                        foundAThreat = true;
                        break;
                    }
                }
            }
        }
        if (!foundAThreat)
        {
            whiteThreatened.threatenedInfo &= ~(1UL << 4);
        }
        foundAThreat = false;
    }
    //Black turn.
    else
    {
        defense = 0;
        kingSquare = blackLocations.getKingLocation();
        for (int x = kingSquare.x + 1; x < boardXBoxes; ++x)
        {
            if (board[x][kingSquare.y] == 0)
            {
                continue;
            }
            else if ((board[x][kingSquare.y] & Piece::black) == Piece::black
                     || ((board[x][kingSquare.y] & Piece::rook) != Piece::rook
                         && (board[x][kingSquare.y] & Piece::queen) != Piece::queen))
            {
                defense++;
                if (defense == 2)
                {
                    break;
                }
            }
            else
            {
                if ((board[x][kingSquare.y] & Piece::rook) == Piece::rook
                        || (board[x][kingSquare.y] & Piece::queen) == Piece::queen)
                {
                    blackThreatened.straightRightBox = {x, kingSquare.y};
                    if (defense == 0)
                    {
                        blackThreatened.attackedInfo |= KingThreatenedInfo::straightRightThreatened;
                        blackThreatened.threatenedInfo |= KingThreatenedInfo::straightRightThreatened;
                        blackThreatened.amountAttacked++;
                        blackThreatened.attackedfromBox = {x, kingSquare.y} ;
                        foundAThreat = true;
                        break;
                    }
                    else
                    {
                        blackThreatened.threatenedInfo |= KingThreatenedInfo::straightRightThreatened;
                        foundAThreat = true;
                        break;
                    }
                }
            }
        }
        if (!foundAThreat)
        {
            blackThreatened.threatenedInfo &= ~(1UL << 4);
        }
    }
}

void Board::updateUpLeftThreats(char currentTurn, BoardState* currentBoardState)
{
    int defense;
    uint8_t** board = currentBoardState->getBoard();
    bool foundAThreat = false;
    Box kingSquare;
    int x, y;

    //White turn.
    if (currentTurn == 'w')
    {
        defense = 0;
        kingSquare = whiteLocations.getKingLocation();
        x = kingSquare.x;
        y = kingSquare.y;
        while (--x >= 0 && --y >= 0)
        {
            if (board[x][y] == 0)
            {
                continue;
            }
            else if ((board[x][y] & Piece::white) == Piece::white
                     || ((board[x][y] & Piece::bishop) != Piece::bishop
                         && (board[x][y] & Piece::queen) != Piece::queen))
            {
                defense++;
                if (defense == 2)
                {
                    break;
                }
            }
            else
            {
                if ((board[x][y] & Piece::bishop) == Piece::bishop
                        || (board[x][y] & Piece::queen) == Piece::queen)
                {
                    whiteThreatened.upLeftBox = {x, y};

                    if (defense == 0)
                    {
                        whiteThreatened.attackedInfo |= KingThreatenedInfo::upLeftThreatened;
                        whiteThreatened.threatenedInfo |= KingThreatenedInfo::upLeftThreatened;
                        whiteThreatened.amountAttacked++;
                        whiteThreatened.attackedfromBox = {x, y};
                        foundAThreat = true;
                        break;
                    }
                    else
                    {
                        whiteThreatened.threatenedInfo |= KingThreatenedInfo::upLeftThreatened;
                        foundAThreat = true;
                        break;
                    }
                }
            }
        }
        if (!foundAThreat)
        {
            whiteThreatened.threatenedInfo &= ~(1UL << 1);
        }
    }
    //Black turn.
    else
    {
        defense = 0;
        kingSquare = blackLocations.getKingLocation();
        x = kingSquare.x;
        y = kingSquare.y;
        while (--x >= 0 && --y >= 0)
        {
            if (board[x][y] == 0)
            {
                continue;
            }
            else if ((board[x][y] & Piece::black) == Piece::black
                     || ((board[x][y] & Piece::bishop) != Piece::bishop
                         && (board[x][y] & Piece::queen) != Piece::queen))
            {
                defense++;
                if (defense == 2)
                {
                    break;
                }
            }
            else
            {
                if ((board[x][y] & Piece::bishop) == Piece::bishop
                        || (board[x][y] & Piece::queen) == Piece::queen)
                {
                    blackThreatened.upLeftBox = {x, y};
                    if (defense == 0)
                    {
                        blackThreatened.attackedInfo |= KingThreatenedInfo::upLeftThreatened;
                        blackThreatened.threatenedInfo |= KingThreatenedInfo::upLeftThreatened;
                        blackThreatened.amountAttacked++;
                        blackThreatened.attackedfromBox = {x, y};
                        foundAThreat = true;
                        break;
                    }
                    else
                    {
                        blackThreatened.threatenedInfo |= KingThreatenedInfo::upLeftThreatened;
                        foundAThreat = true;
                        break;
                    }
                }
            }
        }
        if (!foundAThreat)
        {
            blackThreatened.threatenedInfo &= ~(1UL << 1);
        }
    }
}

void Board::updateUpRightThreats(char currentTurn, BoardState* currentBoardState)
{
    int defense;
    uint8_t** board = currentBoardState->getBoard();
    bool foundAThreat = false;
    Box kingSquare;
    int x, y;

    //White turn.
    if (currentTurn == 'w')
    {
        defense = 0;
        kingSquare = whiteLocations.getKingLocation();
        x = kingSquare.x;
        y = kingSquare.y;
        while (++x < boardXBoxes && --y >= 0)
        {
            if (board[x][y] == 0)
            {
                continue;
            }
            else if ((board[x][y] & Piece::white) == Piece::white
                     || ((board[x][y] & Piece::bishop) != Piece::bishop
                         && (board[x][y] & Piece::queen) != Piece::queen))
            {
                defense++;
                if (defense == 2)
                {
                    break;
                }
            }
            else
            {
                if ((board[x][y] & Piece::bishop) == Piece::bishop
                        || (board[x][y] & Piece::queen) == Piece::queen)
                {
                    whiteThreatened.upRightBox = {x, y};
                    if (defense == 0)
                    {
                        whiteThreatened.attackedInfo |= KingThreatenedInfo::upRightThreatened;
                        whiteThreatened.threatenedInfo |= KingThreatenedInfo::upRightThreatened;
                        whiteThreatened.amountAttacked++;
                        whiteThreatened.attackedfromBox = {x, y};
                        foundAThreat = true;
                        break;
                    }
                    else
                    {
                        whiteThreatened.threatenedInfo |= KingThreatenedInfo::upRightThreatened;
                        foundAThreat = true;
                        break;
                    }
                }
            }
        }
        if (!foundAThreat)
        {
            whiteThreatened.threatenedInfo &= ~(1UL << 3);
        }
        foundAThreat = false;
    }
    //Black turn.
    else
    {
        defense = 0;
        kingSquare = blackLocations.getKingLocation();
        x = kingSquare.x;
        y = kingSquare.y;
        while (++x < boardXBoxes && --y >= 0)
        {
            if (board[x][y] == 0)
            {
                continue;
            }
            else if ((board[x][y] & Piece::black) == Piece::black
                     || ((board[x][y] & Piece::bishop) != Piece::bishop
                         && (board[x][y] & Piece::queen) != Piece::queen))
            {
                defense++;
                if (defense == 2)
                {
                    break;
                }
            }
            else
            {
                if ((board[x][y] & Piece::bishop) == Piece::bishop
                        || (board[x][y] & Piece::queen) == Piece::queen)
                {
                    blackThreatened.upRightBox = {x, y};
                    if (defense == 0)
                    {
                        blackThreatened.attackedInfo |= KingThreatenedInfo::upRightThreatened;
                        blackThreatened.threatenedInfo |= KingThreatenedInfo::upRightThreatened;
                        blackThreatened.amountAttacked++;
                        blackThreatened.attackedfromBox = {x, y};
                        foundAThreat = true;
                        break;
                    }
                    else
                    {
                        blackThreatened.threatenedInfo |= KingThreatenedInfo::upRightThreatened;
                        foundAThreat = true;
                        break;
                    }
                }
            }
        }
        if (!foundAThreat)
        {
            blackThreatened.threatenedInfo &= ~(1UL << 3);
        }
    }
}

void Board::updateDownLeftThreats(char currentTurn, BoardState* currentBoardState)
{
    int defense;
    uint8_t** board = currentBoardState->getBoard();
    bool foundAThreat = false;
    Box kingSquare;
    int x, y;

    //White turn.
    if (currentTurn == 'w')
    {
        defense = 0;
        kingSquare = whiteLocations.getKingLocation();
        x = kingSquare.x;
        y = kingSquare.y;
        while (--x >= 0 && ++y < boardYBoxes)
        {
            if (board[x][y] == 0)
            {
                continue;
            }
            else if ((board[x][y] & Piece::white) == Piece::white
                     || ((board[x][y] & Piece::bishop) != Piece::bishop
                         && (board[x][y] & Piece::queen) != Piece::queen))
            {
                defense++;
                if (defense == 2)
                {
                    break;
                }
            }
            else
            {
                if ((board[x][y] & Piece::bishop) == Piece::bishop
                        || (board[x][y] & Piece::queen) == Piece::queen)
                {
                    whiteThreatened.downLeftBox = {x, y};
                    if (defense == 0)
                    {
                        whiteThreatened.attackedInfo |= KingThreatenedInfo::downLeftThreatened;
                        whiteThreatened.threatenedInfo |= KingThreatenedInfo::downLeftThreatened;
                        whiteThreatened.amountAttacked++;
                        whiteThreatened.attackedfromBox = {x, y};
                        foundAThreat = true;
                        break;
                    }
                    else
                    {
                        whiteThreatened.threatenedInfo |= KingThreatenedInfo::downLeftThreatened;
                        foundAThreat = true;
                        break;
                    }
                }
            }
        }
        if (!foundAThreat)
        {
            whiteThreatened.threatenedInfo &= ~(1UL << 7);
        }
        foundAThreat = false;
    }
    //Black turn,
    else
    {
        defense = 0;
        kingSquare = blackLocations.getKingLocation();
        x = kingSquare.x;
        y = kingSquare.y;
        while (--x >= 0 && ++y < boardYBoxes)
        {
            if (board[x][y] == 0)
            {
                continue;
            }
            else if ((board[x][y] & Piece::black) == Piece::black
                     || ((board[x][y] & Piece::bishop) != Piece::bishop
                         && (board[x][y] & Piece::queen) != Piece::queen))
            {
                defense++;
                if (defense == 2)
                {
                    break;
                }
            }
            else
            {
                if ((board[x][y] & Piece::bishop) == Piece::bishop
                        || (board[x][y] & Piece::queen) == Piece::queen)
                {

                    blackThreatened.downLeftBox = {x, y};
                    if (defense == 0)
                    {
                        blackThreatened.attackedInfo |= KingThreatenedInfo::downLeftThreatened;
                        blackThreatened.threatenedInfo |= KingThreatenedInfo::downLeftThreatened;

                        blackThreatened.amountAttacked++;
                        blackThreatened.attackedfromBox = {x, y};
                        foundAThreat = true;
                        break;
                    }
                    else
                    {
                        blackThreatened.threatenedInfo |= KingThreatenedInfo::downLeftThreatened;
                        foundAThreat = true;
                        break;
                    }
                }
            }
        }
        if (!foundAThreat)
        {
            blackThreatened.threatenedInfo &= ~(1UL << 7);
        }
    }
}

void Board::updateDownRightThreats(char currentTurn, BoardState* currentBoardState)
{
    int defense;
    uint8_t** board = currentBoardState->getBoard();
    bool foundAThreat = false;
    Box kingSquare;
    int x, y;

    //White turn.
    if (currentTurn == 'w')
    {
        defense = 0;
        kingSquare = whiteLocations.getKingLocation();
        x = kingSquare.x;
        y = kingSquare.y;
        while (++x < boardXBoxes && ++y < boardYBoxes)
        {
            if (board[x][y] == 0)
            {
                continue;
            }
            else if ((board[x][y] & Piece::white) == Piece::white
                     || ((board[x][y] & Piece::bishop) != Piece::bishop
                         && (board[x][y] & Piece::queen) != Piece::queen))
            {
                defense++;
                if (defense == 2)
                {
                    break;
                }
            }
            else
            {
                if ((board[x][y] & Piece::bishop) == Piece::bishop
                        || (board[x][y] & Piece::queen) == Piece::queen)
                {
                    whiteThreatened.downRightBox = {x, y};
                    if (defense == 0)
                    {
                        whiteThreatened.attackedInfo |= KingThreatenedInfo::downRightThreatened;
                        whiteThreatened.threatenedInfo |= KingThreatenedInfo::downRightThreatened;
                        whiteThreatened.amountAttacked++;
                        whiteThreatened.attackedfromBox = {x, y};
                        foundAThreat = true;
                        break;
                    }
                    else
                    {
                        whiteThreatened.threatenedInfo |= KingThreatenedInfo::downRightThreatened;
                        foundAThreat = true;
                        break;
                    }
                }
            }
        }
        if (!foundAThreat)
        {
            whiteThreatened.threatenedInfo &= ~(1UL << 5);
        }
        foundAThreat = false;
    }
    //Black turn.
    else
    {
        defense = 0;
        kingSquare = blackLocations.getKingLocation();
        x = kingSquare.x;
        y = kingSquare.y;
        while (++x < boardXBoxes && ++y < boardYBoxes)
        {
            if (board[x][y] == 0)
            {
                continue;
            }
            else if ((board[x][y] & Piece::black) == Piece::black
                     || ((board[x][y] & Piece::bishop) != Piece::bishop
                         && (board[x][y] & Piece::queen) != Piece::queen))
            {
                defense++;
                if (defense == 2)
                {
                    break;
                }
            }
            else
            {
                if ((board[x][y] & Piece::bishop) == Piece::bishop
                        || (board[x][y] & Piece::queen) == Piece::queen)
                {
                    blackThreatened.downRightBox = {x, y};
                    if (defense == 0)
                    {
                        blackThreatened.attackedInfo |= KingThreatenedInfo::downRightThreatened;
                        blackThreatened.threatenedInfo |= KingThreatenedInfo::downRightThreatened;
                        blackThreatened.amountAttacked++;
                        blackThreatened.attackedfromBox = {x, y};
                        foundAThreat = true;
                        break;
                    }
                    else
                    {
                        blackThreatened.threatenedInfo |= KingThreatenedInfo::downRightThreatened;
                        foundAThreat = true;
                        break;
                    }
                }
            }
        }
        if (!foundAThreat)
        {
            blackThreatened.threatenedInfo &= ~(1UL << 5);
        }
    }
}

void Board::updateAllThreats(char currentTurn, BoardState* currentBoardState)
{
    updateStraightDownThreats(currentTurn, currentBoardState);
    updateStraightUpThreats(currentTurn, currentBoardState);
    updateStraightLeftThreats(currentTurn, currentBoardState);
    updateStraightRightThreats(currentTurn, currentBoardState);
    updateUpLeftThreats(currentTurn, currentBoardState);
    updateUpRightThreats(currentTurn, currentBoardState);
    updateDownLeftThreats(currentTurn, currentBoardState);
    updateDownRightThreats(currentTurn, currentBoardState);
}

void Board::updateMoveToThreats(Move lastMove, BoardState* currentBoardState)
{
    char currentTurn = currentBoardState->getCurrentTurn();
    Box kingBox;

    if (currentTurn == 'w')
    {
        kingBox = whiteLocations.getKingLocation();
    }
    else
    {
        kingBox = blackLocations.getKingLocation();
    }

    uint8_t** board = currentBoardState->getBoard();

    if ((board[lastMove.endBox.x][lastMove.endBox.y] & Piece::king) == Piece::king)
    {
        updateAllThreats(currentTurn, currentBoardState);
    }
    else
    {
        //Now we look at where the new piece moved to.
        if (inSameRow(lastMove.endBox, kingBox))
        {
            if (lastMove.endBox.x > kingBox.x)
            {
                updateStraightRightThreats(currentTurn, currentBoardState);
            }
            else
            {
                updateStraightLeftThreats(currentTurn, currentBoardState);
            }
        }
        else if (inSameCol(lastMove.endBox, kingBox))
        {
            if (lastMove.endBox.y > kingBox.y)
            {
                updateStraightDownThreats(currentTurn, currentBoardState);
            }
            else
            {
                updateStraightUpThreats(currentTurn, currentBoardState);
            }
        }
        else if (inSameDiagonal(lastMove.endBox, kingBox))
        {

            if (lastMove.endBox.x > kingBox.x)
            {
                updateUpRightThreats(currentTurn, currentBoardState);
            }
            else
            {
                updateDownLeftThreats(currentTurn, currentBoardState);
            }
        }
        else if (inSameReverseDiagonal(lastMove.endBox, kingBox))
        {
            if (lastMove.endBox.x > kingBox.x)
            {
                updateDownRightThreats(currentTurn, currentBoardState);
            }
            else
            {
                updateUpLeftThreats(currentTurn, currentBoardState);
            }
        }
    }
}

//Update the current player king threats based on the last move from other player.
void Board::updateThreats(Move lastMove, BoardState* currentBoardState)
{
    //See where other player moved from.
    char currentTurn = currentBoardState->getCurrentTurn();
    Box kingBox;

    if (currentTurn == 'w')
    {
        kingBox = whiteLocations.getKingLocation();
        if ((currentBoardState->getBoard()[lastMove.endBox.x][lastMove.endBox.y])
                == (Piece::king | Piece::white))
        {
            updateAllThreats(currentTurn, currentBoardState);
            return;
        }
    }
    else
    {
        kingBox = blackLocations.getKingLocation();
        if ((currentBoardState->getBoard()[lastMove.endBox.x][lastMove.endBox.y])
                == (Piece::king | Piece::black))
        {
            updateAllThreats(currentTurn, currentBoardState);
            return;
        }
    }

    if (currentTurn == 'b')
    {
        if(currentBoardState->getBoard()[lastMove.endBox.x][lastMove.endBox.y]
                == (Piece::white | Piece::knight))
        {
            int xDiff = abs(lastMove.endBox.x - kingBox.x);
            int yDiff = abs(lastMove.endBox.y - kingBox.y);
            int sumOfLeftAndRight = xDiff + yDiff;

            if (sumOfLeftAndRight == 3 && xDiff != 0 && yDiff != 0)
            {
                blackThreatened.amountAttacked++;
                blackThreatened.attackedByKnight = true;
                blackThreatened.attackedfromBox = {lastMove.endBox.x, lastMove.endBox.y};
            }
        }
        else if (currentBoardState->getBoard()[lastMove.endBox.x][lastMove.endBox.y]
                 == (Piece::white | Piece::pawn))
        {
            if (lastMove.endBox.x == kingBox.x + 1)
            {
                //If it's in one of the diagonal rows
                if (lastMove.endBox.y == kingBox.y + 1)
                {
                    blackThreatened.amountAttacked++;
                    blackThreatened.threatenedInfo |= KingThreatenedInfo::downRightThreatened;
                    blackThreatened.attackedInfo |= KingThreatenedInfo::downRightThreatened;
                    blackThreatened.attackedfromBox = {lastMove.endBox.x, lastMove.endBox.y};
                }
            }
            else if (lastMove.endBox.x == kingBox.x - 1)
            {
                if (lastMove.endBox.y == kingBox.y + 1)
                {
                    blackThreatened.amountAttacked++;
                    blackThreatened.threatenedInfo |= KingThreatenedInfo::downLeftThreatened;
                    blackThreatened.attackedInfo |= KingThreatenedInfo::downLeftThreatened;
                    blackThreatened.attackedfromBox = {lastMove.endBox.x, lastMove.endBox.y};
                }
            }
        }
    }
    else
    {
        if (currentBoardState->getBoard()[lastMove.endBox.x][lastMove.endBox.y]
                == (Piece::black | Piece::knight))
        {
            int xDiff = abs(lastMove.endBox.x - kingBox.x);
            int yDiff = abs(lastMove.endBox.y - kingBox.y);
            int sumOfLeftAndRight = xDiff + yDiff;

            if (sumOfLeftAndRight == 3 && xDiff != 0 && yDiff != 0)
            {
                whiteThreatened.amountAttacked++;
                whiteThreatened.attackedByKnight = true;
                whiteThreatened.attackedfromBox = {lastMove.endBox.x, lastMove.endBox.y};
            }
        }
        else if (currentBoardState->getBoard()[lastMove.endBox.x][lastMove.endBox.y]
                 == (Piece::black | Piece::pawn))
        {
            if (lastMove.endBox.x == kingBox.x + 1)
            {
                //If it's in one of the diagonal rows
                if (lastMove.endBox.y == kingBox.y - 1)
                {
                    whiteThreatened.amountAttacked++;
                    whiteThreatened.threatenedInfo |= KingThreatenedInfo::upRightThreatened;
                    whiteThreatened.attackedInfo |= KingThreatenedInfo::upRightThreatened;
                    whiteThreatened.attackedfromBox = {lastMove.endBox.x, lastMove.endBox.y};
                }
            }
            else if (lastMove.endBox.x == kingBox.x - 1)
            {
                if (lastMove.endBox.y == kingBox.y - 1)
                {
                    whiteThreatened.amountAttacked++;
                    whiteThreatened.threatenedInfo |= KingThreatenedInfo::upLeftThreatened;
                    whiteThreatened.attackedInfo |= KingThreatenedInfo::upLeftThreatened;
                    whiteThreatened.attackedfromBox = {lastMove.endBox.x, lastMove.endBox.y};
                }
            }
        }
    }

    //If we castled we update the rook.
    if (lastMove.kingSideCastle || lastMove.queenSideCastle)
    {
        updateStraightDownThreats(currentTurn, currentBoardState);
        updateStraightUpThreats(currentTurn, currentBoardState);
        updateStraightLeftThreats(currentTurn, currentBoardState);
        updateStraightRightThreats(currentTurn, currentBoardState);
    }
    if (inSameRow(kingBox, lastMove.endBox))
    {
        if (kingBox.x < lastMove.endBox.x)
        {
            updateStraightRightThreats(currentTurn, currentBoardState);
        }
        else
        {
            updateStraightLeftThreats(currentTurn, currentBoardState);
        }
    }
    else if (inSameCol(kingBox, lastMove.endBox))
    {
        if (kingBox.y < lastMove.endBox.y)
        {
            updateStraightDownThreats(currentTurn, currentBoardState);
        }
        else
        {
            updateStraightUpThreats(currentTurn, currentBoardState);
        }
    }
    else if (inSameDiagonal(kingBox, lastMove.endBox))
    {

        if (kingBox.x < lastMove.endBox.x)
        {
            updateUpRightThreats(currentTurn, currentBoardState);
        }
        else
        {
            updateDownLeftThreats(currentTurn, currentBoardState);
        }
    }
    else if (inSameReverseDiagonal(kingBox, lastMove.endBox))
    {

        if (kingBox.x < lastMove.endBox.x)
        {
            updateDownRightThreats(currentTurn, currentBoardState);
        }
        else
        {
            updateUpLeftThreats(currentTurn, currentBoardState);
        }
    }

    //Now we look at where the new piece moved to.
    if (inSameRow(lastMove.startBox, kingBox) && !inSameRow(lastMove.startBox, lastMove.endBox))
    {

        if (lastMove.startBox.x > kingBox.x)
        {
            updateStraightRightThreats(currentTurn, currentBoardState);
        }
        else
        {
            updateStraightLeftThreats(currentTurn, currentBoardState);
        }

    }
    else if (inSameCol(lastMove.startBox, kingBox) && !inSameCol(lastMove.startBox, lastMove.endBox))
    {

        if (lastMove.startBox.y > kingBox.y)
        {
            updateStraightDownThreats(currentTurn, currentBoardState);
        }
        else
        {
            updateStraightUpThreats(currentTurn, currentBoardState);
        }
    }
    else if (inSameDiagonal(lastMove.startBox, kingBox)
             && !inSameDiagonal(lastMove.startBox, lastMove.endBox))
    {
        if (lastMove.startBox.x > kingBox.x)
        {
            updateUpRightThreats(currentTurn, currentBoardState);
        }
        else
        {
            updateDownLeftThreats(currentTurn, currentBoardState);
        }

    }
    else if (inSameReverseDiagonal(lastMove.startBox, kingBox)
             && !inSameReverseDiagonal(lastMove.startBox, lastMove.endBox))
    {
        if (lastMove.startBox.x > kingBox.x)
        {
            updateDownRightThreats(currentTurn, currentBoardState);
        }
        else
        {
            updateUpLeftThreats(currentTurn, currentBoardState);
        }
    }
}

//Update en passant.
void Board::updateEnPassant(Box startBox, Box endBox, BoardState* currentBoardState)
{
    if ((currentBoardState->getBoard()[startBox.x][startBox.y] & Piece::pawn) == Piece::pawn)
    {

        if (abs(startBox.y - endBox.y) == 2)
        {
            currentBoardState->setEnPassantX(startBox.x);
            currentBoardState->setEnPassantY((startBox.y + endBox.y) / 2);
        }
        else
        {
            currentBoardState->setEnPassantX(-1);
            currentBoardState->setEnPassantY(-1);
        }
    }
    else
    {
        currentBoardState->setEnPassantX(-1);
        currentBoardState->setEnPassantY(-1);
        currentBoardState->setEnPassantY(-1);
    }
}

//Update castling.
void Board::updateCastling(Box startBox, Box endBox, BoardState* currentBoardState)
{
    //If the king moved.
    int queenSideX = 0;
    int kingSideX = boardXBoxes - 1;
    int whiteY = boardYBoxes - 1;
    int blackY = 0;

    uint8_t** board = boardState->getBoard();

    if (currentBoardState->getCurrentTurn() == 'w')
    {
        if ((board[startBox.x][startBox.y] & Piece::king) == Piece::king)
        {
            currentBoardState->setWhiteCanKingsideCastle(false);
            currentBoardState->setWhiteCanQueensideCastle(false);
        }
        if (startBox.x == queenSideX && startBox.y == whiteY)
        {
            currentBoardState->setWhiteCanQueensideCastle(false);

        }
        else if (startBox.x == kingSideX && startBox.y == whiteY)
        {
            currentBoardState->setWhiteCanKingsideCastle(false);
        }
        if (endBox.x == kingSideX && endBox.y == blackY)
        {
            currentBoardState->setBlackCanKingsideCastle(false);
        }
        else if (endBox.x == queenSideX && endBox.y == blackY)
        {
            currentBoardState->setBlackCanQueensideCastle(false);
        }
    }
    else
    {
        if ((board[startBox.x][startBox.y] & Piece::king) == Piece::king)
        {
            currentBoardState->setBlackCanKingsideCastle(false);
            currentBoardState->setBlackCanQueensideCastle(false);
        }
        if (startBox.x == queenSideX && startBox.y == blackY)
        {
            currentBoardState->setBlackCanQueensideCastle(false);
        }
        else if (startBox.x == kingSideX && startBox.y == blackY)
        {
            currentBoardState->setBlackCanKingsideCastle(false);
        }
        //If the move take a rook.
        if (endBox.x == kingSideX && endBox.y == whiteY)
        {
            currentBoardState->setWhiteCanKingsideCastle(false);
        }
        else if (endBox.x == queenSideX && endBox.y == whiteY)
        {
            currentBoardState->setWhiteCanQueensideCastle(false);
        }
    }
}

//Update highlight king box.
void Board::updateHighlightKingBox()
{
    if (kingInCheck(boardState))
    {
        findKingLocation(&highlightKingBox.x, &highlightKingBox.y, boardState);
    }
    else
    {
        highlightKingBox.x = highlightKingBox.y = -1;
    }

    if (gameOver)
    {
        switchTurns(boardState);
        findKingLocation(&winnerKing.x, &winnerKing.y, boardState);
    }
}
//-------------------------------------------------------------//

//CHECKING.
//Check position.
bool Board::inSameRow(Box box1, Box box2)
{
    return box1.y == box2.y;
}

bool Board::inSameCol(Box box1, Box box2)
{
    return box1.x == box2.x;
}

bool Board::inSameDiagonal(Box box1, Box box2)
{
    int increase = 1;
    if (box1.x < box2.x)
    {
        while (box1.x + increase <= box2.x)
        {
            if (box1.x + increase == box2.x && box1.y - increase == box2.y)
            {
                return true;
            }
            ++increase;
        }
        return false;
    }
    else
    {
        while (box2.x + increase <= box1.x)
        {
            if (box2.x + increase == box1.x && box2.y - increase == box1.y)
            {
                return true;
            }
            ++increase;
        }
        return false;
    }
}

bool Board::inSameReverseDiagonal(Box box1, Box box2)
{
    int increase = 1;
    if (box1.x < box2.x)
    {
        while (box1.x + increase <= box2.x)
        {
            if (box1.x + increase == box2.x && box1.y + increase == box2.y)
            {
                return true;
            }
            ++increase;
        }
        return false;
    }
    else
    {
        while (box2.x + increase <= box1.x)
        {
            if (box2.x + increase == box1.x && box2.y + increase == box1.y)
            {
                return true;
            }
            ++increase;
        }
        return false;
    }
}

//Check piece.
bool Board::pieceIsCurrentPlayersPiece(int x, int y, BoardState* currentBoardState)
{
    char currentTurn = currentBoardState->getCurrentTurn();
    uint8_t** board = currentBoardState->getBoard();
    if ((currentTurn == 'w' && (board[x][y] & Piece::white) == Piece::white)
            || (currentTurn == 'b' && (board[x][y] & Piece::black) == Piece::black))
    {
        return true;
    }
    return false;
}

//Check en passant.
bool Board::isEnPassant(Box startBox, Box endBox, BoardState* currentBoardState)
{
    if ((currentBoardState->getBoard()[startBox.x][startBox.y] & Piece::pawn) == Piece::pawn)
    {
        if (endBox.x == currentBoardState->getEnPassantX()
                && endBox.y == currentBoardState->getEnPassantY())
        {
            return true;
        }
    }
    return false;
}

bool Board::canEnPassant(Box pawnBox, Box endBox, BoardState* currentBoardState)
{
    char currentTurn = currentBoardState->getCurrentTurn();
    Box kingBox;
    uint8_t enemyPiece;
    uint8_t** board = currentBoardState->getBoard();

    if (currentTurn == 'w')
    {
        kingBox = whiteLocations.getKingLocation();
        enemyPiece = Piece::black;
    }
    else
    {
        kingBox = blackLocations.getKingLocation();
        enemyPiece = Piece::white;
    }

    if (inSameRow(kingBox, {pawnBox.x, pawnBox.y}))
    {
        //If the pawn is on the right side of the king.
        if (pawnBox.x > kingBox.x)
        {
            //En passant to the right.
            if (endBox.x > pawnBox.x)
            {
                for (int i = pawnBox.x + 2; i < boardXBoxes; ++i)
                {
                    if (board[i][pawnBox.y] == 0)
                    {
                        continue;
                    }
                    else if ((board[i][pawnBox.y] == (enemyPiece | Piece::rook))
                             || (board[i][pawnBox.y] == (enemyPiece | Piece::queen)))
                    {
                        for (int j = pawnBox.x - 1; j > kingBox.x; --j)
                        {
                            if (board[j][pawnBox.y] != 0)
                            {
                                return true;
                            }
                        }
                        return false;
                    }
                    else
                    {
                        break;
                    }
                }
            }
            //En passant to the left
            //but we still search from the right side.
            else
            {
                for (int i = pawnBox.x +1; i < boardXBoxes; ++i)
                {
                    if (board[i][pawnBox.y] == 0)
                    {
                        continue;
                    }
                    else if ((board[i][pawnBox.y] == (enemyPiece | Piece::rook))
                             || (board[i][pawnBox.y] == (enemyPiece | Piece::queen)))
                    {
                        for (int j = pawnBox.x - 2; j > kingBox.x; --j)
                        {
                            if (board[j][pawnBox.y] != 0)
                            {
                                return true;
                            }
                        }
                        return false;
                    }
                    else
                    {
                        break;
                    }
                }
            }

        }
        //If the pawn is on the left side of the king.
        else
        {
            //En passant to right.
            if (endBox.x > pawnBox.x)
            {
                for (int i = pawnBox.x - 1; i >=0; --i)
                {
                    if (board[i][pawnBox.y] == 0)
                    {
                        continue;
                    }
                    else if ((board[i][pawnBox.y] == (enemyPiece | Piece::rook))
                             || (board[i][pawnBox.y] == (enemyPiece | Piece::queen)))
                    {
                        for (int j = pawnBox.x + 2; j < kingBox.x; j++)
                        {
                            if (board[j][pawnBox.y] != 0)
                            {
                                return true;
                            }
                        }
                        return false;
                    }
                    else
                    {
                        break;
                    }
                }
            }
            //En passant to the left.
            else
            {
                for (int i = pawnBox.x - 2; i >=0; --i)
                {
                    if (board[i][pawnBox.y] == 0)
                    {
                        continue;
                    }
                    else if ((board[i][pawnBox.y] == (enemyPiece | Piece::rook))
                             || (board[i][pawnBox.y] == (enemyPiece | Piece::queen)))
                    {
                        for (int j = pawnBox.x + 1; j < kingBox.x; j++)
                        {
                            if (board[j][pawnBox.y] != 0)
                            {
                                return true;
                            }
                        }
                        return false;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }

    }
    return true;
}

//Check legal.
bool Board::inPseudoMoves(struct Move& newMove)
{
    for (unsigned i = 0; i < pseudoLegalMoves.size(); i++)
    {
        if (newMove.startBox.x == pseudoLegalMoves.at(i).startBox.y &&
                newMove.startBox.y == pseudoLegalMoves.at(i).startBox.y &&
                newMove.endBox.x == pseudoLegalMoves.at(i).endBox.x &&
                newMove.endBox.y == pseudoLegalMoves.at(i).endBox.y)
        {
            newMove.kingSideCastle = pseudoLegalMoves.at(i).kingSideCastle;
            newMove.queenSideCastle = pseudoLegalMoves.at(i).queenSideCastle;
            newMove.enPassant = pseudoLegalMoves.at(i).enPassant;
            newMove.isPromotion = pseudoLegalMoves.at(i).isPromotion;
            newMove.promotionType = pseudoLegalMoves.at(i).promotionType;

            return true;
        }
    }
    return false;
}

bool Board::inLegalMoves(struct Move& newMove)
{
    for (unsigned i = 0; i < legalMoves.size(); i++)
    {
        if ( newMove.startBox.x == legalMoves.at(i).startBox.x &&
                newMove.startBox.y == legalMoves.at(i).startBox.y &&
                newMove.endBox.x == legalMoves.at(i).endBox.x &&
                newMove.endBox.y == legalMoves.at(i).endBox.y
           )
        {
            newMove.kingSideCastle = legalMoves.at(i).kingSideCastle;
            newMove.queenSideCastle = legalMoves.at(i).queenSideCastle;
            newMove.enPassant = legalMoves.at(i).enPassant;
            newMove.isPromotion = legalMoves.at(i).isPromotion;
            newMove.promotionType = legalMoves.at(i).promotionType;

            return true;
        }
    }
    return false;
}

//Check and checkmate.
bool Board::kingInCheck(BoardState* currentBoardState)
{
    //If the current player king is under attack then the king is in check.
    int kingX, kingY;
    findKingLocation(&kingX, &kingY, currentBoardState);

    bool result = squareAttacked({kingX, kingY}, currentBoardState);

    return result;
}

bool Board::kingAttacked(BoardState* currentBoardState)
{
    char currentTurn = currentBoardState->getCurrentTurn();
    if (currentTurn == 'w')
    {
        return whiteThreatened.amountAttacked >= 1;
    }
    return blackThreatened.amountAttacked >= 1;
}

//Check attacked.
bool Board::squareAttacked(Box box, BoardState* currentBoardState)
{
    int x = box.x;
    int y = box.y;
    char currentTurn = currentBoardState->getCurrentTurn();
    uint8_t** board = currentBoardState->getBoard();

    //Pawns attack differently from their moves.
    //Black turn.
    if (currentTurn == 'b')
    {
        if (y + 1 < boardYBoxes)
        {
            if (x - 1 >= 0)
            {
                if ((board[x - 1][y + 1]) == (Piece::pawn | Piece::white))
                {
                    return true;
                }
            }
            if (x + 1 < boardXBoxes)
            {
                if (board[x + 1][y + 1] == (Piece::pawn | Piece::white))
                {
                    return true;
                }
            }
        }
    }
    //White turn.
    else
    {
        if (y - 1 >= 0)
        {
            if (x - 1 >= 0)
            {
                if (board[x - 1][y - 1] == (Piece::pawn | Piece::black))
                {
                    return true;
                }
            }
            if (x + 1 < boardXBoxes)
            {
                if (board[x + 1][y - 1] == (Piece::pawn | Piece::black))
                {
                    return true;
                }
            }
        }
    }

    uint8_t yourColor;
    uint8_t enemyColor;

    if (currentTurn == 'w')
    {
        enemyColor = Piece::black;
        yourColor = Piece::white;
    }
    else
    {
        enemyColor = Piece::white;
        yourColor = Piece::black;
    }

    //Going to the right.
    if (x + 1 < boardXBoxes)
    {
        if (board[x + 1][y] == (enemyColor | Piece::king))
        {
            return true;
        }
    }
    for (int currX = x + 1; currX < boardXBoxes; ++currX)
    {
        if (board[currX][y] == 0)
        {
            continue;
        }
        //If it's an enemy piece.
        else if (board[currX][y] == (enemyColor | Piece::queen)
                 || board[currX][y] == (enemyColor | Piece::rook))
        {
            return true;
        }
        //If it's your piece.
        else
        {
            if ((board[currX][y] ) != (Piece::king | yourColor))
            {
                break;
            }
        }
    }

    //Going to the left.
    if (x - 1 >= 0)
    {
        if (board[x - 1][y] == (enemyColor | Piece::king))
        {
            return true;
        }
    }
    for (int currX = x - 1; currX >= 0; --currX)
    {
        if (board[currX][y] == 0)
        {
            continue;
        }
        //If it's an enemy piece.
        else if (board[currX][y] == (enemyColor | Piece::queen)
                 || board[currX][y] == (enemyColor | Piece::rook))
        {
            return true;
        }
        //If it's your piece.
        else
        {
            if (board[currX][y] != (Piece::king | yourColor))
            {
                break;
            }

        }
    }

    //Going down.
    if (y - 1 >= 0)
    {
        if (board[x][y - 1] == (enemyColor | Piece::king))
        {
            return true;
        }
    }
    for (int currY = y - 1; currY >= 0; --currY)
    {
        if (board[x][currY] == 0)
        {
            continue;
        }
        //If it's an enemy piece.
        else if (board[x][currY] == (enemyColor | Piece::queen)
                 || board[x][currY] == (enemyColor | Piece::rook))
        {
            return true;
        }
        //If it's your piece.
        else
        {
            if (board[x][currY] != (Piece::king | yourColor))
            {
                break;
            }
        }
    }

    //Going up.
    if (y + 1 < boardYBoxes)
    {
        if (board[x][y + 1] == (enemyColor | Piece::king))
        {
            return true;
        }
    }
    for (int currY = y + 1; currY < boardYBoxes; ++currY)
    {
        if (board[x][currY] == 0)
        {
            continue;
        }
        //If it's an enemy piece.
        else if (board[x][currY] == (enemyColor | Piece::queen)
                 || board[x][currY] == (enemyColor | Piece::rook))
        {
            return true;
        }
        //If it's your piece.
        else
        {
            if (board[x][currY] != (Piece::king | yourColor))
            {
                break;
            }
        }
    }

    int increase = 1;

    //Going up right.
    if (x + increase < boardXBoxes && y + increase < boardYBoxes)
    {
        if (board[x + increase][y + increase] == (enemyColor | Piece::king))
        {
            return true;
        }
    }
    while (x + increase < boardXBoxes && y + increase < boardYBoxes)
    {
        if (board[x + increase][y + increase] == 0)
        {
            ++increase;
            continue;
        }
        //If it's an enemy piece.
        else if (board[x + increase][y + increase] == (enemyColor | Piece::queen)
                 || board[x + increase][y + increase] == (enemyColor | Piece::bishop))
        {
            return true;
        }
        //If it's your piece.
        else
        {
            if (board[x+increase][y+increase] != (Piece::king | yourColor))
            {
                break;
            }
        }
        ++increase;
    }
    increase = 1;

    //Going up left.
    if (x - increase >= 0 && y + increase < boardYBoxes)
    {
        if (board[x - increase][y + increase] == (enemyColor | Piece::king))
        {
            return true;
        }
    }
    while (x - increase >= 0 && y + increase < boardYBoxes)
    {
        if (board[x - increase][y + increase] == 0)
        {
            ++increase;
            continue;
        }
        //If it's an enemy piece.
        else if (board[x - increase][y + increase] == (enemyColor | Piece::queen)
                 || board[x - increase][y + increase] == (enemyColor | Piece::bishop))
        {
            return true;
        }
        //If it's your piece.
        else
        {
            if (board[x - increase][y + increase] != (Piece::king | yourColor))
            {
                break;
            }
        }
        ++increase;
    }
    increase = 1;

    //Going down left.
    if (x - increase >= 0 && y - increase >= 0)
    {
        if (board[x - increase][y - increase] == (enemyColor | Piece::king))
        {
            return true;
        }
    }
    while (x - increase >= 0 && y - increase >= 0)
    {
        if (board[x - increase][y - increase] == 0)
        {
            ++increase;
            continue;
        }
        //If it's an enemy piece.
        else if (board[x - increase][y - increase] == (enemyColor | Piece::queen)
                 || board[x - increase][y - increase] == (enemyColor | Piece::bishop))
        {
            return true;
        }
        //If it's your piece.
        else
        {
            if (board[x - increase][y - increase] != (Piece::king | yourColor))
            {
                break;
            }
        }
        ++increase;
    }
    increase = 1;

    //Going down right.
    if (x + increase < boardXBoxes && y - increase >= 0)
    {
        if (board[x + increase][y - increase] == (enemyColor | Piece::king))
        {
            return true;
        }
    }
    while (x + increase < boardXBoxes && y - increase >= 0)
    {
        if (board[x + increase][y - increase] == 0)
        {
            ++increase;
            continue;
        }
        //If it's an enemy piece.
        else if (board[x + increase][y - increase] == (enemyColor | Piece::queen)
                 || board[x + increase][y - increase] == (enemyColor | Piece::bishop))
        {
            return true;
        }
        //If it's your piece.
        else
        {
            if (board[x + increase][y - increase] != (Piece::king | yourColor))
            {
                break;
            }
        }
        ++increase;
    }

    //Check if there are any knights attacking the square.
    //White turn.
    if (currentTurn == 'w')
    {
        for (unsigned i = 0; i < blackLocations.getKnightLocations().size(); i++)
        {
            int xDiff = abs(blackLocations.getKnightLocations().at(i).x - box.x);
            int yDiff = abs(blackLocations.getKnightLocations().at(i).y - box.y);
            int sumOfLeftAndRight = xDiff + yDiff;

            if (sumOfLeftAndRight == 3 && xDiff != 0 && yDiff != 0)
            {
                return true;
            }
        }
    }
    //Black turn.
    else
    {
        for (unsigned i = 0; i < whiteLocations.getKnightLocations().size(); i++)
        {
            int xDiff = abs(whiteLocations.getKnightLocations().at(i).x - box.x);
            int yDiff = abs(whiteLocations.getKnightLocations().at(i).y - box.y);
            int sumOfLeftAndRight = xDiff + yDiff;

            if (sumOfLeftAndRight == 3 && xDiff != 0 && yDiff != 0)
            {
                return true;
            }
        }
    }

    return false;
}

bool Board::doesBoxBlockAttack(Box box, BoardState* currentBoardState)
{
    char currentTurn = currentBoardState->getCurrentTurn();
    uint8_t attackedInfo;
    Box attackedfromBox;
    Box kingBox;
    bool attackedByKnight;

    //White turn.
    if (currentTurn == 'w')
    {
        attackedInfo = whiteThreatened.attackedInfo;
        attackedfromBox = {whiteThreatened.attackedfromBox.x, whiteThreatened.attackedfromBox.y};
        attackedByKnight = whiteThreatened.attackedByKnight;
        kingBox = {whiteLocations.getKingLocation().x, whiteLocations.getKingLocation().y};
    }
    //Black turn.
    else
    {
        attackedInfo = blackThreatened.attackedInfo;
        attackedfromBox = {blackThreatened.attackedfromBox.x, blackThreatened.attackedfromBox.y};
        attackedByKnight = blackThreatened.attackedByKnight;
        kingBox = {blackLocations.getKingLocation().x, blackLocations.getKingLocation().y};
    }

    //When you're attacked by a knight the only way to block it with a piece is to capture it.
    if (attackedByKnight)
    {
        return box.x == attackedfromBox.x && box.y == attackedfromBox.y;
    }
    if (box.x == attackedfromBox.x && box.y == attackedfromBox.y)
    {
        return true;
    }

    switch (attackedInfo)
    {
    case KingThreatenedInfo::straightDownThreatened:
        return box.x == attackedfromBox.x && box.y < attackedfromBox.y && box.y > kingBox.y;
        break;
    case KingThreatenedInfo::straightUpThreatened:
        return box.x == attackedfromBox.x && box.y > attackedfromBox.y && box.y < kingBox.y;
        break;
    case KingThreatenedInfo::straightLeftThreatened:
        return box.y == attackedfromBox.y && box.x > attackedfromBox.x && box.x < kingBox.x;
        break;
    case KingThreatenedInfo::straightRightThreatened:
        return box.y == attackedfromBox.y && box.x < attackedfromBox.x && box.x > kingBox.x;
        break;
    case KingThreatenedInfo::upLeftThreatened:
        if (inSameReverseDiagonal(box, kingBox))
        {
            if (box.x < kingBox.x && box.x > attackedfromBox.x)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
        break;
    case KingThreatenedInfo::upRightThreatened:
        if (inSameDiagonal(box, kingBox))
        {
            return box.x > kingBox.x && box.x < attackedfromBox.x;
        }
        break;
    case KingThreatenedInfo::downLeftThreatened:
        if (inSameDiagonal(box, kingBox))
        {
            return box.x < kingBox.x && box.x > attackedfromBox.x;
        }
        break;
    case KingThreatenedInfo::downRightThreatened:
        if (inSameReverseDiagonal(box, kingBox))
        {
            return box.x > kingBox.x && box.x < attackedfromBox.x;
        }
        return false;
        break;
    default:
        std::cout << "Error in switch" << std::endl;
        break;
    }
}
//-------------------------------------------------------------//

//MOVING PIECES.
//Pick up.
void Board::attemptPickupPiece(int x, int y, BoardState* currentBoardState)
{
    //Check if there's a piece on the box we're clicking.
    if (currentBoardState->getBoard()[x][y] != 0)
    {
        //Check if it's the piece for the current player turn.
        if (pieceIsCurrentPlayersPiece(x, y, currentBoardState))
        {
            draggingPiece = true;
            draggingPieceBox.x = x;
            draggingPieceBox.y = y;
            draggingPieceTexture = getTextureAtLocation(x, y, currentBoardState);
            createHighlightMoves(x, y);

            //White piece.
            if (currentBoardState->getCurrentTurn() == 'w')
            {
                whiteLocations.removePiece(currentBoardState->getBoard()[x][y], { x,y });
            }
            //Black piece.
            else
            {
                blackLocations.removePiece(currentBoardState->getBoard()[x][y], { x,y });
            }
        }
    }
}

//Place down.
void Board::attemptPlacePiece(int x, int y, BoardState* currentBoardState)
{
    Move newMove = {draggingPieceBox.x, draggingPieceBox.y, x, y};
    uint8_t** board = currentBoardState->getBoard();

    //Check if the move was legal.
    if (inLegalMoves(newMove))
    {
        if (currentBoardState->getCurrentTurn() == 'w')
        {
            whiteLocations.addPiece(board[draggingPieceBox.x][draggingPieceBox.y], draggingPieceBox);
        }
        else
        {
            blackLocations.addPiece(board[draggingPieceBox.x][draggingPieceBox.y], draggingPieceBox);
        }
        if (newMove.isPromotion)
        {
            promotionMove =
            {
                {
                    newMove.startBox.x,
                    newMove.startBox.y
                },
                {
                    newMove.endBox.x,
                    newMove.endBox.y
                },
                false,
                false,
                false,
                true,
                newMove.promotionType
            };

            waitingForPromotionChoice = true;
        }
        else
        {
            makeMove(newMove, currentBoardState);
            nextTurn(currentBoardState);
        }
        stopDraggingPiece();
    }
    else
    {
        if (currentBoardState->getCurrentTurn() == 'w')
        {
            whiteLocations.addPiece(board[draggingPieceBox.x][draggingPieceBox.y], draggingPieceBox);
        }
        else
        {
            blackLocations.addPiece(board[draggingPieceBox.x][draggingPieceBox.y], draggingPieceBox);
        }
        stopDraggingPiece();
    }
}

//Stop dragging.
void Board::stopDraggingPiece()
{
    draggingPiece = false;
    draggingPieceBox.x = draggingPieceBox.y = -1;
}
//-------------------------------------------------------------//

//ADD MOVES.
void Board::attemptAddMove(Move move, BoardState* currentBoardState, std::vector<Move>& moves)
{
    char currentTurn = currentBoardState->getCurrentTurn();

    //White turn.
    if (currentTurn == 'w')
    {
        if (whiteThreatened.amountAttacked >= 1)
        {
            if (doesBoxBlockAttack(move.endBox, currentBoardState))
            {
                moves.push_back({{move.startBox.x, move.startBox.y}, {move.endBox.x, move.endBox.y},
                    move.kingSideCastle, move.queenSideCastle,
                    move.enPassant, move.isPromotion, move.promotionType});
            }
        }
        else
        {
            moves.push_back({{move.startBox.x, move.startBox.y}, {move.endBox.x, move.endBox.y},
                move.kingSideCastle, move.queenSideCastle,
                move.enPassant, move.isPromotion, move.promotionType});
        }
    }
    //Black turn.
    else
    {
        if (blackThreatened.amountAttacked >= 1)
        {
            if (doesBoxBlockAttack(move.endBox, currentBoardState))
            {
                moves.push_back({{move.startBox.x, move.startBox.y}, {move.endBox.x, move.endBox.y},
                    move.kingSideCastle, move.queenSideCastle,
                    move.enPassant, move.isPromotion, move.promotionType});
            }
        }
        else
        {
            moves.push_back({{move.startBox.x, move.startBox.y}, {move.endBox.x, move.endBox.y},
                move.kingSideCastle, move.queenSideCastle,
                move.enPassant, move.isPromotion, move.promotionType});
        }
    }
}

//Pawn moves.
void Board::addStraightUpPawnMoves(Box box, BoardState* currentBoardState, std::vector<Move>& moves)
{
    int x = box.x;
    int y = box.y;

    if (y - 1 >= 0)
    {
        if (currentBoardState->getBoard()[x][y - 1] == 0)
        {
            //If it's a promotion.
            if (y - 1 == 0)
            {
                attemptAddMove({{x, y}, {x, y - 1}, false, false, false, true, 'q'},
                currentBoardState, moves);
                attemptAddMove({{x, y}, {x, y - 1}, false, false, false, true, 'r'},
                currentBoardState, moves);
                attemptAddMove({{x, y}, {x, y - 1}, false, false, false, true, 'n'},
                currentBoardState, moves);
                attemptAddMove({{x, y}, {x, y - 1}, false, false, false, true, 'b'},
                currentBoardState, moves);
            }
            else
            {
                attemptAddMove({{x, y}, {x, y - 1}, false, false, false, false, ' '},
                currentBoardState, moves);
            }

            //We only can move forward 2 if the space is open.
            //If it's in the starting position.
            if (y == boardYBoxes - 2)
            {
                if (y - 2 >= 0)
                {
                    if (currentBoardState->getBoard()[x][y - 2] == 0)
                    {
                        attemptAddMove({{x, y}, {x, y - 2}, false, false, false, false, ' '},
                        currentBoardState, moves);
                    }
                }
            }
        }
    }
}

void Board::addStraightDownPawnMoves(Box box, BoardState* currentBoardState, std::vector<Move>& moves)
{
    int x = box.x;
    int y = box.y;

    if (y + 1 < boardYBoxes)
    {
        if (currentBoardState->getBoard()[x][y + 1] == 0)
        {
            if (y + 1 == boardYBoxes - 1)
            {
                attemptAddMove({{x, y}, {x, y + 1}, false, false, false, true, 'r'},
                currentBoardState, moves);
                attemptAddMove({{x, y}, {x, y + 1}, false, false, false, true, 'n'},
                currentBoardState, moves);
                attemptAddMove({{x, y}, {x, y + 1}, false, false, false, true, 'b'},
                currentBoardState, moves);
                attemptAddMove({{x, y}, {x, y + 1}, false, false, false, true, 'q'},
                currentBoardState, moves);
            }
            else
            {
                attemptAddMove({{x, y}, {x, y + 1}, false, false, false, false, ' '},
                currentBoardState, moves);
            }

            //We only can move forward 2 if the space is open.
            //If it's in the starting position.
            if (y == 1)
            {
                if (y + 2 < boardYBoxes)
                {
                    if (currentBoardState->getBoard()[x][y + 2] == 0)
                    {
                        attemptAddMove({{x, y}, {x, y + 2}, false, false, false, false, ' '},
                        currentBoardState, moves);
                    }
                }
            }
        }
    }
}

void Board::addDownLeftPawnMoves(Box box, BoardState* currentBoardState, std::vector<Move>& moves)
{
    int x = box.x;
    int y = box.y;

    if (y + 1 < boardYBoxes)
    {
        //If we're not at the left edge of the board.
        if (x - 1 >= 0)
        {
            if (currentBoardState->getBoard()[x - 1][y + 1] != 0
                    && !pieceIsCurrentPlayersPiece(x - 1, y + 1, currentBoardState))
            {
                //Promotion.
                if (y + 1 == boardYBoxes - 1)
                {
                    attemptAddMove({{x, y}, {x - 1, y + 1}, false, false, false, true, 'q'},
                    currentBoardState, moves);
                    attemptAddMove({{x, y}, {x - 1, y + 1}, false, false, false, true, 'r'},
                    currentBoardState, moves);
                    attemptAddMove({{x, y}, {x - 1, y + 1}, false, false, false, true, 'b'},
                    currentBoardState, moves);
                    attemptAddMove({{x, y}, {x - 1, y + 1}, false, false, false, true, 'n'},
                    currentBoardState, moves);
                }
                else
                {
                    attemptAddMove({{x, y}, {x - 1, y + 1}, false, false, false, false, ' '},
                    currentBoardState, moves);
                }
            }
            else if (x - 1 == currentBoardState->getEnPassantX()
                     && y + 1 == currentBoardState->getEnPassantY())
            {
                if (canEnPassant({x, y}, {x - 1, y + 1}, currentBoardState))
                {
                    attemptAddMove({{x, y}, {x - 1, y + 1}, false, false, true, false, ' '},
                    currentBoardState, moves);
                }
            }
        }
    }
}

void Board::addDownRightPawnMoves(Box box, BoardState* currentBoardState, std::vector<Move>& moves)
{
    int x = box.x;
    int y = box.y;

    if (y + 1 < boardYBoxes)
    {
        //If we're not at the right edge of the board.
        if (x + 1 < boardXBoxes)
        {
            if (currentBoardState->getBoard()[x + 1][y + 1] != 0
                    && !pieceIsCurrentPlayersPiece(x + 1, y + 1, currentBoardState))
            {
                if (y + 1 == boardYBoxes - 1)
                {
                    attemptAddMove({{x, y}, {x + 1, y + 1}, false, false, false, true, 'r'},
                    currentBoardState, moves);
                    attemptAddMove({{x, y}, {x + 1, y + 1}, false, false, false, true, 'q'},
                    currentBoardState, moves);
                    attemptAddMove({{x, y}, {x + 1, y + 1}, false, false, false, true, 'b'},
                    currentBoardState, moves);
                    attemptAddMove({{x, y}, {x + 1, y + 1}, false, false, false, true, 'n'},
                    currentBoardState, moves);
                }
                else
                {
                    attemptAddMove({{x, y}, {x + 1, y + 1}, false, false, false, false, ' '},
                    currentBoardState, moves);
                }
            }
            //En passant.
            else if (x + 1 == currentBoardState->getEnPassantX()
                     && y + 1 == currentBoardState->getEnPassantY())
            {
                if (canEnPassant({x, y}, {x + 1, y + 1}, currentBoardState))
                {
                    attemptAddMove({{x, y}, {x + 1, y + 1}, false, false, true, false, ' '},
                    currentBoardState, moves);
                }
            }
        }
    }
}

void Board::addUpRightPawnMoves(Box box, BoardState* currentBoardState, std::vector<Move>& moves)
{
    int x = box.x;
    int y = box.y;

    if (y - 1 >= 0)
    {
        //If we're not at the right edge of the board.
        if (x + 1 < boardXBoxes)
        {
            if (currentBoardState->getBoard()[x + 1][y - 1] != 0
                    && !pieceIsCurrentPlayersPiece(x + 1, y - 1, currentBoardState))
            {
                if (y - 1 == 0)
                {
                    attemptAddMove({{x, y}, {x + 1,y - 1 }, false, false, false, true, 'q'},
                    currentBoardState, moves);
                    attemptAddMove({{x, y}, {x + 1, y - 1}, false, false, false, true, 'r' },
                    currentBoardState, moves);
                    attemptAddMove({{x, y}, {x + 1, y - 1}, false, false, false, true, 'n' },
                    currentBoardState, moves);
                    attemptAddMove({{x, y}, {x + 1, y - 1}, false, false, false, true, 'b'},
                    currentBoardState, moves);
                }
                else
                {
                    attemptAddMove({{x, y}, {x + 1, y - 1}, false, false, false, false, ' '},
                    currentBoardState, moves);
                }

            }
            //En passant.
            else if (x + 1 == currentBoardState->getEnPassantX() && y - 1
                     == currentBoardState->getEnPassantY())
            {
                //We only can en passant if it doesn't put our king in a check.
                if (canEnPassant({x, y}, {x + 1, y - 1}, currentBoardState))
                {
                    attemptAddMove({{x, y}, {x + 1, y - 1}, false, false, true, false, ' '},
                    currentBoardState, moves);
                }
            }
        }
    }
}

void Board::addUpLeftPawnMoves(Box box, BoardState* currentBoardState, std::vector<Move>& moves)
{
    int x = box.x;
    int y = box.y;

    if (y - 1 >= 0)
    {
        //If we're not at the left edge of the board.
        if (x - 1 >= 0)
        {
            if (currentBoardState->getBoard()[x - 1][y - 1] != 0 && !pieceIsCurrentPlayersPiece(x - 1, y - 1, currentBoardState))
            {
                if (y - 1 == 0)
                {
                    attemptAddMove({{x, y}, {x - 1, y - 1}, false, false, false, true, 'q'},
                    currentBoardState, moves);
                    attemptAddMove({{x, y}, {x - 1, y - 1}, false, false, false, true, 'r'},
                    currentBoardState, moves);
                    attemptAddMove({{x, y}, {x - 1, y - 1}, false, false, false, true, 'n'},
                    currentBoardState, moves);
                    attemptAddMove({{x, y}, {x - 1, y - 1}, false, false, false, true, 'b'},
                    currentBoardState, moves);
                }
                else
                {
                    attemptAddMove({{x, y}, {x - 1, y - 1}, false, false, false, false, ' '},
                    currentBoardState, moves);
                }
            }
            //En passant.
            else if (x - 1 == currentBoardState->getEnPassantX() && y - 1
                     == currentBoardState->getEnPassantY())
            {
                if (canEnPassant({x, y}, {x - 1, y - 1}, currentBoardState))
                {
                    attemptAddMove({{x, y}, {x - 1, y - 1}, false, false, true, false, ' '},
                    currentBoardState, moves);
                }
            }
        }
    }
}

//Other pieces moves.
void Board::addStraightUpMoves(Box box, BoardState* currentBoardState, std::vector<Move>& moves)
{
    int x = box.x;
    int y = box.y;

    for (int currY = y - 1; currY >= 0; --currY)
    {
        if (currentBoardState->getBoard()[x][currY] == 0)
        {
            attemptAddMove({{x, y}, {x, currY}, false, false, false}, currentBoardState, moves);
        }
        else if (pieceIsCurrentPlayersPiece(x, currY, currentBoardState))
        {
            break;
        }
        else if (!pieceIsCurrentPlayersPiece(x, currY, currentBoardState))
        {
            attemptAddMove({{x, y}, {x, currY}, false, false, false}, currentBoardState, moves);
            break;
        }
    }
}

void Board::addStraightDownMoves(Box box, BoardState* currentBoardState, std::vector<Move>& moves)
{
    int x = box.x;
    int y = box.y;

    for (int currY = y + 1; currY < boardYBoxes; ++currY)
    {
        if (currentBoardState->getBoard()[x][currY] == 0)
        {
            attemptAddMove({{x, y}, {x, currY}, false, false, false}, currentBoardState, moves);
        }
        else if (pieceIsCurrentPlayersPiece(x, currY, currentBoardState))
        {
            break;
        }
        else if (!pieceIsCurrentPlayersPiece(x, currY, currentBoardState))
        {
            attemptAddMove({{x, y}, {x, currY}, false, false, false}, currentBoardState, moves);
            break;
        }
    }
}

void Board::addStraightLeftMoves(Box box, BoardState* currentBoardState, std::vector<Move>& moves)
{
    int x = box.x;
    int y = box.y;

    for (int currX = x - 1; currX >= 0; --currX)
    {
        if (currentBoardState->getBoard()[currX][y] == 0)
        {
            attemptAddMove({{x, y}, {currX, y}, false, false, false}, currentBoardState, moves);
        }
        else if (pieceIsCurrentPlayersPiece(currX, y, currentBoardState))
        {
            break;
        }
        else if (!pieceIsCurrentPlayersPiece(currX, y, currentBoardState))
        {
            attemptAddMove({{x, y}, {currX, y}, false, false, false}, currentBoardState, moves);
            break;
        }
    }
}

void Board::addStraightRightMoves(Box box, BoardState* currentBoardState, std::vector<Move>& moves)
{
    int x = box.x;
    int y = box.y;

    for (int currX = x + 1; currX < boardXBoxes; ++currX)
    {
        if (currentBoardState->getBoard()[currX][y] == 0)
        {
            attemptAddMove({{x, y}, {currX, y}, false, false, false}, currentBoardState, moves);
        }
        else if (pieceIsCurrentPlayersPiece(currX, y, currentBoardState))
        {
            break;
        }
        else if (!pieceIsCurrentPlayersPiece(currX, y, currentBoardState))
        {
            attemptAddMove({{x, y}, {currX, y}, false, false, false}, currentBoardState, moves);
            break;
        }
    }
}

void Board::addUpLeftMoves(Box box, BoardState* currentBoardState, std::vector<Move>& moves)
{
    int x = box.x;
    int y = box.y;

    for (int change = 1; x - change >= 0 && y - change >= 0; ++change)
    {
        if (currentBoardState->getBoard()[x - change][y - change] == 0)
        {
            attemptAddMove({{x, y}, {x - change, y - change}, false, false, false},
            currentBoardState, moves);
        }
        else if (pieceIsCurrentPlayersPiece(x - change, y - change, currentBoardState))
        {
            break;
        }
        else if (!pieceIsCurrentPlayersPiece(x - change, y - change, currentBoardState))
        {
            attemptAddMove({{x, y}, {x - change, y - change}, false, false, false},
            currentBoardState, moves);
            break;
        }
    }
}

void Board::addUpRightMoves(Box box, BoardState* currentBoardState, std::vector<Move>& moves)
{
    int x = box.x;
    int y = box.y;

    for (int change = 1; x + change < boardXBoxes && y - change >= 0; ++change)
    {
        if (currentBoardState->getBoard()[x + change][y - change] == 0)
        {
            attemptAddMove({{x, y}, {x + change, y - change}, false, false, false},
            currentBoardState, moves);
        }
        else if (pieceIsCurrentPlayersPiece(x + change, y - change, currentBoardState))
        {
            break;
        }
        else if (!pieceIsCurrentPlayersPiece(x + change, y - change, currentBoardState))
        {
            attemptAddMove({{x, y}, {x + change,y - change}, false, false, false},
            currentBoardState, moves);
            break;
        }
    }
}

void Board::addDownLeftMoves(Box box, BoardState* currentBoardState, std::vector<Move>& moves)
{
    int x = box.x;
    int y = box.y;

    for (int change = 1; x - change >= 0 && y + change < boardYBoxes; ++change)
    {
        if (currentBoardState->getBoard()[x - change][y + change] == 0)
        {
            attemptAddMove({{x, y}, {x - change, y + change}, false, false, false},
            currentBoardState, moves);
        }
        else if (pieceIsCurrentPlayersPiece(x - change, y + change, currentBoardState))
        {
            break;
        }
        else if (!pieceIsCurrentPlayersPiece(x - change, y + change, currentBoardState))
        {
            attemptAddMove({{x, y}, {x - change, y + change}, false, false, false},
            currentBoardState, moves);
            break;
        }
    }
}

void Board::addDownRightMoves(Box box, BoardState* currentBoardState, std::vector<Move>& moves)
{
    int x = box.x;
    int y = box.y;

    for (int change = 1; x + change < boardXBoxes && y + change < boardYBoxes; ++change)
    {
        if (currentBoardState->getBoard()[x + change][y + change] == 0)
        {
            attemptAddMove({{x, y}, {x + change, y + change}, false, false, false },
            currentBoardState, moves);
        }
        else if (pieceIsCurrentPlayersPiece(x + change, y + change, currentBoardState))
        {
            break;
        }
        else if (!pieceIsCurrentPlayersPiece(x + change, y + change, currentBoardState))
        {
            attemptAddMove({{x, y}, {x + change, y + change}, false, false, false},
            currentBoardState, moves);
            break;
        }
    }
}

//Make move.
void Board::makeMove(struct Move move, BoardState* currentBoardState)
{
    StoreMove* newStore = new StoreMove();
    uint8_t** board = currentBoardState->getBoard();
    char turn = currentBoardState->getCurrentTurn();

    int enPassantX = currentBoardState->getEnPassantX();
    int enPassantY = currentBoardState->getEnPassantY();
    newStore->setPreviousEnPassant({enPassantX, enPassantY});

    //Check en passant.
    if (isEnPassant(move.startBox, move.endBox, currentBoardState))
    {
        //White turn.
        if (turn == 'w')
        {
            board[enPassantX][enPassantY + 1] = 0;
            blackLocations.removePawn({enPassantX, enPassantY + 1});
        }
        //Black turn.
        else
        {
            board[enPassantX][enPassantY - 1] = 0;
            whiteLocations.removePawn({enPassantX, enPassantY - 1});
        }
    }
    //Check castle (King side).
    if (move.kingSideCastle)
    {
        board[move.endBox.x - 1][move.endBox.y] = board[move.endBox.x + 1][move.endBox.y];
        board[move.endBox.x + 1][move.endBox.y] = 0;
        //White turn.
        if (turn == 'w')
        {
            whiteLocations.updateRook({move.endBox.x + 1, move.endBox.y},
                                      {move.endBox.x - 1, move.endBox.y });
        }
        //Black turn.
        else
        {
            blackLocations.updateRook({move.endBox.x + 1, move.endBox.y},
                                      {move.endBox.x - 1, move.endBox.y });
        }
    }
    //Check castle (Queen side).
    else if (move.queenSideCastle)
    {
        board[move.endBox.x + 1][move.endBox.y] = board[move.endBox.x - 2][move.endBox.y];
        board[move.endBox.x - 2][move.endBox.y] = 0;
        //White turn.
        if (turn == 'w')
        {
            whiteLocations.updateRook({move.endBox.x - 2, move.endBox.y},
                                      {move.endBox.x + 1, move.endBox.y});
        }
        //Black turn.
        else
        {
            blackLocations.updateRook({ move.endBox.x - 2, move.endBox.y },
                                      {move.endBox.x + 1, move.endBox.y});
        }
    }

    //Update en passant.
    updateEnPassant(move.startBox, move.endBox, currentBoardState);

    //Update castle.
    newStore->setPreviousCastles(currentBoardState->getWhiteCanKingsideCastle(),
                                 currentBoardState->getWhiteCanQueensideCastle(),
                                 currentBoardState->getBlackCanKingsideCastle(),
                                 currentBoardState->getBlackCanQueensideCastle());
    updateCastling(move.startBox, move.endBox, currentBoardState);

    //Check capture.
    if (board[move.endBox.x][move.endBox.y] != 0)
    {
        newStore->setPieceTaken(board[move.endBox.x][move.endBox.y]);
        newStore->setTookPiece(true);
        //White turn.
        if (turn == 'w')
        {
            blackLocations.removePiece(board[move.endBox.x][move.endBox.y], move.endBox);
        }
        //Black turn.
        else
        {
            whiteLocations.removePiece(board[move.endBox.x][move.endBox.y], move.endBox);
        }
    }
    else
    {
        newStore->setTookPiece(false);
    }

    //Check promotion.
    if (move.isPromotion)
    {
        switch (move.promotionType)
        {
        //Promote queen.
        case 'q':
            //White turn.
            if (turn == 'w')
            {
                board[move.endBox.x][move.endBox.y] = Piece::white | Piece::queen;
                whiteLocations.getQueenLocations().push_back(move.endBox);
                whiteLocations.removePawn(move.startBox);
            }
            //Black turn.
            else
            {
                board[move.endBox.x][move.endBox.y] = Piece::black | Piece::queen;
                blackLocations.getQueenLocations().push_back(move.endBox);
                blackLocations.removePawn(move.startBox);
            }
            break;
        //Promote rook.
        case 'r':
            //White turn.
            if (turn == 'w')
            {
                board[move.endBox.x][move.endBox.y] = Piece::white | Piece::rook;
                whiteLocations.getRookLocations().push_back(move.endBox);
                whiteLocations.removePawn(move.startBox);
            }
            //Black turn.
            else
            {
                board[move.endBox.x][move.endBox.y] = Piece::black | Piece::rook;
                blackLocations.getRookLocations().push_back(move.endBox);
                blackLocations.removePawn(move.startBox);
            }
            break;
        //Promote knight.
        case 'n':
            //White turn.
            if (turn == 'w')
            {
                board[move.endBox.x][move.endBox.y] = Piece::white | Piece::knight;
                whiteLocations.getKnightLocations().push_back(move.endBox);
                whiteLocations.removePawn(move.startBox);
            }
            //Black turn.
            else
            {
                board[move.endBox.x][move.endBox.y] = Piece::black | Piece::knight;
                blackLocations.getKnightLocations().push_back(move.endBox);
                blackLocations.removePawn(move.startBox);
            }
            break;
        //Promote bishop.
        case 'b':
            //White turn.
            if (turn == 'w')
            {
                board[move.endBox.x][move.endBox.y] = Piece::white | Piece::bishop;
                whiteLocations.getBishopLocations().push_back({ move.endBox });
                whiteLocations.removePawn(move.startBox);
            }
            //Black turn.
            else
            {
                board[move.endBox.x][move.endBox.y] = Piece::black | Piece::bishop;
                blackLocations.getBishopLocations().push_back({ move.endBox });
                blackLocations.removePawn(move.startBox);
            }
            break;
        }
    }
    else
    {
        board[move.endBox.x][move.endBox.y] = board[move.startBox.x][move.startBox.y];
        //White turn.
        if (turn == 'w')
        {
            whiteLocations.updatePiece(board[move.endBox.x][move.endBox.y], move.startBox, move.endBox);
        }
        //Black turn.
        else
        {
            blackLocations.updatePiece(board[move.endBox.x][move.endBox.y], move.startBox, move.endBox);
        }
    }

    //Update threats.
    board[move.startBox.x][move.startBox.y] = 0;
    newStore->setMove(move);
    //White turn.
    if (turn == 'w')
    {
        newStore->setThreatInfo(whiteThreatened.threatenedInfo, blackThreatened.threatenedInfo,
                                whiteThreatened.attackedInfo, blackThreatened.attackedInfo,
                                whiteThreatened.amountAttacked, whiteThreatened.attackedByKnight,
                                whiteThreatened.attackedfromBox);
    }
    //Black turn.
    else
    {
        newStore->setThreatInfo(whiteThreatened.threatenedInfo, blackThreatened.threatenedInfo,
                                whiteThreatened.attackedInfo, blackThreatened.attackedInfo,
                                blackThreatened.amountAttacked, blackThreatened.attackedByKnight,
                                blackThreatened.attackedfromBox);
    }

    newStore->setThreatBoxes(whiteThreatened.straightLeftBox, whiteThreatened.upLeftBox,
                             whiteThreatened.straightUpBox, whiteThreatened.upRightBox,
                             whiteThreatened.straightRightBox, whiteThreatened.downRightBox,
                             whiteThreatened.straightDownBox, whiteThreatened.downLeftBox,

                             blackThreatened.straightLeftBox, blackThreatened.upLeftBox,
                             blackThreatened.straightUpBox, blackThreatened.upRightBox,
                             blackThreatened.straightRightBox, blackThreatened.downRightBox,
                             blackThreatened.straightDownBox, blackThreatened.downLeftBox
                            );
    moveStack.push(newStore);

    //Update attack info.
    //White turn.
    if (turn == 'w')
    {
        whiteThreatened.attackedInfo = 0;
        whiteThreatened.amountAttacked = 0;
    }
    //Black turn.
    else
    {
        blackThreatened.attackedInfo = 0;
        blackThreatened.amountAttacked = 0;
    }

    updateThreats(move, currentBoardState);
    switchTurns(currentBoardState);
    updateThreats(move, currentBoardState);
}
//-------------------------------------------------------------//

//PROMOTE.
//Promote pawn.
void Board::promoteQueen(BoardState* currentBoardState)
{
    makeMove({{promotionMove.startBox.x, promotionMove.startBox.y},
        {promotionMove.endBox.x, promotionMove.endBox.y},
        false, false, false, true, 'q'}, currentBoardState);
}

void Board::promoteRook(BoardState* currentBoardState)
{
    makeMove({{promotionMove.startBox.x, promotionMove.startBox.y},
        {promotionMove.endBox.x, promotionMove.endBox.y},
        false, false, false, true, 'r' }, currentBoardState);
}

void Board::promoteKnight(BoardState* currentBoardState)
{
    makeMove({{promotionMove.startBox.x, promotionMove.startBox.y},
        {promotionMove.endBox.x, promotionMove.endBox.y},
        false, false, false, true, 'n' }, currentBoardState);
}

void Board::promoteBishop(BoardState* currentBoardState)
{
    makeMove({{promotionMove.startBox.x, promotionMove.startBox.y },
        {promotionMove.endBox.x, promotionMove.endBox.y},
        false, false, false, true, 'b'}, currentBoardState);
}

//Choose promote piece.
void Board::tryPickingPromotionPiece(int x, int y, BoardState* currentBoardState)
{
    if (y == 3 || y == 4)
    {
        switch (x / 2)
        {
        case 0:
            promoteQueen(currentBoardState);
            break;
        case 1:
            promoteRook(currentBoardState);
            break;
        case 2:
            promoteBishop(currentBoardState);
            break;
        case 3:
            promoteKnight(currentBoardState);
            break;
        }
        waitingForPromotionChoice = false;
        nextTurn(currentBoardState);
    }
}
//-------------------------------------------------------------//

//TURN.
void Board::switchTurns(BoardState* currentBoardState)
{
    currentBoardState->setCurrentTurn((currentBoardState->getCurrentTurn() == 'w') ? 'b' : 'w');
}

void Board::nextTurn(BoardState* currentBoardState)
{
    legalMoves = calculateLegalMoves(currentBoardState);

    if (isGameOver(currentBoardState) == 1)
    {
        gameOver = true;
    }
    updateHighlightKingBox();
}
//-------------------------------------------------------------//

//OTHER.
//Handle mouse event.
void Board::handleMouseButtonDown(SDL_MouseButtonEvent& e, BoardState* currentBoardState)
{
    int x, y, boardX, boardY;

    if (e.button == SDL_BUTTON_LEFT)
    {
        SDL_GetMouseState(&x, &y);

        if (x < Game::boardTopLeftX || x > Game::boardTopLeftX + getWidth())
        {

        }
        else if (y < Game::boardTopLeftY || y > Game::boardTopLeftY + getHeight())
        {

        }
        else
        {
            boardX = (x - Game::boardTopLeftX) / boxXWidth;
            boardY = (y - Game::boardTopLeftY) / boxYHeight;
            if (waitingForPromotionChoice)
            {
                tryPickingPromotionPiece(boardX, boardY, currentBoardState);
            }
            else
            {
                if (!draggingPiece)
                {
                    attemptPickupPiece(boardX, boardY, currentBoardState);
                }
                else
                {
                    attemptPlacePiece(boardX, boardY, currentBoardState);
                }
            }
        }
    }
    //Right mouse click to stop dragging.
    else if (e.button == SDL_BUTTON_RIGHT)
    {
        if (draggingPiece)
        {
            if (currentBoardState->getCurrentTurn() == 'w')
            {
                whiteLocations.addPiece(currentBoardState->getBoard()[draggingPieceBox.x][draggingPieceBox.y], draggingPieceBox);
            }
            else
            {
                blackLocations.addPiece(currentBoardState->getBoard()[draggingPieceBox.x][draggingPieceBox.y], draggingPieceBox);
            }
            stopDraggingPiece();
        }
    }
}

//Load board (from fen).
void Board::loadBoardFromFen(const char* fenNotation, BoardState* currentBoardState)
{
    int index = 0;

    uint8_t** board = currentBoardState->getBoard();

    for (int rank = 0; rank < boardYBoxes; rank++)
    {
        int column = 0;
        while (fenNotation[index] != '/' && fenNotation[index] != ' ')
        {
            if (isdigit(fenNotation[index]))
            {
                column += (fenNotation[index] - '0');
                index++;
            }
            else
            {
                switch (fenNotation[index])
                {
                case 'P':
                    board[column][rank] = Piece::white | Piece::pawn;
                    break;
                case 'p':
                    board[column][rank] = Piece::black | Piece::pawn;
                    break;
                case 'R':
                    board[column][rank] = Piece::white | Piece::rook;
                    break;
                case 'r':
                    board[column][rank] = Piece::black | Piece::rook;
                    break;
                case 'N':
                    board[column][rank] = Piece::white | Piece::knight;
                    break;
                case 'n':
                    board[column][rank] = Piece::black | Piece::knight;
                    break;
                case 'B':
                    board[column][rank] = Piece::white | Piece::bishop;
                    break;
                case 'b':
                    board[column][rank] = Piece::black | Piece::bishop;
                    break;
                case 'Q':
                    board[column][rank] = Piece::white | Piece::queen;
                    break;
                case 'q':
                    board[column][rank] = Piece::black | Piece::queen;
                    break;
                case 'K':
                    board[column][rank] = Piece::white | Piece::king;
                    break;
                case 'k':
                    board[column][rank] = Piece::black | Piece::king;
                    break;
                }
                ++index;
                ++column;
            }
        }
        ++index;
    }

    //Current player turn.
    currentBoardState->setCurrentTurn(fenNotation[index]);
    index += 2;
    currentBoardState->setWhiteCanKingsideCastle(false);
    currentBoardState->setBlackCanKingsideCastle(false);
    currentBoardState->setWhiteCanQueensideCastle(false);
    currentBoardState->setBlackCanQueensideCastle(false);

    //Castling.
    if (fenNotation[index] == '-')
    {
        index += 2;
    }
    else
    {
        while (fenNotation[index] != ' ')
        {
            switch (fenNotation[index])
            {
            case 'K':
                currentBoardState->setWhiteCanKingsideCastle(true);
                break;
            case 'k':
                currentBoardState->setBlackCanKingsideCastle(true);
                break;
            case 'Q':
                currentBoardState->setWhiteCanQueensideCastle(true);
                break;
            case 'q':
                currentBoardState->setBlackCanQueensideCastle(true);
                break;
            }
            ++index;
        }
        ++index;
    }

    //En passant.
    if (fenNotation[index] == '-')
    {
        index += 2;
        currentBoardState->setEnPassantX(-1);
        currentBoardState->setEnPassantY(-1);
    }
    else
    {
        currentBoardState->setEnPassantX(fenNotation[index] - 'a');
        ++index;
        currentBoardState->setEnPassantY(boardYBoxes - (fenNotation[index] - '0'));
        ++index;
    }
}

//Resize window.
void Board::resize()
{
    boxXWidth = Window::screenWidth / boardXBoxes;
    boxYHeight = Window::screenHeight / boardYBoxes;

    if (boxXWidth > boxYHeight)
    {
        boxXWidth = boxYHeight;
    }
    else
    {
        boxYHeight = boxXWidth;
    }
}

//Highlight moves.
void Board::createHighlightMoves(int x, int y)
{
    highlightBoxes.clear();

    for (unsigned i = 0; i < legalMoves.size(); i++)
    {
        if (legalMoves.at(i).startBox.x == x &&
                legalMoves.at(i).startBox.y == y)
        {
            highlightBoxes.push_back(
            {
                legalMoves.at(i).endBox.x,
                legalMoves.at(i).endBox.y }
            );
        }
    }
}

//Find king location.
void Board::findKingLocation(int* xPos, int* yPos, BoardState* currentBoardState)
{
    char currentPlayer = currentBoardState->getCurrentTurn();
    Box kingBox;

    //White king.
    if (currentPlayer == 'w')
    {
        kingBox = whiteLocations.getKingLocation();
    }
    //Black king.
    else
    {
        kingBox = blackLocations.getKingLocation();
    }

    *xPos = kingBox.x;
    *yPos = kingBox.y;
}

//Game over.
int Board::isGameOver(BoardState* currentBoardState)
{
    std::vector<Move> currentLegalMoves = calculateLegalMoves(currentBoardState);
    if (currentLegalMoves.size() == 0)
    {
        winner = (currentBoardState->getCurrentTurn() == 'w' ? 'b' : 'w');
        return 1;
    }
    else
    {
        return 0;
    }
}
//-------------------------------------------------------------//



