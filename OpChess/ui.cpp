#include <stdlib.h>

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

void DEBUG_print_board(Board* board){
	printf("%llx\n", board->black_side);
	printf("%llx\n", board->black_center);
	printf("%llx\n", board->white_center);
	printf("%llx\n", board->white_side);
	printf("\n");
}

void print_board(Board* board, bool flip = false) 
{
	Color bg = WHITE;
	reset_color();
	printf("  A B C D E F G H\n");
	reset_color();
	printf("%d ", 8);
	for(u64 i = 8; i < 16; ++i){
		print_piece((board->black_side & ((u64)0xf << 4 * i)) >> 4 * i, bg, false);
		bg = (Color)!bg;
	}
	reset_color();
	printf("\n");
	
	bg = (Color)!bg;
	printf("%d ", 7);
	for(u64 i = 0; i < 8; ++i){
		print_piece((board->black_side & ((u64)0xf << 4 * i)) >> 4 * i, bg, false);
		bg = (Color)!bg;
	}
	reset_color();
	printf("\n");
	
	bg = (Color)!bg;
	reset_color();
	printf("%d ", 6);
	for(u64 i = 8; i < 16; ++i){
		print_piece((board->black_center & ((u64)0xf << 4 * i)) >> 4 * i, bg, false);
		bg = (Color)!bg;
	}
	reset_color();
	printf("\n");
	
	bg = (Color)!bg;
	printf("%d ", 5);
	for(u64 i = 0; i < 8; ++i){
		print_piece((board->black_center & ((u64)0xf << 4 * i)) >> 4 * i, bg, false);
		bg = (Color)!bg;
	}
	reset_color();
	printf("\n");
	
	bg = (Color)!bg;
	reset_color();
	printf("%d ", 4);
	for(u64 i = 0; i < 8; ++i){
		print_piece((board->white_center & ((u64)0xf << 4 * i)) >> 4 * i, bg, false);
		bg = (Color)!bg;
	}
	reset_color();
	printf("\n");
	
	bg = (Color)!bg;
	printf("%d ", 3);
	for(u64 i = 8; i < 16; ++i){
		print_piece((board->white_center & ((u64)0xf << 4 * i)) >> 4 * i, bg, false);
		bg = (Color)!bg;
	}
	reset_color();
	printf("\n");
	
	bg = (Color)!bg;
	reset_color();
	printf("%d ", 2);
	for(u64 i = 8; i < 16; ++i){
		print_piece((board->white_side & ((u64)0xf << 4 * i)) >> 4 * i, bg, false);
		bg = (Color)!bg;
	}
	reset_color();
	printf("\n");
	
	bg = (Color)!bg;
	printf("%d ", 1);
	for(u64 i = 0; i < 8; ++i){
		print_piece((board->white_side & ((u64)0xf << 4 * i)) >> 4 * i, bg, false);
		bg = (Color)!bg;
	}
	reset_color();
	printf("\n");
}
