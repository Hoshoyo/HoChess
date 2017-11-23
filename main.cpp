#define _CRT_SECURE_NO_WARNINGS
#include "common.h"
#include "chess.h"

void print_board(Game_State* state, bool flip = false);

int main(int argc, char** argv)
{
	s32 line, column;
	s32 line_to, column_to;

	Game_State state;
	while (true) {
		print_board(&state);
		scanf("%d %d", &line, &column);
		scanf("%d %d", &line_to, &column_to);
	}
	return 0;
}