//#include "Minefield.h"
#include "Tile.h"

#include <vector>
#include <cstdlib>
#include <iostream>
#include <ctime>

#include <SFML/Graphics.hpp>


class Minefield {
public:
    Minefield(int height, int width, int mines_count): height(height), width(width), mines_count(mines_count),
    mines_left(mines_count){
       create_minefield();
       set_adjacent_tiles_all_tiles();
    };

    Minefield() {
        set_difficulty();
        create_minefield();
        set_adjacent_tiles_all_tiles();
    };

    int get_width() {
        return width;
    }

    int get_height() {
        return height;
    }

    /**
     * Draws all tiles to the window
     */
    void draw_minefield(sf::RenderWindow& window) {
        for (auto &row: minefield) {
            for (auto &tile: row) {
                tile.draw_tile(window);
            }
        }
    }


    int mouse_click(sf::Vector2u mouse_position, sf::Mouse::Button button, sf::RenderWindow& window) {
        if (is_within_minefield(mouse_position)) {
            Tile& clicked_tile = find_clicked_tile(mouse_position);
            if (first_click) { //this makes sure the first clicked tile isnt a mine
                bomb_tiles = deploy_bombs(clicked_tile);
            }

            switch (execute_click(clicked_tile, button)) {
                case 1:
                    end_the_game(window, true);
                    return 1;
                case -1:
                    end_the_game(window, false);
                    return 1;
            }
            first_click = false;
        }
        return 0;
    }

    void flag_tile(const Tile& tile) {
        flag_tile(tile.get_row(), tile.get_col());
    }

    void flag_tile(int row, int col) {
        minefield[row][col].flag(mines_left);
    }

    void click_tile(Tile& tile, sf::Mouse::Button button){
        click_tile(tile.get_row(), tile.get_col(), button);
    }

    void click_tile(int row, int col, sf::Mouse::Button button) {
        execute_click(minefield[row][col], button);
    }

    void end_the_game(sf::RenderWindow& window, bool game_won) {
        std::cout << "\n\n!!!\nGAME OVER\n!!!" << std::endl;
        if (game_won) {
            std::cout << "VICTORY" << std::endl;
        } else
            std::cout << "LOSS" << std::endl;
        std::cout << "Press any mouse key to close the window." << std::endl;

        window.clear(sf::Color(125, 125, 125));
        draw_minefield(window);
        window.display();
    }


    Tile& get_tile(int x, int y) {
        return minefield[x][y];
    }

    int get_mines_left() {
        return mines_left;
    }

private:
    const sf::Vector2u starting_point = {0, 0};
    int width = 0;
    int height = 0;
    int mines_count = 0;
    int mines_left = 0;
    bool first_click = true;
    std::vector<std::vector<Tile>> minefield;
    std::vector<Tile*> bomb_tiles;

    void set_difficulty() {
        char dif;
        std::cout << "Choose your difficulty: " << std::endl;
        std::cout << "Easy   (Height: 9 Width: 9 Mines: 10)   [1]" << std::endl;
        std::cout << "Normal (Height: 16 Width: 16 Mines: 40) [2]" << std::endl;
        std::cout << "Hard   (Height: 16 Width: 30 Mines: 99) [3]" << std::endl;
        std::cout << "Custom [anything else]" << std::endl;
        std::cin >> dif;

        switch(dif) {
            case '1':
                height = 9;
                width = 9;
                mines_count = 10;
                break;
            case '2':
                height = 16;
                width = 16;
                mines_count = 40;
                break;
            case '3':
                height = 16;
                width = 30;
                mines_count = 99;
                break;
            default:
                initialize_minefield();
        }
        mines_left = mines_count;
    }

    void initialize_minefield() {
        std::cout << "Choose height of the minefield:" << std::endl;
        std::cin >> height;
        std::cout << "Choose width of the minefield:" << std::endl;
        std::cin >> width;
        std::cout << "Choose amount of bombs:" << std::endl;
        std::cin >> mines_count;
        mines_left = mines_count;
    }

