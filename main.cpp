#include <SFML/Graphics.hpp>
#include "Minefield.h"
#include "MinesweeperAI.h"

#include <iostream>
#include <unistd.h>

using namespace sf;

int main() {
    bool end_the_game = false;

    //Minefield minefield(20, 20, 50);
    //Minefield minefield(30, 40, 280);
    //Minefield minefield(40, 40, 300);

    Minefield minefield;

    RenderWindow window(VideoMode(minefield.get_width()*16, minefield.get_height()*16), "Minesweeper", sf::Style::Close );

    //Icon only
    Image icon;
    icon.loadFromFile("./bomb_img.jpg");
    window.setIcon(512, 512 , icon.getPixelsPtr());

    MinesweeperAI ai;
    bool launch_ai = false;

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            switch(event.type) {
                case Event::Closed:
                    window.close();
                    break;
                case Event::MouseButtonPressed:
                    if (end_the_game) { //so that we end the game after a mouse click
                        return 0;
                    }
                    if (event.mouseButton.button == Mouse::Right || event.mouseButton.button == Mouse::Left) {
                        Vector2u mouse_pos = (Vector2u) Mouse::getPosition(window);
                        if (minefield.mouse_click(mouse_pos, event.mouseButton.button, window)) {
                            end_the_game = true;
                        }
                    }
                    break;
            }
        }
        window.clear(Color(125, 125, 125));

        // display minefield
        minefield.draw_minefield(window);
        window.display();
    }
    return 0;
}
