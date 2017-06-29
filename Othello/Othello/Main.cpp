#include "Board.h"
#include "Heuristic.h"
#include <string.h>
#include <time.h>

using namespace std;

//This struct represents a line of play.
struct Line {
	int movesNumber = 0;
	uint64_t moves[32];
};
//The principal variation obtained after a search. The first move 
//is the move for the computer.
Line principalVariation;

int negamaxAlphaBeta(Board& board, int depth, int alpha, int beta, Line& pv) {
	Line line;
	if (board.isGameComplete()) return pv.movesNumber = 0, board.gameValue();
	if (depth == 0) return pv.movesNumber = 0, Heuristic::eval(board);
	int res = -INF;
	uint64_t moves = board.getPossibleMoves(board.currentPlayer());
	if (moves != 0) {
		vector<uint64_t> movesList = board.bitboardToList(moves);
		for (const uint64_t& move : movesList) {
			uint64_t stonesToFlip = board.makeMove(move);
			int v = -negamaxAlphaBeta(board, depth - 1, -beta, -alpha, line);
			board.backMove(stonesToFlip, move);
			if (v >= beta) return v;
			alpha = max(alpha, v);
			if (v > res) {
				pv.moves[0] = move;
				memcpy(pv.moves + 1, line.moves, line.movesNumber * sizeof(uint64_t));
				pv.movesNumber = line.movesNumber + 1;
				res = v;
			}
		}
	}
	else {
		uint64_t stonesToFlip = board.makeMove(0);
		int v = -negamaxAlphaBeta(board, depth - 1, -beta, -alpha, line);
		board.backMove(stonesToFlip, 0);
		if (v >= beta) return v;
		alpha = max(alpha, v);
		if (v > res) {
			pv.moves[0] = 0;
			memcpy(pv.moves + 1, line.moves, line.movesNumber * sizeof(uint64_t));
			pv.movesNumber = line.movesNumber + 1;
			res = v;
		}
	}
	return res;
}

int negamaxAlphaBeta(Board& board) {
	return negamaxAlphaBeta(board, 6, -INF, INF, principalVariation);
}

int staticNegamaxAlphaBeta(Board& board, int depth, int alpha, int beta, Line& pv) {
	Line line;
	if (board.isGameComplete()) return pv.movesNumber = 0, board.gameValue();
	if (depth == 0) return pv.movesNumber = 0, Heuristic::staticEval(board);
	int res = -INF;
	uint64_t moves = board.getPossibleMoves(board.currentPlayer());
	if (moves != 0) {
		vector<uint64_t> movesList = board.bitboardToList(moves);
		for (const uint64_t& move : movesList) {
			uint64_t stonesToFlip = board.makeMove(move);
			int v = -staticNegamaxAlphaBeta(board, depth - 1, -beta, -alpha, line);
			board.backMove(stonesToFlip, move);
			if (v >= beta) return v;
			alpha = max(alpha, v);
			if (v > res) {
				pv.moves[0] = move;
				memcpy(pv.moves + 1, line.moves, line.movesNumber * sizeof(uint64_t));
				pv.movesNumber = line.movesNumber + 1;
				res = v;
			}
		}
	}
	else {
		uint64_t stonesToFlip = board.makeMove(0);
		int v = -staticNegamaxAlphaBeta(board, depth - 1, -beta, -alpha, line);
		board.backMove(stonesToFlip, 0);
		if (v >= beta) return v;
		alpha = max(alpha, v);
		if (v > res) {
			pv.moves[0] = 0;
			memcpy(pv.moves + 1, line.moves, line.movesNumber * sizeof(uint64_t));
			pv.movesNumber = line.movesNumber + 1;
			res = v;
		}
	}
	return res;
}

int staticNegamaxAlphaBeta(Board& board) {
	return staticNegamaxAlphaBeta(board, 5, -INF, INF, principalVariation);
}

