#include <SFML/Graphics.hpp>
#include <memory>
#include <iostream>
#include "HumanPlayer.hpp"
#include "AIPlayer.hpp"
#include "GameWindow.hpp"

const float VIRTUAL_WIDTH = 600;
const float VIRTUAL_HEIGHT = 660;  // 多出 60px 顯示資訊欄


bool showModeSelection(sf::RenderWindow& window, sf::Font& font, bool& isPvP) {
    sf::Text title("Select Game Mode", font, 36);
    title.setPosition(140, 50);
    title.setFillColor(sf::Color::Black);

    sf::Text pvpText("1. Player vs Player", font, 28);
    pvpText.setPosition(180, 150);
    pvpText.setFillColor(sf::Color::Blue);

    sf::Text pvcText("2. Player vs Computer", font, 28);
    pvcText.setPosition(180, 220);
    pvcText.setFillColor(sf::Color::Blue);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return false;
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                auto worldPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                if (pvpText.getGlobalBounds().contains(worldPos)) {
                    isPvP = true;
                    return true;
                } else if (pvcText.getGlobalBounds().contains(worldPos)) {
                    isPvP = false;
                    return true;
                }
            }
        }

        window.clear(sf::Color(255, 255, 240));
        window.draw(title);
        window.draw(pvpText);
        window.draw(pvcText);
        window.display();
    }

    return false;  // 視窗被關閉
}

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
        if (!showModeSelection(window, font, isPvP))
            break;

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
