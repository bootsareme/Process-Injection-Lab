/*
    Simple Blackjack Console Game (courtesy of GPT-5)

    This program simulates a simple blackjack game:
    - The player starts with a balance of 100 units.
    - Each round, the player bets units and draws cards against the dealer.
    - Cards are randomly generated between 1-11.
    - Player can 'hit' or 'stand'.
    - Dealer draws until reaching 17+.
    - Round winner adjusts balance.
*/

#include "Victim.h"

BlackjackGame game; // fixed offset in static DATA section

int main() 
{
    game.play();
    return 0;
}