//static board vs dynamic board
//first 2 moves are random to not get the same result each time 
void play() {
	Board board;
	while (!board.isGameComplete()) {
		if (board.getPossibleMovesNumber(board.currentPlayer()) != 0) {
			uint64_t m;
			if (board.turns % 2 == 0) {
				//uncomment random player block and comment static board if-else block
				//for random player vs dynamic board
				/*uint64_t possibleMoves = board.getPossibleMoves(board.currentPlayer());
				vector<uint64_t> allMoves = board.bitboardToList(possibleMoves);
				m = allMoves.at(rand() % allMoves.size());*/
				if (board.turns == 0) {
					uint64_t possibleMoves = board.getPossibleMoves(board.currentPlayer());
					vector<uint64_t> allMoves = board.bitboardToList(possibleMoves);
					m = allMoves.at(rand() % allMoves.size());
				}
				else {
					cout << "Static negamax value: " << staticNegamaxAlphaBeta(board) << endl;
					cout << "Turns so far: " << board.turns << endl;
					cout << "Static principal variation: " << principalVariation.movesNumber << endl;
					for (int i = 0; i < principalVariation.movesNumber; i++) {
						cout << " " << board.moveToString(principalVariation.moves[i]);
					}
					cout << endl;
					m = principalVariation.moves[0];
				}
			}
			else {
				if (board.turns == 1) {
					uint64_t possibleMoves = board.getPossibleMoves(board.currentPlayer());
					vector<uint64_t> allMoves = board.bitboardToList(possibleMoves);
					m = allMoves.at(rand() % allMoves.size());
				}
				else {
					cout << "Negamax value: " << negamaxAlphaBeta(board) << endl;
					cout << "Turns so far: " << board.turns << endl;
					cout << "Principal variation: " << principalVariation.movesNumber << endl;
					for (int i = 0; i < principalVariation.movesNumber; i++) {
						cout << " " << board.moveToString(principalVariation.moves[i]);
					}
					cout << endl;
					m = principalVariation.moves[0];
				}
			}
			board.makeMove(m);
			cout << "The move chosen was: " << board.moveToString(m) << endl;
			cout << endl << board.toString() << endl;
		}
		else {
			cout << "Player " << board.currentPlayer() << " had to pass" << endl << endl;
			board.turns = board.turns + 1;
		}
	}
	cout << "Black:" << board.stonesNumber[0] << "  White:" << board.stonesNumber[1] << endl;
	if (board.stonesNumber[0] > board.stonesNumber[1]) {
		cout << "Black won!" << endl;
	}
	else {
		cout << "White won!" << endl;
	}
}

void playWithHumain() {
	Board board;
	cout << board.toString();
	uint64_t moveHumain, m;
	while (!board.isGameComplete()) {
		if (board.getPossibleMovesNumber(board.currentPlayer()) != 0) {
			int row, col;
			bool legalCoord = false;
			if (board.turns % 2 == 0) {
				uint64_t possibleMoves = board.getPossibleMoves(board.currentPlayer());
				vector<uint64_t> humainPossibleMoves = board.bitboardToList(possibleMoves);
				do {
					cout << "Humain, your possible moves: ";
					for (int i = 0; i < humainPossibleMoves.size(); i++)
						cout << board.moveToString(humainPossibleMoves[i]);
					cout << endl << "Humain, enter row and col: ";
					cin >> row >> col;
					if (board.isCoordLegal(row, col)) {
						legalCoord = true;
						moveHumain = board.coordToBitboard(row, col);
					}
					if (!(legalCoord && board.isMoveLegal(moveHumain, board.currentPlayer())))
						cout << "illegal coord, try again" << endl;
				} while (!(legalCoord && board.isMoveLegal(moveHumain, board.currentPlayer())));
				board.makeMove(moveHumain);
				cout << endl << board.toString() << endl;
			}
			else {
				cout << "Negamax value: " << negamaxAlphaBeta(board) << endl;
				cout << "Turns so far: " << board.turns << endl;
				cout << "Principal variation: " << principalVariation.movesNumber << endl;
				for (int i = 0; i < principalVariation.movesNumber; i++) {
					cout << " " << board.moveToString(principalVariation.moves[i]);
				}
				cout << endl;
				m = principalVariation.moves[0];
				board.makeMove(m);
				cout << "The move chosen was: " << board.moveToString(m) << endl;
				cout << endl << board.toString() << endl;
			}
		}
		else {
			cout << (board.currentPlayer() == 0 ? "Humain" : "Computer") << " had to pass" << endl << endl;
			board.turns = board.turns + 1;
		}
	}
	cout << "Humain:" << board.stonesNumber[0] << "  Computer:" << board.stonesNumber[1] << endl;
	if (board.stonesNumber[0] > board.stonesNumber[1]) {
		cout << "Humain won!" << endl;
	}
	else {
		cout << "Computer won!" << endl;
	}
}
/*01010011 01101111 01110000 01101000 01101001 01100101 00100000
01000001 01110110 01111010 01101000 01100001 01101110 01110100
01100001 01100100 01111010 01100101 00001101*/

int main() {
	srand(time(NULL));
	playWithHumain();
	return 0;
}