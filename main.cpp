// User commands: Undo, play card, communicate information (in the sense of narrowing the possibilities to see
// how the computer reacts to changes in the state)?
// Pieces of info for a state: Player #, calculated win%, cards, known opp cards, show win % per card?, legal moves
#include "player.cpp"
#include "objective.cpp"
#include <iostream>
#include <cstring>
#include <iomanip>
#include <vector>
#include <random>
#include <set>
#include <algorithm>
#include "card.cpp"
bool compareCards(const Card &a, const Card &b)
{
    if (a.suit != b.suit)
    {
        return a.suit < b.suit;
    }
    return a.value < b.value;
}
int main()
{
    std::vector<Card> deck;
    for (size_t suit = 0; suit < 5; suit++)
    {
        for (size_t value = 1; value < 10; value++)
        {
            if (suit < 4 || (value < 5 && value > 1))
            {
                deck.push_back({value, static_cast<Suit>(suit)});
            }
        }
    }
    std::vector<Card> p0_hand;
    std::vector<Card> p1_hand;
    std::vector<Card> p2_hand;
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(deck.begin(), deck.end() - 1, g);
    std::shuffle(deck.begin(), deck.end() - 1, g);
    for (size_t i = 0; i < 13; i++)
    {
        p0_hand.push_back(deck[3 * i + 2]);

        p1_hand.push_back(deck[3 * i + 1]);

        p2_hand.push_back(deck[3 * i]);
    }
    Player p0(p0_hand, deck, 0);
    Player p1(p1_hand, deck, 1);
    Player p2(p2_hand, deck, 2);
    Objective obj0(OBTAIN_CARDS, 1, {{3, PINK}}, {}, 0, false);            // {{3,PINK}} is only important param
    Objective obj1(TAKE_EXACTLY_N_TRICKS, 1, {{9, BLACK}}, {}, 12, false); // 1 is only important param
    Objective obj2(OBTAIN_CARDS, 1, {{4, YELLOW}}, {}, 0, false);          // {{4,YELLOW}} is only important param
    std::vector<std::vector<Objective>> all_objectives;
    std::vector<std::vector<bool>> all_objectives_bool;
    all_objectives.resize(3);
    all_objectives[0].push_back(obj0);
    all_objectives[1].push_back(obj1);
    all_objectives[2].push_back(obj2);
    all_objectives_bool = {{true}, {false}, {true}};
    p0.hand = {{4, BLACK}};
    p1.hand = {{7, BLUE}, {3, BLUE}};
    p2.hand = {{5, PINK}, {6, BLUE}};
    std::vector<Card> curr_trick = {{4, BLUE}};
    size_t leader_inx = 0;
    p0.unknowns.clear();
    p1.unknowns.clear();
    p2.unknowns.clear();
    std::set_union(p1.hand.begin(), p1.hand.end(), p2.hand.begin(), p2.hand.end(), std::inserter(p0.unknowns, p0.unknowns.begin()));
    std::set_union(p2.hand.begin(), p2.hand.end(), p0.hand.begin(), p0.hand.end(), std::inserter(p1.unknowns, p1.unknowns.begin()));
    std::set_union(p0.hand.begin(), p0.hand.end(), p1.hand.begin(), p1.hand.end(), std::inserter(p2.unknowns, p2.unknowns.begin()));
    std::string garbage;
    std::vector<Player> players = {p0, p1, p2};
    Player *curr_player = &players[0];
    Player *left_player = &players[1];
    Player *right_player = &players[2];
    players[0].update_state(left_player, curr_player, right_player, all_objectives, all_objectives_bool, leader_inx, curr_trick);
    // curr_trick.push_back({6, BLUE});
    // players[1].hand.erase(players[1].hand.find({6, BLUE}));
    // players[0].update_state(left_player, curr_player, right_player, all_objectives, all_objectives_bool, leader_inx, curr_trick);
    while (players[0].hand_size() > 0)
    {
        std::cout << players[0].find_best_card(left_player, curr_player, right_player, all_objectives, all_objectives_bool, leader_inx, curr_trick);
        curr_trick.push_back(players[leader_inx].print_info(leader_inx, curr_trick, all_objectives));
        players[0].update_state(left_player, curr_player, right_player, all_objectives, all_objectives_bool, leader_inx, curr_trick);
        std::cout << players[1].find_best_card(left_player, curr_player, right_player, all_objectives, all_objectives_bool, leader_inx, curr_trick);
        curr_trick.push_back(players[(leader_inx + 1) % 3].print_info((leader_inx + 1) % 3, curr_trick, all_objectives));
        players[0].update_state(left_player, curr_player, right_player, all_objectives, all_objectives_bool, leader_inx, curr_trick);
        std::cout << players[2].find_best_card(left_player, curr_player, right_player, all_objectives, all_objectives_bool, leader_inx, curr_trick);
        curr_trick.push_back(players[(leader_inx + 2) % 3].print_info((leader_inx + 2) % 3, curr_trick, all_objectives));
        players[0].update_state(left_player, curr_player, right_player, all_objectives, all_objectives_bool, leader_inx, curr_trick);
        std::cout << std::string(58, ' ') << curr_trick[0].stringify() + " " + curr_trick[1].stringify() + " " + curr_trick[2].stringify() + "\n";
        std::cout << std::string(54, ' ') << "Winning Card: " + curr_trick[leader_inx].stringify() + "\n";
        std::cout << "\n\n\n\n\n\n\n\n"
                  << std::string(53, ' ') + "Press any key to continue";
        std::cin >> garbage;
        curr_trick = {};
    }
    std::cout << "\n";
    return 0;
}