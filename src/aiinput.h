#ifndef AI_INPUT_H
#define AI_INPUT_H
/*
   This file is part of the KDE games lskat program
   Copyright (c) 2006 Martin Heni <kde@heni-online.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// KDE includes
#include <kdebug.h>

// local includes
#include "abstractinput.h"
#include "engine_two.h"

// Forward declaration


/**
 * AI input device
 */
class AiInput : public AbstractInput
{
  Q_OBJECT

  public:
    /** Constructor for the input
     *  @param engine  The game engine
     *  @param parent The parent object
     */
    AiInput(EngineTwo* engine, QObject* parent);

    /** Allow or disallow input with this device 
      * @param allowed True if input is allowed
      */
    void setInputAllowed(bool allowed);  

    /** Retrieve the type of device.
      * @return The decice type.
      */
    virtual InputDevice::InputDeviceType type() {return InputDevice::TypeAiInput;}


  public slots:  
    void aiTurn();

  private:
    class Board
    {
      public:
        /** Constructor */
        Board() {}
        /** Copy constructor */
        Board(const Board& board);
        int cards[2][16]; /** Cards of both players or -1 for used cards */
        int playedCard;   /** Currently played card of first player or -1 */
        int points[2];    /** Points of both players */
        int whoseTurn;    /** Whose turn is it 0/1 UNUSED */
        bool firstPlay;   /** True if first player movement phase UNUSED */
    };
    class Move
    {
      public:
        double value;
        int move;
    };


  protected:
    /** Extract the current game board from the engine.
     *  @return The game board.
     */
    Board getBoardFromEngine();

    /** Initiate a new move as first player.
     *  @param p The current player number.
     *  @param board The current game board. 
     *  @return The best move
     */
    Move initiateMove(int p, const Board& board);

    /** Answer a move as second player.
     *  @param p The current player number.
     *  @param board The current game board. 
     *  @return The best move
     */
    Move answerMove(int p, const Board& board);

    /** Evaluate the current game board and return a rating.
     *  @param p The current player (we)
     *  @param current The current game board
     *  @return The rating for this situation.
     */
    double evaluteGame(int p, const AiInput::Board current);

  signals:  
  private:
    EngineTwo* mEngine;
};

#endif
