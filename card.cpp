
#pragma once
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
    bool operator==(const Card &other) const
    {
        return suit == other.suit && value == other.value;
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
#include <functional>

struct CardHash
{
    std::size_t operator()(const Card &card) const
    {
        // Define your hash function implementation here
        // You can combine the hash values of 'value' and 'suit' to create a unique hash
        return std::hash<size_t>()(card.value) ^ std::hash<Suit>()(card.suit);
    }
};

// Specialize the std::hash template for the Card type
namespace std
{
    template <>
    struct hash<Card>
    {
        std::size_t operator()(const Card &card) const
        {
            return CardHash()(card);
        }
    };
}