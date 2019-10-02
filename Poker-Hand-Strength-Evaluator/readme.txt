Oxo Project Extension - Final round poker hand strength
Lucas O'Dowd-Jones

Motivation:
In poker in the final round of betting each player knows the values of 7 cards: 2 hole cards and 5 community cards.
In this final round knowing the strength of your cards is crucial in determining whether a) your cards are likely to win the showdown and b) how much it is worth betting (if your hand is weak, it may be a better strategy to fold rather than investing to see the showdown)

Functionality:
My program calculates the percentage of all the other possible hands that opposing players could have that your hand beats/ties with.
This is a good indicator of how likely you are to win the showdown.

General Implementation:
The way the program does this is by exploring all the possible combinations of hands opposing players could have.
Each combination is formed of 2 cards from the 45 remaining cards the player doesn't know the location of (could be held by another player or could still be in the deck).
The user's hand is then 'played' against each of the potential opposing hands to see whether the user would win, tie or lose in this case.
A classification function generates the maximum rank of these potential hands which are formed of the 2 potential opponent cards and the 5 community cards.

Syntax:
$ ./pokerStrength 4H 10S 5C JD 3H KS AC
The program takes 7 arguments as demonstrated above
The first 2 arguments are the user's hole cards
The following 5 arguments are the community cards (which can also be used by the opponent)
The rank of a card is represented by 1 .. 10 followed by J, Q, K, A
The suit of a card is represented by H,S,C,D (Hearts, Spades, Clubs and Diamonds respectively)

$ ./pokerStrength
Executing the program with no arguments runs the automated testing, which automatically tests logical functions

Other features:
Input validation
