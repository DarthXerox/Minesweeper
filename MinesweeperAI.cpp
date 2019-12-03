#include "Minefield.h"

#include <SFML/Graphics.hpp>

/**
 * Applies 3 hard rules, then calculates probability
 **/
class MinesweeperAI {   
public:
    MinesweeperAI() = default;

    /**
     * Returns 1 if a successful change of the minefield was made
     * 0 otherwise
     * */
    //OPTIMIZE with last tile coordinates so it starts to iterate from there
    int check_all_numbers(Minefield& minefield, bool basic_rules_only) {
        //std::cout << "Hm" << std::endl;
        for (int i = 0; i < minefield.get_height(); i++) {
            for (int j = 0; j < minefield.get_width(); j++) {
                Tile& tile = minefield.get_tile(i ,j);
                //std::cout <<"Tile val: "<<tile.get_value()<<std::endl;
                int tile_num = tile.get_num();
                //std::cout <<"and its num: "<<tile_num<<std::endl;
                //std::cout<<"Coordinates: i= "<<i<<" j= "<<j<<std::endl;
                if (tile.is_num()) {
                    //std::cout<<"It is a NUMBER"<<std::endl;
                    int flagged_adjacent_tiles = 0, not_flagged_adjacent_tiles = 0;
                    std::vector<Tile*> adjacent_tiles = tile.get_adjacent_tiles();
                    get_adajcent_tiles_info(adjacent_tiles, flagged_adjacent_tiles, not_flagged_adjacent_tiles);
                    //std::cout<<flagged_adjacent_tiles<<not_flagged_adjacent_tiles<<std::endl;

                    if (basic_rules_only) {                                                               
                        //all adjacent covered tiles are bombs
                        if (tile_num - flagged_adjacent_tiles == not_flagged_adjacent_tiles && not_flagged_adjacent_tiles > 0) { 
                            flag_all_adjacent_covered_tiles(adjacent_tiles, minefield);
                            //std::cout<<"flagged!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<std::endl;
                            return 1;
                        } //all adjacent covered tiles are not bombs AND we there are some covered adjacent tiles
                        if (tile_num == flagged_adjacent_tiles && not_flagged_adjacent_tiles > 0) { 
                            uncover_all_adjacent_covered_tiles(adjacent_tiles, minefield);
                            //std::cout<<"uncovered!!!!!!!!!!!!!!!!!!!!!!!!!"<<std::endl;
                            return 1;
                        } 
                    } /*else {
                        if (tile_num - flagged_adjacent_tiles == 1 && not_flagged_adjacent_tiles == 2) {
                            std::vector<Tile*> uncovered = get_not_flagged_covered(adjacent_tiles);
                            if (uncovered[0]->are_non_diagonal_neighbours(*uncovered[1])) {
                                Tile* tile_to_click = get_non_bomb_tile(minefield, uncovered, i, j);
                                std::cout <<"Initial tile: "<<i<<" " <<j<<std::endl;
                                if (tile_to_click) {
                                    std::cout <<"Clicked tile: "<<tile_to_click->get_row()<<" " <<tile_to_click->get_col()<<std::endl;
                                    minefield.execute_click(*tile_to_click, sf::Mouse::Button::Left);
                                    std::cout <<"Just applied the third rule!!!"<<std::endl;
                                    return 1;
                                } else {
                                    std::cout<<"Wtf"<<std::endl;
                                }
                            } else {
                                std::cout<<"Neighbours not working"<<std::endl;
                            }


                            REFACTOR!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

                        }*/
                }
            }
        }
        /*if (basic_rules_only) {
            i = 0;
            j = 0;
            if (check_all_numbers(minefield, false, i, j) == 0) {
                std::cout <<"No other possibilities for AI" <<std::endl;
                std::cout <<"Mines left: "<<minefield.mines_left<<std::endl;
                return 0;
            } else {
                return check_all_numbers(minefield, true, i, j);
            }
        } */
        std::cout <<"No other possibilities for AI" <<std::endl;
        std::cout <<"Mines left: "<<minefield.get_mines_left()<<std::endl;
        return 0;
    }




    std::vector<std::vector<Tile>> assemble_tile_chunks(Minefield minefield);


