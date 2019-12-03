#include <SFML/Graphics.hpp>
#include <iostream>


class Tile {
public:
    Tile(int row, int col): row(row), col(col) {
        load_texture();
    }

    /*
    Tile(unsigned x, unsigned y) {
        square.setPosition(x, y);
        load_texture();
    }
     */

    void set_origin_to_middle() {
        square.setOrigin(tile_size.x / 2.0, tile_size.y / 2.0);
    }


    enum tile_values {
        CLEAR_COVERED = 0,
        FLAG = 1,
        QUESTION_MARK_COVERED = 2,

        EXPLODED_BOMB = 3,
        INCORRECT_BOMB = 4,
        BOMB = 5,
        QUESTION_MARK_UNCOVERED = 6,
        NUM_8 = 7,
        NUM_7 = 8,
        NUM_6 = 9,
        NUM_5 = 10,
        NUM_4 = 11,
        NUM_3 = 12,
        NUM_2 = 13,
        NUM_1 = 14,
        CLEAR_UNCOVERED = 15
    };

    std::vector<Tile*> get_adjacent_tiles() {
        return adjacent_tiles;
    }

    void load_texture() {
        texture.loadFromFile("./tiles_img.png");
        square.setTexture(texture);

        tile_size.x = texture.getSize().x;
        tile_size.y = texture.getSize().y/16;
        display_texture();
    }


    void display_texture() {
        set_texture(value);
    }

    void display_texture(tile_values _value) {
        value = _value;
        set_texture(value);
    }

    bool is_flagged() {
        return value == FLAG;
    }

    void flag(int& mines_left) {
        if (get_is_covered()) { //this check is probably not necessary
            switch (value) {
                case CLEAR_COVERED:
                    value = FLAG;
                    --mines_left;
                    std::cout << "Mines left: " << mines_left << std::endl;
                    break;
                case FLAG:
                    value = QUESTION_MARK_COVERED;
                    ++mines_left;
                    std::cout << "Mines left: " << mines_left << std::endl;
                    break;
                case QUESTION_MARK_COVERED:
                    value = CLEAR_COVERED;
                    break;
                default:
                    break;
            }
            display_texture();
        }
    }

    int uncover_tile(unsigned adjacent_bombs_amount) {
        set_probability(0.0);

        if (is_bomb) {
            value = EXPLODED_BOMB;
            display_texture();
            return 0;
        }
        if (adjacent_bombs_amount == 0) {
            value = CLEAR_UNCOVERED;
            display_texture();
            return 1;
        }
        //any number of adjacent bombs
        value = static_cast<tile_values>(NUM_1 + 1 - adjacent_bombs_amount);
        display_texture();
        return 0;
    }

    void set_position(sf::Vector2u pos) {
        square.setPosition(pos.x, pos.y);
    }

    sf::Vector2u get_position() {
        unsigned x = square.getPosition().x;
        unsigned y = square.getPosition().y;
        return {x, y};
    }

    sf::Vector2u get_size() {
        return tile_size;
    }


    // Rectangle-contains-point check
    bool contains_point(sf::Vector2u point) {
        return square.getGlobalBounds().contains(point.x, point.y);
    }


    void draw_tile(sf::RenderWindow& window) {
        window.draw(square);
    }

    int get_value() {
        return value;
    }

    double get_probability() {
        return probability_is_bomb;
    }

    void set_probability(double probability) {
        probability_is_bomb = probability;
    }

    void set_probability(int all_mines) {
        probability_is_bomb = 1.0 / all_mines;
    }

    /**
     * if tile is num (enum tiles in <NUM_1, NUM_8>) then return its num(1-8) 
     **/ 
    int get_num() {
        return (int) Tile::tile_values::NUM_1 - (int) get_value() + 1;
    }

    bool is_num() {
        return get_num() <= 8 && get_num() >= 1;
    }

    bool operator <(Tile& tile) {
        if (get_row() == tile.get_row()) {
            return get_col() < tile.get_col();
        }
        return get_row() < tile.get_row();
    }
 
    int get_col() const {
        return col;
    }

    int get_row() const {
        return row;
    }

    /**
     * Used by AI
     */
    bool are_non_diagonal_neighbours(const Tile tile) {
        if (row == tile.get_row()) {
            if (col + 1 == tile.get_col() || col - 1 == tile.get_col())
                return true;
        } else if (col == tile.get_col()) {
            if (row + 1 == tile.get_row() || row - 1 == tile.get_row())
                return true;
        }
        return false;
    }

    void set_adjacent_tiles(std::vector<std::vector<Tile>>& minefield) {
        std::vector<Tile*> adj_tiles;
        std::vector<int> increments = {-1, 0, 1};

        for (int i : increments) {
            for (int j : increments) {
                if (row + i >= 0 && col + j >= 0 && row + i < minefield.size() && col + j < minefield[0].size()
                    && (i != 0 || j != 0)) {
                    adj_tiles.push_back(&minefield[row + i][col + j]);

                }
            }
        }
        /*
        if (row + 1 < minefield.size()) { //upper row
            adj_tiles.push_back(&minefield[row+1][col]);

            if (col - 1 >= 0) {
                adj_tiles.push_back(&minefield[row+1][col-1]);
            }
            if (col + 1 < minefield[0].size()) {
                adj_tiles.push_back(&minefield[row + 1][col + 1]);
            }
        }

        //same row
        if (col - 1 >= 0)
            adj_tiles.push_back(&minefield[row][col-1]);
        if (col + 1 < minefield[0].size())
            adj_tiles.push_back(&minefield[row][col + 1]);

        if (row - 1 >= 0) { //lower row
            adj_tiles.push_back(&minefield[row-1][col]);

            if (col - 1 >= 0) {
                adj_tiles.push_back(&minefield[row-1][col-1]);
            }
            if (col + 1 < minefield[0].size()) {
                adj_tiles.push_back(&minefield[row-1][col+1]);
            }

        }*/

        //refresh textures!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        //NOT necessary anymore YAY!
        /*for (auto &_tile: adj_tiles)
            _tile->load_texture();*/

        this->adjacent_tiles = adj_tiles;
    }

    bool get_is_bomb() {
        return is_bomb;
    }

    void set_is_bomb() {
        is_bomb = true;
    }

    bool get_is_covered() {
        return value <= QUESTION_MARK_COVERED && value >= CLEAR_COVERED;
    }

private:
    sf::Sprite square;
    sf::Vector2u tile_size;
    sf::Texture texture;
    int row = 0;
    int col = 0;
    enum tile_values value = CLEAR_COVERED;
    bool is_bomb = false;
    std::vector<Tile*> adjacent_tiles;
    double probability_is_bomb = 0.0;

    /**
     * Chooses which state of tile is to be drawn
     * @param offset - number of picture within the texture
     */
    void set_texture(int offset) {
        square.setTexture(texture);
        sf::IntRect new_texture_rect = sf::IntRect(0, (0 + offset) * tile_size.y, tile_size.x, tile_size.y);
        square.setTextureRect(new_texture_rect);
    }
};
