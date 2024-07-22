#pragma once
#include <iostream>
#include <numeric>
#include <cstring>
#include <string>
#include <iomanip>
#include <vector>
#include <set>
#include "objective.cpp"
#include "card.cpp"
#include <cassert>
#include <unordered_map>
size_t trick_winner(std::vector<Card> &trick)
{
    size_t winner = 0;
    for (size_t i = 1; i < trick.size(); i++)
    {
        Card &card = trick[i];
        if (card.suit == trick[winner].suit)
        {
            if (card.value > trick[winner].value)
            {
                winner = i;
            }
        }
        else if (card.suit == Suit::BLACK)
        {
            winner = i;
        }
    }
    return winner;
}
struct State
{
    Suit left_players_right_player_poss_suits;
    // size_t winner_inx; Can be calculated from curr_trick and leader_inx, used to remove curr_trick cards from winner's won_cards set
    Suit right_players_left_player_poss_suits;
    std::vector<std::pair<size_t, size_t>> completed_objectives;
    size_t leader_inx;            // Whoever played first card of curr_trick
    std::vector<Card> curr_trick; // Curr_trick.back() is card_just_played is also card_removed_from_left_and_right_players_unkowns
    // curr_player, left_player, and right_player pointers are changed in update state. To unupdate them, we need to find player_inx of curr_player
    // This can be done using curr_trick.size() and leader_inx.
    //------------------------------------------------------------------------------------
    // THINGS THAT ARE CHANGED:
    // left_player->right_player_poss_suits
    // Would change either left_player->won_cards, curr_player->won_cards, or right_player->won_cards
    // left_player->unknowns
    // right_player->left_player_poss_suits
    // right_player->unknowns
    // all_objectives_bool
    // possibly leader_inx
    // curr_trick
    // left_player, curr_player, and right_player pointers probably will change
};
class Player
{
public:
    Player() {}
    Player(std::vector<Card> &hand_in, std::vector<Card> &deck, size_t player_inx) : player_inx(player_inx)
    {
        for (Card c : deck)
        {
            unknowns.insert(c);
        }
        for (Card &c : hand_in)
        {
            hand.insert(c);
            unknowns.erase(unknowns.find(c));
        }
        left_player_poss_suits = {PINK,
                                  YELLOW,
                                  BLUE,
                                  GREEN,
                                  BLACK};
        right_player_poss_suits = {PINK,
                                   YELLOW,
                                   BLUE,
                                   GREEN,
                                   BLACK};
    }
    void update_unknowns(std::vector<Card> &curr_trick)
    {
        for (Card c : curr_trick)
        {
            auto it = unknowns.find(c);
            if (it != unknowns.end()) // This check may be obselete with our current invariants
            {
                unknowns.erase(it);
            }
        }
    }
    void update_poss_suits(Suit &s, std::set<Suit> &poss_suits)
    {
        auto it = poss_suits.find(s);
        if (it != poss_suits.end())
        {
            poss_suits.erase(it);
        }
    }
    Card string_to_card(std::string s)
    {

        if (s.size() == 2)
        {
            try
            {
                char c = s[0];
                size_t value = c - '0';
                Suit suit;
                switch (std::toupper(s[1]))
                {
                case 'P':
                    suit = PINK;
                    break;
                case 'Y':
                    suit = YELLOW;
                    break;
                case 'B':
                    suit = BLUE;
                    break;
                case 'G':
                    suit = GREEN;
                    break;
                case 'T':
                    suit = BLACK;
                    if (value > 4)
                    {
                        throw std::runtime_error("ERROR");
                    }
                    break;
                default:
                    throw std::runtime_error("ERROR");
                    break;
                }
                if (value < 1 || value > 9)
                {
                    throw std::runtime_error("ERROR");
                }
                return {value, suit};
            }
            catch (...)
            {
                return {9, BLACK};
            }
        }
        return {9, BLACK};
    }
    size_t hand_size()
    {
        return hand.size();
    }
    std::set<Card> get_legal_moves(std::set<Card> &hand, std::vector<Card> &curr_trick)
    {
        if (curr_trick.empty())
        {
            return hand;
        }
        std::set<Card> legal_moves;
        std::set<Card> illegal_moves;
        for (Card c : hand)
        {
            if (c.suit == curr_trick[0].suit)
            {
                legal_moves.insert(c);
            }
            else
            {
                illegal_moves.insert(c);
            }
        }
        return legal_moves.empty() ? illegal_moves : legal_moves;
    }
    bool impossibleUnknown(Player *left_player, Player *curr_player, Player *right_player, std::vector<std::vector<Objective>> &all_objectives, std::vector<std::vector<bool>> &all_objectives_bool, size_t &leader_inx, std::vector<Card> &curr_trick)
    {
        // Consider making this take into account known cards of other players. Ie if my player to my right has a 5 BLUE, that's more
        // information than knowing that one of my opponents has the 5 BLUE.
        auto hand_rec = curr_player->hand;
        auto unknowns_rec = curr_player->unknowns;
        auto won_cards_rec = curr_player->won_cards;
        bool impossible = false;
        size_t trick_index = 12 - hand_rec.size();
        for (size_t old_j = 0; old_j < 3 && !impossible; old_j++)
        {
            size_t j = (old_j + curr_player->player_inx) % 3; // j is always equal to the index of the player whose objectives we are checking
            switch (old_j)
            {
            case 0:
                won_cards_rec = curr_player->won_cards;
                break;
            case 1:
                won_cards_rec = left_player->won_cards;
                break;
            case 2:
                won_cards_rec = right_player->won_cards;
                break;
            }
            for (size_t i = 0; i < all_objectives[j].size(); i++)
            {
                if (all_objectives_bool[j][i])
                {
                    continue;
                }
                Objective &obj = all_objectives[j][i];
                bool boolean1 = true;
                bool boolean2 = true;
                size_t count1 = 0;
                switch (obj.type)
                {
                // Checks if the card has been played by someone else
                case Objective_Type::OBTAIN_CARDS:
                    for (Card c : obj.cards)
                    {
                        if (unknowns_rec.find(c) == unknowns_rec.end() && hand_rec.find(c) == hand_rec.end() && std::find(won_cards_rec.begin(), won_cards_rec.end(), c) == won_cards_rec.end())
                        {
                            // Checks if the card has not been played in this trick or the player has no chance of winning
                            if (std::find(curr_trick.begin(), curr_trick.end(), c) == curr_trick.end() || (leader_inx + trick_winner(curr_trick)) % 3 != j)
                            {
                                impossible = true;
                                break;
                            }
                        }
                    }
                    break;
                // Checks if one of the cards has been collected but not the other
                case Objective_Type::OBTAIN_CARD_WITH:
                    boolean1 = unknowns_rec.find(obj.cards[0]) == unknowns_rec.end() && hand_rec.find(obj.cards[0]) == hand_rec.end();
                    boolean2 = unknowns_rec.find(obj.cards[1]) == unknowns_rec.end() && hand_rec.find(obj.cards[1]) == hand_rec.end();
                    if (boolean1 && boolean2)
                    {
                        // Checks if both cards are not in the trick or the player is not winning
                        boolean1 = std::find(curr_trick.begin(), curr_trick.end(), obj.cards[0]) != curr_trick.end() && std::find(curr_trick.begin(), curr_trick.end(), obj.cards[1]) != curr_trick.end();
                        if (!boolean1 || (leader_inx + trick_winner(curr_trick)) % 3 != j)
                        {
                            impossible = true;
                        }
                    }
                    break;
                // Checks if the trick has already happened
                case Objective_Type::TAKE:
                    if (obj.trick_to_win < trick_index || (leader_inx + trick_winner(curr_trick)) % 3 != j)
                    {
                        impossible = true;
                        break;
                    }
                    break;
                // Checks if the trick has already happened
                case Objective_Type::DONT_TAKE:
                    if (obj.trick_to_win < trick_index)
                    {
                        impossible = true;
                        break;
                    }
                    break;
                case Objective_Type::OBTAIN_ALL_CARDS_OF_COLOR:
                    // Does not yet work for other players
                    if (curr_player->player_inx != j)
                    {
                        continue;
                    }
                    if (std::find_if(left_player->won_cards.begin(), left_player->won_cards.end(), [obj](Card const &c)
                                     { return c.suit == obj.suits[0]; }) != left_player->won_cards.end() ||
                        std::find_if(right_player->won_cards.begin(), right_player->won_cards.end(), [obj](Card const &c)
                                     { return c.suit == obj.suits[0]; }) != right_player->won_cards.end())
                    {
                        impossible = true;
                    }
                    break;
                case Objective_Type::TAKE_EXACTLY_N_TRICKS:
                    // Check if possible to win enough
                    if (hand_rec.size() + won_cards_rec.size() / 3 < obj.number)
                    {
                        impossible = true;
                        break;
                    }
                    // Check if too many have been won
                    if (won_cards_rec.size() / 3 > obj.number)
                    {
                        impossible = true;
                        break;
                    }
                    break;
                case Objective_Type::TAKE_TRICK_WITH_ODD:
                    // Counts number of odd cards.
                    count1 = std::count_if(curr_trick.begin(), curr_trick.end(), [](Card const &c)
                                           { return c.value % 2 == 1; });
                    // If all the cards in the trick are odd you can skip this check
                    if (count1 == curr_trick.size())
                    {
                        boolean1 = std::find_if(hand_rec.begin(), hand_rec.end(), [](Card const &c)
                                                { return c.value % 2 == 1; }) != hand_rec.end();
                        if (!boolean1)
                        {
                            impossible = true;
                            break;
                        }
                        // Removes the card from the trick that is the players so the count is correct
                        count1--;
                    }
                    // Checks if including the cards in the trick there are enough in the other players hands
                    boolean2 = count1 + std::count_if(hand_rec.begin(), hand_rec.end(), [](Card const &c)
                                                      { return c.value % 2 == 1; }) >=
                               2;
                    if (!boolean2)
                    {
                        impossible = true;
                    }
                    break;
                case Objective_Type::TAKE_TRICK_WITH_EVEN:
                    // Counts number of even cards.
                    count1 = std::count_if(curr_trick.begin(), curr_trick.end(), [](const Card &c)
                                           { return c.value % 2 == 0; });
                    // If all the cards in the trick are even you can skip this check
                    if (count1 == curr_trick.size())
                    {
                        boolean1 = std::find_if(hand_rec.begin(), hand_rec.end(), [](Card const &c)
                                                { return c.value % 2 == 0; }) != hand_rec.end();
                        if (!boolean1)
                        {
                            impossible = true;
                            break;
                        }
                        // Removes the card from the trick that is the players so the count is correct
                        count1--;
                    }
                    // Checks if including the cards in the trick there are enough in the other players hands
                    boolean1 = count1 + std::count_if(hand_rec.begin(), hand_rec.end(), [](Card const &c)
                                                      { return c.value % 2 == 0; }) >=
                               2;
                    if (!boolean1)
                    {
                        impossible = true;
                    }
                    break;
                default:
                    break;
                }
                if (impossible)
                {
                    break;
                }
            }
        }
        return impossible;
    }
    bool impossibleKnown(Player *left_player, Player *curr_player, Player *right_player, std::vector<std::vector<Objective>> &all_objectives, std::vector<std::vector<bool>> &all_objectives_bool, size_t &leader_inx, std::vector<Card> &curr_trick, std::vector<Card> left_player_cards, std::vector<Card> right_player_cards)
    {
        return false;
    }
    bool guaranteedSuccess(std::vector<std::vector<bool>> &all_objectives_bool)
    {
        return std::find_if(all_objectives_bool.begin(), all_objectives_bool.end(), [](std::vector<bool> &x)
                            { return std::find(x.begin(), x.end(), false) == x.end(); }) == all_objectives_bool.end();
    }
    bool checkForSuccess(Player *left_player, Player *curr_player, Player *right_player, std::vector<std::vector<Objective>> &all_objectives, std::vector<std::vector<bool>> &all_objectives_bool, size_t &leader_inx, std::vector<Card> &curr_trick)
    {
        for (size_t i = 0; i < 3; i++)
        {
            Player *player = (i == left_player->player_inx ? left_player : i == curr_player->player_inx ? curr_player
                                                                                                        : right_player);
            for (size_t j = 0; j < all_objectives_bool[i].size(); j++)
            {
                if (all_objectives_bool[i][j])
                {
                    continue;
                }
                Objective &obj = all_objectives[i][j];
                size_t count1 = 0;
                size_t count2 = 0;
                switch (obj.type)
                {
                case Objective_Type::OBTAIN_EXACTLY_COLORS:
                    count1 = std::count_if(player->won_cards.begin(), player->won_cards.end(), [&obj](const Card &c)
                                           { return c.suit == obj.suits[0]; });
                    if (count1 != obj.number)
                    {
                        return false;
                    }
                    break;
                case Objective_Type::OBTAIN_MORE_OF_COLOR:
                    count1 = std::count_if(player->won_cards.begin(), player->won_cards.end(), [&obj](const Card &c)
                                           { return c.suit == obj.suits[0]; });
                    count2 = std::count_if(player->won_cards.begin(), player->won_cards.end(), [&obj](const Card &c)
                                           { return c.suit == obj.suits[1]; });
                    if (count1 <= count2)
                    {
                        return false;
                    }
                    break;
                case Objective_Type::OBTAIN_EQUAL_OF_COLORS:
                    count1 = std::count_if(player->won_cards.begin(), player->won_cards.end(), [&obj](const Card &c)
                                           { return c.suit == obj.suits[0]; });
                    count2 = std::count_if(player->won_cards.begin(), player->won_cards.end(), [&obj](const Card &c)
                                           { return c.suit == obj.suits[1]; });
                    if (count1 != count2)
                    {
                        return false;
                    }
                    break;
                case Objective_Type::OBTAIN_ALL_CARDS_OF_COLOR:
                    // Checks that the other players never won that suit
                    if (left_player->player_inx != i)
                    {
                        if (std::find_if(left_player->won_cards.begin(), left_player->won_cards.end(), [&obj](const Card &c)
                                         { return c.suit == obj.suits[0]; }) == left_player->won_cards.end())
                        {
                            return false;
                        }
                    }
                    if (right_player->player_inx != i)
                    {
                        if (std::find_if(right_player->won_cards.begin(), right_player->won_cards.end(), [&obj](const Card &c)
                                         { return c.suit == obj.suits[0]; }) == right_player->won_cards.end())
                        {
                            return false;
                        }
                    }
                    if (curr_player->player_inx != i)
                    {
                        if (std::find_if(curr_player->won_cards.begin(), curr_player->won_cards.end(), [&obj](const Card &c)
                                         { return c.suit == obj.suits[0]; }) == curr_player->won_cards.end())
                        {
                            return false;
                        }
                    }
                    break;
                case Objective_Type::TAKE_EXACTLY_N_TRICKS:
                    if (player->won_cards.size() / 3 != obj.number)
                    {
                        return false;
                    }
                    break;
                default:
                    break;
                    ;
                }
            }
        }
        return true;
    }
    State update_state(Player *&left_player, Player *&curr_player, Player *&right_player, std::vector<std::vector<Objective>> &all_objectives, std::vector<std::vector<bool>> &all_objectives_bool, size_t &leader_inx, std::vector<Card> &curr_trick)
    {
        // Updates state based on the card just played, aka curr_trick.back() player by curr_player. Should change left_player, right_player,curr_player, might change objectives_bool,and leader_inx
        // curr_trick cards have not yet gone to won_cards of winning player
        // Only thing that changes between states is card removed from curr_players hand and added to curr_trick
        State ret;
        if (curr_trick.back().suit != curr_trick.front().suit) // If curr_player sluffed a card
        {
            ret.left_players_right_player_poss_suits = curr_trick.front().suit;
            ret.right_players_left_player_poss_suits = curr_trick.front().suit;
            left_player->update_poss_suits(curr_trick.front().suit, left_player->right_player_poss_suits);
            right_player->update_poss_suits(curr_trick.front().suit, right_player->left_player_poss_suits);
        }
        else
        {
            ret.left_players_right_player_poss_suits = *left_player->right_player_poss_suits.begin();
            ret.right_players_left_player_poss_suits = *right_player->left_player_poss_suits.begin();
        }
        left_player->update_unknowns(curr_trick);
        right_player->update_unknowns(curr_trick);
        ret.leader_inx = leader_inx;
        ret.curr_trick = curr_trick;
        if (curr_trick.size() == 3)
        {
            leader_inx = (trick_winner(curr_trick) + leader_inx) % 3;
            Player *temp = curr_player;
            switch ((leader_inx - curr_player->player_inx + 3) % 3)
            {
            case 1:
                curr_player = left_player;
                left_player = right_player;
                right_player = temp;
                break;
            case 2:
                curr_player = right_player;
                right_player = left_player;
                left_player = temp;
                break;
            default:
                break;
            }
            curr_player->win_trick(curr_trick);
            size_t trick_index = 12 - curr_player->hand.size();
            for (size_t i = 0; i < 3; i++)
            {
                for (size_t j = 0; j < all_objectives_bool[i].size(); j++)
                {
                    if (!all_objectives_bool[i][j] && leader_inx == i)
                    {
                        bool bool_for_all_uses = false;
                        size_t int_1_for_all_uses = 0;
                        size_t int_2_for_all_uses = 0;
                        std::set<Suit> set_suits_all_uses;
                        switch (all_objectives[i][j].type)
                        {
                        case Objective_Type::OBTAIN_CARDS:
                            for (Card c : all_objectives[i][j].cards)
                            {
                                if (std::find(curr_player->won_cards.begin(), curr_player->won_cards.end(), c) == curr_player->won_cards.end() && std::find(curr_trick.begin(), curr_trick.end(), c) == curr_trick.end())
                                {
                                    bool_for_all_uses = true;
                                    break;
                                }
                            }
                            if (!bool_for_all_uses)
                            {
                                all_objectives_bool[i][j] = true;
                                ret.completed_objectives.push_back({i, j});
                            }
                            break;
                        case Objective_Type::OBTAIN_CARD_WITH:
                            for (Card c : all_objectives[i][j].cards)
                            {
                                if (std::find(curr_trick.begin(), curr_trick.end(), c) == curr_trick.end())
                                {
                                    bool_for_all_uses = true;
                                    break;
                                }
                            }
                            if (!bool_for_all_uses)
                            {
                                all_objectives_bool[i][j] = true;
                                ret.completed_objectives.push_back({i, j});
                            }
                            break;
                        case Objective_Type::TAKE:
                            if (trick_index == all_objectives[i][j].trick_to_win)
                            {
                                all_objectives_bool[i][j] = true;
                                ret.completed_objectives.push_back({i, j});
                            }
                            break;
                        case Objective_Type::OBTAIN_ALL_CARDS_OF_COLOR:
                            int_1_for_all_uses = std::count_if(curr_player->won_cards.begin(), curr_player->won_cards.end(), [&all_objectives, &i, &j](Card c)
                                                               { return c.suit == all_objectives[i][j].suits[0]; });
                            int_2_for_all_uses = std::count_if(curr_trick.begin(), curr_trick.end(), [&all_objectives, &i, &j](Card c)
                                                               { return c.suit == all_objectives[i][j].suits[0]; });
                            if (int_1_for_all_uses + int_2_for_all_uses >= all_objectives[i][j].number)
                            {
                                all_objectives_bool[i][j] = true;
                                ret.completed_objectives.push_back({i, j});
                            }
                            break;
                        case Objective_Type::OBTAIN_AT_LEAST_DIFF_COLOR:
                            for (Card c : curr_trick)
                            {
                                set_suits_all_uses.insert(c.suit);
                            }
                            for (Card c : curr_player->won_cards)
                            {
                                set_suits_all_uses.insert(c.suit);
                            }
                            if (set_suits_all_uses.size() >= all_objectives[i][j].number)
                            {
                                all_objectives_bool[i][j] = true;
                                ret.completed_objectives.push_back({i, j});
                            }
                            break;
                        case Objective_Type::OBTAIN_EQUAL_OF_COLORS_SAME_TRICK:
                            for (Card c : curr_trick)
                            {
                                set_suits_all_uses.insert(c.suit);
                            }
                            if (set_suits_all_uses.size() == 3 && set_suits_all_uses.find(all_objectives[i][j].suits[0]) != set_suits_all_uses.end() && set_suits_all_uses.find(all_objectives[i][j].suits[1]) != set_suits_all_uses.end())
                            {
                                all_objectives_bool[i][j] = true;
                                ret.completed_objectives.push_back({i, j});
                            }
                            break;
                        case Objective_Type::OBTAIN_CARD_CERTAIN_TRICK:
                            if (trick_index == all_objectives[i][j].trick_to_win && std::find(curr_trick.begin(), curr_trick.end(), all_objectives[i][j].cards[0]) != curr_trick.end())
                            {
                                all_objectives_bool[i][j] = true;
                                ret.completed_objectives.push_back({i, j});
                            }
                            break;
                        case Objective_Type::TAKE_TRICK_WITH_SUM_LESS:
                            int_1_for_all_uses = std::accumulate(curr_trick.begin(), curr_trick.end(), 0, [](size_t acc, Card c)
                                                                 { return acc + c.value; });
                            if (int_1_for_all_uses < all_objectives[i][j].number)
                            {
                                all_objectives_bool[i][j] = true;
                                ret.completed_objectives.push_back({i, j});
                            }
                            break;
                        case Objective_Type::TAKE_TRICK_WITH_SUM_MORE:
                            int_1_for_all_uses = std::accumulate(curr_trick.begin(), curr_trick.end(), 0, [](size_t acc, Card c)
                                                                 { return acc + c.value; });
                            if (int_1_for_all_uses > all_objectives[i][j].number)
                            {
                                all_objectives_bool[i][j] = true;
                                ret.completed_objectives.push_back({i, j});
                            }
                            break;
                        case Objective_Type::TAKE_TRICK_WITH_SUM_EQUAL:
                            int_1_for_all_uses = std::accumulate(curr_trick.begin(), curr_trick.end(), 0, [](size_t acc, Card c)
                                                                 { return acc + c.value; });
                            if (int_1_for_all_uses == all_objectives[i][j].number)
                            {
                                all_objectives_bool[i][j] = true;
                                ret.completed_objectives.push_back({i, j});
                            }
                            break;
                        case Objective_Type::TAKE_TRICK_WITH_ODD:
                            // Checks that there are no even cards in the trick
                            if (std::find_if(curr_trick.begin(), curr_trick.end(), [](Card c)
                                             { return c.value % 2 == 0; }) == curr_trick.end())
                            {
                                all_objectives_bool[i][j] = true;
                                ret.completed_objectives.push_back({i, j});
                            }
                            break;
                        case Objective_Type::TAKE_TRICK_WITH_EVEN:
                            // Checks that there are no odd cards in the trick
                            if (std::find_if(curr_trick.begin(), curr_trick.end(), [](Card c)
                                             { return c.value % 2 == 1; }) == curr_trick.end())
                            {
                                all_objectives_bool[i][j] = true;
                                ret.completed_objectives.push_back({i, j});
                            }
                            break;
                        default:
                            break;
                        }
                    }
                    else if (!all_objectives_bool[i][j])
                    {
                        if (all_objectives[i][j].type == DONT_TAKE)
                        {
                            if (trick_index == all_objectives[i][j].trick_to_win)
                            {
                                all_objectives_bool[i][j] = true;
                                ret.completed_objectives.push_back({i, j});
                            }
                            break;
                        }
                    }
                }
            }
            curr_trick.clear();
        }
        else
        {
            Player *temp = curr_player;
            curr_player = left_player;
            left_player = right_player;
            right_player = temp;
        }
        return ret;
    }
    std::unordered_map<Card, double> calculate_win_prob(Player *&left_player, Player *&curr_player, Player *&right_player, std::vector<std::vector<Objective>> &all_objectives, std::vector<std::vector<bool>> &all_objectives_bool, size_t &leader_inx, std::vector<Card> &curr_trick)
    {
        std::unordered_map<Card, double> ret;
        for (Card c : get_legal_moves(curr_player->hand, curr_trick))
        {
            // UPdate things here assuming curr_player played c
            curr_trick.push_back(c);
            curr_player->hand.erase(curr_player->hand.find(c));
            size_t spots_ahead = curr_trick.size() == 3 ? (trick_winner(curr_trick) + 3 - curr_player->player_inx) % 3 : 1;
            ret[c] = calculate_win_prob_recursive(left_player, curr_player, right_player, all_objectives, all_objectives_bool, leader_inx, curr_trick, curr_player->hand, spots_ahead);
            curr_trick.pop_back();
            curr_player->hand.insert(c);
        }
        return ret;
    } // [IMPORTANT]: RETHINK THIS SO THAT I KNOW THE RETURN TYPES OF EVCERYTHING. DOUBLE NEEDED BC RECURSION BUT I DON'T WANT THE STATE TO INCLUDE THE CARD_JUST_PLAYED
    double calculate_win_prob_recursive(Player *left_player, Player *curr_player, Player *right_player, std::vector<std::vector<Objective>> &all_objectives, std::vector<std::vector<bool>> &all_objectives_bool, size_t &leader_inx, std::vector<Card> &curr_trick, std::set<Card> prev_player_actual_hand, size_t &spots_ahead_compared_to_prev_player)
    {
        // Currently we have a impossibleUnknown and impossibleKnown, but we can add a third one
        // where we check if playing a given card leads to impossiblitly without checking every
        // possibility for the opponents played cards.
        State changes = update_state(left_player, curr_player, right_player, all_objectives, all_objectives_bool, leader_inx, curr_trick);
        double ret = 0;
        if (impossibleUnknown(left_player, curr_player, right_player, all_objectives, all_objectives_bool, leader_inx, curr_trick))
        {
            ret = 0;
        }
        else if ((curr_player->hand.size() == 0 && curr_trick.size() == 0) || guaranteedSuccess(all_objectives_bool)) // Base case where prob can be calculated without recursion
        // IMPORTANT: If the curr_trick has three cards, we have not yet updated leader_inx or  all_objectives_bool yet
        // HOW DO WE DEAL WITH THIS??? -Furthermore, how do we reset it after each iteration. Must the function no change its parameters?
        {
            ret = guaranteedSuccess(all_objectives_bool) || checkForSuccess(left_player, curr_player, right_player, all_objectives, all_objectives_bool, leader_inx, curr_trick);
        }
        else
        {
            double perceived_win_prob = 0;
            // for card in legal move, calculate win prob if that card in played
            std::vector<std::pair<std::vector<Card>, std::vector<Card>>> all_permutations = calc_permutations(curr_player, leader_inx);
            double reciprocal = 1.0 / all_permutations.size();
            auto new_end = std::remove_if(all_permutations.begin(), all_permutations.end(), [&](std::pair<std::vector<Card>, std::vector<Card>> x)
                                          { return curr_player->impossibleKnown(left_player, curr_player, right_player, all_objectives, all_objectives_bool, leader_inx, curr_trick, x.first, x.second); });
            all_permutations.erase(new_end, all_permutations.end());
            double best_perceived_win_prob_so_far = 0;
            for (Card c : get_legal_moves(curr_player->hand, curr_trick))
            {
                curr_trick.push_back(c);
                curr_player->hand.erase(curr_player->hand.find(c));
                perceived_win_prob = 0;
                std::set<Card> left_player_og_hand = left_player->hand;           // These may be able to go one scope wider, it may even be possible to get rid of it if we keep one global copy and
                std::set<Card> right_player_og_hand = right_player->hand;         // one recursive copy
                std::set<Card> left_player_og_unknowns = left_player->unknowns;   // I think these two can be calculated as the
                std::set<Card> right_player_og_unknowns = right_player->unknowns; // union of the two other players hands, so don't need to be saved in memory.
                double poss_wins_pct = 0;
                int num_poss_perms = 0;
                for (std::pair<std::vector<Card>, std::vector<Card>> &perm : all_permutations)
                {

                    left_player->hand.clear();
                    left_player->hand.insert(perm.first.begin(), perm.first.end());
                    right_player->hand.clear();
                    right_player->hand.insert(perm.second.begin(), perm.second.end());
                    left_player->unknowns.clear();
                    std::set_union(right_player->hand.begin(), right_player->hand.end(), curr_player->hand.begin(), curr_player->hand.end(), std::inserter(left_player->unknowns, left_player->unknowns.begin()));
                    right_player->unknowns.clear();
                    std::set_union(left_player->hand.begin(), left_player->hand.end(), curr_player->hand.begin(), curr_player->hand.end(), std::inserter(right_player->unknowns, right_player->unknowns.begin()));
                    size_t spots_ahead = curr_trick.size() == 3 ? (trick_winner(curr_trick) + 3 - curr_player->player_inx) % 3 : 1;
                    double win_prob = calculate_win_prob_recursive(left_player, curr_player, right_player, all_objectives, all_objectives_bool, leader_inx, curr_trick, curr_player->hand, spots_ahead);
                    std::set<Card> perm_second_set;
                    switch (spots_ahead_compared_to_prev_player)
                    {
                    case 0:
                        poss_wins_pct += win_prob;
                        num_poss_perms++;
                        break;
                    case 1:
                        perm_second_set.insert(perm.second.begin(), perm.second.end());
                        if (perm_second_set == prev_player_actual_hand)
                        {
                            poss_wins_pct += win_prob;
                            num_poss_perms++;
                        }
                        break;
                    default:
                        perm_second_set.insert(perm.first.begin(), perm.first.end());
                        if (perm_second_set == prev_player_actual_hand)
                        {
                            poss_wins_pct += win_prob;
                            num_poss_perms++;
                        }
                        break;
                    }
                    perceived_win_prob += reciprocal * win_prob;
                }
                left_player->hand = left_player_og_hand;
                right_player->hand = right_player_og_hand;
                left_player->unknowns = left_player_og_unknowns;
                right_player->unknowns = right_player_og_unknowns;
                curr_trick.pop_back();
                curr_player->hand.insert(c);
                if (perceived_win_prob > best_perceived_win_prob_so_far)
                {
                    ret = poss_wins_pct / num_poss_perms; // We "don't" have a choice as to when we update this bc it's outside our control
                    if (best_perceived_win_prob_so_far > 0.99999)
                    {
                        break;
                    }
                }
            }
            // For poss arrangement of hand, see if it's impossibleKnown. If not, put yourself in the next player's shoes and see what they would do and what the win % is
        }
        // Hypothesis: The winner of the trick (if there was one) is curr_player at this point in time
        if (changes.curr_trick.size() == 3)
        {
            for (Card c : changes.curr_trick)
            {
                curr_player->won_cards.erase(curr_player->won_cards.find(c)); // Hopefully we don't need to check if card is actually there
            }
        }
        Player *temp = curr_player;
        switch ((changes.leader_inx + changes.curr_trick.size() + 5 - curr_player->player_inx) % 3)
        {
        case 1:
            curr_player = left_player;
            left_player = right_player;
            right_player = temp;
            break;
        case 2:
            curr_player = right_player;
            right_player = left_player;
            left_player = temp;
            break;
        default:
            break;
        }
        left_player->right_player_poss_suits.insert(changes.left_players_right_player_poss_suits);
        right_player->left_player_poss_suits.insert(changes.right_players_left_player_poss_suits);
        left_player->unknowns.insert(changes.curr_trick.back());
        right_player->unknowns.insert(changes.curr_trick.back());
        for (std::pair<size_t, size_t> &p : changes.completed_objectives)
        {
            all_objectives_bool[p.first][p.second] = false;
        }
        leader_inx = changes.leader_inx;
        curr_trick = changes.curr_trick;
        // Use changes to undo changes

        return ret;
    }
    std::vector<std::pair<std::vector<Card>, std::vector<Card>>> calc_permutations(Player *curr_player, size_t &leader_inx)
    {
        std::vector<Card> left_player_hand;
        std::vector<Card> right_player_hand;
        std::vector<Card> free_movers;
        for (Suit s : {PINK, YELLOW, BLUE, GREEN, BLACK})
        {
            auto itr = curr_player->right_player_poss_suits.find(s);
            auto itl = curr_player->left_player_poss_suits.find(s);
            if (itr != curr_player->right_player_poss_suits.end() && itl != curr_player->left_player_poss_suits.end())
            {
                for (Card c : curr_player->unknowns)
                {
                    if (c.suit == s)
                    {
                        free_movers.push_back(c);
                    }
                }
            }
            else if (itr != curr_player->right_player_poss_suits.end() && itl == curr_player->left_player_poss_suits.end())
            {
                for (Card c : curr_player->unknowns)
                {
                    if (c.suit == s)
                    {
                        left_player_hand.push_back(c);
                    }
                }
            }
            else if (itr == curr_player->right_player_poss_suits.end() && itl != curr_player->left_player_poss_suits.end())
            {
                for (Card c : curr_player->unknowns)
                {
                    if (c.suit == s)
                    {
                        right_player_hand.push_back(c);
                    }
                }
            }
        }
        std::vector<std::pair<std::vector<Card>, std::vector<Card>>> ret;
        size_t left_hand_size;
        size_t right_hand_size;
        if (leader_inx == (curr_player->player_inx + 2) % 3)
        { // Player to left has one less card than player to right, player to the left has as many cards as this->hand
            left_hand_size = curr_player->hand.size();
            right_hand_size = curr_player->hand.size() - 1;
            all_permutations(left_player_hand, right_player_hand, free_movers, left_hand_size, right_hand_size, ret);
        }
        else if (leader_inx == (curr_player->player_inx + 1) % 3)
        { // Player to left and right have as many cards as this->hand
            left_hand_size = curr_player->hand.size() - 1;
            right_hand_size = curr_player->hand.size() - 1;
            all_permutations(left_player_hand, right_player_hand, free_movers, left_hand_size, right_hand_size, ret);
        }
        else
        { // Player to left and right have one more card than this->hand
            left_hand_size = curr_player->hand.size();
            right_hand_size = curr_player->hand.size();
            all_permutations(left_player_hand, right_player_hand, free_movers, left_hand_size, right_hand_size, ret);
        }
        return ret;
    }
    void all_permutations(std::vector<Card> &left_player_hand, std::vector<Card> &right_player_hand, std::vector<Card> &free_movers, size_t &left_hand_max, size_t &right_hand_max, std::vector<std::pair<std::vector<Card>, std::vector<Card>>> &ret)
    {
        if (left_player_hand.size() == left_hand_max)
        {
            for (auto c : free_movers)
            {
                right_player_hand.push_back(c);
            }
            ret.push_back({left_player_hand, right_player_hand});
            for (size_t i = 0; i < free_movers.size(); i++)
            {
                right_player_hand.pop_back();
            }
        }
        else if (right_player_hand.size() == right_hand_max)
        {
            for (auto c : free_movers)
            {
                left_player_hand.push_back(c);
            }
            ret.push_back({left_player_hand, right_player_hand});
            for (size_t i = 0; i < free_movers.size(); i++)
            {
                left_player_hand.pop_back();
            }
        }
        else
        {
            right_player_hand.push_back(free_movers.back());
            free_movers.pop_back();
            all_permutations(left_player_hand, right_player_hand, free_movers, left_hand_max, right_hand_max, ret);
            left_player_hand.push_back(right_player_hand.back());
            right_player_hand.pop_back();
            all_permutations(left_player_hand, right_player_hand, free_movers, left_hand_max, right_hand_max, ret);
            free_movers.push_back(left_player_hand.back());
            left_player_hand.pop_back();
        }
    }
    void win_trick(std::vector<Card> &cards_won)
    {
        for (Card &c : cards_won)
        {
            won_cards.insert(c);
        }
    }
    Card print_info(size_t player_number, std::vector<Card> cards_played, std::vector<std::vector<Objective>> &all_objectives)
    {
        // Pieces of info for a state: Player #, calculated win%, cards, known opp cards, show win % per card?, legal moves
        std::string lines = "\n\n\n";
        lines += std::string(60, ' ') + "\033[0m" + "PLAYER " + std::to_string(player_number) + "\033[0m\nCards won (" + std::to_string(won_cards.size() / 3) + "): ";
        for (Card c : won_cards)
        {
            lines += c.stringify() + " ";
        }
        lines += "\n\n" + std::string(58, ' ') + "Card Lead: ";
        Card lead_card = cards_played[0];
        lines += (!lead_card.isValid()) ? "__" : lead_card.stringify();
        lines += (cards_played.size() > 1) ? " (" + cards_played[1].stringify() + ")" : "";
        lines += "\n\n";
        std::string left_player_info, right_player_info = "";
        left_player_info += "\033[31m";
        for (Suit s : left_player_poss_suits)
        {
            switch (s)
            {
            case PINK:
                left_player_info += "\033[38;5;13mPP\033[0m";
                break;
            case YELLOW:
                left_player_info += "\033[33mYY\033[0m";
                break;
            case BLUE:
                left_player_info += "\033[34mBB\033[0m";
                break;
            case GREEN:
                left_player_info += "\033[32mGG\033[0m";
                break;
            default:
                left_player_info += "\033[30mTT\033[0m";
                break;
            }
            left_player_info += " ";
        }
        left_player_info += "\033[0m";
        right_player_info += "\033[38;5;214m";
        for (Suit s : right_player_poss_suits)
        {
            switch (s)
            {
            case PINK:
                right_player_info += "\033[38;5;13mPP\033[0m";
                break;
            case YELLOW:
                right_player_info += "\033[33mYY\033[0m";
                break;
            case BLUE:
                right_player_info += "\033[34mBB\033[0m";
                break;
            case GREEN:
                right_player_info += "\033[32mGG\033[0m";
                break;
            default:
                right_player_info += "\033[30mTT\033[0m";
                break;
            }
            right_player_info += " ";
        }
        right_player_info += "\033[0m";
        lines += std::string(0, ' ') + "\033[31m" + "Player " + std::to_string((player_number + 1) % 3) + ": " + "\033[0m" + left_player_info;
        lines += "\033[38;5;214m" + std::string(70, ' ') + "Player " + std::to_string((player_number + 2) % 3) + ": " + "\033[0m" + right_player_info;
        std::string objectives_string;
        for (Objective &obj : all_objectives[player_inx])
        {
            objectives_string += obj.stringify() + " | ";
        }
        lines += std::string(40, ' ') + "\n\nObjective(s): " + objectives_string + "\n\n" + std::string(40, ' ') + "Hand: ";
        std::set<Card> legal_moves;
        std::set<Card> illegal_moves;
        if (lead_card.isValid())
        {
            for (Card c : hand)
            {
                if (c.suit == lead_card.suit)
                {
                    legal_moves.insert(c);
                }
                else
                {
                    illegal_moves.insert(c);
                }
            }
            if (legal_moves.size() == 0)
            {
                legal_moves = illegal_moves;
                illegal_moves = {};
            }
        }
        else
        {
            legal_moves = hand;
        }
        for (Card c : legal_moves)
        {
            lines += c.stringify() + " ";
        }
        lines += std::string(0, ' ') + "\033[1m" + "| " + "\033[0m";
        for (Card c : illegal_moves)
        {
            lines += c.stringify() + " ";
        }
        lines += "\n" + std::string(55, ' ') + "Choose a card: ";
        std::cout << lines;
        std::string user_card_string;
        std::cin >> user_card_string;
        Card user_card = string_to_card(user_card_string);
        bool user_done = false;
        std::string y_or_n;
        while (!user_done)
        {
            while (legal_moves.count(user_card) == 0)
            {
                if (!user_card.isValid())
                {
                    std::cout << lines << "(" + user_card_string + " - invalid) ";
                }
                else
                {
                    std::cout << lines << "(" + user_card_string + " - illegal) ";
                }
                std::cin >> user_card_string;
                user_card = string_to_card(user_card_string);
            }
            user_done = true;
            // double win_prob = calculate_win_prob(user_card);
            // std::cout << lines << user_card.stringify() << "\n";
            // std::cout << std::string(48, ' ') + "Would you like to play the " + user_card.stringify() + "? (Y/N) ";
            // std::cin >> y_or_n;
            // if (std::toupper(y_or_n[0]) == 'Y')
            // {
            //     user_done = true;
            // }
            // else
            // {
            //     std::cout << lines;
            //     std::cin >> user_card_string;
            // }
        }
        hand.erase(hand.find(user_card));
        // std::cout << "Calculate prob of winning? (Y/N)";
        // std::cin >> y_or_n;
        // if (std::toupper(y_or_n[0]) == 'Y')
        // {
        //     double win_prob = calculate_win_prob(user_card);
        //     std::cout << lines << user_card.stringify() << "\n" + std::string(55, ' ') + "Win prob: \033[1m" << std::fixed << std::setprecision(2) << win_prob << "%\033[0m\n";
        //     std::cout << "Press any key to continue ";
        //     std::cin >> y_or_n;
        // }
        return user_card;
    }
    std::string find_best_card(Player *&left_player, Player *&curr_player, Player *&right_player, std::vector<std::vector<Objective>> &all_objectives, std::vector<std::vector<bool>> &all_objectives_bool, size_t &leader_inx, std::vector<Card> &curr_trick)
    {
        std::unordered_map<Card, double> card_probs = calculate_win_prob(left_player, curr_player, right_player, all_objectives, all_objectives_bool, leader_inx, curr_trick);
        std::pair<Card, double> best_pair = {{9, BLACK}, -0.1};
        for (auto &pair : card_probs)
        {
            if (pair.second > best_pair.second)
            {
                best_pair = pair;
            }
        }
        return "Best card: " + best_pair.first.stringify() + ", Win Prob: " + std::to_string(best_pair.second * 100) + "%\n";
    }

    //  private:
    std::set<Card> hand;
    std::set<Suit> left_player_poss_suits;
    std::set<Suit> right_player_poss_suits;
    std::set<Card> won_cards;
    size_t player_inx;
    std::set<Card> unknowns; // All the cards that aren't in your hand and haven't been played yet. Cards in curr_trick are NOT unkwons.
};
