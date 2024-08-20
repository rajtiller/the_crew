#pragma once
#include <iostream>
#include <cstring>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <set>
#include <algorithm>
enum Suit
{
    PINK = 0,
    YELLOW = 1,
    BLUE = 2,
    GREEN = 3,
    BLACK = 4
};
std::string suitToString(Suit suit)
{
    std::string str = "";
    switch (suit)
    {
    case PINK:
        str = "pink";
        break;
    case YELLOW:
        str = "yellow";
        break;
    case BLUE:
        str = "blue";
        break;
    case GREEN:
        str = "green";
        break;
    case BLACK:
        str = "black";
        break;
    }
    return str;
}

Suit stringToSuit(const std::string &str)
{
    // Fix later
    // std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    if (str == "PINK")
    {
        return PINK;
    }
    else if (str == "YELLOW")
    {
        return YELLOW;
    }
    else if (str == "BLUE")
    {
        return BLUE;
    }
    else if (str == "GREEN")
    {
        return GREEN;
    }
    else if (str == "BLACK")
    {
        return BLACK;
    }
    else
    {
        throw std::runtime_error("Invalid suit string");
    }
}
std::vector<Suit> suitsFromStream(std::stringstream &input)
{
    std::vector<Suit> suits;
    std::string part;
    input >> part;
    if (part != "[")
    {
        throw std::runtime_error("Invalid suit string must start with '['");
    }
    // Gets the suits
    while (input >> part)
    {
        if (part == "]")
        {
            break;
        }
        suits.push_back(stringToSuit(part));
    }
    return suits;
}
class Card
{
public:
    size_t value;
    Suit suit;
    Card()
    {
        this->value = 0;
        this->suit = PINK;
    }
    Card(size_t value, size_t suit)
    {
        this->value = value;
        this->suit = static_cast<Suit>(suit);
    }
    Card(size_t value, Suit suit)
    {
        this->value = value;
        this->suit = suit;
    }
    Card(const std::string &input)
    {
        this->value = std::stoi(input.substr(0, 1));
        if (input[1] == 'P')
        {
            this->suit = PINK;
        }
        else if (input[1] == 'Y')
        {
            this->suit = YELLOW;
        }
        else if (input[1] == 'B')
        {
            this->suit = BLUE;
        }
        else if (input[1] == 'G')
        {
            this->suit = GREEN;
        }
        else if (input[1] == 'T')
        {
            this->suit = BLACK;
        }
        else
        {
            throw std::runtime_error("Invalid card string");
        }
    }
    size_t hash()
    {
        return suit * 10 + value;
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
    bool operator>(const Card &other) const
    {
        if (suit == other.suit)
        {
            return value > other.value;
        }
        return suit > other.suit;
    }
    bool operator==(const Card &other) const
    {
        return suit == other.suit && value == other.value;
    }
};
std::vector<Card> cardsFromStream(std::stringstream &input)
{
    std::vector<Card> cards;
    std::string part;
    input >> part;
    if (part != "[")
    {
        throw std::runtime_error("Invalid card string must start with '['");
    }
    // Gets the cards
    while (input >> part)
    {
        if (part == "]")
        {
            break;
        }
        cards.push_back(Card(part));
    }
    return cards;
}
std::ostream &
operator<<(std::ostream &os, const Card &c)
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
#include <functional>

struct CardHash
{
    std::size_t operator()(const Card &card) const
    {
        // Define your hash function implementation here
        // You can combine the hash values of 'value' and 'suit' to create a unique hash
        return card.suit * 10 + card.value;
    }
};