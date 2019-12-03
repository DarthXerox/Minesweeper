#include <SFML/Graphics.hpp>
#include "Minefield.h"
#include "MinesweeperAI.h"

#include <iostream>
#include <unistd.h>

using namespace sf;

int main() {
    bool end_the_game = false;

    Minefield minefield(20, 20, 50);
    //Minefield minefield(30, 40, 280);
    //Minefield minefield;

    RenderWindow window(VideoMode(minefield.get_width()*16, minefield.get_height()*16), "Minesweeper", sf::Style::Close );

    //Icon only
    Image icon;
    icon.loadFromFile("./bomb_img.jpg");
    window.setIcon(512, 512 , icon.getPixelsPtr());

    MinesweeperAI ai;
    bool launch_ai = false;

    //int last_tile_row = 0, last_tile_col = 0;
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            switch(event.type) {
                case Event::Closed:
                    window.close();
                    break;
                case Event::MouseButtonPressed:
                    if (!launch_ai) {
                        if (end_the_game) { //so that we end the game after a mouse click
                        return 0;
                        }
                        if (event.mouseButton.button == Mouse::Right || event.mouseButton.button == Mouse::Left) {
                            Vector2u mouse_pos = (Vector2u) Mouse::getPosition(window);
                            if (minefield.mouse_click(mouse_pos, event.mouseButton.button, window)) {
                                end_the_game = true;
                            }
                        }
                        window.clear(Color(125, 125, 125));

                        // display minefield
                        minefield.draw_minefield(window);
                        window.display();

                        //std::cout <<"Switch AI? [1/0]"<<std::endl;
                        //std::cin >>launch_ai;
                    } 
                    break;
            }
        }
        /*if (launch_ai){
            launch_ai = ai.check_all_numbers(minefield, true);
            /*if (!launch_ai) {
                last_tile_row = 0;
                last_tile_col = 0;
                launch_ai = ai.check_all_numbers(minefield, true, last_tile_row, last_tile_col);                
            }
            if (minefield.mines_left == 0) {
                end_the_game = true;
                minefield.end_the_game(window, 1);
            }*/
            //ai.uncover_all_adjacent_covered_tiles(minefield.get_adjacent_tiles(minefield.get_tile(1,1)), minefield);
            //usleep(200); // 2s       
            //launch_ai = false;   
        //}

        window.clear(Color(125, 125, 125));

        // display minefield
        minefield.draw_minefield(window);
        window.display();
    }
    return 0;
}
