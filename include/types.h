#pragma once

#ifndef _GNU_SOURCE
#define _GNU_SOURCE /* 리눅스 확장 기능 사용 */
#endif

// C library headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>
#include <errno.h>
#include <ctype.h>
#include <netdb.h>
#include <fcntl.h>
#include <setjmp.h>
#include <signal.h>
#include <termios.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/resource.h>

#ifndef __FreeBSD__
#include <crypt.h>
#endif


#pragma GCC diagnostic ignored "-Wunused-result"

/* ======================================================== */
/* 기본 자료형 정의                                         */
/* ======================================================== */

// Changed all bytes and short to int or unsigned <Old Comments reserved>
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


// Logical operators from interpreter.c
#define NOT !
#define AND &&
#define OR ||