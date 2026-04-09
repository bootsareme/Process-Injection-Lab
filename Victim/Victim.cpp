#include "Victim.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

void BlackjackGame::play() 
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    while (this->playerBalance > 0) 
    {
        std::cout << "\n=== Round " << this->roundsPlayed + 1 << " ===\n";
        this->playerHand.clear();
        this->dealerHand.clear();

        this->placeBet();
        this->dealInitialCards();
        this->playerTurn();
        this->dealerTurn();

        this->settleRound();
        this->roundsPlayed++;
    }

    std::cout << "Game over! You ran out of balance.\n";
}

void BlackjackGame::placeBet()
{
    do {
        unsigned bet;
        std::cout << "How much do you want to bet? $";
        std::cin >> bet;

        if (bet >= 50) 
        {
            this->betAmount = bet;
            break;
        }

        std::cout << "You must bet at least $50 to continue playing!\n";
    } while (true);
}

void BlackjackGame::dealInitialCards() 
{
    this->playerHand.push_back(rand() % 11 + 1);
    this->playerHand.push_back(rand() % 11 + 1);
    this->dealerHand.push_back(rand() % 11 + 1);
    this->dealerHand.push_back(rand() % 11 + 1);
}

void BlackjackGame::playerTurn() 
{
    char choice = 'h';
    while (choice == 'h' && this->handSum(this->playerHand) < 21) 
    {
        std::cout << "Your hand: ";
        for (int card : playerHand) 
            std::cout << card << " ";

        std::cout << "(sum=" << this->handSum(this->playerHand) << ")\n";
        std::cout << "Hit or stand (h/s)? ";
        std::cin >> choice;

        if (choice == 'h')
            this->playerHand.push_back(rand() % 11 + 1);
    }
}

void BlackjackGame::dealerTurn() {
    while (this->handSum(this->dealerHand) < 17)
        this->dealerHand.push_back(rand() % 11 + 1);

    std::cout << "Dealer hand sum: " << this->handSum(this->dealerHand) << "\n";
}

int BlackjackGame::handSum(const std::vector<int>& hand) const 
{
    int sum = 0;

    for (int card : hand) 
        sum += card;

    return sum;
}

void BlackjackGame::settleRound() 
{
    int playerTotal = this->handSum(this->playerHand);
    int dealerTotal = this->handSum(this->dealerHand);

    if (playerTotal > 21) 
    {
        std::cout << "You busted!\n";
        this->playerBalance -= this->betAmount;
    }
    else if (dealerTotal > 21 || playerTotal > dealerTotal) 
    {
        std::cout << "You win this round!\n";
        this->playerBalance += this->betAmount;
        this->roundsWon++;
    }
    else if (playerTotal == dealerTotal)
        std::cout << "Push!\n";
    else 
    {
        std::cout << "Dealer wins this round.\n";
        this->playerBalance -= this->betAmount;
    }

    std::cout << "Balance: $" << this->playerBalance << "\n";
}
