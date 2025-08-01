How to Run The Program:
------------------------
Go to scenarios_failures.txt or scenarios_success.txt to see the scenarios that will be run.
Scenarios that are in scenarios_failures.txt will fail if the players win and vice versa.
To run the program, execute the following command in the terminal: `./a_run_analyses.sh`
This will create well-formatted output files titled `fail.*.output` or `success.*.output` in the current directory.
To see any of these, run `cat fail.*.output` or `cat success.*.output` in the terminal.

About the Game:
--------------------
The Crew is a board game by KOSMOS. It is a trick taking game which consists of a deck of 40 cards.
There are four main colors/suits: pink, yellow, blue, green. Each color/suit has nine cards numbered 1-9.
In addition to these cards, there are four cards of the trump suit (black) which are numbered 1-4. All cards are dealt evenly amongst players.
Unlike euchre or 5-3-2, this trick taking game is cooperative. Before the first trick is played but after cards are dealt, players
must choose objectives which they must complete in order to "pass" that level. Objectives are given a difficulty which varies based
on the number of players, but is always between 1-6. Whoever is dealt the black 4 ("right bower") is the start player
and will lay down the lead the first trick as well as choose their objective first. Similar to any trick taking game, you can only
"sluff" (play a card of a diffirent suit than what was lead) if you have no cards of the lead suit. I will omit the black 1 for 
three player games