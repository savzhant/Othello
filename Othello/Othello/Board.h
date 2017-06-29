#pragma once
#include <iostream>
#include <array>
#include <sstream>
#include <intrin.h>
#include <list>
#include <vector>
using namespace std;

static const int INF = numeric_limits<int>::max();
static const int WON = 50000; //whole border weight value's sum = 3200 for static weights board
static const int LOST = -WON;

struct Board {

	static const int BLACK = 0;
	static const int WHITE = 1;

	static const int boardSize = 8;
	array<uint64_t, 2> bitboards{};
	array<int, 2> stonesNumber{};

	static const uint64_t DOWN = 0xffffffffffffffULL;
	static const uint64_t UP = 0xffffffffffffff00ULL;
	static const uint64_t LEFT = 0xfefefefefefefefeULL;
	static const uint64_t RIGHT = 0x7f7f7f7f7f7f7f7fULL;
	static const uint64_t DOWN_RIGHT = 0x7f7f7f7f7f7f7fULL;
	static const uint64_t DOWN_LEFT = 0xfefefefefefefeULL;
	static const uint64_t UP_RIGHT = 0x7f7f7f7f7f7f7f00ULL;
	static const uint64_t UP_LEFT = 0xfefefefefefefe00ULL;
	
	static const uint64_t CORNERS = 0x8100000000000081ULL;


	int turns = 0;

	inline uint64_t occupiedSquares() const {
		return bitboards[BLACK] | bitboards[WHITE];
	}

	inline uint64_t emptySquares() const {
		return ~occupiedSquares();
	}

	inline int currentPlayer() const {
		return turns & 1 ? WHITE : BLACK;
	}

	Board();
	bool isGameComplete() const;
	bool get(uint64_t board, int row, int col) const; //is there a player's stone in this coordinate
	uint64_t coordToBitboard(int row, int col) const;
	bool isCoordLegal(int row, int col);
	uint64_t getPossibleMoves(int player) const;
	vector<uint64_t> bitboardToList(uint64_t board) const;
	string playerToString(int player) const;
	string toString() const;
	string printBitboard(uint64_t bitboard) const;
	string moveToString(uint64_t move) const; //move bitboard to coordinates
	uint64_t getStonesToFlip(uint64_t move); //get line, column, 2 diagonals from position of legal move and return stones to flip
	uint64_t makeMove(uint64_t move);
	void backMove(uint64_t stonesToFlip, uint64_t move); 
	void updateBoard(uint64_t stonesToFlip);
	void updateStonesNumber(); 
	int getPossibleMovesNumber(int player) const;
	int emptySquaresLeft() const;
	int getPotentialMovesNumber(int player) const;
	int getCornersNumber(int player) const;
	bool nearEndOfGame();
	bool isMoveLegal(uint64_t move, int player) const;
	bool hasWon(int player) const;
	int gameValue(int player) const;
	int gameValue() const;
	int wonBy() const;
};
