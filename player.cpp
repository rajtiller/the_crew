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
int trick_winner(std::vector<Card> &trick)
{
    int winner = 0;
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
    void updateUnknowns(std::vector<Card> &curr_trick)
    {
        for (Card c : curr_trick)
        {
            auto it = unknowns.find(c);
            if (it != unknowns.end())
            {
                unknowns.erase(it);
            }
        }
    }
    Card string_to_card(std::string s)
    {

        if (s.size() == 2)
        {
            try
            {
                char c = s[0];
                int value = c - '0';
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
    void update_left_player_poss_suits(Suit &s)
    {
        auto it = left_player_poss_suits.find(s);
        if (it != left_player_poss_suits.end())
        {
            left_player_poss_suits.erase(it);
        }
    }
    void update_right_player_poss_suits(Suit &s)
    {
        auto it = right_player_poss_suits.find(s);
        if (it != right_player_poss_suits.end())
        {
            right_player_poss_suits.erase(it);
        }
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
        auto hand_rec = curr_player->hand;
        auto unknowns_rec = curr_player->unknowns;
        auto won_cards_rec = curr_player->won_cards;
        bool impossible = false;
        int trick_index = 12 - hand_rec.size();
        for (int i = 0; i < all_objectives[player_inx].size(); i++)
        {
            if (all_objectives_bool[player_inx][i])
            {
                continue;
            }
            Objective obj = all_objectives[player_inx][i];
            switch (obj.type)
            {
            // Checks if the card has been played by someone else
            case Objective_Type::OBTAIN_CARDS:
                for (Card c : obj.cards)
                {
                    if (unknowns_rec.find(c) == unknowns_rec.end() && hand_rec.find(c) == hand_rec.end() && std::find(won_cards_rec.begin(), won_cards_rec.end(), c) == won_cards_rec.end())
                    {
                        // Checks if the card has not been played in this trick or the player has no chance of winning
                        if (std::find(curr_trick.begin(), curr_trick.end(), c) == curr_trick.end() || (leader_inx + trick_winner(curr_trick)) % 3 != player_inx)
                        {
                            impossible = true;
                            break;
                        }
                    }
                }
                break;
            // Checks if one of the cards has been collected but not the other
            case Objective_Type::OBTAIN_CARD_WITH:
                bool card1_played = unknowns_rec.find(obj.cards[0]) == unknowns_rec.end() && hand_rec.find(obj.cards[0]) == hand_rec.end();
                bool card2_played = unknowns_rec.find(obj.cards[1]) == unknowns_rec.end() && hand_rec.find(obj.cards[1]) == hand_rec.end();
                if (card1_played && card2_played)
                {
                    // Checks if both cards are not in the trick or the player is not winning
                    bool cards_in_trick = std::find(curr_trick.begin(), curr_trick.end(), obj.cards[0]) != curr_trick.end() && std::find(curr_trick.begin(), curr_trick.end(), obj.cards[1]) != curr_trick.end();
                    if (!cards_in_trick || (leader_inx + trick_winner(curr_trick)) % 3 != player_inx)
                    {
                        impossible = true;
                    }
                }
                break;
            // Checks if the trick has already happened
            case Objective_Type::TAKE:
                if (obj.trick_to_win < trick_index)
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
                if (std::find_if(left_player->won_cards.begin(), left_player->won_cards.end(), [obj](Card &constc)
                                 { return c.suit == obj.suits[0]; }) != left_player->won_cards.end() ||
                    std::find_if(right_player->won_cards.begin(), right_player->won_cards.end(), [obj](Card &constc)
                                 { return c.suit == obj.suits[0]; }) != right_player->won_cards.end())
                {
                    impossible = true;
                }
                break;
            case Objective_Type::TAKE_EXACTLY_N_TRICKS:
                // Check if possible to win enough
                if (hand_rec.size() + won_cards.size() < obj.number)
                {
                    impossible = true;
                    break;
                }
                // Check if too many have been won
                if (won_cards.size() > obj.number)
                {
                    impossible = true;
                    break;
                }
                break;
            case Objective_Type::TAKE_TRICK_WITH_ODD:
                // Counts number of odd cards.
                int odd_in_trick = std::count_if(curr_trick.begin(), curr_trick.end(), [](Card &const c)
                                                 { return c.value % 2 == 1; });
                // If all the cards in the trick are odd you can skip this check
                if (odd_in_trick == curr_trick.size())
                {
                    bool have_odd = std::find_if(hand_rec.begin(), hand_rec.end(), [](Card &const c)
                                                 { return c.value % 2 == 1; }) != hand_rec.end();
                    if (!have_odd)
                    {
                        impossible = true;
                        break;
                    }
                    // Removes the card from the trick that is the players so the count is correct
                    odd_in_trick--;
                }
                // Checks if including the cards in the trick there are enough in the other players hands
                bool opponent_has_at_least_two_odds = odd_in_trick + std::count_if(hand_rec.begin(), hand_rec.end(), [](Card &const c)
                                                                                   { return c.value % 2 == 1; }) >=
                                                      2;
                if (!opponent_has_at_least_two_odds)
                {
                    impossible = true;
                }
                break;
            case Objective_Type::TAKE_TRICK_WITH_EVEN:
                // Counts number of even cards.
                int even_in_trick = std::count_if(curr_trick.begin(), curr_trick.end(), [](Card &const c)
                                                  { return c.value % 2 == 0; });
                // If all the cards in the trick are even you can skip this check
                if (even_in_trick == curr_trick.size())
                {
                    bool have_even = std::find_if(hand_rec.begin(), hand_rec.end(), [](Card &const c)
                                                  { return c.value % 2 == 0; }) != hand_rec.end();
                    if (!have_even)
                    {
                        impossible = true;
                        break;
                    }
                    // Removes the card from the trick that is the players so the count is correct
                    even_in_trick--;
                }
                // Checks if including the cards in the trick there are enough in the other players hands
                bool opponent_has_at_least_two_evens = even_in_trick + std::count_if(hand_rec.begin(), hand_rec.end(), [](Card &const c)
                                                                                     { return c.value % 2 == 0; }) >=
                                                       2;
                if (!opponent_has_at_least_two_evens)
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
                int count1 = 0;
                int count2 = 0;
                switch (obj.type)
                {
                case Objective_Type::OBTAIN_EXACTLY_COLORS:
                    count1 = std::count_if(player->won_cards.begin(), player->won_cards.end(), [&obj](Card const &c)
                                           { return c.suit == obj.suits[0]; });
                    if (count1 != obj.number)
                    {
                        return false;
                    }
                    break;
                case Objective_Type::OBTAIN_MORE_OF_COLOR:
                    count1 = std::count_if(player->won_cards.begin(), player->won_cards.end(), [&obj](Card const &c)
                                           { return c.suit == obj.suits[0]; });
                    count2 = std::count_if(player->won_cards.begin(), player->won_cards.end(), [&obj](Card const &c)
                                           { return c.suit == obj.suits[1]; });
                    if (count1 <= count2)
                    {
                        return false;
                    }
                    break;
                case Objective_Type::OBTAIN_EQUAL_OF_COLORS:
                    count1 = std::count_if(player->won_cards.begin(), player->won_cards.end(), [&obj](Card const &c)
                                           { return c.suit == obj.suits[0]; });
                    count2 = std::count_if(player->won_cards.begin(), player->won_cards.end(), [&obj](Card const &c)
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
                        if (std::find_if(left_player->won_cards.begin(), left_player->won_cards.end(), [&obj](Card const &c)
                                         { return c.suit == obj.suits[0]; }) == left_player->won_cards.end())
                        {
                            return false;
                        }
                    }
                    if (right_player->player_inx != i)
                    {
                        if (std::find_if(right_player->won_cards.begin(), right_player->won_cards.end(), [&obj](Card &const c)
                                         { return c.suit == obj.suits[0]; }) == right_player->won_cards.end())
                        {
                            return false;
                        }
                    }
                    if (curr_player->player_inx != i)
                    {
                        if (std::find_if(curr_player->won_cards.begin(), curr_player->won_cards.end(), [&obj](Card &const c)
                                         { return c.suit == obj.suits[0]; }) == curr_player->won_cards.end())
                        {
                            return false;
                        }
                    }
                    break;
                case Objective_Type::TAKE_EXACTLY_N_TRICKS:
                    if (player->won_cards.size() != obj.number)
                    {
                        return false;
                    }
                    break;
                }
            }
        }
    }
    std::pair<std::pair<size_t, std::vector<Card>>, std::vector<std::pair<size_t, size_t>>> &update_state(Player *&left_player, Player *&curr_player, Player *&right_player, std::vector<std::vector<Objective>> &all_objectives, std::vector<std::vector<bool>> &all_objectives_bool, size_t &leader_inx, std::vector<Card> &curr_trick)
    {
        // Updates state based on the card just played, aka curr_trick.back() player by curr_player. Should change left_player, right_player,curr_player, might change objectives_bool,and leader_inx
        // curr_trick cards have not yet gone to won_cards of winning player
        std::pair<std::pair<size_t, std::vector<Card>>, std::vector<std::pair<size_t, size_t>>> ret = {{leader_inx, {}}, {}};
        if (curr_trick.size() == 3)
        {
            size_t winner_inx = (trick_winner(curr_trick) + leader_inx) % 3;
            Player *winner_of_trick;
            int trick_index = 12 - winner_of_trick->hand.size();
            Player *temp = curr_player;
            switch ((winner_inx - curr_player->player_inx + 3) % 3)
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
            ret.first.first = winner_inx;
            for (size_t i = 0; i < 3; i++)
            {
                for (size_t j = 0; j < all_objectives_bool[i].size(); j++)
                {
                    if (!all_objectives_bool[i][j] && winner_inx == i)
                    {
                        bool bool_for_all_uses = false;
                        int int_1_for_all_uses = 0;
                        int int_2_for_all_uses = 0;
                        std::set<Suit> set_suits_all_uses;
                        switch (all_objectives[i][j].type)
                        {
                        case Objective_Type::OBTAIN_CARDS:
                            for (Card c : all_objectives[i][j].cards)
                            {
                                if (std::find(winner_of_trick->won_cards.begin(), winner_of_trick->won_cards.end(), c) == winner_of_trick->won_cards.end() && std::find(curr_trick.begin(), curr_trick.end(), c) == curr_trick.end())
                                {
                                    bool_for_all_uses = true;
                                    break;
                                }
                            }
                            if (!bool_for_all_uses)
                            {
                                all_objectives_bool[i][j] = true;
                                ret.second.push_back({i, j});
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
                                ret.second.push_back({i, j});
                            }
                            break;
                        case Objective_Type::TAKE:
                            if (trick_index == all_objectives[i][j].trick_to_win)
                            {
                                all_objectives_bool[i][j] = true;
                                ret.second.push_back({i, j});
                            }
                            break;
                        case Objective_Type::OBTAIN_ALL_CARDS_OF_COLOR:
                            int_1_for_all_uses = std::count_if(winner_of_trick->won_cards.begin(), winner_of_trick->won_cards.end(), [&all_objectives, &i, &j](Card c)
                                                               { return c.suit == all_objectives[i][j].suits[0]; });
                            int_2_for_all_uses = std::count_if(curr_trick.begin(), curr_trick.end(), [&all_objectives, &i, &j](Card c)
                                                               { return c.suit == all_objectives[i][j].suits[0]; });
                            if (int_1_for_all_uses + int_2_for_all_uses >= all_objectives[i][j].number)
                            {
                                all_objectives_bool[i][j] = true;
                                ret.second.push_back({i, j});
                            }
                            break;
                        case Objective_Type::OBTAIN_AT_LEAST_DIFF_COLOR:
                            for (Card c : curr_trick)
                            {
                                set_suits_all_uses.insert(c.suit);
                            }
                            for (Card c : winner_of_trick->won_cards)
                            {
                                set_suits_all_uses.insert(c.suit);
                            }
                            if (set_suits_all_uses.size() >= all_objectives[i][j].number)
                            {
                                all_objectives_bool[i][j] = true;
                                ret.second.push_back({i, j});
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
                                ret.second.push_back({i, j});
                            }
                            break;
                        case Objective_Type::OBTAIN_CARD_CERTAIN_TRICK:
                            if (trick_index == all_objectives[i][j].trick_to_win && std::find(curr_trick.begin(), curr_trick.end(), all_objectives[i][j].cards[0]) != curr_trick.end())
                            {
                                all_objectives_bool[i][j] = true;
                                ret.second.push_back({i, j});
                            }
                            break;
                        case Objective_Type::TAKE_TRICK_WITH_SUM_LESS:
                            int_1_for_all_uses = std::accumulate(curr_trick.begin(), curr_trick.end(), 0, [](int acc, Card c)
                                                                 { return acc + c.value; });
                            if (int_1_for_all_uses < all_objectives[i][j].number)
                            {
                                all_objectives_bool[i][j] = true;
                                ret.second.push_back({i, j});
                            }
                            break;
                        case Objective_Type::TAKE_TRICK_WITH_SUM_MORE:
                            int_1_for_all_uses = std::accumulate(curr_trick.begin(), curr_trick.end(), 0, [](int acc, Card c)
                                                                 { return acc + c.value; });
                            if (int_1_for_all_uses > all_objectives[i][j].number)
                            {
                                all_objectives_bool[i][j] = true;
                                ret.second.push_back({i, j});
                            }
                            break;
                        case Objective_Type::TAKE_TRICK_WITH_SUM_EQUAL:
                            int_1_for_all_uses = std::accumulate(curr_trick.begin(), curr_trick.end(), 0, [](int acc, Card c)
                                                                 { return acc + c.value; });
                            if (int_1_for_all_uses == all_objectives[i][j].number)
                            {
                                all_objectives_bool[i][j] = true;
                                ret.second.push_back({i, j});
                            }
                            break;
                        case Objective_Type::TAKE_TRICK_WITH_ODD:
                            // Checks that there are no even cards in the trick
                            if (std::find_if(curr_trick.begin(), curr_trick.end(), [](Card c)
                                             { return c.value % 2 == 0; }) == curr_trick.end())
                            {
                                all_objectives_bool[i][j] = true;
                                ret.second.push_back({i, j});
                            }
                            break;
                        case Objective_Type::TAKE_TRICK_WITH_EVEN:
                            // Checks that there are no odd cards in the trick
                            if (std::find_if(curr_trick.begin(), curr_trick.end(), [](Card c)
                                             { return c.value % 2 == 1; }) == curr_trick.end())
                            {
                                all_objectives_bool[i][j] = true;
                                ret.second.push_back({i, j});
                            }
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
                                ret.second.push_back({i, j});
                            }
                            break;
                        }
                    }
                }
            }
        }
        else
        {
            Player *temp = curr_player;
            curr_player = left_player;
            left_player = right_player;
            right_player = temp;
        }
    }
    std::unordered_map<Card, double> calculate_win_prob(size_t &leader_inx, std::vector<Card> &curr_trick, std::vector<std::vector<Objective>> &all_objectives, std::vector<std::vector<bool>> &all_objectives_bool, Player *left_p, Player *right_p)
    {
        std::unordered_map<Card, double> ret;
        for (Card c : get_legal_moves(this->hand, curr_trick))
        {
            ret[c] = 0;
        }
        Player *left_player = left_p;
        Player *curr_player = this;
        Player *right_player = right_p;
        known_left_and_right_player_cards = calc_left_and_right_known_cards(unknowns, right_player_poss_suits, left_player_poss_suits);
        for (Card c : get_legal_moves(hand, curr_trick))
        {
            // UPdate things here assuming curr_player played c

            ret[c] = calculate_win_prob_recursive(left_player, curr_player, right_player, all_objectives, all_objectives_bool, leader_inx, curr_trick);
        }
        return ret;
    } // [IMPORTANT]: RETHINK THIS SO THAT I KNOW THE RETURN TYPES OF EVCERYTHING. DOUBLE NEEDED BC RECURSION BUT I DON'T WANT THE STATE TO INCLUDE THE CARD_JUST_PLAYED
    double calculate_win_prob_recursive(Player *left_player, Player *curr_player, Player *right_player, std::vector<std::vector<Objective>> &all_objectives, std::vector<std::vector<bool>> &all_objectives_bool, size_t &leader_inx, std::vector<Card> &curr_trick)
    {
        // Currently we have a impossibleUnknown and impossibleKnown, but we can add a third one
        // where we check if playing a given card leads to impossiblitly without checking every
        // possibility for the opponents played cards.
        if (impossibleUnknown(left_player, curr_player, right_player, all_objectives, all_objectives_bool, leader_inx, curr_trick))
        {
            return 0;
        }
        std::pair<std::pair<size_t, std::vector<Card>>, std::vector<std::pair<size_t, size_t>>> changes = update_state(left_player, curr_player, right_player, all_objectives, all_objectives_bool, leader_inx, curr_trick);
        if (curr_player->hand.size() == 0 && curr_trick.size() == 0 || guaranteedSuccess(all_objectives_bool)) // Base case where prob can be calculated without recursion
        // IMPORTANT: If the curr_trick has three cards, we have not yet updated leader_inx or  all_objectives_bool yet
        // HOW DO WE DEAL WITH THIS??? -Furthermore, how do we reset it after each iteration. Must the function no change its parameters?
        {
            bool ret = guaranteedSuccess(all_objectives_bool) || checkForSuccess(left_player, curr_player, right_player, all_objectives, all_objectives_bool, leader_inx, curr_trick);
            leader_inx = changes.first.first;
            curr_trick = changes.first.second;
            for (std::pair<size_t, size_t> &inxs : changes.second)
            {
                all_objectives_bool[inxs.first][inxs.second] = false;
            }
            return ret;
        }
        else
        {
            double best_so_far = 0;
            double running_total = 0;
            // for card in legal move, calculate win prob if that card in played
            std::vector<std::pair<std::vector<Card>, std::vector<Card>>> all_permutations = calc_permutations(curr_player->unknowns, curr_player->right_player_poss_suits, curr_player->left_player_poss_suits, leader_inx, curr_player->hand.size());
            double reciprocal = 1.0 / all_permutations.size();
            std::remove_if(all_permutations.begin(), all_permutations.end(), [&](std::pair<std::vector<Card>, std::vector<Card>> x)
                           { return curr_player->impossibleKnown(left_player, curr_player, right_player, all_objectives, all_objectives_bool, leader_inx, curr_trick, x.first, x.second); });
            for (Card c : get_legal_moves(curr_player->hand, curr_trick))
            {
                if (c.suit != curr_trick[0].suit)
                {
                    auto it = left_player->right_player_poss_suits.find(c.suit);
                    if (it != left_player->right_player_poss_suits.end())
                    {
                        left_player->right_player_poss_suits.erase(it);
                    }
                    it = right_player->left_player_poss_suits.find(c.suit);
                    if (it != right_player->left_player_poss_suits.end())
                    {
                        right_player->left_player_poss_suits.erase(it);
                    }
                }
                running_total = 0;
                for (std::pair<std::vector<Card>, std::vector<Card>> &perm : all_permutations)
                {
                    curr_trick.push_back(c);
                    curr_player->hand.erase(curr_player->hand.find(c));
                    running_total += reciprocal * calculate_win_prob_recursive(left_player, curr_player, right_player, all_objectives, all_objectives_bool, leader_inx, curr_trick);
                    curr_trick.pop_back();
                    curr_player->hand.insert(c);
                }
                if (running_total > best_so_far)
                {
                    best_so_far = running_total;
                }
            }
            leader_inx = changes.first.first;
            curr_trick = changes.first.second;
            for (std::pair<size_t, size_t> &inxs : changes.second)
            {
                all_objectives_bool[inxs.first][inxs.second] = false;
            }
            return best_so_far;
            // For poss arrangement of hand, see if it's impossibleKnown. If not, put yourself in the next player's shoes and see what they would do and what the win % is
        }
        assert(false);
        return 0;
    }
    std::pair<std::vector<Card>, std::vector<Card>> calc_left_and_right_known_cards(std::set<Card> &unknowns, std::set<Suit> &right_player_poss_suits, std::set<Suit> &left_player_poss_suits)
    {
        std::vector<Card> left_player_hand;
        std::vector<Card> right_player_hand;
        for (Suit s : {PINK, YELLOW, BLUE, GREEN, BLACK})
        {
            auto itr = right_player_poss_suits.find(s);
            auto itl = left_player_poss_suits.find(s);
            if (itr != right_player_poss_suits.end() && itl == left_player_poss_suits.end())
            {
                for (Card c : unknowns)
                {
                    if (c.suit == s)
                    {
                        left_player_hand.push_back(c);
                    }
                }
            }
            else if (itr == right_player_poss_suits.end() && itl != left_player_poss_suits.end())
            {
                for (Card c : unknowns)
                {
                    if (c.suit == s)
                    {
                        right_player_hand.push_back(c);
                    }
                }
            }
        }
        return {right_player_hand, left_player_hand};
    }
    std::vector<std::pair<std::vector<Card>, std::vector<Card>>> &calc_permutations(std::set<Card> &unknowns, std::set<Suit> &right_player_poss_suits, std::set<Suit> &left_player_poss_suits, size_t &leader_inx, size_t hand_size)
    {
        std::vector<Card> left_player_hand;
        std::vector<Card> right_player_hand;
        std::vector<Card> free_movers;
        for (Suit s : {PINK, YELLOW, BLUE, GREEN, BLACK})
        {
            auto itr = right_player_poss_suits.find(s);
            auto itl = left_player_poss_suits.find(s);
            if (itr == right_player_poss_suits.end() && itl == left_player_poss_suits.end())
            {
                for (Card c : unknowns)
                {
                    if (c.suit == s)
                    {
                        free_movers.push_back(c);
                    }
                }
            }
            else if (itr != right_player_poss_suits.end() && itl == left_player_poss_suits.end())
            {
                for (Card c : unknowns)
                {
                    if (c.suit == s)
                    {
                        left_player_hand.push_back(c);
                    }
                }
            }
            else if (itr == right_player_poss_suits.end() && itl != left_player_poss_suits.end())
            {
                for (Card c : unknowns)
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
        if (leader_inx == (player_inx + 2) % 3)
        { // Player to left has one less card than player to right, player to the left has as many cards as this->hand
            left_hand_size = hand_size;
            right_hand_size = hand_size + 1;
            all_permutations(left_player_hand, right_player_hand, free_movers, left_hand_size, right_hand_size, ret);
        }
        else if (leader_inx == (player_inx + 1) % 3)
        { // Player to left and right have as many cards as this->hand
            left_hand_size = hand_size;
            right_hand_size = hand_size;
            all_permutations(left_player_hand, right_player_hand, free_movers, left_hand_size, right_hand_size, ret);
        }
        else
        { // Player to left and right have one more card than this->hand
            left_hand_size = hand_size + 1;
            right_hand_size = hand_size + 1;
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
            for (auto c : free_movers)
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
            for (auto c : free_movers)
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
    std::string find_best_card(size_t &leader_inx, std::vector<Card> &curr_trick, std::vector<std::vector<Objective>> &all_objectives, std::vector<std::vector<bool>> &all_objectives_bool, Player *left_p, Player *right_p)
    {
        std::unordered_map<Card, double> card_probs = calculate_win_prob(leader_inx, curr_trick, all_objectives, all_objectives_bool, left_p, right_p);
        std::pair<Card, double> best_pair = {{9, BLACK}, -0.1};
        for (auto &pair : card_probs)
        {
            if (pair.second > best_pair.second)
            {
                best_pair = pair;
            }
        }
        return "Best card: " + best_pair.first.stringify() + ", Win Prob: " + std::to_string(best_pair.second) + "%\n";
    }

private:
    std::set<Card> hand;
    std::set<Suit> left_player_poss_suits;
    std::set<Suit> right_player_poss_suits;
    std::set<Card> won_cards;
    size_t player_inx;
    std::pair<std::vector<Card>, std::vector<Card>> known_left_and_right_player_cards;
    std::set<Card> unknowns;
};
