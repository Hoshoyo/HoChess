#include "common.h"
#include "chess.h"
#include "util.h"

void print_board(Game_State* state, bool flip = false);

int main(int argc, char** argv)
{
	s32 line, column;
	s32 line_to, column_to;

	Game_State state;
	while (true) {
		print_board(&state);
		u32 length;
		s8* buffer = read_console(&length);
	}
	return 0;
}