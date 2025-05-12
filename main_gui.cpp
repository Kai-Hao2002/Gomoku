#include <SFML/Graphics.hpp>
#include <memory>
#include <iostream>
#include "HumanPlayer.hpp"
#include "AIPlayer.hpp"
#include "GameWindow.hpp"

const float VIRTUAL_WIDTH = 600;
const float VIRTUAL_HEIGHT = 660;  // 多出 60px 顯示資訊欄

int main() {
    sf::RenderWindow window(sf::VideoMode(VIRTUAL_WIDTH, VIRTUAL_HEIGHT), "Gomoku", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Failed to load font!\n";
        return -1;
    }

    while (window.isOpen()) {
        bool isPvP;
        if (!GameWindow::showModeSelection(window, font, isPvP)) return -1;

        bool restart = false;
        do {
            std::unique_ptr<Player> p1 = std::make_unique<HumanPlayer>('X');
            std::unique_ptr<Player> p2;

            if (isPvP) {
                p2 = std::make_unique<HumanPlayer>('O');
            } else {
                p2 = std::make_unique<AIPlayer>('O');
            }

            GameWindow gameWindow(std::move(p1), std::move(p2), isPvP);
            restart = gameWindow.run(window, font);  // ✅ 修正：加上 font 傳入

            if (!window.isOpen()) break;

        } while (restart);  // 若使用者返回主選單，則重新建構並開始新遊戲
    }

    return 0;
}
