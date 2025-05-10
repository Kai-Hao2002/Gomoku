#include "Board.hpp"

Board::Board() : grid(SIZE, std::vector<char>(SIZE, '.')) {}


void Board::reset() {
    grid = std::vector<std::vector<char>>(SIZE, std::vector<char>(SIZE, ' '));
}


bool Board::placePiece(int row, int col, char symbol) {
    if (row < 0 || row >= SIZE || col < 0 || col >= SIZE || grid[row][col] != '.') return false;
    grid[row][col] = symbol;
    return true;
}

char Board::getCell(int row, int col) const {
    return grid[row][col];
}

bool Board::isFull() const {
    for (const auto& row : grid)
        for (char c : row)
            if (c == '.') return false;
    return true;
}

bool Board::isWin(int row, int col, char symbol)  {  // 添加 const
    int dirs[4][2] = {{0,1},{1,0},{1,1},{1,-1}};
    for (auto& dir : dirs) {
        int dr = dir[0], dc = dir[1];
        int count = 1;
        for (int i = 1; i < 5; ++i) {
            int r = row + dr*i, c = col + dc*i;
            if (r < 0 || r >= SIZE || c < 0 || c >= SIZE || grid[r][c] != symbol) break;
            count++;
        }
        for (int i = 1; i < 5; ++i) {
            int r = row - dr*i, c = col - dc*i;
            if (r < 0 || r >= SIZE || c < 0 || c >= SIZE || grid[r][c] != symbol) break;
            count++;
        }
        if (count >= 5) return true;
    }
    return false;
}