    /**
     * Checks all covered adjacent tiles and increments flagged_amnt for each flagged one and not_falgged_covered_amnt for each not flagged one
     **/
    void get_adajcent_tiles_info(const std::vector<Tile*> adjacent_tiles, int& flagged_amnt, int& not_flagged_covered_amnt) {
        for (Tile* tile : adjacent_tiles) {
            if (tile->get_is_covered()) {
                if (tile->is_flagged()) {
                    flagged_amnt++;
                } else {
                    not_flagged_covered_amnt++;
                }
            }
        }
    }

    void flag_all_adjacent_covered_tiles(std::vector<Tile*> adjacent_tiles, Minefield& minefield) {
        for (Tile* tile : adjacent_tiles) { //this is safe as only already iterated-over tiles are changed
            if (tile->get_is_covered() && !tile->is_flagged()) {
                //tile->flag(minefield.get_mines_left());
                minefield.flag_tile(*tile);
            }
        }
    }

    void uncover_all_adjacent_covered_tiles(std::vector<Tile*> adjacent_tiles, Minefield& minefield) {
        for (Tile* tile : adjacent_tiles) {
            if (tile->get_is_covered() && !tile->is_flagged()) {
                minefield.click_tile(*tile, sf::Mouse::Button::Left);
            }
        }
    }

    std::vector<Tile*> get_not_flagged_covered(std::vector<Tile*> tiles) {
        std::vector<Tile*> not_flagged_covered;
        for (auto tile : tiles) {
            if (tile->get_is_covered() && !tile->is_flagged()) {
                not_flagged_covered.push_back(tile);
            }
        }
        return not_flagged_covered;
    }

    int get_actual_num_tile(Tile& tile, Minefield minefield) {
        if (!tile.is_num()) {
            return 0;
        }
        int flagged_adjacent_tiles = 0, not_flagged_adjacent_tiles = 0;
        std::vector<Tile*> adjacent_tiles = tile.get_adjacent_tiles();
        get_adajcent_tiles_info(adjacent_tiles, flagged_adjacent_tiles, not_flagged_adjacent_tiles);
        return tile.get_num() - flagged_adjacent_tiles;
    }

    /**
     * @param tiles should be of size two
     * */
    Tile* get_non_bomb_tile(Minefield& minefield, std::vector<Tile*> tiles, int num_row, int num_col) {
        int next_num_row = 0, next_num_col = 0;
        int nb_row = 0, nb_col = 0;
        if (tiles.size() != 2) 
            return nullptr;
        
        if (tiles[0]->get_row() == tiles[1]->get_row()) {
            next_num_row = num_row;
            if (tiles[0]->get_col() == num_col) {
                next_num_col == tiles[1]->get_col();
            } else {
                next_num_col == tiles[0]->get_col();
            }
        } else if (tiles[0]->get_col() == tiles[1]->get_col()) {
            next_num_col = num_col;
            if (tiles[0]->get_row() == num_row) {
                next_num_row == tiles[1]->get_row();
            } else {
                next_num_row == tiles[0]->get_row();
            }
        } else {
            return nullptr;
        }
        std::cout<<"Next num coor: "<<next_num_row<<" "<<next_num_col<<std::endl;
        int next_num_value = get_actual_num_tile(minefield.get_tile(next_num_row, next_num_col), minefield);
        if (next_num_value == 0) {
            return nullptr;
        }
        int increment = 1;
        //if (next_num_value == 2) increment = 0; 

        if (next_num_col == num_col) {
            nb_col = tiles[0]->get_col();
            if (num_row > next_num_row) {
                nb_row = next_num_row - increment;
            } else {
                nb_row = next_num_row + increment;
            }
        } else if (next_num_row == num_row) {
            nb_row = tiles[0]->get_row();
            if (num_col > next_num_col) {
                nb_col = next_num_col - increment;
            } else {
                nb_col = next_num_col + increment;
            }                  
        } else {
            return nullptr;
        }
        std::cout<<"Non bomb coor: "<<nb_row<<" "<<nb_col<<std::endl;


        if (nb_col < 0 || nb_row < 0 || nb_col >= minefield.get_width() || nb_row >= minefield.get_height())
            return nullptr;

        return &minefield.get_tile(nb_row, nb_col);
    }

};
