#include "Board.h"
using namespace std;

Board::Board()
{
	bitboards[BLACK] = 0x1008000000ULL;
	bitboards[WHITE] = 0x810000000ULL;
	updateStonesNumber();
}

bool Board::isGameComplete() const
{
	return emptySquares() == 0
		|| (getPossibleMovesNumber(currentPlayer()) == 0
			&& getPossibleMovesNumber(1 - currentPlayer()) == 0);
}

bool Board::get(uint64_t board, int row, int col) const
{
	return board & 1ULL << (row * 8 + col);
}

uint64_t Board::coordToBitboard(int row, int col) const {
	uint64_t move = 1ULL << (row * 8 + col);
	return move;
}

bool Board::isCoordLegal(int row, int col) {
	return row >= 0 && row <= 7 && col >= 0 && col <= 7;
}

uint64_t Board::getPossibleMoves(int player) const
{
	uint64_t moves = 0;
	uint64_t candidates = 0;

	//DOWN DIRECTION
	candidates = (((bitboards[player] & DOWN) << 8) & bitboards[1 - player]) & DOWN;
	while (candidates != 0) {
		moves |= emptySquares() & (candidates << 8);
		candidates = (((candidates & DOWN) << 8) & bitboards[1 - player]) & DOWN;
	}

	//UP DIRECTION
	candidates = (((bitboards[player] & UP) >> 8) & bitboards[1 - player]) & UP;
	while (candidates != 0) {
		moves |= emptySquares() & (candidates >> 8);
		candidates = (((candidates & UP) >> 8) & bitboards[1 - player]) & UP;
	}

	//RIGHT DIRECTION
	candidates = (((bitboards[player] & RIGHT) << 1) & bitboards[1 - player]) & RIGHT;
	while (candidates != 0) {
		moves |= emptySquares() & (candidates << 1);
		candidates = (((candidates & RIGHT) << 1) & bitboards[1 - player]) & RIGHT;
	}

	//LEFT DIRECTION
	candidates = (((bitboards[player] & LEFT) >> 1) & bitboards[1 - player]) & LEFT;
	while (candidates != 0) {
		moves |= emptySquares() & (candidates >> 1);
		candidates = (((candidates & LEFT) >> 1) & bitboards[1 - player]) & LEFT;
	}

	//DOWN-RIGHT DIRECTION
	candidates = (((bitboards[player] & DOWN_RIGHT) << 9) & bitboards[1 - player]) & DOWN_RIGHT;
	while (candidates != 0) {
		moves |= emptySquares() & (candidates << 9);
		candidates = (((candidates & DOWN_RIGHT) << 9) & bitboards[1 - player]) & DOWN_RIGHT;
	}

	//DOWN-LEFT DIRECTION
	candidates = (((bitboards[player] & DOWN_LEFT) << 7) & bitboards[1 - player]) & DOWN_LEFT;
	while (candidates != 0) {
		moves |= emptySquares() & (candidates << 7);
		candidates = (((candidates & DOWN_LEFT) << 7) & bitboards[1 - player]) & DOWN_LEFT;
	}

	//UP-RIGHT DIRECTION
	candidates = (((bitboards[player] & UP_RIGHT) >> 7) & bitboards[1 - player]) & UP_RIGHT;
	while (candidates != 0) {
		moves |= emptySquares() & (candidates >> 7);
		candidates = (((candidates & UP_RIGHT) >> 7) & bitboards[1 - player]) & UP_RIGHT;
	}

	//UP-LEFT DIRECTION
	candidates = (((bitboards[player] & UP_LEFT) >> 9) & bitboards[1 - player]) & UP_LEFT;
	while (candidates != 0) {
		moves |= emptySquares() & (candidates >> 9);
		candidates = (((candidates & UP_LEFT) >> 9) & bitboards[1 - player]) & UP_LEFT;
	}

	return moves; //bitboard of possible moves
}

vector<uint64_t> Board::bitboardToList(uint64_t board) const
{
	vector<uint64_t> moves;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (get(board, i, j))
				moves.push_back(coordToBitboard(i, j));
		}
	}
	return moves;
}

string Board::playerToString(int player) const
{
	return player == BLACK ? "#" : "o";
}

