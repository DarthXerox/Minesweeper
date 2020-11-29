//#include "Minefield.h"
#include "Tile.h"

#include <vector>
#include <cstdlib>
#include <iostream>
#include <ctime>

#include <SFML/Graphics.hpp>


class Minefield {
public:
    enum click_outcome {
        END_GAME,
        CONTINUE_GAME,
        START_AI
    };

    enum within_minefield {
        OUT_OF_BOUNDS = 0,
        TILE,
        AI_BUTTON
    };

    Minefield(size_t height, size_t width, size_t mines_count): height(height), width(width), mines_count(mines_count),
    mines_left(mines_count){
       create_minefield();
       set_adjacent_tiles_all_tiles();
    };

    Minefield() {
        set_difficulty();
        create_minefield();
        set_adjacent_tiles_all_tiles();
    };

    size_t get_width() const {
        return width;
    }

    size_t get_height() const {
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
        window.draw(ai_button);
        window.draw(text);
    }

    click_outcome mouse_click_tile(Tile& clicked_tile, sf::Mouse::Button button) {
        if (first_click) { //this makes sure the first clicked tile isnt a mine
            bomb_tiles = deploy_bombs(clicked_tile);
        }

        switch (execute_click(clicked_tile, button)) {
            case 1:
                end_the_game(true);
                return END_GAME;
            case -1:
                end_the_game(false);
                return END_GAME;
        }
        first_click = false;

        return CONTINUE_GAME;
    }


    click_outcome mouse_click(sf::Vector2u mouse_position, sf::Mouse::Button button) {
        if (is_within_minefield(mouse_position) == AI_BUTTON) {
            if (!first_click) {
                return START_AI;
            }
        }
        else if (is_within_minefield(mouse_position) == TILE) {
            return mouse_click_tile(find_clicked_tile(mouse_position), button);
        }
        return CONTINUE_GAME;
    }

    void flag_tile(const Tile& tile) {
        flag_tile(tile.get_row(), tile.get_col());
    }

    void flag_tile(size_t row, size_t col) {
        minefield[row][col].flag(mines_left);
    }

    void click_tile(const Tile& tile, sf::Mouse::Button button){
        click_tile(tile.get_row(), tile.get_col(), button);
    }

    void click_tile(size_t row, size_t col, sf::Mouse::Button button) {
        execute_click(minefield[row][col], button);
    }

    void end_the_game(bool game_won) {
        std::cout << "\n\n!!!\nGAME OVER\n!!!" << std::endl;
        if (game_won) {
            std::cout << "VICTORY" << std::endl;
        } else
            std::cout << "LOSS" << std::endl;
        std::cout << "Press any mouse key to close the window." << std::endl;

        //window.clear(sf::Color(125, 125, 125));
        //draw_minefield(window);
        //window.display();
    }


    Tile& get_tile(size_t x, size_t y) {
        return minefield[x][y];
    }

    size_t get_mines_left() const {
        return mines_left;
    }

    bool check_game_won() {
        if (check_all_bombs_flagged()) {
            for (auto& row : minefield) {
                for (auto tile_ptr : row) {
                    if (tile_ptr.get_is_covered() && tile_ptr.get_value() != Tile::tile_values::FLAG) {
                        return false; // all other tiles must be clear
                    }
                }
            }

            return true;
        }

        return false;
    }

private:
    const sf::Vector2u starting_point = {0, 0};
    size_t height = 0;
    size_t width = 0;
    size_t mines_count = 0;
    size_t mines_left = 0;
    bool first_click = true;
    std::vector<std::vector<Tile>> minefield;
    std::vector<Tile*> bomb_tiles;
    sf::Sprite ai_button;
    sf::Texture ai_button_texture;
    sf::Font font;
    sf::Text text;

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
        for (size_t i = 0; i < height; ++i) {
            std::vector<Tile> row;
            for (size_t j = 0; j < width; ++j) {
                Tile tile(i, j);
                sf::Vector2u pos = sf::Vector2u(starting_point.x + j * tile.get_size().x,
                                starting_point.y + i * tile.get_size().y);
                tile.set_position(pos);
                //tile.set_probability(mines_left);

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

        if (!font.loadFromFile("./res/OpenSans-Regular.ttf"))
        {
            std::terminate();
        }
        text.setFont(font);
        text.setString("Start AI");
        text.setCharacterSize(16);
        text.setFillColor(sf::Color::Black);
        text.setStyle(sf::Text::Bold);
        text.setPosition(width * Tile::width / 2 - 2 * Tile::width, (height + 0.5f) * Tile::height - 2);

        ai_button.setOrigin(0, 0);
        //ai_button_texture = sf::Texture();
        ai_button_texture.loadFromFile("./res/tiles_img.png");
        ai_button.setTexture(ai_button_texture);
        sf::IntRect new_texture_rect = sf::IntRect(0, 0,
                                                   Tile::width, Tile::height);
        ai_button.setTextureRect(new_texture_rect);
        ai_button.setScale(width, 2);
        ai_button.move(0, ((float ) height * Tile::height));
    }


    std::vector<Tile*> deploy_bombs(Tile& first_clicked_tile) {
        std::vector<Tile*> mines;
        size_t i = 0;

        //srand(time(nullptr));
        srand(5); // for debugging
        while (i < mines_count) {
            size_t x = rand() % height;
            size_t y = rand() % width;
            if (first_clicked_tile.get_col() != y && first_clicked_tile.get_row() != x && !minefield[x][y].get_is_bomb()) {
                minefield[x][y].set_is_bomb();
                mines.push_back(&minefield[x][y]);
                ++i;
            }
        }
        return mines;
    }

    int execute_click(Tile& tile, sf::Mouse::Button button) {
        if (!tile.get_is_covered())
            return 0;

        if (button == sf::Mouse::Right ) {
            tile.flag(mines_left);

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
        if (check_game_won())
            return 1;
        return 0;
    }



    bool check_all_bombs_flagged() {
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


    within_minefield is_within_minefield(sf::Vector2u position) const {
        if (!height || !width) {
            return OUT_OF_BOUNDS; //invalid minefield size
        }
        if ((position.x <= starting_point.x + width * Tile::width) && (position.x >= starting_point.x)
            && (position.y >= starting_point.y)) {
            if (position.y <= starting_point.y + height * Tile::height) {
                return TILE;
            }
            if (position.y <= starting_point.y + (height + 2) * Tile::height) {
                return AI_BUTTON;
            }
        }
        return OUT_OF_BOUNDS;
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



