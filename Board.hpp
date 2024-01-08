#pragma once
#include <vector>
#include <SDL.h>

#include "Move.hpp"
#include "BoardState.hpp"
#include "Box.hpp"
#include "MoveStack.hpp"
#include "PieceLocations.hpp"
#include "KingThreatenedInfo.hpp"

class Board
{
private:
    SDL_Color boardColor1;
    SDL_Color boardColor2;

    BoardState* boardState;

    PieceLocations whiteLocations;
    PieceLocations blackLocations;

    KingThreatenedInfo whiteThreatened;
    KingThreatenedInfo blackThreatened;

    //Moving pieces.
    bool draggingPiece;
    Box draggingPieceBox;
    SDL_Texture* draggingPieceTexture;

    //Moves.
    std::vector<Move> pseudoLegalMoves;
    std::vector<Move> legalMoves;

    std::vector<Box> highlightBoxes;

    Box highlightKingBox;

    Move promotionMove;
    bool waitingForPromotionChoice;

    char winner;
    Box winnerKing;
    bool gameOver;

    MoveStack moveStack;

public:
    static int boxXWidth;
    static int boxYHeight;
    static int boardXBoxes;
    static int boardYBoxes;

//INITIALIZE.
    //Initialize everything.
    void init();

    //Initialize piece locations.
    void initializePieceLocations(BoardState* currentBoardState);

    //Initialize king threatened.
    void initializeKingsThreatened(BoardState* currentBoardState);
//--------------------------------------------//

//DELETE.
    //Delete board.
    ~Board();

    //Clear moves.
    void clearMoves();
//--------------------------------------------//

//GETTER.
    int getWidth();
    int getHeight();
    BoardState* getBoardState();
    SDL_Texture* getTextureAtLocation(int x, int y, BoardState* boardState);
//--------------------------------------------//

//FUNCTIONS IN GAME.
    //Reset game.
    void reset();
    //Redo move.
    void unMakeMove(BoardState*);
    //Make random move.
    void makeRandomMove(BoardState*);
//--------------------------------------------//

//RENDER.
    //Render everything.
    void render(BoardState* boardState);

    //Render board.
    void renderBoard();

    //Render piece.
    void renderPieceTexture(SDL_Texture*, int x, int y);
    void renderPiece(Box, BoardState* boardState);
    void renderPieces(BoardState* boardState);
    void renderDraggedPiece();

    //Render boxes.
    void renderAttackedSquares();
    void renderBox(Box box, SDL_Color color);
    void renderKingBox();

    //Render moves.
    void renderHighlightMoves();
    void renderPreviousMove();

    //Render promotion options (Queen, Rook, Bishop, Knight).
    void renderPromotionOptions();
//--------------------------------------------//

//CALCULATING.
    //Calculate pseudo legal moves.
    std::vector<Move> calculatePseudoLegalMoves(BoardState* boardState);
    void calculateRookMoves(Box box, BoardState* currentBoardState, std::vector<Move>& currentPseudo);
    void calculateKnightMoves(Box box, BoardState* currentBoardState, std::vector<Move>& currentPseudo);
    void calculateBishopMoves(Box box, BoardState* currentBoardState, std::vector<Move>& currentPseudo);
    void calculateKingMoves(Box box, BoardState* currentBoardState, std::vector<Move>& currentPseudo);
    void calculateQueenMoves(Box box, BoardState* currentBoardState, std::vector<Move>& currentPseudo);
    void calculatePawnUpMoves(Box box, BoardState* currentBoardState, std::vector<Move>& currentPseudo);
    void calculatePawnDownMoves(Box box, BoardState* currentBoardState, std::vector<Move>& currentPseudo);
    void calculateCastlingMoves(Box box, BoardState* currentBoardState, std::vector<Move>& currentPseudo);

    //Calculate legal moves.
    std::vector<Move> calculateLegalMoves(BoardState* currentBoardState);
    void calculateRookLegalMoves(Box rookBox, Box kingBox, BoardState* currentBoardState,
                                 std::vector<Move>& currentLegalMoves, KingThreatenedInfo currentKingInfo);
    void calculateKnightLegalMoves(Box knightBox, Box kingBox, BoardState* currentBoardState,
                                   std::vector<Move>& currentLegalMoves, KingThreatenedInfo currentKingInfo);
    void calculateBishopLegalMoves(Box bishopBox, Box kingBox, BoardState* currentBoardState,
                                   std::vector<Move>& currentLegalMoves, KingThreatenedInfo currentKingInfo);
    void calculateKingLegalMoves(Box kingBox, BoardState* currentBoardState,
                                 std::vector<Move>& currentLegalMoves, KingThreatenedInfo currentKingInfo);
    void calculateQueenLegalMoves(Box queenBox, Box kingBox, BoardState* currentBoardState,
                                  std::vector<Move>& currentLegalMoves, KingThreatenedInfo currentKingInfo);
    void calculatePawnLegalMoves(Box pawnBox, Box kingBox, BoardState* currentBoardState,
                                 std::vector<Move>& currentLegalMoves, KingThreatenedInfo currentKingInfo);
    void calculateCastlingLegalMoves(Box kingBox, BoardState* currentBoardState,
                                     std::vector<Move>& currentLegalMoves);
//--------------------------------------------//

//UPDATING.
    //Update loop.
    void update();

