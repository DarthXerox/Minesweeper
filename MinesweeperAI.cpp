#include <algorithm>
#include <unistd.h>

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
                    get_adjacent_tiles_info(adjacent_tiles, flagged_adjacent_tiles, not_flagged_adjacent_tiles);
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
                    } else {
                        // checks if a number has number + 1 tiles around itself
                        // then finds another same number that has those exact tiles around itself
                        // this other number is called "brother" number
                        // this other tile's any different tiles are not bombs
                        if (tile_num - flagged_adjacent_tiles + 1 == not_flagged_adjacent_tiles) {
                            std::vector<Tile*> non_flagged_adj = tile.get_not_flagged_covered();

                            std::cout << "Switched to harder rule" << std::endl;
                            std::cout << "Clicked tile: " << tile.get_row() << " "
                                      << tile.get_col() << std::endl;
                            usleep(1000000);

                            /*//surrounding_tiles.push_back(non_flagged_adj[0]->get_adjacent_tiles());
                            std::vector<std::vector<Tile*>> surrounding_tiles;
                            for (Tile* non_flagged : non_flagged_adj) {
                                std::vector<Tile*> v(non_flagged->get_adjacent_tiles());
                                surrounding_tiles.push_back(v);
                            }
                            std::cout << "ok1" << std::endl;

                            for (auto& vec : surrounding_tiles) { //the vectors are now pretending to be sets
                                std::sort(vec.begin(), vec.end(), Tile::TilePtrComparator());
                            }
                            std::cout << "ok2" << std::endl;

                            std::vector<Tile*> brother_nums = surrounding_tiles[0];
                            for (int it = 1; it < surrounding_tiles.size(); it++) {
                                std::cout << "before next union: " << std::endl;
                                for (Tile* t : brother_nums) {
                                    std::cout << *t;
                                }
                                std::cout << std::endl;

                                std::cout << "The other set: " << std::endl;
                                for (Tile* t : surrounding_tiles[it]) {
                                    std::cout << *t;
                                }
                                std::cout << std::endl;

                                std::vector<Tile*> union_;
                                set_intersection(brother_nums.begin(), brother_nums.end(), surrounding_tiles[it].begin(),
                                        surrounding_tiles[it].end(), std::back_inserter(union_));
                                brother_nums = union_;

                                std::cout << "after next union: " << std::endl;
                                for (Tile* t : brother_nums) {
                                    std::cout << *t;
                                }
                                std::cout << std::endl;
                            }

                            std::vector<Tile*> new_brothers;
                            for (int k = 0; k < brother_nums.size(); k++) {
                                if (brother_nums[k]->is_num() && !(*brother_nums[k] == tile)) {
                                    new_brothers.push_back(brother_nums[k]);
                                }
                            }
                            brother_nums = new_brothers;

                            std::cout << "Only valid tiles: " << std::endl;
                            for (Tile* t : new_brothers) {
                                std::cout << *t;
                            }
                            std::cout << std::endl;*/

                            std::vector<Tile*> brother_nums = get_brother_nums(tile, non_flagged_adj);

                            // valid brother nums are those that dont

                            if (brother_nums.empty()) {
                                continue;
                            }

                            for (Tile* adj_tile : brother_nums[0]->get_different_covered(tile.get_adjacent_tiles())) {
                                bool to_click = true;
                                for (Tile* t : non_flagged_adj) {
                                    if (*t == *adj_tile) {
                                        to_click = false;
                                        break;
                                    }
                                }

                                if (to_click) {
                                    std::cout << "Gonna be clicked: " << *adj_tile << std::endl;
                                    minefield.click_tile(*adj_tile, sf::Mouse::Button::Left);
                                }
                            }
                            std::cout << "ok4" << std::endl;

                            return 1;
                        }

                        /*if (tile_num - flagged_adjacent_tiles == 1 && not_flagged_adjacent_tiles == 2) {
                            std::vector<Tile *> uncovered = get_not_flagged_covered(adjacent_tiles);
                            if (uncovered[0]->are_non_diagonal_neighbours(*uncovered[1])) {
                                Tile *tile_to_click = get_non_bomb_tile(minefield, uncovered, i, j);
                                std::cout << "Initial tile: " << i << " " << j << std::endl;
                                if (tile_to_click) {

                                    minefield.click_tile(*tile_to_click, sf::Mouse::Button::Left);
                                    std::cout << "Just applied the third rule!!!" << std::endl;
                                    return 1;
                                } else {
                                    std::cout << "Wtf" << std::endl;
                                }
                            } else {
                                std::cout << "Neighbours not working" << std::endl;
                            }


                            //REFACTOR
                            //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

                        }*/
                    }
                }
            }
        }
        if (basic_rules_only) {
            if (check_all_numbers(minefield, false) == 0) {
                std::cout <<"No other possibilities for AI" <<std::endl;
                std::cout <<"Mines left: "<<minefield.get_mines_left()<<std::endl;
                return 0;
            } else {
                return check_all_numbers(minefield, true);
            }
        }
        std::cout <<"No other possibilities for AI" <<std::endl;
        std::cout <<"Mines left: "<<minefield.get_mines_left()<<std::endl;
        return 0;
    }


    std::vector<Tile*> get_brother_nums(Tile& init_tile, std::vector<Tile*>& non_flagged_adj) {
        std::vector<std::vector<Tile*>> surrounding_tiles;
        for (Tile* non_flagged : non_flagged_adj) {
            //std::vector<Tile*> v(non_flagged->get_adjacent_tiles());
            surrounding_tiles.push_back(get_only_correct_nums(non_flagged, &init_tile));
        }

        std::vector<Tile*> brother_nums = surrounding_tiles[0];
        for (unsigned long it = 1; it < surrounding_tiles.size(); it++) {
            std::cout << "before next union: " << std::endl;
            for (Tile* t : brother_nums) {
                std::cout << *t;
            }
            std::cout << std::endl;

            std::cout << "The other set: " << std::endl;
            for (Tile* t : surrounding_tiles[it]) {
                std::cout << *t;
            }
            std::cout << std::endl;

            std::vector<Tile*> intersection;
            set_intersection(brother_nums.begin(), brother_nums.end(), surrounding_tiles[it].begin(),
                             surrounding_tiles[it].end(), std::back_inserter(intersection));
            brother_nums = intersection;

            std::cout << "after next union: " << std::endl;
            for (Tile* t : brother_nums) {
                std::cout << *t;
            }
            std::cout << std::endl;
        }

        return brother_nums;
    }


    std::vector<std::vector<Tile>> assemble_tile_chunks(Minefield minefield);

    /**
     * Filters through adjacent tiles of tile and returns only those that are n um and are not equal to other_tile
     * Correct nums are considered those that have the same actual number as the other_tile
     * @param tile
     * @param other_tile
     * @return
     */
    std::vector<Tile*> get_only_correct_nums(Tile* tile, Tile* other_tile) {
        std::vector<Tile*> nums;
        for (Tile* t : tile->get_adjacent_tiles()) {
            if (t->is_num() && !(*t == *other_tile) && t->get_actual_tile_num() == other_tile->get_actual_tile_num()) {
                nums.push_back(t);
            }
        }
        return nums;
    }


    /**
     * Checks all covered adjacent tiles and increments flagged_amnt for each flagged one and not_falgged_covered_amnt for each not flagged one
     **/
    void get_adjacent_tiles_info(const std::vector<Tile*> adjacent_tiles, int& flagged_amnt, int& not_flagged_covered_amnt) {
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

    int get_actual_num_tile(Tile& tile, Minefield minefield) {
        if (!tile.is_num()) {
            return 0;
        }
        int flagged_adjacent_tiles = 0, not_flagged_adjacent_tiles = 0;
        std::vector<Tile*> adjacent_tiles = tile.get_adjacent_tiles();
        get_adjacent_tiles_info(adjacent_tiles, flagged_adjacent_tiles, not_flagged_adjacent_tiles);
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
                next_num_col = tiles[1]->get_col();
            } else {
                next_num_col = tiles[0]->get_col();
            }
        } else if (tiles[0]->get_col() == tiles[1]->get_col()) {
            next_num_col = num_col;
            if (tiles[0]->get_row() == num_row) {
                next_num_row = tiles[1]->get_row();
            } else {
                next_num_row = tiles[0]->get_row();
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
