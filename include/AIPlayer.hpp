#ifndef AIPLAYER_HPP
#define AIPLAYER_HPP

#include "Player.hpp"
#include "Board.hpp"
#include <utility>
#include <vector>

class AIPlayer : public Player {
public:
    AIPlayer(char symbol);
    void makeMove(Board& board, int& row, int& col) override;

private:
    char opponentSymbol;

    int evaluateBoard(Board& board);
    std::vector<std::pair<int, int>> generateMoves(Board& board);
    int minimax(Board& board, int depth, bool maximizing, int alpha, int beta);
    std::pair<int, int> findBestMove(Board& board);

    int evaluateLine(const std::vector<char>& line, char currentSymbol);
};

#endif
