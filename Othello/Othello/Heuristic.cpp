#include "Heuristic.h"
#include "Board.h"

const int Heuristic::weight[8][8] = { { 10000, -3000, 1000, 800, 800, 1000, -3000, 10000 },
{ -3000, -5000, -450, -500, -500, -450, -5000, -3000 },
{ 1000, -450, 30, 10, 10, 30, -450, 1000 },
{ 800, -500, 10, 50, 50, 10, -500, 800 },
{ 800, -500, 10, 50, 50, 10, -500, 800 },
{ 1000, -450, 30, 10, 10, 30, -450, 1000 },
{ -3000, -5000, -450, -500, -500, -450, -5000, -3000 },
{ 10000, -3000, 1000, 800, 800, 1000, -3000, 10000 } };

int Heuristic::eval(const Board& oth)
{
	double coinParity = 100 * (((double)(oth.stonesNumber[oth.currentPlayer()] - oth.stonesNumber[1 - oth.currentPlayer()])) /
		((double)(oth.stonesNumber[oth.currentPlayer()] + oth.stonesNumber[1 - oth.currentPlayer()])));

	double actualMobility = 0;
	double possibleMovesSum = (oth.getPossibleMovesNumber(oth.currentPlayer()) + oth.getPossibleMovesNumber(1 - oth.currentPlayer()));
	if (possibleMovesSum != 0)
		actualMobility = 100 * (double)(oth.getPossibleMovesNumber(oth.currentPlayer()) - oth.getPossibleMovesNumber(1 - oth.currentPlayer()))
		/ (double)possibleMovesSum;

	double potentialMobility = 0;
	double potentialMovesSum = (oth.getPotentialMovesNumber(oth.currentPlayer()) + oth.getPotentialMovesNumber(1 - oth.currentPlayer()));
	if (potentialMovesSum != 0)
		potentialMobility = 100 * (double)(oth.getPotentialMovesNumber(oth.currentPlayer()) - oth.getPotentialMovesNumber(1 - oth.currentPlayer()))
		/ (double)potentialMovesSum;

	double corners = 0;
	double cornersNumberSum = (oth.getCornersNumber(oth.currentPlayer()) + oth.getCornersNumber(1 - oth.currentPlayer()));
	if (cornersNumberSum != 0)
		corners = 100 * (double)(oth.getCornersNumber(oth.currentPlayer()) - oth.getCornersNumber(1 - oth.currentPlayer())) / (double)cornersNumberSum;

	if (oth.emptySquaresLeft() >= 45) {
		return ((15 * coinParity) + (35 * (actualMobility + potentialMobility)) + (20 * corners));
	}
	else {
		if (oth.emptySquaresLeft() >= 20 && oth.emptySquaresLeft() < 45)
		{
			return ((15 * coinParity) + (20 * (actualMobility + potentialMobility)) + (35 * corners));
		}
		else
		{
			return ((60 * coinParity) + (5 * (actualMobility + potentialMobility)) + (5 * corners));
		}
	}

}

int Heuristic::staticEval(const Board& oth)
{
	int i, j, currentPlayerWeight = 0, enemyWeight = 0;
	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			if (oth.get(oth.bitboards[oth.currentPlayer()], i, j)) {
				currentPlayerWeight += weight[i][j];
			}
			else {
				if (oth.get(oth.bitboards[1 - oth.currentPlayer()], i, j)) {
					enemyWeight += weight[i][j];
				}
			}
		}
	}
	return currentPlayerWeight - enemyWeight;
}