    void create_minefield() {
        for (int i = 0; i < height; ++i) {
            std::vector<Tile> row;
            for (int j = 0; j < width; ++j) {
                Tile tile(i, j);
                sf::Vector2u pos = sf::Vector2u(starting_point.x + j * tile.get_size().x,
                                starting_point.y + i * tile.get_size().y);
                tile.set_position(pos);
                tile.set_probability(mines_left);

                row.push_back(tile);
            }
            minefield.push_back(row);
        }
        //reload all textures
        for (auto &row: minefield) {
            for (auto &tile: row) {
                tile.load_texture();
                //tile.set_origin_to_middle();
            }
        }
    }


    std::vector<Tile*> deploy_bombs(Tile& first_clicked_tile) {
        std::vector<Tile*> mines;
        int i = 0;

        //srand(time(nullptr));
        srand(5); // for debugging
        while (i < mines_count) {
            int x = rand() % height;
            int y = rand() % width;
            if (first_clicked_tile.get_col() != y && first_clicked_tile.get_row() != x && !minefield[x][y].get_is_bomb()) {
                minefield[x][y].set_is_bomb();
                mines.push_back(&minefield[x][y]);
            }
            else
                --i; //we repeat once more
            ++i;
        }
        return mines;
    }

    int execute_click(Tile& tile, sf::Mouse::Button button) {
        if (!tile.get_is_covered())
            return 0;

        if (button == sf::Mouse::Right) {
            tile.flag(mines_left);
            if (check_game_won())
                return 1;
        }
        else { //left button
            if (!tile.is_flagged()) {
                if (tile.get_is_bomb() && !first_click) {
                    explode(tile);
                    return -1;
                }
                else {
                    int adjacent_bombs_amount = count_adjacent_bombs(tile);
                    tile.uncover_tile(adjacent_bombs_amount);
                    if (!adjacent_bombs_amount) {//if there are no bombs around
                        for (auto &adjacent_tile: tile.get_adjacent_tiles()) {
                            execute_click(*adjacent_tile, sf::Mouse::Left);
                        }
                    }
                }
            }
        }
        return 0;
    }

    bool check_game_won() {
        if (mines_left == 0) {
            for (auto tile_p: bomb_tiles) {
                if (!tile_p->is_flagged()) {
                    return false;
                }
            }
            return true;
        }
        return false;
    }


    /**
     * We go over every tile to check if it si bomb (to display it) and if it si flagged (to display incorrect bomb)
     * @param triggered_bomb
     */
    void explode(Tile& triggered_bomb) {
        for (auto &row: minefield) {
            for (auto &tile: row) {
                if (tile.is_flagged()) {
                    if (!tile.get_is_bomb())
                        tile.display_texture(Tile::INCORRECT_BOMB);
                }
                else if (tile.get_is_bomb()) {
                    tile.display_texture(Tile::BOMB);
                }
            }
        }
        triggered_bomb.display_texture(Tile::EXPLODED_BOMB);
    }


    int count_adjacent_bombs(Tile& tile) {
        int counter = 0;

        std::vector<Tile*> adjacent_tiles = tile.get_adjacent_tiles();
        for (auto &_tile: adjacent_tiles) {
            if (_tile->get_is_bomb())
                ++counter;
        }
        return counter;
    }


    void set_adjacent_tiles_all_tiles() {
        for (auto &row: minefield) {
            for (auto &tile: row) {
                tile.set_adjacent_tiles(minefield);
            }
        }
    }


    bool is_within_minefield(sf::Vector2u position) {
        if (!height || !width) {
            return false; //invalid minefield size
        }
        sf::Vector2u tile_size = minefield[0][0].get_size();
        return ((position.x <= starting_point.x + width * tile_size.x) && (position.x >= starting_point.x)) &&
               ((position.y <= starting_point.y + height * tile_size.y) && (position.y >= starting_point.y));
    }


    /*
     * This should ALWAYS return a correct clicked tile
     * Last return is here only for debugging purpose
     */
    Tile& find_clicked_tile(sf::Vector2u mouse_position) {
        for (auto &row: minefield) {
            for (auto &tile: row) {
                if (tile.contains_point(mouse_position)) {
                    return tile;
                }
            }
        }

        return minefield[0][0];
    }
};



