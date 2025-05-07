#include "AIPlayer.hpp"
#include <limits>
#include <thread>
#include <chrono>
#include <future>
#include <iostream>

AIPlayer::AIPlayer(char symbol) : Player(symbol) {
    opponentSymbol = (symbol == 'X') ? 'O' : 'X';
}

// --- 強化版：評估整體盤面 --- //
int AIPlayer::evaluateBoard(Board& board) {
    int score = 0;
    std::vector<std::vector<char>> directions;

    // 橫列
    for (int i = 0; i < Board::SIZE; ++i) {
        std::vector<char> row;
        for (int j = 0; j < Board::SIZE; ++j) {
            row.push_back(board.getCell(i, j));
        }
        score += evaluateLine(row, symbol);
        score -= evaluateLine(row, opponentSymbol);
    }

    // 直行
    for (int j = 0; j < Board::SIZE; ++j) {
        std::vector<char> col;
        for (int i = 0; i < Board::SIZE; ++i) {
            col.push_back(board.getCell(i, j));
        }
        score += evaluateLine(col, symbol);
        score -= evaluateLine(col, opponentSymbol);
    }

    // 左上到右下斜線
    for (int k = 0; k <= 2 * (Board::SIZE - 1); ++k) {
        std::vector<char> diag;
        for (int i = 0; i < Board::SIZE; ++i) {
            int j = k - i;
            if (j >= 0 && j < Board::SIZE) {
                diag.push_back(board.getCell(i, j));
            }
        }
        score += evaluateLine(diag, symbol);
        score -= evaluateLine(diag, opponentSymbol);
    }

    // 右上到左下斜線
    for (int k = -Board::SIZE + 1; k < Board::SIZE; ++k) {
        std::vector<char> diag;
        for (int i = 0; i < Board::SIZE; ++i) {
            int j = i - k;
            if (j >= 0 && j < Board::SIZE) {
                diag.push_back(board.getCell(i, j));
            }
        }
        score += evaluateLine(diag, symbol);
        score -= evaluateLine(diag, opponentSymbol);
    }

    return score;
}

// --- 單行評分：根據連線數與封閉程度 --- //
int AIPlayer::evaluateLine(const std::vector<char>& line, char currentSymbol) {
    int score = 0;
    const int n = line.size();

    for (int i = 0; i <= n - 5; ++i) {
        int count = 0;
        bool blockedLeft = false, blockedRight = false;

        for (int j = 0; j < 5; ++j) {
            if (line[i + j] == currentSymbol) {
                count++;
            } else if (line[i + j] != '.') {
                count = -1;
                break;
            }
        }

        if (count > 0) {
            if (i == 0 || line[i - 1] != '.') blockedLeft = true;
            if (i + 5 >= n || line[i + 5] != '.') blockedRight = true;

            int base = 1;
            if (count == 5) base = 100000;
            else if (count == 4) base = 10000;
            else if (count == 3) base = 1000;
            else if (count == 2) base = 100;
            else base = 10;

            if (blockedLeft && blockedRight) base /= 4;
            else if (blockedLeft || blockedRight) base /= 2;

            score += base;
        }
    }
    return score;
}

std::vector<std::pair<int, int>> AIPlayer::generateMoves(Board& board) {
    std::vector<std::pair<int, int>> moves;
    for (int i = 0; i < Board::SIZE; ++i) {
        for (int j = 0; j < Board::SIZE; ++j) {
            if (board.getCell(i, j) == '.') {
                moves.push_back({i, j});
            }
        }
    }
    return moves;
}

int AIPlayer::minimax(Board& board, int depth, bool maximizing, int alpha, int beta) {
    if (depth == 0 || board.isFull()) {
        return evaluateBoard(board);
    }

    auto moves = generateMoves(board);
    if (maximizing) {
        int maxEval = std::numeric_limits<int>::min();
        for (auto [r, c] : moves) {
            board.placePiece(r, c, symbol);
            if (board.isWin(r, c, symbol)) {
                board.placePiece(r, c, '.');
                return 100000;
            }
            int eval = minimax(board, depth - 1, false, alpha, beta);
            board.placePiece(r, c, '.');
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) break;
        }
        return maxEval;
    } else {
        int minEval = std::numeric_limits<int>::max();
        for (auto [r, c] : moves) {
            board.placePiece(r, c, opponentSymbol);
            if (board.isWin(r, c, opponentSymbol)) {
                board.placePiece(r, c, '.');
                return -100000;
            }
            int eval = minimax(board, depth - 1, true, alpha, beta);
            board.placePiece(r, c, '.');
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) break;
        }
        return minEval;
    }
}

// --- 平行運算版本：用 std::async 執行每個落子位置的搜尋 --- //
std::pair<int, int> AIPlayer::findBestMove(Board& board) {
    int bestScore = std::numeric_limits<int>::min();
    std::pair<int, int> bestMove = {-1, -1};

    std::vector<std::future<std::pair<int, std::pair<int, int>>>> futures;
    for (auto [r, c] : generateMoves(board)) {
        if (board.getCell(r, c) != '.') continue;

        // ✅ 平行運算：每一個位置都用 std::async 開一條背景執行緒
        futures.push_back(std::async(std::launch::async, [&, r, c]() {
            Board copy = board;
            copy.placePiece(r, c, symbol);
            int score = minimax(copy, 3, false, INT_MIN, INT_MAX);
            return std::make_pair(score, std::make_pair(r, c));
        }));
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
    std::tie(row, col) = findBestMove(board);
    auto end = std::chrono::steady_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "AI decided move in " << duration << " ms.\n";
}
