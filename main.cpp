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
    Objective obj0(OBTAIN_CARDS, 1, {{4, BLACK}}, {}, 0, false); // {{4,BLACK}} is only important param
    Objective obj1(OBTAIN_CARDS, 1, {{3, PINK}}, {}, 0, false); // {{3,PINK}} is only important param
    Objective obj2(TAKE, 77, {{9, BLACK}}, {}, 2, false);        // 2 is only important param
    std::vector<std::vector<Objective>> all_objectives;
    std::vector<std::vector<bool>> all_objectives_bool;
    all_objectives.resize(3);
    all_objectives[0].push_back(obj0);
    all_objectives[1].push_back(obj1);
    all_objectives[2].push_back(obj2);
    all_objectives_bool = {{false}, {false}, {false}};
    std::vector<Player> players = {p0, p1, p2};
    Player *curr_player = &players[0];
    Player *left_player = &players[1];
    Player *right_player = &players[2];
    size_t leader_inx = 0;
    std::string garbage;
    std::vector<Card> curr_trick;
    while (players[0].hand_size() > 0)
    {
        curr_trick.push_back(players[leader_inx].print_info(leader_inx, {{9, BLACK}}, all_objectives));
        players[0].update_state(left_player, curr_player, right_player, all_objectives, all_objectives_bool, leader_inx, curr_trick);
        curr_trick.push_back(players[(leader_inx + 1) % 3].print_info((leader_inx + 1) % 3, curr_trick, all_objectives));
        players[0].update_state(left_player, curr_player, right_player, all_objectives, all_objectives_bool, leader_inx, curr_trick);
        curr_trick.push_back(players[(leader_inx + 2) % 3].print_info((leader_inx + 2) % 3, curr_trick, all_objectives));
        players[0].update_state(left_player, curr_player, right_player, all_objectives, all_objectives_bool, leader_inx, curr_trick);
        std::cout << std::string(58, ' ') << curr_trick[0].stringify() + " " + curr_trick[1].stringify() + " " + curr_trick[2].stringify() + "\n";
        std::cout << std::string(54, ' ') << "Winning Card: " + curr_trick[leader_inx].stringify() + "\n";
        std::cout << players[0].find_best_card(left_player, curr_player, right_player, all_objectives, all_objectives_bool, leader_inx, curr_trick);
        std::cout << "\n\n\n\n\n\n\n\n"
                  << std::string(53, ' ') + "Press any key to continue";
        std::cin >> garbage;
        curr_trick = {};
    }
    std::cout << "\n";
    return 0;
}