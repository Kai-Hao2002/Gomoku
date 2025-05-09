#ifndef BOARD_HPP
#define BOARD_HPP

#include <vector>

class Board {
public:
    static const int SIZE = 16;  // 改為 16x16
    Board();

    bool placePiece(int row, int col, char symbol);
    char getCell(int row, int col) const;
    bool isFull() const;
    bool isWin(int row, int col, char symbol);
    void reset(); // ← 加這一行

private:
    std::vector<std::vector<char>> grid;
};

#endif
