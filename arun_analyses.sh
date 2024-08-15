#!/bin/bash
# OBTAIN_CARDS = 0,                      // Obtain a specified card or cards
# OBTAIN_CARD_WITH = 1,                  // Obtain a specified card with a another card
# TAKE = 2,                              // Take a specified trick, TRICKS ARE 0-INDEXED
# DONT_TAKE = 3,                         // Don't take a specified trick
# OBTAIN_AT_LEAST_OF_COLOR = 4,          // Obtain at least a specified number of cards for a specified color
# OBTAIN_AT_LEAST_DIFF_COLOR = 5,        // Obtain at least a specified number of specified different colors
# OBTAIN_EXACTLY_COLORS = 6,             // Obtain an exact specified number of a specified color
# OBTAIN_MORE_OF_COLOR = 7,              // Obtain more of a specified color than a different specified color
# OBTAIN_EQUAL_OF_COLORS = 8,            // Obtain an equal number of two specified colors
# OBTAIN_EQUAL_OF_COLORS_SAME_TRICK = 9, // Obtain an equal number of two specified colors in the same trick.
# OBTAIN_CARD_CERTAIN_TRICK = 10,        // Obtain a specified card during a previously specified trick in the game (ex: last)
# OBTAIN_ALL_CARDS_OF_COLOR = 11,        // Obtain all cards of a specified color
# TAKE_EXACTLY_N_TRICKS = 12,            // Take exactly the specified number of tricks
# TAKE_TRICK_WITH_SUM_LESS = 13,         // Take a trick whose cards sum to less than a specified value
# TAKE_TRICK_WITH_SUM_MORE = 14,         // Take a trick whose cards sum to more than a specified value
# TAKE_TRICK_WITH_SUM_EQUAL = 15,        // Take a trick whose cards sum to the specified value(s)
# TAKE_TRICK_WITH_ODD = 16,              // Take a trick whose cards are all odd
# TAKE_TRICK_WITH_EVEN = 17,             // Take a trick whose cards are all even
# // DONT_OPEN_TRICK_WITH = 18,             // Don't open a trick with any of the spcified colors
make clean
make all
# dealAllCardsRandomly [cardsInP0Hand] [cardsInP1Hand] [cardsInP2Hand] leaderInx [currTrick] pInx [objType number [cards] [suits] trickNum]
# wonCards, leftPlayerPossSuits, and rightPllayerPossSuits aren't able to be set with this setup
# don't use things that rely on a trick's index, haven't worked that out yet
echo -e "RUNNING ACCURACY TESTS:\n"

./main.exe 