string Board::toString() const
{
	stringbuf buffer;
	ostream os(&buffer);
	os << "  ";
	for (int k = 0; k < 8; ++k) os << " " << k;
	os << endl << "  -----------------";
	os << endl;
	for (int i = 0; i < 8; ++i) {
		os << i << "|";
		for (int j = 0; j < 8; ++j) {
			os << " " << (get(bitboards[BLACK], i, j) ? playerToString(BLACK) :
				get(bitboards[WHITE], i, j) ? playerToString(WHITE) : ".");
		}
		os << " |" << i << endl;
	}
	os << " ";
	os << " -----------------" << endl;
	os << "  ";
	for (int k = 0; k < 8; ++k) os << " " << k;
	os << endl;
	return buffer.str();
}

string Board::printBitboard(uint64_t bitboard) const
{
	stringbuf buffer;
	ostream os(&buffer);
	os << "  ";
	for (int k = 0; k < 8; ++k) os << " " << k;
	os << endl << "  -----------------";
	os << endl;
	for (int i = 0; i < 8; ++i) {
		os << i << "|";
		for (int j = 0; j < 8; ++j) {
			os << " " << (get(bitboard, i, j) ? "x" : ".");
		}
		os << " |" << i << endl;
	}
	os << " ";
	os << " -----------------" << endl;
	os << "  ";
	for (int k = 0; k < 8; ++k) os << " " << k;
	os << endl;
	return buffer.str();
}

string Board::moveToString(uint64_t move) const
{
	if (move != 0) {
		stringbuf buffer;
		ostream os(&buffer);
		unsigned long x, y;
		int xx, yy;
		_BitScanForward64(&x, move);
		_BitScanForward64(&y, move);
		xx = x / 8;
		yy = y % 8;
		os << "(" << xx << "," << yy << ")";
		return buffer.str();
	}
	else {
		return "Pass";
	}
}

//if the move is empty, dilation is 0
uint64_t Board::getStonesToFlip(uint64_t move)
{
	uint64_t dilation = 0;
	if (move != 0) {
		uint64_t columnUp, columnDown,
			lineLeft, lineRight,
			diagonalDownRight, diagonalDownLeft,
			diagonalUpRight, diagonalUpLeft;
		uint64_t diff = move ^ bitboards[currentPlayer()]; //exclude the move from currenPlayer's bitboard for next operations

		//columnUp
		columnUp = move;
		while (!(diff & columnUp)) {
			columnUp |= columnUp >> 8;
			if (diff & columnUp)
				break;
			if ((columnUp & emptySquares()) || (columnUp & ~UP)) {
				columnUp = 0;
				break;
			}
		}
		//columnDown
		columnDown = move;
		while (!(diff & columnDown)) {
			columnDown |= columnDown << 8;
			if (diff & columnDown)
				break;
			if ((columnDown & emptySquares()) || (columnDown & ~DOWN)) {
				columnDown = 0;
				break;
			}
		}

		//lineLeft
		lineLeft = move;
		while (!(diff & lineLeft)) {
			lineLeft |= lineLeft >> 1;
			if (diff & lineLeft)
				break;
			if ((lineLeft & emptySquares()) || (lineLeft & ~LEFT)) {
				lineLeft = 0;
				break;
			}
		}

		//lineRight
		lineRight = move;
		while (!(diff & lineRight)) {
			lineRight |= lineRight << 1;
			if (diff & lineRight)
				break;
			if ((lineRight & emptySquares()) || (lineRight & ~RIGHT)) {
				lineRight = 0;
				break;
			}
		}

		//diagonalDownRight
		diagonalDownRight = move;
		while (!(diff & diagonalDownRight)) {
			diagonalDownRight |= diagonalDownRight << 9;
			if (diff & diagonalDownRight)
				break;
			if ((diagonalDownRight & emptySquares()) || (diagonalDownRight & ~DOWN_RIGHT)) { //delete line if it finds empty square or it arrives to border 
				diagonalDownRight = 0;
				break;
			}
		}

		//diagonalDownLeft
		diagonalDownLeft = move;
		while (!(diff & diagonalDownLeft)) {
			diagonalDownLeft |= diagonalDownLeft << 7;
			if (diff & diagonalDownLeft)
				break;
			if ((diagonalDownLeft & emptySquares()) || (diagonalDownLeft & ~DOWN_LEFT)) {
				diagonalDownLeft = 0;
				break;
			}
		}

		//diagonalUpLeft
		diagonalUpLeft = move;
		while (!(diff & diagonalUpLeft)) {
			diagonalUpLeft |= diagonalUpLeft >> 9;
			if (diff & diagonalUpLeft)
				break;
			if ((diagonalUpLeft & emptySquares()) || (diagonalUpLeft & ~UP_LEFT)) {
				diagonalUpLeft = 0;
				break;
			}
		}

		//diagonalUpRight
		diagonalUpRight = move;
		while (!(diff & diagonalUpRight)) {
			diagonalUpRight |= diagonalUpRight >> 7;
			if (diff & diagonalUpRight)
				break;
			if ((diagonalUpRight & emptySquares()) || (diagonalUpRight & ~UP_RIGHT)) {
				diagonalUpRight = 0;
				break;
			}
		}

		dilation = columnUp | columnDown
			| lineLeft | lineRight
			| diagonalDownRight | diagonalDownLeft
			| diagonalUpRight | diagonalUpLeft;

		return (dilation) ^ (bitboards[currentPlayer()] & dilation);
	}
	else {
		return dilation;
	}
}

