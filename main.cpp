// User commands: Undo, play card, communicate information (in the sense of narrowing the possibilities to see
// how the computer reacts to changes in the state)?
// Pieces of info for a state: Player #, calculated win%, cards, known opp cards, show win % per card?, legal moves
#include "player.cpp"
#include "objective.cpp"
#include <iostream>
// #include <sstream>
#include <fstream>
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
int main(int argc, char **argv)
{
    std::vector<Card> deck;
    std::vector<Card> p0_hand;
    std::vector<Card> p1_hand;
    std::vector<Card> p2_hand;

    std::string curr_string = (argv[2]);
    std::ifstream if1(curr_string);
    std::stringstream input_file;
    input_file << if1.rdbuf();
    if1.close();
    for (int i = 0; i < std::stoi(argv[1]); i++)
    {
        std::getline(input_file, curr_string);
    }
    input_file >> curr_string;
    if (curr_string == "false" || curr_string == "False")
    {
        p0_hand = cardsFromStream(input_file);
        p1_hand = cardsFromStream(input_file);
        p2_hand = cardsFromStream(input_file);
    }
    else
    {
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
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(deck.begin(), deck.end() - 1, g);
        for (size_t i = 0; i < 13; i++)
        {
            p0_hand.push_back(deck[3 * i + 2]);

            p1_hand.push_back(deck[3 * i + 1]);

            p2_hand.push_back(deck[3 * i]);
        }
    }
    Player p0(p0_hand, p1_hand, p2_hand, 0);
    Player p1(p1_hand, p2_hand, p0_hand, 1); // I could do this without making players prolly but not worth it
    Player p2(p2_hand, p0_hand, p1_hand, 2);
    std::vector<Player> players = {p0, p1, p2};
    std::vector<std::vector<Objective>> all_objectives(3); // Might need to resize inner vectors

    size_t leader_inx;
    input_file >> leader_inx; // prolly works
    std::vector<Card> curr_trick = cardsFromStream(input_file);

    while (input_file >> curr_string && curr_string != "false" && curr_string != "true" && curr_string != "False" && curr_string != "True") // simplify this later
    {
        size_t player_inx = std::stoi(curr_string);
        Objective obj(input_file);
        all_objectives[player_inx].push_back(obj);
    }

    std::vector<std::vector<bool>> all_objectives_bool(3);
    all_objectives_bool[0].resize(all_objectives[0].size());
    all_objectives_bool[1].resize(all_objectives[1].size());
    all_objectives_bool[2].resize(all_objectives[2].size());

    std::string garbage;

    Player *curr_player = &players[0];
    Player *left_player = &players[1];
    Player *right_player = &players[2];

    while (players[curr_player->player_inx].hand_size() > 0)
    {
        players[0].print_info(curr_player, curr_trick, all_objectives);
        std::cout << players[0].find_best_card(left_player, curr_player, right_player, all_objectives, all_objectives_bool, leader_inx, curr_trick);
        // std::cout << "Press any key to continue: ";
        // std::cin >> garbage;
        // curr_trick.push_back(players[leader_inx].print_info(leader_inx, curr_trick, all_objectives));
        // players[0].update_state(left_player, curr_player, right_player, all_objectives, all_objectives_bool, leader_inx, curr_trick);
        // std::cout << players[1].find_best_card(left_player, curr_player, right_player, all_objectives, all_objectives_bool, leader_inx, curr_trick);
        // curr_trick.push_back(players[(leader_inx + 1) % 3].print_info((leader_inx + 1) % 3, curr_trick, all_objectives));
        // players[0].update_state(left_player, curr_player, right_player, all_objectives, all_objectives_bool, leader_inx, curr_trick);
        // std::cout << players[2].find_best_card(left_player, curr_player, right_player, all_objectives, all_objectives_bool, leader_inx, curr_trick);
        // curr_trick.push_back(players[(leader_inx + 2) % 3].print_info((leader_inx + 2) % 3, curr_trick, all_objectives));
        // players[0].update_state(left_player, curr_player, right_player, all_objectives, all_objectives_bool, leader_inx, curr_trick);
        // std::cout << std::string(58, ' ') << curr_trick[0].stringify() + " " + curr_trick[1].stringify() + " " + curr_trick[2].stringify() + "\n";
        // std::cout << std::string(54, ' ') << "Winning Card: " + curr_trick[leader_inx].stringify() + "\n";
        // std::cout << "\n\n\n\n\n\n\n\n"
        //           << std::string(53, ' ') + "Press any key to continue";
        // std::cin >> garbage;
        // curr_trick = {};
    }

    std::cout << "\n";
    bool ret = players[0].guaranteedSuccess(all_objectives_bool);
    return ret;
}