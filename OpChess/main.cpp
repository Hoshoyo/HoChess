#include <stdio.h>
#include "common.h"

struct Board
{
	u64 white_side;
	u64 white_center;
	u64 black_center;
	u64 black_side;
};

typedef u64 Piece;

const u64 PIECE_NONE 			= 0;
const u64 PIECE_WHITE_PAWN		= 1;
const u64 PIECE_WHITE_BISHOP	= 2;
const u64 PIECE_WHITE_KNIGHT	= 3;
const u64 PIECE_WHITE_ROOK		= 4;
const u64 PIECE_WHITE_QUEEN	= 5;
const u64 PIECE_WHITE_KING		= 6;

const u64 PIECE_BLACK_PAWN		= 7;
const u64 PIECE_BLACK_BISHOP	= 8;
const u64 PIECE_BLACK_KNIGHT	= 9;
const u64 PIECE_BLACK_ROOK 	= 10;
const u64 PIECE_BLACK_QUEEN	= 11;
const u64 PIECE_BLACK_KING		= 12;

inline void board_put(Board* board, Piece piece, s32 rank, s32 file){
	u32* rank_ptr = (u32*)((u8*)board + (32 * rank));
	*rank_ptr = *rank_ptr & !(0xf << (file * 4));
	*rank_ptr = *rank_ptr | (piece << (file * 4));
}

void board_init(Board* board){
	// black_side High    = rank 8
	// black_side Low     = rank 7

	// black_center High  = rank 6
	// black_center Low   = rank 5

	// white_center High  = rank 4
	// white_center Low   = rank 3

	// white_side High    = rank 2
	// white_side Low     = rank 1

	// R  N  B  Q  K  B  N  R
	board->white_side = 0 		| (PIECE_WHITE_ROOK) 			| (PIECE_WHITE_KNIGHT << 4) |
	(PIECE_WHITE_BISHOP << 8) 	| (PIECE_WHITE_QUEEN << 12) 	| (PIECE_WHITE_KING << 16) |
	(PIECE_WHITE_BISHOP << 20) | (PIECE_WHITE_KNIGHT << 24) 	| (PIECE_WHITE_ROOK << 28) |
	(PIECE_WHITE_PAWN << 32) 	| (PIECE_WHITE_PAWN << 36) 	| (PIECE_WHITE_PAWN << 40) |
	(PIECE_WHITE_PAWN << 44) 	| (PIECE_WHITE_PAWN << 48) 	| (PIECE_WHITE_PAWN << 52) |
	(PIECE_WHITE_PAWN << 56) 	| (PIECE_WHITE_PAWN << 60);
	
	board->white_center = 0;
	board->black_center = 0;

	board->black_side = 0 		| (PIECE_BLACK_PAWN) 			| (PIECE_BLACK_PAWN << 4) | 
	(PIECE_BLACK_PAWN << 8) 	| (PIECE_BLACK_PAWN << 12) 	| (PIECE_BLACK_PAWN << 16) | 
	(PIECE_BLACK_PAWN << 20) 	| (PIECE_BLACK_PAWN << 24) 	| (PIECE_BLACK_PAWN << 28) |
	(PIECE_BLACK_ROOK << 32)	| (PIECE_BLACK_KNIGHT << 36) 	| (PIECE_BLACK_BISHOP << 40) |
	(PIECE_BLACK_QUEEN << 44)	| (PIECE_BLACK_KING << 48) 	| (PIECE_BLACK_BISHOP << 52) |
	(PIECE_BLACK_KNIGHT << 56)	| (PIECE_BLACK_ROOK << 60);
}

#include "ui.cpp"

int main(int argc, char** argv) {
	Board board = {};
	board_init(&board);
	board_put(&board, PIECE_WHITE_ROOK, 4, 2);

	DEBUG_print_board(&board);
	print_board(&board);

	return 0;
}
