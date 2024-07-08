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
    for (int suit = 0; suit < 5; suit++)
    {
        for (int value = 1; value < 10; value++)
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
    Player p0(p0_hand);
    Player p1(p1_hand);
    Player p2(p2_hand);
    std::vector<Player> players = {p0, p1, p2};
    size_t leader_inx = 0;
    std::string garbage;
    for (auto c : deck)
    {
        std::cout << c;
    }
    while (p0.hand_size() > 0)
    {
        players[leader_inx].print_info(leader_inx, {3, BLACK});
        std::cin >> garbage;
        players[(leader_inx + 1) % 3].print_info((leader_inx + 1) % 3, {2, GREEN});
        std::cin >> garbage;
        players[(leader_inx + 2) % 3].print_info((leader_inx + 2) % 3, {9, PINK});
        std::cin >> garbage;
    }
    std::cout << "\n";
    return 0;
}