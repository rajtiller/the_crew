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
rm *.output
# dealAllCardsRandomly [cardsInP0Hand] [cardsInP1Hand] [cardsInP2Hand] leaderInx [currTrick] pInx [objType number [cards] [suits] trickNum]
# wonCards, leftPlayerPossSuits, and rightPllayerPossSuits aren't able to be set with this setup
# don't use things that rely on a trick's index, haven't worked that out yet

# Things to test:
# OBTAIN_CARDS
# OBTAIN_CARD_WITH

echo -e "\033[90m\nRUNNING\033[0m\033[1m SUCCESSFUL\033[0m\033[90m SCENARIOS:\n-----------------------------"
line_count=$(wc -l < "scenarios_successes.txt")
for num in $(seq 1 "$line_count")
do
  start_time=$(gdate +%s%3N)
  ./main.exe $num scenarios_successes.txt > "success.$num.output"
  if [ $? -eq 0 ]; then
    echo -e -n "Test $num:\033[32m PASSED\033[90m"
  else
    echo -e -n "Test $num:\033[31m FAILED\033[90m"
  fi
  end_time=$(gdate +%s%3N)
  elapsed_time=$((end_time - start_time))
  seconds_time=$(echo "scale=3; $elapsed_time / 1000" | bc)
  if [ $elapsed_time -gt 5000 ]; then
    echo -e "\033[31m ($seconds_time seconds)\033[90m"
  else
    echo -e "\033[32m ($seconds_time seconds)\033[90m"
  fi
done
echo -e "\033[90m\nRUNNING\033[0m\033[1m FAILURE\033[0m\033[90m SCENARIOS:\n--------------------------"
line_count=$(wc -l < "scenarios_failures.txt")
for num in $(seq 1 "$line_count")
do
  start_time=$(gdate +%s%3N)
  ./main.exe $num scenarios_failures.txt > "fail.$num.output"
  if [ $? -eq 1 ]; then
    echo -e -n "Test $num:\033[32m PASSED\033[90m"
  else
    echo -e -n "Test $num:\033[31m FAILED\033[90m"
  fi
  end_time=$(gdate +%s%3N)
  elapsed_time=$((end_time - start_time))
  seconds_time=$(echo "scale=3; $elapsed_time / 1000" | bc)
  if [ $elapsed_time -gt 5000 ]; then
    echo -e "\033[31m ($seconds_time seconds)\033[90m"
  else
    echo -e "\033[32m ($seconds_time seconds)\033[90m"
  fi
done
echo -e "\033[0m"
#  cat success.4.output