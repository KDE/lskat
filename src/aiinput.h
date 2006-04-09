#ifndef AI_INPUT_H
#define AI_INPUT_H

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
     *  @param dir  The grafix icon directory
     *  @param parent The parent object
     */
    AiInput(EngineTwo* engine, QString dir, QObject* parent);

    /** Allow or disallow input with this device 
      * @param allowed True if input is allowed
      */
    void setInputAllowed(bool allowed);  


  public slots:  
    void aiTurn();

  private:
    class Board
    {
      public:
        /** Constructor */
        Board() {};
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
