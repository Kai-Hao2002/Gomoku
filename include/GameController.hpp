#ifndef GAMECONTROLLER_HPP
#define GAMECONTROLLER_HPP

#include "Board.hpp"
#include "Player.hpp"
#include "AIPlayer.hpp"
#include "HumanPlayer.hpp"

class GameController {
public:
    GameController(bool vsAI);
    ~GameController();

    const Board& getBoard() const;
    char getCurrentSymbol() const;
    bool makeMove(int row, int col);
    bool isGameOver() const;
    char getWinnerSymbol() const;

private:
    Board board;
    Player* p1;
    Player* p2;
    Player* current;
    bool gameOver;
    char winner;

    void switchTurn();
};

#endif
