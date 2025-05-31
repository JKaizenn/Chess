/***********************************************************************
 * Header File:
 *    SPACE
 * Author:
 *   Jessen Forbush & Roger Galan
 * Summary:
 *    The space
 ************************************************************************/

#pragma once

#include "piece.h"

class TestSpace;

 /***************************************************
  * SPACE
  * A space is a trivial piece
  ***************************************************/
class Space : public Piece
{
   friend TestSpace;
public:
   Space(int c, int r) : Piece(c, r) {}
   ~Space() {}
   PieceType getType() const { return SPACE; }
   void display(ogstream* pgout) const {}
   void getMoves(set<Move>& moves, const Board& board) const { moves.clear(); }
};
