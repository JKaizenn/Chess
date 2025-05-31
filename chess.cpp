/**********************************************************************
* Source File:
*    Lab 04: Chess
* Author:
*    Jessen Forbush & Roger Galan
* Summary:
*    Play the game of chess
************************************************************************/

#include "uiInteract.h"   // for Interface
#include "uiDraw.h"       // for OGSTREAM
#include "position.h"     // for POSITION
#include "piece.h"        // for PIECE and company
#include "board.h"        // for BOARD
#include "test.h"
#include <set>            // for STD::SET
#include <cassert>        // for ASSERT
#include <fstream>        // for IFSTREAM
#include <string>         // for STRING
#include <iostream>
using namespace std;

/*************************************
 * CALLBACK
 * All the interesting work happens here, when
 * I get called back from OpenGL to draw a frame.
 * When I am finished drawing, then the graphics
 * engine will wait until the proper amount of
 * time has passed and put the drawing on the screen.
 **************************************/
void callBack(Interface *pUI, void * p)
{
   // the first step is to cast the void pointer into a game object. This
   // is the first step of every single callback function in OpenGL.
   Board * pBoard = (Board *)p;

   Move move;
   set<Move> possible;
   
   // Get the possible moves from the previous (source) location.
   if (pUI->getPreviousPosition().isValid())
      (*pBoard)[pUI->getPreviousPosition()].getMoves(possible, *pBoard);
   
   // Create the move that matches the source/dest to one of the possible moves.
   if (pUI->getSelectPosition().isValid() && pUI->getPreviousPosition().isValid())
      move = Move(pUI->getPreviousPosition(), pUI->getSelectPosition(), possible);
   
   // move
   if (possible.find(move) != possible.end())
   {
      pBoard->move(move);
      pUI->clearSelectPosition();
   }
   // Draw current possible moves.
   else if (pUI->getSelectPosition().isValid())
   {
      possible.clear();
      (*pBoard)[pUI->getSelectPosition()].getMoves(possible, *pBoard);
   }
   
   // if we clicked on a blank spot, then it is not selected
   if (pUI->getSelectPosition().isValid() && (*pBoard)[pUI->getSelectPosition()].getType() == SPACE)
      pUI->clearSelectPosition();
   
   // draw the board with possible moves
   pBoard->display(pUI->getHoverPosition(), pUI->getSelectPosition(), possible);
}

/*********************************
 * READ FILE
 * Read and execute moves from a file
 *************************************/
void readFile(const char* fileName, Board & board)
{
   ifstream fin(fileName);
   if (fin.fail())
      return;
   
   string textMove;
   while (fin >> textMove)
   {
      Move move(textMove);
      move.setWhiteMove(board.whiteTurn());
      board.move(move);
   }
   
   fin.close();
}

/*********************************
 * MAIN
 * Entry point for the chess program
 *********************************/
#ifdef _WIN32
#include <windows.h>
int WINAPI WinMain(
   _In_ HINSTANCE hInstance,
   _In_opt_ HINSTANCE hPrevInstance,
   _In_ PSTR pCmdLine,
   _In_ int nCmdShow)
#else // !_WIN32
int main(int argc, char** argv)
#endif // !_WIN32
{
   // Run unit tests
   testRunner();
   
   // Initialize graphics and game
   Interface ui("Chess");
   ogstream* pgout = new ogstream;
   Board board(pgout);

   // If a moves file was provided , execute those moves
   if (argc > 1) {
       cout << "Loading moves from: " << argv[1] << endl;
       readFile(argv[1], board);
   }

   // Start the game loop
   ui.run(callBack, (void *)(&board));
   
   // Cleanup
   delete pgout;
   return 0;
}
