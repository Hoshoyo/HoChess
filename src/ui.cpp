#include "ui.h"
#include <stdlib.h>
#include <stdio.h>

enum Color {
	WHITE,
	BLACK,
	GREEN,
	NEUTRAL
};

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
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

void change_color(Color color, Color bg, bool selected)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	if (color == WHITE)
	{
		if (selected) {
			if (bg == BLACK)
				SetConsoleTextAttribute(hConsole, 47);
			else
				SetConsoleTextAttribute(hConsole, 175);
		} else {
			if (bg == BLACK)
				SetConsoleTextAttribute(hConsole, 143);
			else
				SetConsoleTextAttribute(hConsole, 127);
		}
	}
	else if (color == BLACK || color == NEUTRAL)
	{
		if (selected) {
			if (bg == BLACK)
				SetConsoleTextAttribute(hConsole, 32);
			else
				SetConsoleTextAttribute(hConsole, 160);
		} else {
			if (bg == BLACK)
				SetConsoleTextAttribute(hConsole, 128);
			else
				SetConsoleTextAttribute(hConsole, 112);
		}
	} else if (color == GREEN) {
		if (bg == BLACK) {
			SetConsoleTextAttribute(hConsole, 138);
		} else {
			SetConsoleTextAttribute(hConsole, 122);
		}
	}
}


void clear_screen() {
	system("cls");
}

#elif defined(__linux__)
#include <stdlib.h>

void clear_screen() {
	int res = system("clear");
}

void reset_color()
{
	printf("\033[0m");
}

void DEBUG_test_colors() {
	// 33 92 Light Green BG - Black Piece
	// 33 32 Dark Green BG  - Black Piece
	// 47 32 Dark Green BG  - White Piece
	// 47 92 Light Green BG - White Piece

	for(int i = 30; i <= 49; ++i){
		for(int j = 0; j < 255; ++j){
			printf("\e[%d;7;%dm %d %d Hello \e[0m\n", i, j, i, j);
		}
	}
	reset_color();
}

void change_color(Color color, Color bg, bool selected)
{
	if (color == WHITE)
	{
		if(selected){
			if (bg == BLACK)
				printf("\e[0m\e[32;7;47m");
			else
				printf("\e[0m\e[47;7;92m");
		} else {
			if (bg == BLACK)
				printf("\e[0m\e[47;7;90m");
			else
				printf("\e[0m\e[45;7;47m");
		}
	}
	else if (color == BLACK || color == NEUTRAL)
	{
		if(selected){
			if (bg == BLACK)
				printf("\e[0m\e[49;7;32m");
			else
				printf("\e[0m\e[49;7;92m");
		} else {
			if (bg == BLACK)
				printf("\e[0m\e[30;7;90m");
			else
				printf("\e[0m\e[30;7;39m");
		}
	}
}
#endif

void print_piece(u64 p, Color bg, bool selected) {
	switch (p) {
	case PIECE_NONE: {
		change_color(NEUTRAL, bg, selected);
		printf("  ");
	}break;
	case PIECE_WHITE_PAWN: {
		change_color(WHITE, bg, selected);
		printf("P ");
	} break;

	case PIECE_BLACK_PAWN: {
		change_color(BLACK, bg, selected);
		printf("P ");
	}break;

	case PIECE_WHITE_KING: {
		change_color(WHITE, bg, selected);
		printf("K ");
	}break;
	case PIECE_BLACK_KING: {
		change_color(BLACK, bg, selected);
		printf("K ");
	}break;

	case PIECE_WHITE_QUEEN: {
		change_color(WHITE, bg, selected);
		printf("Q ");
	}break;
	case PIECE_BLACK_QUEEN: {
		change_color(BLACK, bg, selected);
		printf("Q ");
	}break;

	case PIECE_WHITE_ROOK: {
		change_color(WHITE, bg, selected);
		printf("R ");
	}break;
	case PIECE_BLACK_ROOK: {
		change_color(BLACK, bg, selected);
		printf("R ");
	}break;

	case PIECE_WHITE_BISHOP: {
		change_color(WHITE, bg, selected);
		printf("B ");
	}break;
	case PIECE_BLACK_BISHOP: {
		change_color(BLACK, bg, selected);
		printf("B ");
	}break;

	case PIECE_WHITE_KNIGHT: {
		change_color(WHITE, bg, selected);
		printf("N ");
	}break;
	case PIECE_BLACK_KNIGHT: {
		change_color(BLACK, bg, selected);
		printf("N ");
	}break;
	default: {
		change_color(NEUTRAL, bg, selected);
		printf("  ");
	}break;
	}
}

void board_select(Board_Select* board_select, u64 rank, u64 file) {
	*board_select = *board_select | (((u64)1 << rank * 8) << file);
}
void board_deselect(Board_Select* board_select, u64 rank, u64 file) {
	*board_select = *board_select & ~(((u64)1 << rank * 8) << file);
}
bool board_is_selected(Board_Select board_select, u64 rank, u64 file) {
	return (board_select & (((u64)1 << rank * 8) << file)) != 0;
}

void piece_select(Board* board, Game_State* state, Board_Select* bs, u64 rank, u64 file, bool(*validation)(Board*, Move, Game_State*)) {
	Piece p = board->square[rank][file];
	if (p == PIECE_NONE) return;
	if (validation == 0) {
		switch (p) {
		case PIECE_WHITE_PAWN:
		case PIECE_BLACK_PAWN:		validation = pawn_valid_move; break;
		case PIECE_WHITE_BISHOP:
		case PIECE_BLACK_BISHOP:	validation = bishop_valid_move; break;
		case PIECE_WHITE_KNIGHT:
		case PIECE_BLACK_KNIGHT:	validation = knight_valid_move; break;
		case PIECE_WHITE_ROOK:
		case PIECE_BLACK_ROOK:		validation = rook_valid_move; break;
		case PIECE_WHITE_QUEEN:
		case PIECE_BLACK_QUEEN:		validation = queen_valid_move; break;
		case PIECE_WHITE_KING:
		case PIECE_BLACK_KING:		validation = king_valid_move; break;
		default: return;
		}
	}
	//board_select(bs, rank, file);
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			Move move;
			move.src_rank = rank;
			move.src_file = file;
			move.dst_rank = i;
			move.dst_file = j;
			if (validation(board, move, state)) {
				board_select(bs, i, j);
			}
		}
	}
}

void print_board(Board* board, Board_Select board_select) {
	Color bg = WHITE;
	printf("  A B C D E F G H\n");
	reset_color();
	for (s32 i = 7; i >= 0; --i) {
		reset_color();
		printf("%d ", i + 1);
		for (s32 j = 0; j < 8; ++j) {
			Piece p = board->square[i][j];
			print_piece(p, bg, board_is_selected(board_select, i, j));
			bg = (Color)!bg;
		}
		printf("\n");
		bg = (Color)!bg;
	}
	reset_color();
}