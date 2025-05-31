/***********************************************************************
 * Source File:
 *    BOARD
 * Author:
 *    Roger Galan & Jessen Forbush
 * Summary:
 *    A collection of pieces and a small amount of game state
 ************************************************************************/

#include "board.h"
#include "uiDraw.h"
#include "position.h"
#include "piece.h"
#include "pieceSpace.h"
#include "pieceKnight.h"
#include "pieceRook.h"
#include "pieceBishop.h"
#include "pieceQueen.h"
#include "pieceKing.h"
#include "piecePawn.h"
#include <cassert>
#include <utility>
using namespace std;

/***********************************************
 * BOARD : RESET
 *         Initialize the board with standard chess starting positions
 ***********************************************/
void Board::reset(bool fFree)
{
   // Free existing pieces if requested
   if (fFree)
      free();

   // White pieces (row 0)
   board[0][0] = new Rook(0, 0, true);     // a1
   board[1][0] = new Knight(1, 0, true);   // b1
   board[2][0] = new Bishop(2, 0, true);   // c1
   board[3][0] = new Queen(3, 0, true);    // d1
   board[4][0] = new King(4, 0, true);     // e1
   board[5][0] = new Bishop(5, 0, true);   // f1
   board[6][0] = new Knight(6, 0, true);   // g1
   board[7][0] = new Rook(7, 0, true);     // h1

   // White pawns (row 1)
   for (int c = 0; c < 8; c++)
      board[c][1] = new Pawn(c, 1, true);

   // Black pieces (row 7)
   board[0][7] = new Rook(0, 7, false);    // a8
   board[1][7] = new Knight(1, 7, false);  // b8
   board[2][7] = new Bishop(2, 7, false);  // c8
   board[3][7] = new Queen(3, 7, false);   // d8
   board[4][7] = new King(4, 7, false);    // e8
   board[5][7] = new Bishop(5, 7, false);  // f8
   board[6][7] = new Knight(6, 7, false);  // g8
   board[7][7] = new Rook(7, 7, false);    // h8

   // Black pawns (row 6)
   for (int c = 0; c < 8; c++)
      board[c][6] = new Pawn(c, 6, false);

   // Fill empty squares with Space objects (rows 2-5)
   for (int r = 2; r < 6; r++)
      for (int c = 0; c < 8; c++)
         board[c][r] = new Space(c, r);
}

/***********************************************
* BOARD : GET (const)
*         Get a piece from a given position (read-only)
***********************************************/
const Piece& Board::operator [] (const Position& pos) const
{
   Piece* p = board[pos.getCol()][pos.getRow()];
   return *p;
}

/***********************************************
* BOARD : GET
*         Get a piece from a given position (modifiable)
***********************************************/
Piece& Board::operator [] (const Position& pos)
{
   Piece* p = board[pos.getCol()][pos.getRow()];
   return *p;
}

/***********************************************
* BOARD : DISPLAY
*         Render the board and all pieces to the screen
***********************************************/
void Board::display(const Position& posHover, const Position& posSelect, const set<Move>& possibleMoves) const
{
   if (pgout != nullptr)
   {
      // Draw the chess board background
      pgout->drawBoard();
      
      // Highlight squares based on mouse and selection state
      pgout->drawHover(posHover);
      pgout->drawSelected(posSelect);

      // Show possible moves for selected piece
      for (const Move& move : possibleMoves)
      {
         pgout->drawPossible(move.getDest());
      }

      // Draw all pieces on the board
      for (int r = 0; r < 8; r++)
      {
         for (int c = 0; c < 8; c++)
         {
            if (board[c][r] != nullptr)
            {
               board[c][r]->display(pgout);
            }
         }
      }
   }
}

/************************************************
 * BOARD : CONSTRUCTOR
 *         Initialize an empty board and optionally reset to starting position
 ************************************************/
Board::Board(ogstream* pgout, bool noreset) : pgout(pgout), numMoves(0)
{
   // Initialize all board positions to null
   for (int r = 0; r < 8; r++)
      for (int c = 0; c < 8; c++)
         board[c][r] = nullptr;

   // Set up initial chess position unless noreset is specified
   if (!noreset)
      reset();
}

