#include <iostream>
#include <cstring>
#include <string>
#include <iomanip>
#include <vector>
#include <set>
enum Suit
{
    PINK = 0,
    YELLOW = 1,
    BLUE = 2,
    GREEN = 3,
    BLACK = 4
};
class Card
{
public:
    int value;
    Suit suit;
    Card()
    {
        this->value = 0;
        this->suit = PINK;
    }
    Card(int value, int suit)
    {
        this->value = value;
        this->suit = static_cast<Suit>(suit);
    }
    Card(int value, Suit suit)
    {
        this->value = value;
        this->suit = suit;
    }
    bool isValid()
    {
        return (suit != BLACK) || (value != 9);
    }
    std::string stringify()
    {
        switch (suit)
        {
        case PINK:
            return "\033[38;5;13m" + std::to_string(value) + "P\033[0m";
            break;
        case YELLOW:
            return "\033[33m" + std::to_string(value) + "Y\033[0m";
            break;
        case BLUE:
            return "\033[34m" + std::to_string(value) + "B\033[0m";
            break;
        case GREEN:
            return "\033[32m" + std::to_string(value) + "G\033[0m";
            break;
        default:
            return "\033[30m" + std::to_string(value) + "T\033[0m";
            break;
        }
    }
    friend std::ostream &operator<<(std::ostream &os, const Card &card);
    bool operator<(const Card &other) const
    {
        if (suit == other.suit)
        {
            return value < other.value;
        }
        return suit < other.suit;
    }
};
std::ostream &operator<<(std::ostream &os, const Card &c)
{
    switch (c.suit)
    {
    case PINK:
        os << "\033[38;5;13m" << std::to_string(c.value) << "P\033[0m";
        break;
    case YELLOW:
        os << "\033[33m" << std::to_string(c.value) << "Y\033[0m";
        break;
    case BLUE:
        os << "\033[34m" << std::to_string(c.value) << "B\033[0m";
        break;
    case GREEN:
        os << "\033[32m" << std::to_string(c.value) << "G\033[0m";
        break;
    default:
        os << "\033[30m" << std::to_string(c.value) << "T\033[0m";
        break;
    }
    return os;
}
class Player
{
public:
    Player() {}
    Player(std::vector<Card> &hand_in)
    {
        for (Card &c : hand_in)
        {
            hand.insert(c);
        }
        left_player_known_cards.insert({9, PINK});
        right_player_known_cards.insert({3, BLACK});
    }
    Card select_card(Card &c)
    {
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
    size_t hand_size()
    {
        return hand.size();
    }
    double calculate_win_prob(Card &c)
    {
        double ret = 100;
        return ret;
    }
    void print_info(size_t player_number, Card lead_card)
    {
        // Pieces of info for a state: Player #, calculated win%, cards, known opp cards, show win % per card?, legal moves
        std::string lines = "\n\n\n";
        lines += std::string(60, ' ') + "\033[0m" + "PLAYER " + std::to_string(player_number) + "\033[0m" + "\n\n" + std::string(58, ' ') + "Card Lead: ";
        lines += (!lead_card.isValid()) ? "__" : lead_card.stringify();
        lines += "\n\n";
        std::string new_line = "";
        std::string left_player_info, right_player_info = "";
        for (Card c : left_player_known_cards)
        {
            left_player_info += c.stringify() + " ";
        }
        left_player_info += "\033[31m";
        for (size_t i = 0; i < hand.size() - left_player_known_cards.size(); i++)
        {
            left_player_info += "__ ";
        }
        left_player_info += "\033[0m";
        for (Card c : right_player_known_cards)
        {
            right_player_info += c.stringify() + " ";
        }
        right_player_info += "\033[38;5;214m";
        for (size_t i = 0; i < hand.size() - right_player_known_cards.size(); i++)
        {
            right_player_info += "__ ";
        }
        right_player_info += "\033[0m";
        new_line += std::string(0, ' ') + "\033[31m" + "Player" + std::to_string((player_number + 1) % 3) + ": " + "\033[0m" + left_player_info;
        new_line += "\033[38;5;214m" + std::string(40, ' ') + "Player" + std::to_string((player_number + 2) % 3) + ": " + "\033[0m" + right_player_info;
        lines += new_line + "\n\n\n" + std::string(40, ' ') + "Hand: ";
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
            double win_prob = calculate_win_prob(user_card);
            std::cout << lines << user_card.stringify() << "\n" + std::string(55, ' ') + "Win prob: \033[1m" << std::fixed << std::setprecision(2) << win_prob << "%\033[0m\n";
            std::cout << std::string(48, ' ') + "Would you like to play the " + user_card.stringify() + "? (Y/N) ";
            std::string y_or_n;
            std::cin >> y_or_n;
            if (std::toupper(y_or_n[0]) == 'Y')
            {
                user_done = true;
            }
            else
            {
                std::cout << lines;
                std::cin >> user_card_string;
            }
        }
    }

private:
    std::set<Card> hand;
    double perceived_win_percentage;
    std::set<Card> left_player_known_cards;
    std::set<Card> right_player_known_cards;
};