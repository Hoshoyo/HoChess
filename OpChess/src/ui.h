#pragma once
#include "chess.h"

void print_board(Board* board, Board_Select board_select);

void board_select(Board_Select* board_select, u64 rank, u64 file);
void board_deselect(Board_Select* board_select, u64 rank, u64 file);
bool board_is_selected(Board_Select board_select, u64 rank, u64 file);

void piece_select(Board* board, Game_State* game_state, Board_Select* bs, u64 rank, u64 file, bool(*validation)(Board*, Move, Game_State*));