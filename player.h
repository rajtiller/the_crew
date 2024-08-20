// #ifndef PLAYER_H
// #define PLAYER_H

// #include <set>
// #include <vector>
// #include <string>
// #include <unordered_map>
// #include <algorithm>
// #include <numeric>
// #include <iostream>

// #include "Card.cpp"
// #include "Objective.cpp"

// struct State
// {
//   Suit left_players_right_player_poss_suits;
//   // size_t winner_inx; Can be calculated from curr_trick and leader_inx, used to remove curr_trick cards from winner's won_cards set
//   Suit right_players_left_player_poss_suits;
//   std::vector<std::pair<size_t, size_t>> completed_objectives;
//   size_t leader_inx;            // Whoever played first card of curr_trick
//   std::vector<Card> curr_trick; // Curr_trick.back() is card_just_played is also card_removed_from_left_and_right_players_unkowns
//                                 // curr_player, left_player, and right_player pointers are changed in update state. To unupdate them, we need to find player_inx of curr_player
//                                 // This can be done using curr_trick.size() and leader_inx.
//                                 //------------------------------------------------------------------------------------
//                                 // THINGS THAT ARE CHANGED:
//                                 // left_player->right_player_poss_suits
//                                 // Would change either left_player->won_cards, curr_player->won_cards, or right_player->won_cards
//                                 // left_player->unknowns
//                                 // right_player->left_player_poss_suits
//                                 // right_player->unknowns
//                                 // all_objectives_bool
//                                 // possibly leader_inx
//                                 // curr_trick
//                                 // left_player, curr_player, and right_player pointers probably will change
// };
// class Player
// {
// public:
//   Player();
//   Player(std::vector<Card> &hand_in, std::vector<Card> &opp1_hand,
//          std::vector<Card> &opp2_hand, size_t player_inx);

//   size_t hash();

//   bool curr_player_might_win_trick(std::set<Card> &curr_player_hand,
//                                    std::vector<Card> &curr_trick);
//   void update_unknowns(std::vector<Card> &curr_trick);
//   void update_poss_suits(Suit &s, std::set<Suit> &poss_suits);
//   Card string_to_card(std::string s);
//   size_t hand_size();
//   std::set<Card> get_legal_moves(std::set<Card> &hand,
//                                  std::vector<Card> &curr_trick);

//   bool impossibleUnknown(Player *left_player, Player *curr_player,
//                          Player *right_player,
//                          std::vector<std::vector<Objective>> &all_objectives,
//                          std::vector<std::vector<bool>> &all_objectives_bool,
//                          size_t &leader_inx, std::vector<Card> &curr_trick);

//   bool impossibleKnown(Player *left_player, Player *curr_player,
//                        Player *right_player,
//                        std::vector<std::vector<Objective>> &all_objectives,
//                        std::vector<std::vector<bool>> &all_objectives_bool,
//                        size_t &leader_inx, std::vector<Card> &curr_trick,
//                        std::vector<Card> left_player_cards,
//                        std::vector<Card> right_player_cards);

//   bool guaranteedSuccess(std::vector<std::vector<bool>> &all_objectives_bool);
//   bool checkForSuccess(Player *left_player, Player *curr_player,
//                        Player *right_player,
//                        std::vector<std::vector<Objective>> &all_objectives,
//                        std::vector<std::vector<bool>> &all_objectives_bool,
//                        size_t &leader_inx, std::vector<Card> &curr_trick);

//   State update_state(Player *&left_player, Player *&curr_player,
//                      Player *&right_player,
//                      std::vector<std::vector<Objective>> &all_objectives,
//                      std::vector<std::vector<bool>> &all_objectives_bool,
//                      size_t &leader_inx, std::vector<Card> &curr_trick);

//   std::vector<std::pair<double, Card>> calculate_win_prob(
//       Player *&left_player, Player *&curr_player, Player *&right_player,
//       std::vector<std::vector<Objective>> &all_objectives,
//       std::vector<std::vector<bool>> &all_objectives_bool,
//       size_t &leader_inx, std::vector<Card> &curr_trick);

//   double calculate_win_prob_recursive(Player *left_player,
//                                       Player *curr_player,
//                                       Player *right_player,
//                                       std::vector<std::vector<Objective>>
//                                           &all_objectives,
//                                       std::vector<std::vector<bool>>
//                                           &all_objectives_bool,
//                                       size_t &leader_inx,
//                                       std::vector<Card> &curr_trick,
//                                       std::set<Card> prev_player_actual_hand,
//                                       size_t spots_ahead_compared_to_prev_player);

//   std::vector<std::pair<std::vector<Card>, std::vector<Card>>>
//   calc_permutations(Player *curr_player, size_t &leader_inx);

//   void all_permutations(std::vector<Card> &left_player_hand,
//                         std::vector<Card> &right_player_hand,
//                         std::vector<Card> &free_movers, size_t &left_hand_max,
//                         size_t &right_hand_max,
//                         std::vector<std::pair<std::vector<Card>,
//                                               std::vector<Card>>> &ret);

//   void win_trick(std::vector<Card> &cards_won);
//   void print_info(Player *curr_player, std::vector<Card> curr_trick,
//                   std::vector<std::vector<Objective>> &all_objectives) const;
//   void find_best_card(Player *&left_player, Player *&curr_player,
//                       Player *&right_player,
//                       std::vector<std::vector<Objective>> &all_objectives,
//                       std::vector<std::vector<bool>> &all_objectives_bool,
//                       size_t &leader_inx, std::vector<Card> &curr_trick,
//                       std::unordered_map<StateV2, bool> &all_states);

//   std::set<Card> hand;
//   std::set<Suit> left_player_poss_suits;
//   std::set<Suit> right_player_poss_suits;
//   std::set<Card> won_cards;
//   size_t player_inx;
//   std::set<Card> unknowns;
// };

// #endif