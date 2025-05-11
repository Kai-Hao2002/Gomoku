#ifndef AIPLAYER_HPP
#define AIPLAYER_HPP

#include "Player.hpp"
#include "Board.hpp"
#include <utility>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <optional> // 加在其他 #include 下方

class AIPlayer : public Player {
public:
    AIPlayer(char symbol);
    void makeMove(Board& board, int& row, int& col) override;
    std::optional<std::pair<int, int>> findBlockingMoveIfThreat(Board& board);
    std::optional<std::pair<int, int>> findWinningMoveIfAvailable(Board& board);

private:
    char opponentSymbol;

    // --- 核心演算法 --- //
    int evaluateBoard(Board& board);
    std::vector<std::pair<int, int>> generateMoves(Board& board);
    int minimax(Board& board, int depth, bool maximizing, int alpha, int beta);
    std::pair<int, int> findBestMove(Board& board);
    int evaluateLine(const std::vector<char>& line, char currentSymbol);
    bool hasDangerousThree(Board& board, char checkSymbol);
    

    // --- Zobrist Hashing --- //
    uint64_t zobristTable[Board::SIZE][Board::SIZE][2]; // [row][col][0: X, 1: O]
    std::unordered_map<uint64_t, int> transpositionTable;
    void initZobrist();
    uint64_t computeZobristHash(const Board& board);
};

#endif
