#pragma once

#include <sys/types.h>

#pragma GCC diagnostic ignored "-Wunused-result"

/* ======================================================== */
/* 기본 자료형 정의                                         */
/* ======================================================== */

// Changed all bytes and short to int or unsigned
typedef short sbyte;
typedef unsigned ubyte;
typedef int sh_int;
typedef unsigned ush_int;
typedef char bool;
typedef unsigned short byte;
typedef long long LONGLONG;

#ifndef TRUE
#   define TRUE  1
#endif

#ifndef FALSE
#   define FALSE 0
#endif

/* Don't use NULL pointer( (void*)0 ) for null character ('\0') */
#ifndef NUL
#   define NUL '\0'
#endif

#ifndef NULL
#   define NULL  (void *)0
#endif