// if move=0 then it means the player passed, nothing changes but turns
uint64_t Board::makeMove(uint64_t move)
{
	bitboards[currentPlayer()] |= move;
	uint64_t stonesToFlip = getStonesToFlip(move);

	updateBoard(stonesToFlip);
	updateStonesNumber();
	++turns;
	return stonesToFlip;
}

void Board::backMove(uint64_t stonesToFlip, uint64_t move)
{
	updateBoard(stonesToFlip);
	--turns;
	bitboards[currentPlayer()] ^= move;
	updateStonesNumber();
}

void Board::updateBoard(uint64_t stonesToFlip)
{
	bitboards[currentPlayer()] |= stonesToFlip;
	bitboards[1 - currentPlayer()] ^= stonesToFlip;
}

void Board::updateStonesNumber()
{
	stonesNumber[currentPlayer()] = __popcnt64(bitboards[currentPlayer()]);
	stonesNumber[1 - currentPlayer()] = __popcnt64(bitboards[1 - currentPlayer()]);
}

int Board::getPossibleMovesNumber(int player) const
{
	return __popcnt64(getPossibleMoves(player));
}

bool Board::nearEndOfGame()
{
	return __popcnt64(emptySquares()) < 15 ? true : false;
}

int Board::emptySquaresLeft() const
{
	return __popcnt64(emptySquares());
}

int Board::getPotentialMovesNumber(int player) const
{
	uint64_t potentialMoves = 0;
	//UP
	potentialMoves |= ((bitboards[1 - player] & UP) >> 8) & emptySquares();
	//DOWN
	potentialMoves |= ((bitboards[1 - player] & DOWN) << 8) & emptySquares();
	//RIGHT
	potentialMoves |= ((bitboards[1 - player] & RIGHT) << 1) & emptySquares();
	//LEFT
	potentialMoves |= ((bitboards[1 - player] & LEFT) >> 1) & emptySquares();
	//DOWN-RIGHT
	potentialMoves |= ((bitboards[1 - player] & DOWN_RIGHT) << 9) & emptySquares();
	//DOWN-LEFT
	potentialMoves |= ((bitboards[1 - player] & DOWN_LEFT) << 7) & emptySquares();
	//UP-RIGHT
	potentialMoves |= ((bitboards[1 - player] & UP_RIGHT) >> 7) & emptySquares();
	//UP-LEFT
	potentialMoves |= ((bitboards[1 - player] & UP_LEFT) >> 9) & emptySquares();
	return __popcnt64(potentialMoves);
}

int Board::getCornersNumber(int player) const
{
	return __popcnt64(bitboards[player] & CORNERS);
}

bool Board::isMoveLegal(uint64_t move, int player) const
{
	return (move & getPossibleMoves(player));
}

//(if game is ended) Compare players stones number
bool Board::hasWon(int player) const {
	return stonesNumber[player] > stonesNumber[1 - player];
}

//value according to player
int Board::gameValue(int player) const
{
	if (wonBy() == -1) return 0;
	return wonBy() == player ? WON : LOST;
}

int Board::gameValue() const
{
	return gameValue(currentPlayer());
}

//who won: Black, white or tie.
int Board::wonBy() const
{
	if (hasWon(BLACK)) return BLACK;
	if (hasWon(WHITE)) return WHITE;
	return -1;
}