    //Update threats.
    void updateStraightUpThreats(char currentTurn, BoardState* boardState);
    void updateStraightDownThreats(char currentTurn, BoardState* boardState);
    void updateStraightLeftThreats(char currentTurn,BoardState* boardState);
    void updateStraightRightThreats(char currentTurn, BoardState* boardState);

    void updateUpLeftThreats(char currentTurn, BoardState* boardState);
    void updateUpRightThreats(char currentTurn, BoardState* boardState);
    void updateDownLeftThreats(char currentTurn, BoardState* boardState);
    void updateDownRightThreats(char currentTurn, BoardState* boardState);

    void updateMoveToThreats(Move move, BoardState*);
    void updateThreats(Move move, BoardState*);
    void updateAllThreats(char curentTurn, BoardState* currentBoardState);

    //Update en passant.
    void updateEnPassant(Box fromBox, Box toBox, BoardState* boardState);

    //Update castling.
    void updateCastling(Box fromBox, Box toBox, BoardState* boardState);

    //Update highlight king box.
    void updateHighlightKingBox();
//--------------------------------------------//

//CHECKING.
    //Check position.
    bool inSameRow(Box box1, Box box2);
    bool inSameCol(Box box1, Box box2);
    bool inSameDiagonal(Box box1, Box box2); //Bottom left to top right
    bool inSameReverseDiagonal(Box box1, Box box2); //Bottom right to top left.

    //Check piece.
    bool pieceIsCurrentPlayersPiece(int x, int y, BoardState* boardState);

    //Check en passant.
    bool isEnPassant(Box fromBox, Box toBox, BoardState* boardState);
    bool canEnPassant(Box, Box, BoardState*);

    //Check legal.
    bool inPseudoMoves(struct Move& move);
    bool inLegalMoves(struct Move& move);

    //Check and checkmate.
    bool kingInCheck(BoardState* currentBoardState);
    bool kingAttacked(BoardState* currentBoardState);

    //Check attacked.
    bool squareAttacked(Box box, BoardState* currentBoardState);
    bool doesBoxBlockAttack(Box box, BoardState* currentBoardState);
//--------------------------------------------//

//MOVING PIECES.
//Pick up.
    void attemptPickupPiece(int x, int y, BoardState* boardState);

//Place down.
    void attemptPlacePiece(int x, int y, BoardState* boardState);

//Stop dragging.
    void stopDraggingPiece();
//--------------------------------------------//

//ADD MOVES.
    void attemptAddMove(Move move, BoardState* currentBoardState, std::vector<Move>& moves);

    //Pawn moves.
    void addStraightUpPawnMoves(Box box, BoardState* currentBoardState, std::vector<Move>& moves);
    void addStraightDownPawnMoves(Box box, BoardState* currentBoardState, std::vector<Move>& moves);
    void addDownLeftPawnMoves(Box box, BoardState* currentBoardState, std::vector<Move>& moves);
    void addDownRightPawnMoves(Box box, BoardState* currentBoardState, std::vector<Move>& moves);
    void addUpLeftPawnMoves(Box box, BoardState* currentBoardState, std::vector<Move>& moves);
    void addUpRightPawnMoves(Box box, BoardState* currentBoardState, std::vector<Move>& moves);

    //Other pieces moves.
    void addStraightUpMoves(Box box, BoardState* currentBoardState, std::vector<Move>& moves);
    void addStraightDownMoves(Box box, BoardState* currentBoardState, std::vector<Move>& moves);
    void addStraightLeftMoves(Box box, BoardState* currentBoardState, std::vector<Move>& moves);
    void addStraightRightMoves(Box box, BoardState* currentBoardState, std::vector<Move>& moves);

    void addUpLeftMoves(Box box, BoardState* currentBoardState, std::vector<Move>& moves);
    void addUpRightMoves(Box box, BoardState* currentBoardState, std::vector<Move>& moves);
    void addDownLeftMoves(Box box, BoardState* currentBoardState, std::vector<Move>& moves);
    void addDownRightMoves(Box box, BoardState* currentBoardState, std::vector<Move>& moves);

    //Make move.
    void makeMove(struct Move move, BoardState* currentBoardState);
//--------------------------------------------//

//PROMOTE.
    //Promote pawn.
    void promoteQueen(BoardState* boardState);
    void promoteRook(BoardState* boardState);
    void promoteBishop(BoardState* boardState);
    void promoteKnight(BoardState* boardState);

    //Choose promote piece.
    void tryPickingPromotionPiece(int x, int y, BoardState* currentBoardState);
//--------------------------------------------//

//TURN.
    void switchTurns(BoardState* currentBoardState);
    void nextTurn(BoardState* curentBoardState);
//--------------------------------------------//

//OTHER.
    //Handle mouse event.
    void handleMouseButtonDown(SDL_MouseButtonEvent& event, BoardState* currentBoardState);

    //Load board (from fen).
    void loadBoardFromFen(const char* fen, BoardState* boardState);

    //Resize window.
    void resize();

    //Highlight moves.
    void createHighlightMoves(int x, int y);

    //Game over.
    int isGameOver(BoardState* currentBoardState);

    //Find king location.
    void findKingLocation(int* x, int* y, BoardState* currentBoardState);
//--------------------------------------------//
};

