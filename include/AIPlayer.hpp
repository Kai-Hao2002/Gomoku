#ifndef AIPLAYER_HPP
#define AIPLAYER_HPP

#include "Player.hpp"
#include "Board.hpp"
#include <utility>
#include <vector>
#include <unordered_map>
#include <cstdint>

class AIPlayer : public Player {
public:
    AIPlayer(char symbol);
    void makeMove(Board& board, int& row, int& col) override;

private:
    char opponentSymbol;

    // --- 核心演算法 --- //
    int evaluateBoard(Board& board);
    std::vector<std::pair<int, int>> generateMoves(Board& board);
    int minimax(Board& board, int depth, bool maximizing, int alpha, int beta);
    std::pair<int, int> findBestMove(Board& board);
    int evaluateLine(const std::vector<char>& line, char currentSymbol);

    // --- Zobrist Hashing --- //
    uint64_t zobristTable[Board::SIZE][Board::SIZE][2]; // [row][col][0: X, 1: O]
    std::unordered_map<uint64_t, int> transpositionTable;
    void initZobrist();
    uint64_t computeZobristHash(const Board& board);
};

#endif
