#include <windows.h>
#include "chess.h"

enum Color {
	WHITE,
	BLACK,
	GREEN,
	NEUTRAL
};

void DEBUG_test_colors() {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	for (int i = 0; i < 1024; ++i) {
		SetConsoleTextAttribute(hConsole, i);
		printf("%d HELLO\n", i);
	}
	return;
}

void reset_color()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, 7);
}

void change_color(Color color, Color bg)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	if (color == WHITE)
	{
		if (bg == BLACK)
			SetConsoleTextAttribute(hConsole, 143);
		else
			SetConsoleTextAttribute(hConsole, 127);
	}
	else if (color == BLACK || color == NEUTRAL)
	{
		if (bg == BLACK)
			SetConsoleTextAttribute(hConsole, 128);
		else
			SetConsoleTextAttribute(hConsole, 112);
	} else if (color == GREEN) {
		if (bg == BLACK) {
			SetConsoleTextAttribute(hConsole, 138);
		} else {
			SetConsoleTextAttribute(hConsole, 122);
		}
	}
}

void print_piece(Piece p, Color bg) {
	switch (p) {
	case PIECE_NONE: {
		change_color(NEUTRAL, bg);
		printf("  ");
	}break;
	case PIECE_WHITE_PAWN: {
		change_color(WHITE, bg);
		printf("P ");
	} break;

	case PIECE_BLACK_PAWN: {
		change_color(BLACK, bg);
		printf("P ");
	}break;

	case PIECE_WHITE_KING: {
		change_color(WHITE, bg);
		printf("K ");
	}break;
	case PIECE_BLACK_KING: {
		change_color(BLACK, bg);
		printf("K ");
	}break;

	case PIECE_WHITE_QUEEN: {
		change_color(WHITE, bg);
		printf("Q ");
	}break;
	case PIECE_BLACK_QUEEN: {
		change_color(BLACK, bg);
		printf("Q ");
	}break;

	case PIECE_WHITE_ROOK: {
		change_color(WHITE, bg);
		printf("R ");
	}break;
	case PIECE_BLACK_ROOK: {
		change_color(BLACK, bg);
		printf("R ");
	}break;

	case PIECE_WHITE_BISHOP: {
		change_color(WHITE, bg);
		printf("B ");
	}break;
	case PIECE_BLACK_BISHOP: {
		change_color(BLACK, bg);
		printf("B ");
	}break;

	case PIECE_WHITE_KNIGHT: {
		change_color(WHITE, bg);
		printf("N ");
	}break;
	case PIECE_BLACK_KNIGHT: {
		change_color(BLACK, bg);
		printf("N ");
	}break;
	default: {
		change_color(NEUTRAL, bg);
		printf("  ");
	}break;
	}
}

void print_board(Game_State* state, bool flip = false) 
{
	Board &b = state->board;
	Color bg = WHITE;
	reset_color();
	printf("  A B C D E F G H\n");

	if (flip) {
		for (int i = 0; i < 8; ++i) {
			reset_color();
			printf("%d ", i + 1);
			for (int j = 0; j < 8; ++j) {
				Piece p = state->board.piece[i][j];
				print_piece(p, bg);
				bg = (Color)!bg;
			}
			bg = (Color)!bg;
			printf("\n");
		}
	} else {
		for (int i = 7; i >= 0; --i) {
			reset_color();
			printf("%d ", i + 1);
			for (int j = 0; j < 8; ++j) {
				Piece p = state->board.piece[i][j];
				print_piece(p, bg);
				bg = (Color)!bg;
			}
			bg = (Color)!bg;
			printf("\n");
		}
	}
	reset_color();
}