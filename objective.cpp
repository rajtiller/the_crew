#pragma once
#include <iostream>
#include <cstring>
#include <string>
#include <iomanip>
#include <vector>
#include <set>
#include "player.cpp"
#include "card.cpp"
enum Objective_Type
{
    OBTAIN_CARDS = 0,                      // Obtain a specified card or cards
    OBTAIN_CARD_WITH = 1,                  // Obtain a specified card with a another card
    TAKE = 2,                              // Take a specified trick
    DONT_TAKE = 3,                         // Don't take a specified trick
    OBTAIN_AT_LEAST_OF_COLOR = 4,          // Obtain at least a specified number of cards for a specified color
    OBTAIN_AT_LEAST_DIFF_COLOR = 5,        // Obtain at least a specified number of specified different colors
    OBTAIN_EXACTLY_COLORS = 6,             // Obtain an exact specified number of a specified color
    OBTAIN_MORE_OF_COLOR = 7,              // Obtain more of a specified color than a different specified color
    OBTAIN_EQUAL_OF_COLORS = 8,            // Obtain an equal number of two specified colors
    OBTAIN_EQUAL_OF_COLORS_SAME_TRICK = 9, // Obtain an equal number of two specified colors in the same trick.
    OBTAIN_CARD_CERTAIN_TRICK = 10,        // Obtain a specified card during a previously specified trick in the game (ex: last)
    OBTAIN_ALL_CARDS_OF_COLOR = 11,        // Obtain all cards of a specified color
    TAKE_EXACTLY_N_TRICKS = 12,            // Take exactly the specified number of tricks
    TAKE_TRICK_WITH_SUM_LESS = 13,         // Take a trick whose cards sum to less than a specified value
    TAKE_TRICK_WITH_SUM_MORE = 14,         // Take a trick whose cards sum to more than a specified value
    TAKE_TRICK_WITH_SUM_EQUAL = 15,        // Take a trick whose cards sum to the specified value(s)
    TAKE_TRICK_WITH_ODD = 16,              // Take a trick whose cards are all odd
    TAKE_TRICK_WITH_EVEN = 17,             // Take a trick whose cards are all even
    // DONT_OPEN_TRICK_WITH = 18,             // Don't open a trick with any of the spcified colors
};

class Objective
{
public:
    Objective_Type type = OBTAIN_CARDS;
    int number;
    std::vector<Card> cards;
    std::vector<Suit> suits;
    int trick_to_win;
    bool indefinite;
    Objective(Objective_Type type_in, int number_in, std::vector<Card> cards_in, std::vector<Suit> suits_in, int tricks_in, bool indefinite_in) : type(type_in), number(number_in), cards(cards_in), suits(suits_in), trick_to_win(tricks_in), indefinite(indefinite_in) {}
    Objective() {}
    std::string stringify()
    {
        std::string result = "";
        switch (this->type)
        {
        case OBTAIN_CARDS:
            result += "Obtain the card(s)";
            for (size_t i = 0; i < cards.size(); i++)
            {
                result += " " + cards[i].stringify();
            }
            result += ".";
            break;
        case OBTAIN_CARD_WITH:
            result += "Obtain the card " + cards[0].stringify() + " with the card " + cards[1].stringify() + ".";
            break;
        case TAKE:
            result += "Take trick " + std::to_string(trick_to_win) + ".";
            break;
        case DONT_TAKE:
            result += "Don't take trick " + std::to_string(trick_to_win) + ".";
            break;
        case OBTAIN_AT_LEAST_OF_COLOR:
            result += "Obtain at least " + std::to_string(number) + " card(s) of the color " + suitToString(suits[0]) + ".";
            break;
        case OBTAIN_AT_LEAST_DIFF_COLOR:
            result += "Obtain at least " + std::to_string(number) + " different color(s) of card(s).";
            break;
        case OBTAIN_EXACTLY_COLORS:
            result += "Obtain exactly " + std::to_string(number) + " card(s) of the color " + suitToString(suits[0]) + ".";
            break;
        case OBTAIN_MORE_OF_COLOR:
            result += "Obtain more of the color " + suitToString(suits[0]) + " than the color " + suitToString(suits[1]) + ".";
            break;
        case OBTAIN_EQUAL_OF_COLORS:
            result += "Obtain an equal number of the color " + suitToString(suits[0]) + " and the color " + suitToString(suits[1]) + ".";
            break;
        case OBTAIN_EQUAL_OF_COLORS_SAME_TRICK:
            result += "Obtain an equal number of the color " + suitToString(suits[0]) + " and the color " + suitToString(suits[1]) + " in the same trick.";
            break;
        case OBTAIN_CARD_CERTAIN_TRICK:
            result += "Obtain the card " + cards[0].stringify() + " in the trick " + std::to_string(trick_to_win) + ".";
            break;
        case OBTAIN_ALL_CARDS_OF_COLOR:
            result += "Obtain all cards of the color " + suitToString(suits[0]) + ".";
            break;
        case TAKE_EXACTLY_N_TRICKS:
            result += "Take exactly " + std::to_string(number) + " trick(s).";
            break;
        case TAKE_TRICK_WITH_SUM_LESS:
            result += "Take a trick whose cards sum to less than " + std::to_string(number) + ".";
            break;
        case TAKE_TRICK_WITH_SUM_MORE:
            result += "Take a trick whose cards sum to more than " + std::to_string(number) + ".";
            break;
        case TAKE_TRICK_WITH_SUM_EQUAL:
            result += "Take a trick whose cards sum to " + std::to_string(number) + ".";
            break;
        case TAKE_TRICK_WITH_ODD:
            result += "Take a trick whose cards are all odd.";
            break;
        case TAKE_TRICK_WITH_EVEN:
            result += "Take a trick whose cards are all even.";
            break;
        // case DONT_OPEN_TRICK_WITH:
        //     result += "Don't open a trick with any of the color " + suitToString(suits[0]) + ".";
        //     break;
        default:
            result += "Not Implemented.";
            break;
        }
        return result;
    }
};