/************************************************
 * BOARD : FREE
 *         Deallocate all pieces and reset board to null pointers
 ************************************************/
void Board::free()
{
   for (int r = 0; r < 8; r++)
      for (int c = 0; c < 8; c++)
         if (board[c][r] != nullptr)
         {
            delete board[c][r];
            board[c][r] = nullptr;
         }
}

/**********************************************
 * BOARD : ASSERT BOARD
 *         Verify all board positions contain valid pieces (for debugging)
 *********************************************/
void Board::assertBoard()
{
   for (int r = 0; r < 8; r++)
      for (int c = 0; c < 8; c++)
         assert(board[c][r] != nullptr);
}

/**********************************************
 * BOARD EMPTY : CONSTRUCTOR
 * Test utility - creates an empty board with a default space piece
 *********************************************/
BoardEmpty::BoardEmpty() : BoardDummy(), pSpace(nullptr)
{
   pSpace = new Space(0, 0);
}

/**********************************************
 * BOARD EMPTY : DESTRUCTOR
 * Clean up the default space piece
 *********************************************/
BoardEmpty::~BoardEmpty()
{
   delete pSpace;
}

/**********************************************
 * BOARD : MOVE
 *         Execute a chess move including special cases (castling, en passant, promotion)
 *********************************************/
void Board::move(const Move& move)
{
   Position source = move.getSrc();
   Position dest = move.getDest();

   // Update piece movement tracking before incrementing move counter
   (*this)[source].setLastMove(numMoves);
   numMoves++;

   // Handle en passant capture (captured pawn is adjacent, not at destination)
   if (move.getMoveType() == Move::MoveType::ENPASSANT)
   {
      if (move.getWhiteMove())
      {
         // White captures black pawn below destination square
         delete board[dest.getCol()][dest.getRow() - 1];
         board[dest.getCol()][dest.getRow() - 1] = new Space(dest.getCol(), dest.getRow() - 1);
      }
      else
      {
         // Black captures white pawn above destination square
         delete board[dest.getCol()][dest.getRow() + 1];
         board[dest.getCol()][dest.getRow() + 1] = new Space(dest.getCol(), dest.getRow() + 1);
      }
   }
   // Handle regular captures (piece at destination square)
   else if (move.getCapture() != SPACE)
   {
      delete board[dest.getCol()][dest.getRow()];
      board[dest.getCol()][dest.getRow()] = new Space(dest.getCol(), dest.getRow());
   }

   // Check for pawn promotion before moving pieces
   bool isPawnPromotion = (board[source.getCol()][source.getRow()]->getType() == PieceType::PAWN) &&
                         (dest.getRow() == 0 || dest.getRow() == 7);

   // Handle king-side castling (move rook from h-file to f-file)
   if ((board[source.getCol()][source.getRow()]->getType() == PieceType::KING) &&
       (move.getMoveType() == Move::MoveType::CASTLE_KING))
   {
      std::swap(board[source.getCol() + 3][source.getRow()], board[source.getCol() + 1][source.getRow()]);
      board[source.getCol() + 1][source.getRow()]->setPosition(Position(source.getCol() + 1, source.getRow()));
   }

   // Handle queen-side castling (move rook from a-file to d-file)
   if ((board[source.getCol()][source.getRow()]->getType() == PieceType::KING) &&
         (move.getMoveType() == Move::MoveType::CASTLE_QUEEN))
   {
      std::swap(board[source.getCol() - 4][source.getRow()], board[source.getCol() - 1][source.getRow()]);
      board[source.getCol() - 1][source.getRow()]->setPosition(Position(source.getCol() - 1, source.getRow()));
   }

   // Execute the main piece movement
   std::swap(board[source.getCol()][source.getRow()], board[dest.getCol()][dest.getRow()]);
   board[dest.getCol()][dest.getRow()]->setPosition(dest);

   // Handle pawn promotion (replace pawn with queen)
   if (isPawnPromotion)
   {
      delete board[dest.getCol()][dest.getRow()];
      board[dest.getCol()][dest.getRow()] = new Queen(dest.getCol(), dest.getRow(), move.getWhiteMove());
   }
}
