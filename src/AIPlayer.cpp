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
        score -= evaluateLine(row, opponentSymbol); // 加入防守視角
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

            int base = 0;
            if (count == 5) base = 100000;
            else if (count == 4) base = (blockedLeft || blockedRight) ? 10000 : 50000;
            else if (count == 3) base = (!blockedLeft && !blockedRight) ? 5000 : 1000;
            else if (count == 2) base = 200;
            else base = 50;

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
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    std::chrono::milliseconds maxTime(1000);  // 設定最大思考時間為 1000 毫秒
    
    uint64_t hash = computeZobristHash(board);
    auto now = std::chrono::high_resolution_clock::now();
    if (now - startTime > maxTime) {
        return evaluateBoard(board);  // 超過時間限制，直接返回評分
    }
    // 檢查轉置表
    if (transpositionTable.count(hash)) {
        // 如果轉置表中有此狀態，返回已儲存的評分
        return transpositionTable[hash];
    }

    if (depth == 0 || board.isFull()) {
        int eval = evaluateBoard(board);
        // 將此狀態和評分存入轉置表
        transpositionTable[hash] = eval;
        return eval;
    }

    auto moves = generateMoves(board);
    int bestVal = maximizing ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();

    // 使用平行化或順序處理
    std::vector<std::future<int>> futures;

    for (auto [r, c] : moves) {
        board.placePiece(r, c, maximizing ? symbol : opponentSymbol);

        if (board.isWin(r, c, maximizing ? symbol : opponentSymbol)) {
            int score = maximizing ? 100000 : -100000;
            bestVal = maximizing ? std::max(bestVal, score) : std::min(bestVal, score);
            board.placePiece(r, c, '.');
            if (maximizing) {
                alpha = std::max(alpha, bestVal);
            } else {
                beta = std::min(beta, bestVal);
            }
            if (beta <= alpha) break;
            continue;
        }

        futures.push_back(std::async(std::launch::async, [this, r, c, maximizing, depth, alpha, beta, &board]() {
            int score = minimax(board, depth - 1, !maximizing, alpha, beta);
            board.placePiece(r, c, '.');
            return score;
        }));
    }

    for (auto& f : futures) {
        int score = f.get();
        if (maximizing) {
            bestVal = std::max(bestVal, score);
            alpha = std::max(alpha, score);
        } else {
            bestVal = std::min(bestVal, score);
            beta = std::min(beta, score);
        }

        if (beta <= alpha) break;
    }

    // 存儲最終結果到轉置表
    transpositionTable[hash] = bestVal;
    return bestVal;
}


std::pair<int, int> AIPlayer::findBestMove(Board& board) {
    // 1. 優先檢查是否有可以獲勝的步驟
    auto winningMove = findWinningMoveIfAvailable(board);
    if (winningMove) {
        return *winningMove;  // 如果有獲勝步驟，直接返回
    }

    // 2. 優先阻止對手的四連或三連威脅
    auto blockingMove = findBlockingMoveIfThreat(board);
    if (blockingMove) {
        return *blockingMove;  // 如果有阻止對手的步驟，返回
    }

    // 3. 沒有威脅時：使用 minimax + evaluateBoard() 找最好的進攻位置
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
                int score = minimax(copy, 4, false, INT_MIN, INT_MAX);
                return std::make_pair(score, std::make_pair(r, c));
            }));
        } else {
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
bool AIPlayer::hasDangerousThree(Board& board, char checkSymbol) {
    const int SIZE = Board::SIZE;
    auto isValid = [&](int r, int c) {
        return r >= 0 && r < SIZE && c >= 0 && c < SIZE;
    };

    auto checkPattern = [&](int r, int c, int dr, int dc) {
        std::string line;
        for (int i = -1; i <= 4; ++i) {  // 掃 6 格長的區段，涵蓋中間空格情形
            int nr = r + dr * i;
            int nc = c + dc * i;
            if (!isValid(nr, nc)) {
                line += " ";  // 邊界算阻擋
            } else {
                line += board.getCell(nr, nc);
            }
        }

        // 危險三連模式：中間插空或兩端都空的三個連續棋子
        return (line.find(".XXX.") != std::string::npos ||
                line.find("X.XX")  != std::string::npos ||
                line.find("XX.X")  != std::string::npos);
    };

    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            if (board.getCell(i, j) != checkSymbol) continue;

            // 檢查四個方向：橫、直、斜（↘）、反斜（↙）
            if (checkPattern(i, j, 0, 1) ||     // →
                checkPattern(i, j, 1, 0) ||     // ↓
                checkPattern(i, j, 1, 1) ||     // ↘
                checkPattern(i, j, 1, -1)) {    // ↙
                return true;
            }
        }
    }

    return false;
}
std::optional<std::pair<int, int>> AIPlayer::findBlockingMoveIfThreat(Board& board) {
    auto checkLine = [&](int startR, int startC, int dr, int dc, int length) -> std::optional<std::pair<int, int>> {
        int count = 0;
        std::pair<int, int> emptySpot = {-1, -1};

        for (int i = 0; i < length; ++i) {
            int r = startR + i * dr;
            int c = startC + i * dc;
            if (r < 0 || r >= Board::SIZE || c < 0 || c >= Board::SIZE)
                return std::nullopt;

            char cell = board.getCell(r, c);
            if (cell == opponentSymbol) {
                count++;
            } else if (cell == '.') {
                if (emptySpot.first == -1)
                    emptySpot = {r, c};
                else
                    return std::nullopt; // 多於一個空格，不算連線威脅
            } else {
                return std::nullopt;
            }
        }

        if ((length == 5 && count == 4) || (length == 4 && count == 3))
            return emptySpot;

        return std::nullopt;
    };

    // 優先檢查 4 連（5 格中有一空）=> 急需阻止
    for (int r = 0; r < Board::SIZE; ++r) {
        for (int c = 0; c < Board::SIZE; ++c) {
            for (auto [dr, dc] : std::vector<std::pair<int, int>>{{0,1}, {1,0}, {1,1}, {1,-1}}) {
                auto move = checkLine(r, c, dr, dc, 5);
                if (move) return move;
            }
        }
    }

    // 若無，再檢查 3 連（4 格中有一空）=> 提前防守
    for (int r = 0; r < Board::SIZE; ++r) {
        for (int c = 0; c < Board::SIZE; ++c) {
            for (auto [dr, dc] : std::vector<std::pair<int, int>>{{0,1}, {1,0}, {1,1}, {1,-1}}) {
                auto move = checkLine(r, c, dr, dc, 4);
                if (move) return move;
            }
        }
    }

    return std::nullopt;
}


std::optional<std::pair<int, int>> AIPlayer::findWinningMoveIfAvailable(Board& board) {
    auto moves = generateMoves(board);

    for (auto& move : moves) {
        auto [r, c] = move;
        Board copy = board;
        copy.placePiece(r, c, symbol);  // 嘗試這步驟

        if (copy.isWin(r, c, symbol)) {
            return move;  // 如果這步驟可以獲勝，返回
        }
    }

    return std::nullopt;  // 如果沒有獲勝的步驟，返回 nullopt
}
