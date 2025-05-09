#include <SFML/Graphics.hpp>
#include <memory>
#include <iostream>
#include "HumanPlayer.hpp"
#include "AIPlayer.hpp"
#include "GameWindow.hpp"

const float VIRTUAL_WIDTH = 640;
const float VIRTUAL_HEIGHT = 640;

int main() {
    sf::RenderWindow window(sf::VideoMode(VIRTUAL_WIDTH, VIRTUAL_HEIGHT), "Gomoku", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Failed to load font!\n";
        return 1;
    }

    sf::Text title("Select Game Mode", font, 36);
    title.setPosition(140, 50);
    title.setFillColor(sf::Color::Black);

    sf::Text pvpText("1. Player vs Player", font, 28);
    pvpText.setPosition(180, 150);
    pvpText.setFillColor(sf::Color::Blue);

    sf::Text pvcText("2. Player vs Computer", font, 28);
    pvcText.setPosition(180, 220);
    pvcText.setFillColor(sf::Color::Blue);

    bool modeSelected = false;
    bool isPvP = true;

    while (window.isOpen() && !modeSelected) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                auto mousePos = sf::Mouse::getPosition(window);
                auto worldPos = window.mapPixelToCoords(mousePos);

                if (pvpText.getGlobalBounds().contains(worldPos)) {
                    isPvP = true;
                    modeSelected = true;
                } else if (pvcText.getGlobalBounds().contains(worldPos)) {
                    isPvP = false;
                    modeSelected = true;
                }
            }
        }

        window.clear(sf::Color(255, 255, 240));
        window.draw(title);
        window.draw(pvpText);
        window.draw(pvcText);
        window.display();
    }

    if (!modeSelected)
        return 0;

    std::unique_ptr<Player> p1 = std::make_unique<HumanPlayer>('X');
    std::unique_ptr<Player> p2;

    if (isPvP) {
        p2 = std::make_unique<HumanPlayer>('O');
    } else {
        p2 = std::make_unique<AIPlayer>('O');
    }

    GameWindow gameWindow(std::move(p1), std::move(p2), isPvP);
    gameWindow.run(window);

    return 0;
}  