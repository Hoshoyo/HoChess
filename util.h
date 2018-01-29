#pragma once
#include "common.h"

s8* read_console(u32* out_length);
bool string_equal(const s8* s1, const s8* s2);
bool string_equal_until_length(const s8* s1, const s8* s2, u32 length);
void eat_spaces(s8** at);
