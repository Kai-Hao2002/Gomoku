#include "AIPlayer.hpp"
#include <limits>
#include <thread>
#include <chrono>
#include <future>
#include <random>
#include <iostream>
#include <unordered_map>

AIPlayer::AIPlayer(char symbol) : Player(symbol) {
    opponentSymbol = (symbol == 'X') ? 'O' : 'X';
    initZobrist();
}

void AIPlayer::initZobrist() {
    std::mt19937_64 rng(42); // 固定種子便於重現
    std::uniform_int_distribution<uint64_t> dist;

    for (int i = 0; i < Board::SIZE; ++i) {
        for (int j = 0; j < Board::SIZE; ++j) {
            zobristTable[i][j][0] = dist(rng); // X
            zobristTable[i][j][1] = dist(rng); // O
        }
    }
}

uint64_t AIPlayer::computeZobristHash(const Board& board) {
    uint64_t hash = 0;
    for (int i = 0; i < Board::SIZE; ++i) {
        for (int j = 0; j < Board::SIZE; ++j) {
            char cell = board.getCell(i, j);
            if (cell == 'X') hash ^= zobristTable[i][j][0];
            else if (cell == 'O') hash ^= zobristTable[i][j][1];
        }
    }
    return hash;
}

int AIPlayer::evaluateBoard(Board& board) {
    int score = 0;

    // 橫列
    for (int i = 0; i < Board::SIZE; ++i) {
        std::vector<char> row;
        for (int j = 0; j < Board::SIZE; ++j) row.push_back(board.getCell(i, j));
        score += evaluateLine(row, symbol);
        score -= evaluateLine(row, opponentSymbol);
    }

    // 直行
    for (int j = 0; j < Board::SIZE; ++j) {
        std::vector<char> col;
        for (int i = 0; i < Board::SIZE; ++i) col.push_back(board.getCell(i, j));
        score += evaluateLine(col, symbol);
        score -= evaluateLine(col, opponentSymbol);
    }

    // 對角線
    for (int k = 0; k <= 2 * (Board::SIZE - 1); ++k) {
        std::vector<char> diag1, diag2;
        for (int i = 0; i < Board::SIZE; ++i) {
            int j1 = k - i;
            int j2 = i - (k - Board::SIZE + 1);
            if (j1 >= 0 && j1 < Board::SIZE) diag1.push_back(board.getCell(i, j1));
            if (j2 >= 0 && j2 < Board::SIZE) diag2.push_back(board.getCell(i, j2));
        }
        score += evaluateLine(diag1, symbol);
        score -= evaluateLine(diag1, opponentSymbol);
        score += evaluateLine(diag2, symbol);
        score -= evaluateLine(diag2, opponentSymbol);
    }

    return score;
}

int AIPlayer::evaluateLine(const std::vector<char>& line, char currentSymbol) {
    int score = 0;
    const int n = line.size();
    for (int i = 0; i <= n - 5; ++i) {
        int count = 0;
        bool blockedLeft = false, blockedRight = false;

        for (int j = 0; j < 5; ++j) {
            if (line[i + j] == currentSymbol) count++;
            else if (line[i + j] != '.') {
                count = -1;
                break;
            }
        }

        if (count > 0) {
            if (i == 0 || line[i - 1] != '.') blockedLeft = true;
            if (i + 5 >= n || line[i + 5] != '.') blockedRight = true;

            int base = (count == 5) ? 100000 :
                       (count == 4) ? 10000 :
                       (count == 3) ? 1000 :
                       (count == 2) ? 100 : 10;

            if (blockedLeft && blockedRight) base /= 4;
            else if (blockedLeft || blockedRight) base /= 2;

            score += base;
        }
    }
    return score;
}

// --- 只產生鄰近已下棋子的空格（效率優化） --- //
std::vector<std::pair<int, int>> AIPlayer::generateMoves(Board& board) {
    std::vector<std::pair<int, int>> moves;
    const int range = 1;

    for (int i = 0; i < Board::SIZE; ++i) {
        for (int j = 0; j < Board::SIZE; ++j) {
            if (board.getCell(i, j) != '.') continue;

            for (int dx = -range; dx <= range; ++dx) {
                for (int dy = -range; dy <= range; ++dy) {
                    int ni = i + dx, nj = j + dy;
                    if (ni >= 0 && ni < Board::SIZE && nj >= 0 && nj < Board::SIZE &&
                        board.getCell(ni, nj) != '.') {
                        moves.emplace_back(i, j);
                        goto next;
                    }
                }
            }
        next:;
        }
    }

    return moves;
}

// --- Minimax + Alpha-Beta + Zobrist Transposition Table --- //
int AIPlayer::minimax(Board& board, int depth, bool maximizing, int alpha, int beta) {
    uint64_t hash = computeZobristHash(board);
    if (transpositionTable.count(hash)) return transpositionTable[hash];

    if (depth == 0 || board.isFull()) {
        int eval = evaluateBoard(board);
        transpositionTable[hash] = eval;
        return eval;
    }

    auto moves = generateMoves(board);
    int bestVal = maximizing ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();

    for (auto [r, c] : moves) {
        board.placePiece(r, c, maximizing ? symbol : opponentSymbol);
        int score = 0;

        if (board.isWin(r, c, maximizing ? symbol : opponentSymbol)) {
            score = maximizing ? 100000 : -100000;
        } else {
            score = minimax(board, depth - 1, !maximizing, alpha, beta);
        }

        board.placePiece(r, c, '.');

        if (maximizing) {
            bestVal = std::max(bestVal, score);
            alpha = std::max(alpha, score);
        } else {
            bestVal = std::min(bestVal, score);
            beta = std::min(beta, score);
        }

        if (beta <= alpha) break;
    }

    transpositionTable[hash] = bestVal;
    return bestVal;
}

// --- 最佳化平行運算：避免過多 async 呼叫 --- //
std::pair<int, int> AIPlayer::findBestMove(Board& board) {
    int bestScore = std::numeric_limits<int>::min();
    std::pair<int, int> bestMove = {-1, -1};

    auto moves = generateMoves(board);
    const int PARALLEL_LIMIT = 8;
    std::vector<std::future<std::pair<int, std::pair<int, int>>>> futures;

    for (size_t i = 0; i < moves.size(); ++i) {
        auto [r, c] = moves[i];
        Board copy = board;
        copy.placePiece(r, c, symbol);

        if (i < PARALLEL_LIMIT) {
            futures.push_back(std::async(std::launch::async, [=, &board]() {
                Board copy = board;
                copy.placePiece(r, c, symbol);
                int score = minimax(copy, 4, false, INT_MIN, INT_MAX); // 深度 4
                return std::make_pair(score, std::make_pair(r, c));
            }));
        }
        else {
            int score = minimax(copy, 4, false, INT_MIN, INT_MAX);
            if (score > bestScore) {
                bestScore = score;
                bestMove = {r, c};
            }
        }
    }

    for (auto& f : futures) {
        auto [score, move] = f.get();
        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
    }

    return bestMove;
}

void AIPlayer::makeMove(Board& board, int& row, int& col) {
    std::cout << "AI (" << symbol << ") is thinking...\n";
    std::cout.flush();

    auto start = std::chrono::steady_clock::now();
    transpositionTable.clear(); // 每次重新開始
    std::tie(row, col) = findBestMove(board);
    auto end = std::chrono::steady_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "AI decided move in " << duration << " ms.\n";
}
