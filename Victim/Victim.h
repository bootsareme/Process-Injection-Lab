#pragma once
#include <vector>

class BlackjackGame {
public:
    BlackjackGame() = default;
    ~BlackjackGame() = default;
    void play();

private:
    std::vector<int> playerHand;
    std::vector<int> dealerHand;
    int playerBalance = 100;
    int roundsPlayed = 0;
    int roundsWon = 0;
    unsigned betAmount = 0;

    void placeBet();
    void dealInitialCards();
    void playerTurn();
    void dealerTurn();
    int handSum(const std::vector<int>& hand) const;
    void settleRound();
};
