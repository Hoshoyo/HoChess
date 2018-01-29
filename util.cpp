#include "util.h"

s8 console_buffer[2048] = { };

#if defined(_WIN32) || defined(_WIN64)
s8* read_console(u32* out_length) {
	DWORD read;
	ReadConsole(
		GetStdHandle(STD_INPUT_HANDLE),
		console_buffer,
		sizeof(console_buffer),
		&read, 0
	);
	*out_length = read;
	while (read == 2048) {
		s8 dump_buffer[2048];
		ReadConsole(
			GetStdHandle(STD_INPUT_HANDLE),
			dump_buffer,
			sizeof(dump_buffer),
			&read, 0
		);
	}
	if (read >= 1 && console_buffer[read - 1] == '\n') {
		console_buffer[read - 1] = 0;
		*out_length -= 1;
	}
	if (read >= 2 && console_buffer[read - 2] == '\r') {
		console_buffer[read - 2] = 0;
		*out_length -= 1;
	}
	return console_buffer;
}

#elif defined(__linux__)
#include <unistd.h>
s8* read_console(u32* out_length) {
	int f = dup(STDIN_FILENO);
	size_t size = read(f, console_buffer, 2048);
	*out_length = size;
	while(size == 2048){
		s8 dump_buffer[2048];
		size = read(f, dump_buffer, sizeof(dump_buffer));
	}
	if (size >= 1 && console_buffer[size - 1] == '\n') {
		console_buffer[size - 1] = 0;
		*out_length -= 1;
	}
	if (size >= 2 && console_buffer[size - 2] == '\r') {
		console_buffer[size - 2] = 0;
		*out_length -= 1;
	}
	close(f);
	return console_buffer;
}
#endif

bool string_equal(const s8* s1, const s8* s2) {
	while (*s1++ == *s2++) {
		if (*s1 == 0 && *s2 == 0) return true;
	}
	return false;
}

bool string_equal_until_length(const s8* s1, const s8* s2, u32 length) {
	for (int i = 0; i < length; ++i) {
		if (s1[i] != s2[i])
			return false;
		if (s1[i] == 0 && s2[i] == 0)
			break;
	}
	return true;
}

void eat_spaces(s8** at) {
	while (true) {
		s8 c = **at;
		if (c == ' ' || c == '\t' || c == '\v' || c == '\f' || c == '\r')
			++(*at);
		else
			break;
	}
}
