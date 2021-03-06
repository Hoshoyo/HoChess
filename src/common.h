#pragma once
#define internal static
#define null 0
#define FLAG(X) (1 << X)

typedef int 		s32;
typedef long long 	s64;
typedef short 		s16;
typedef char		s8;
typedef unsigned int 		u32;
typedef unsigned long long 	u64;
typedef unsigned short 		u16;
typedef unsigned char		u8;
typedef float r32;
typedef double r64;

#define USE_CRT 1

#if USE_CRT
#include <stdio.h>
#include <malloc.h>
#endif

#if defined(_WIN64) || defined(_WIN32)
#include <windows.h>
#include <windowsx.h>

#else
//#error OS not yet supported
#endif
