/**************************************************************************
*  file: utility.c, Utility module.                       Part of DIKUMUD *
*  Usage: Utility procedures                                              *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete information. *
************************************************************************* */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <stdarg.h>

#include "structs.h"
#include "comm.h"
#include "utils.h"
#include "colors.h"

extern struct time_data time_info;
extern struct room_data *world;

/* defined in spell_parser.c */
extern char *spells[];

void send_to_char(char *messg, struct char_data *ch);

int MIN(int a, int b)
{
	return (a < b ? a : b);
}

int MAX(int a, int b)
{
	return (a > b ? a : b);
}

/* creates a random number in interval [from;to] */
int number(int from, int to)
{
	register int d, tmp;

	if (from == to)
		return from;
	if (from > to) {
		tmp = from;
		from = to;
		to = tmp;
	}

	d = to - from + 1;

	if (d < 2)
		d = 2;
	return ((random() % d) + from);
}

/* simulates dice roll */
int dice(int number, int size)
{
	int r;
	int sum = 0;

	if (size < 1)
		return 1;
	for (r = 1; r <= number; r++)
		sum += ((random() % size) + 1);

	return sum;
}

/* returns: 0 if equal, 1 if arg1 > arg2, -1 if arg1 < arg2  */
/* scan 'till found different or end of both                 */
int str_cmp(char *arg1, char *arg2)
{
	int chk, i;

	if (!arg1)
		return (!arg2 ? 0 : -1);
	else if (!arg2)
		return (1);
	for (i = 0; *(arg1 + i) || *(arg2 + i); i++) {
		if ((chk = LOWER(*(arg1 + i)) - LOWER(*(arg2 + i)))) {
			if (chk < 0)
				return (-1);
			else
				return (1);
		}
	}
	return (0);
}

/* returns: 0 if equal, 1 if arg1 > arg2, -1 if arg1 < arg2  */
/* scan 'till found different, end of both, or n reached     */
int strn_cmp(char *arg1, char *arg2, int n)
{
	int chk, i;

	for (i = 0; (*(arg1 + i) || *(arg2 + i)) && (n > 0); i++, n--) {
		if ((chk = LOWER(*(arg1 + i)) - LOWER(*(arg2 + i)))) {
			if (chk < 0)
				return (-1);
			else
				return (1);
		}
	}
	return (0);
}

/* writes a string to the log */
/* 잠재적 문제를 가진 함수라 대체
void log(char *str)
{
	time_t ct;
	char *tmstr;
	static int count = 0;

	ct = time(0);
	tmstr = asctime(localtime(&ct));
	if (count++ % 10 == 0) {
		*(tmstr + strlen(tmstr) - 1) = '\0';
		fprintf(stderr, "Current time : %s\n", tmstr);
		*(tmstr + strlen(tmstr) - 5) = '\0';
		fprintf(stderr, "%s :: %s\n", &tmstr[11], str);
	} else {
		*(tmstr + strlen(tmstr) - 6) = '\0';
		fprintf(stderr, "%s :: %s\n", &tmstr[11], str);
	}
	fflush(stderr);
}
 */
void sprintbit(long vektor, char *names[], char *result)
{
	long nr;

	*result = '\0';

	for (nr = 0; vektor; vektor >>= 1) {
		if (IS_SET(1, vektor)) {
			if (*names[nr] != '\n') {
				strcat(result, names[nr]);
				strcat(result, " ");
			} else {
				strcat(result, "UNDEFINED");
				strcat(result, " ");
			}
		}
		if (*names[nr] != '\n')
			nr++;
	}

	if (!*result)
		strcat(result, "NOBITS");
}

void sprinttype(int type, char *names[], char *result)
{
	int nr;

	for (nr = 0; (*names[nr] != '\n'); nr++) ;
	if (type < nr)
		strcpy(result, names[type]);
	else
		strcpy(result, "UNDEFINED");
}

/* Calculate the REAL time passed over the last t2-t1 centuries (secs) */
struct time_info_data real_time_passed(time_t t2, time_t t1)
{
	long secs;
	struct time_info_data now;

	secs = (long)(t2 - t1);

	now.hours = (secs / SECS_PER_REAL_HOUR) % 24;	/* 0..23 hours */
	secs -= SECS_PER_REAL_HOUR * now.hours;

	now.day = (secs / SECS_PER_REAL_DAY);	/* 0..34 days  */
	secs -= SECS_PER_REAL_DAY * now.day;

	now.month = -1;
	now.year = -1;

	return now;
}

/* Calculate the MUD time passed over the last t2-t1 centuries (secs) */
struct time_info_data mud_time_passed(time_t t2, time_t t1)
{
	long secs;
	struct time_info_data now;

	secs = (long)(t2 - t1);

	now.hours = (secs / SECS_PER_MUD_HOUR) % 24;	/* 0..23 hours */
	secs -= SECS_PER_MUD_HOUR * now.hours;

	now.day = (secs / SECS_PER_MUD_DAY) % 35;	/* 0..34 days  */
	secs -= SECS_PER_MUD_DAY * now.day;

	now.month = (secs / SECS_PER_MUD_MONTH) % 17;	/* 0..16 months */
	secs -= SECS_PER_MUD_MONTH * now.month;

	now.year = (secs / SECS_PER_MUD_YEAR);	/* 0..XX? years */

	return now;
}

struct time_info_data age(struct char_data *ch)
{
	struct time_info_data player_age;

	player_age = mud_time_passed(time(0), ch->player.time.birth);
	player_age.year += 17;	/* All players start at 17 */
	return player_age;
}

void print_increased_skilled(struct char_data *ch, int sk_no)
{
	char buf[256];

	snprintf(buf, sizeof(buf), "Your %s POWER is more skilled!\n\r", spells[sk_no - 1]);
	send_to_char(buf, ch);
}



// 잠재적 문제를 없앤 새로운 log 함수, 251018 -> 이름 변경 by Komo
void mudlog(const char *str)
{
    time_t now;
    struct tm *ts;
    char time_buf[40];

    now = time(0);
    ts = localtime(&now);

    // "HH:MM:SS" 형식의 시간 문자열 생성.
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", ts);

    fprintf(stderr, "%s :: %s\n", time_buf, str);

    fflush(stderr);
}


/* 디버깅 전용 로그 함수 */
void DEBUG_LOG(const char *format, ...)
{
    char debug_buf[MAX_STRING_LENGTH];
    char final_buf[MAX_STRING_LENGTH];
    va_list args;

    va_start(args, format);
    vsnprintf(debug_buf, sizeof(debug_buf), format, args);
    va_end(args);

    snprintf(final_buf, sizeof(final_buf), "DEBUG_LOG: %s", debug_buf);

    // 기존 log() 함수를 호출하여 기록
    log(final_buf);
}


/*
 * process_color_string:
 * MUD 색상 코드(&r, &G 등)가 포함된 문자열(input)을
 * 실제 ANSI 이스케이프 코드로 변환하여 'output' 버퍼에 write...
 */
void process_color_string(const char *input, char *output, int max_out_len)
{
    const char *in_ptr = input;
    char *out_ptr = output;
    int remaining_len = max_out_len - 1; 

    while (*in_ptr && remaining_len > 0) {
        const char *ansi_code = NULL;
        int code_len = 0;

        if (*in_ptr == '&') {
            in_ptr++;

            switch (*in_ptr) {
                case 'n': ansi_code = C_NRM; break;
                
                /* --- 클래식 16색상 (8쌍) --- */
                case 'k': ansi_code = C_BLK; break;
                case 'K': ansi_code = C_B_BLK; break;
                case 'r': ansi_code = C_RED; break;
                case 'R': ansi_code = C_B_RED; break;
                case 'g': ansi_code = C_GRN; break;
                case 'G': ansi_code = C_B_GRN; break;
                case 'y': ansi_code = C_YEL; break;
                case 'Y': ansi_code = C_B_YEL; break;
                case 'b': ansi_code = C_BLU; break;
                case 'B': ansi_code = C_B_BLU; break;
                case 'm': ansi_code = C_MAG; break;
                case 'M': ansi_code = C_B_MAG; break;
                case 'c': ansi_code = C_CYN; break;
                case 'C': ansi_code = C_B_CYN; break;
                case 'w': ansi_code = C_WHT; break;
                case 'W': ansi_code = C_B_WHT; break;
                
                /* 배경색 (숫자) -- 되살릴 방안 고민
                   1 ~ 6, 8은 아래에 사용되었음.
                   9, (, ), -, _, =, + 사용가능하다는 것은 확인
                case '0': ansi_code = C_BG_BLK; break;
                case '1': ansi_code = C_BG_RED; break;
                case '2': ansi_code = C_BG_GRN; break;
                case '3': ansi_code = C_BG_YEL; break;
                case '4': ansi_code = C_BG_BLU; break;
                case '5': ansi_code = C_BG_MAG; break;
                case '6': ansi_code = C_BG_CYN; break;
                case '7': ansi_code = C_BG_WHT; break;   */

                /* 확장 48색상 (24쌍) (a-m, o-z 까지의 알파벳) */

                /* 회색조 (Grays) */
                case 'a': ansi_code = C_GRY1_N; break; /* 어두운 회색 */
                case 'A': ansi_code = C_GRY1_B; break; /* 중간 회색 */
                case 'd': ansi_code = C_GRY2_N; break; /* 중간 밝은 회색 */
                case 'D': ansi_code = C_GRY2_B; break; /* 밝은 회색 */
                
                /* 빨강/분홍 (Reds/Pinks) */
                case 'e': ansi_code = C_RED1_N; break; /* 피 빨강 */
                case 'E': ansi_code = C_RED1_B; break; /* 불 빨강 */
                case 'f': ansi_code = C_RED2_N; break; /* 적갈색 */
                case 'F': ansi_code = C_RED2_B; break; /* 연한 빨강 */
                case 'h': ansi_code = C_PNK1_N; break; /* 진한 분홍 */
                case 'H': ansi_code = C_PNK1_B; break; /* 연한 분홍 */
                case 'i': ansi_code = C_PNK2_N; break; /* 어두운 분홍 */
                case 'I': ansi_code = C_PNK2_B; break; /* 분홍 */

                /* 갈색/주황 (Browns/Oranges) */
                case 'o': ansi_code = C_BRN1_N; break; /* 갈색 */
                case 'O': ansi_code = C_BRN1_B; break; /* 밝은 주황 */
                case 'p': ansi_code = C_ORA1_N; break; /* 어두운 주황 */
                case 'P': ansi_code = C_ORA1_B; break; /* 주황 */
                case 'q': ansi_code = C_GLD1_N; break; /* 금색 */
                case 'Q': ansi_code = C_GLD1_B; break; /* 밝은 금색 */
                case 's': ansi_code = C_TAN1_N; break; /* 황갈색 */
                case 'S': ansi_code = C_TAN1_B; break; /* 연한 노랑 */

                /* 녹색/노랑 (Greens/Yellows) */
                case 'j': ansi_code = C_OLV1_N; break; /* 올리브 */
                case 'J': ansi_code = C_OLV1_B; break; /* 연두 */
                case 'l': ansi_code = C_FOR1_N; break; /* 숲 녹색 */
                case 'L': ansi_code = C_FOR1_B; break; /* 라임 */
                case 't': ansi_code = C_TEA1_N; break; /* 짙은 청록 */
                case 'T': ansi_code = C_TEA1_B; break; /* 밝은 청록 */
                case 'u': ansi_code = C_MNT1_N; break; /* 민트 */
                case 'U': ansi_code = C_MNT1_B; break; /* 밝은 민트 */
                
                /* 파랑 (Blues) */
                case 'v': ansi_code = C_NVY1_N; break; /* 남색 */
                case 'V': ansi_code = C_NVY1_B; break; /* 밝은 남색 */
                case 'x': ansi_code = C_BLU1_N; break; /* 어두운 파랑 */
                case 'X': ansi_code = C_BLU1_B; break; /* 감청색 */
                case 'z': ansi_code = C_IND1_N; break; /* 남보라 */
                case 'Z': ansi_code = C_IND1_B; break; /* 하늘색 */

				/* 보라 */
                case '1': ansi_code = C_PUR1_N; break; 
                case '!': ansi_code = C_PUR1_B; break; 
                /* 제비꽃 */
                case '2': ansi_code = C_VIO1_N; break; 
                case '@': ansi_code = C_VIO1_B; break; 
                /* 짙은 자홍 */
                case '3': ansi_code = C_MAG1_N; break; 
                case '#': ansi_code = C_MAG1_B; break; 
                /* 바다 녹색 */
                case '4': ansi_code = C_GRN2_N; break; 
                case '$': ansi_code = C_GRN2_B; break; 
                /* 중간 파랑 - %s와 만날 때 주의! */
                case '5': ansi_code = C_BLU2_N; break; 
                case '%': ansi_code = C_BLU2_B; break; 
                /* 밝은 회색 */
                case '6': ansi_code = C_GRY3_N; break; 
                case '^': ansi_code = C_GRY3_B; break;
                /*  &0과 &7을 비워두고, &8/&*만 사용 */
                /* case '0': ansi_code = C_BG_BLK; break; (배경색으로?) */
                /* case '7': ansi_code = C_BG_WHT; break; (배경색을 쓴다면) */

				/* 짙은/중간 녹색 */
                case '8': ansi_code = C_GRN1_N; break; 
                case '*': ansi_code = C_GRN1_B; break; 

                /* '&' 문자 자체를 출력 */
                case '&': ansi_code = "&"; break;
                
                /* 유효하지 않은 코드 */
                default:
                    in_ptr--; /* '&' 문자를 그대로 출력하기 위해 되돌림 */
                    ansi_code = "&";
                    break;
            }
            
            if (ansi_code) {
                code_len = strlen(ansi_code);
                
                if (code_len <= remaining_len) {
                    strcpy(out_ptr, ansi_code);
                    out_ptr += code_len;
                    remaining_len -= code_len;
                } else {
                    break; 
                }
            }
        } else {
            /* 일반 문자: 그대로 1바이트 복사 */
            *out_ptr = *in_ptr;
            out_ptr++;
            remaining_len--;
        }

        if (*in_ptr) { 
            in_ptr++;
        }
    }

    /* [추가, 251130] 버퍼가 꽉 차서 루프가 끝난 경우, 
       마지막이 불완전한 한글 바이트인지 확인하고 정리 */
    if (remaining_len <= 0) {
        int backtrack = 0;
        char *end_ptr = out_ptr; // 현재 끝 위치
        
        while (backtrack < 4 && end_ptr > output) {
            unsigned char ch = (unsigned char)*(end_ptr - 1);
            
            if ((ch & 0x80) == 0) break;

            if ((ch & 0xC0) == 0xC0) {
                *end_ptr = '\0';
                out_ptr = end_ptr - 1;
                break;
            }
            
            end_ptr--;
            out_ptr--;
            backtrack++;
        }
    }

    *out_ptr = '\0';
}


/*
 * colortest 명령어:
 * 1. 인자(argument)가 없으면: 플레이어에게 64색상표를 계열별로 정렬하여 출력
 * 2. 인자(argument)가 있으면: 해당 인자 문자열을 색상 파싱하여 그대로 반사(echo)
 */
void do_colortest(struct char_data *ch, char *argument, int cmd)
{
    if (argument && *argument) {
        /* 인자가 있는 경우: "Echo" 모드  */
        char buf[MAX_STRING_LENGTH * 3];
        snprintf(buf, sizeof(buf), "(colortest) %s&n\n\r", argument);
        send_to_char(buf, ch);
        
        return;
    }

    send_to_char("&n--- 64 Color Test (Sorted by Color Family) ---&n\n\r", ch);
    send_to_char("&n(Format: [Code] [Normal Color] [Bright Color])\n\r", ch);

    /* --- 1. 검정, 흰색, 회색조 (Black, White, Grays) --- */
    send_to_char("\n\r&W--- 1. Grays, Black, & White ---&n\n\r", ch);
    send_to_char("(k/&KK&n): &k Sample Text :: How do you think about this?!  &n / &K Sample Text :: How do you think about this?!  &n\n\r", ch);
    send_to_char("(&ww&n/&WW&n): &w Sample Text :: How do you think about this?!  &n / &W Sample Text :: How do you think about this?!  &n\n\r", ch);
    send_to_char("(&aa&n/&AA&n): &a Sample Text :: How do you think about this?!  &n / &A Sample Text :: How do you think about this?!  &n\n\r", ch);
    send_to_char("(&dd&n/&DD&n): &d Sample Text :: How do you think about this?!  &n / &D Sample Text :: How do you think about this?!  &n\n\r", ch);
    send_to_char("(&66&n/&^^&n): &6 Sample Text :: How do you think about this?!  &n / &^ Sample Text :: How do you think about this?!  &n\n\r", ch);

    /* --- 2. 빨강, 분홍 계열 (Reds & Pinks) --- */
    send_to_char("\n\r&W--- 2. Reds & Pinks ---&n\n\r", ch);
    send_to_char("(&rr&n/&RR&n): &r Sample Text :: How do you think about this?!  &n / &R Sample Text :: How do you think about this?!  &n\n\r", ch);
    send_to_char("(&ee&n/&EE&n): &e Sample Text :: How do you think about this?!  &n / &E Sample Text :: How do you think about this?!  &n\n\r", ch);
    send_to_char("(&ff&n/&FF&n): &f Sample Text :: How do you think about this?!  &n / &F Sample Text :: How do you think about this?!  &n\n\r", ch);
    send_to_char("(&hh&n/&HH&n): &h Sample Text :: How do you think about this?!  &n / &H Sample Text :: How do you think about this?!  &n\n\r", ch);
    send_to_char("(&ii&n/&II&n): &i Sample Text :: How do you think about this?!  &n / &I Sample Text :: How do you think about this?!  &n\n\r", ch);

    /* --- 3. 주황, 노랑, 갈색 계열 (Oranges, Yellows, Browns) --- */
    send_to_char("\n\r&W--- 3. Oranges, Yellows, & Browns ---&n\n\r", ch);
    send_to_char("(&yy&n/&YY&n): &y Sample Text :: How do you think about this?!  &n / &Y Sample Text :: How do you think about this?!  &n\n\r", ch);
    send_to_char("(&oo&n/&OO&n): &o Sample Text :: How do you think about this?!  &n / &O Sample Text :: How do you think about this?!  &n\n\r", ch);
    send_to_char("(&pp&n/&PP&n): &p Sample Text :: How do you think about this?!  &n / &P Sample Text :: How do you think about this?!  &n\n\r", ch);
    send_to_char("(&qq&n/&QQ&n): &q Sample Text :: How do you think about this?!  &n / &Q Sample Text :: How do you think about this?!  &n\n\r", ch);
    send_to_char("(&ss&n/&SS&n): &s Sample Text :: How do you think about this?!  &n / &S Sample Text :: How do you think about this?!  &n\n\r", ch);

    /* --- 4. 녹색 계열 (Greens) --- */
    send_to_char("\n\r&W--- 4. Greens ---&n\n\r", ch);
    send_to_char("(&gg&n/&GG&n): &g Sample Text :: How do you think about this?!  &n / &G Sample Text :: How do you think about this?!  &n\n\r", ch);
    send_to_char("(&jj&n/&JJ&n): &j Sample Text :: How do you think about this?!  &n / &J Sample Text :: How do you think about this?!  &n\n\r", ch);
    send_to_char("(&ll&n/&LL&n): &l Sample Text :: How do you think about this?!  &n / &L Sample Text :: How do you think about this?!  &n\n\r", ch);
    send_to_char("(&uu&n/&UU&n): &u Sample Text :: How do you think about this?!  &n / &U Sample Text :: How do you think about this?!  &n\n\r", ch);
    send_to_char("(&88&n/&**&n): &8 Sample Text :: How do you think about this?!  &n / &* Sample Text :: How do you think about this?!  &n\n\r", ch); /* &n/&N에서 이동 */
    
    /* --- 5. 청록 계열 (Cyans & Teals) --- */
    send_to_char("\n\r&W--- 5. Cyans & Teals (Blue-Greens) ---&n\n\r", ch);
    send_to_char("(&cc&n/&CC&n): &c Sample Text :: How do you think about this?!  &n / &C Sample Text :: How do you think about this?!  &n\n\r", ch);
    send_to_char("(&tt&n/&TT&n): &t Sample Text :: How do you think about this?!  &n / &T Sample Text :: How do you think about this?!  &n\n\r", ch);
    send_to_char("(&44&n/&$$&n): &4 Sample Text :: How do you think about this?!  &n / &$ Sample Text :: How do you think about this?!  &n\n\r", ch); /* Sea Green */
    
    /* --- 6. 파랑 계열 (Blues) --- */
    send_to_char("\n\r&W--- 6. Blues ---&n\n\r", ch);
    send_to_char("(&bb&n/&BB&n): &b Sample Text :: How do you think about this?!  &n / &B Sample Text :: How do you think about this?!  &n\n\r", ch);
    send_to_char("(&vv&n/&VV&n): &v Sample Text :: How do you think about this?!  &n / &V Sample Text :: How do you think about this?!  &n\n\r", ch);
    send_to_char("(&xx&n/&XX&n): &x Sample Text :: How do you think about this?!  &n / &X Sample Text :: How do you think about this?!  &n\n\r", ch);
    send_to_char("(&zz&n/&ZZ&n): &z Sample Text :: How do you think about this?!  &n / &Z Sample Text :: How do you think about this?!  &n\n\r", ch);
    send_to_char("(&55&n/&%%&n): &5 Sample Text :: How do you think about this?!  &n / &% Sample Text :: How do you think about this?!  &n\n\r", ch);

    /* --- 7. 보라, 자홍 계열 (Purples & Magentas) --- */
    send_to_char("\n\r&W--- 7. Purples & Magentas ---&n\n\r", ch);
    send_to_char("(&mm&n/&MM&n): &m Sample Text :: How do you think about this?!  &n / &M Sample Text :: How do you think about this?!  &n\n\r", ch);
    send_to_char("(&11&n/&!!&n): &1 Sample Text :: How do you think about this?!  &n / &! Sample Text :: How do you think about this?!  &n\n\r", ch);
    send_to_char("(&22&n/&@@&n): &2 Sample Text :: How do you think about this?!  &n / &@ Sample Text :: How do you think about this?!  &n\n\r", ch);
    send_to_char("(&33&n/&##&n): &3 Sample Text :: How do you think about this?!  &n / &# Sample Text :: How do you think about this?!  &n\n\r", ch);
    
    send_to_char("\n\r&n--- Test End ---&n\n\r", ch);
}

/* * 시스템(OS)에 상관없이 안전하게 문자열을 이어붙이는 함수
 * dest: 목표 버퍼
 * src: 붙일 내용
 * size: 목표 버퍼의 전체 크기 (sizeof(dest))
 * 251125 by Komo
 */
#ifndef HAVE_STRLCAT
size_t strlcat(char *dest, const char *src, size_t size)
{
    size_t dlen = strlen(dest);
    size_t slen = strlen(src);
    size_t n = size;

    if (n <= dlen) return (slen + n);

    char *d = dest + dlen;
    const char *s = src;
    size_t left = n - dlen - 1;

    while (*s && left > 0) {
        *d++ = *s++;
        left--;
    }
    *d = '\0';

    return (dlen + slen);
}
#endif


/* * 기존 PERS 매크로를 대체하는 함수
 * char name/short_desc(for mobs) or someone? -> PERS 매크로의 주석 옮겨둠
 * 보는 사람(viewer)이 대상(ch)을 볼 수 있으면 이름/설명을, 없으면 "someone"을 반환
 *                  --- 251125 by Komo
 */
const char *get_char_name(struct char_data *ch, struct char_data *viewer)
{
    if (!CAN_SEE(viewer, ch)) {
        return "someone";
    }
    
    // NPC면 short_desc, PC면 name 반환
    return IS_NPC(ch) ? ch->player.short_descr : GET_NAME(ch);
}

// 문자열 끝에 \r 또는 \n이 있을 때만 안전하게 제거하는 함수, 251130 by Komo
void prune_crlf(char *txt) {
    int len = strlen(txt);
    while (len > 0 && (txt[len-1] == '\n' || txt[len-1] == '\r')) {
        txt[--len] = '\0';
    }
}


/*
 * UTF-8 문자열을 바이트 길이(n)에 맞춰 안전하게 복사하는 함수
 * 251130 by Komo
 */
void utf8_safe_strncpy(char *dest, const char *src, size_t n) {
    size_t i;
    
    strncpy(dest, src, n - 1);
    dest[n - 1] = '\0';

    size_t len = strlen(dest);
    
    // UTF-8은 1xxxxxxx 형태로 시작하므로, 최상위 비트 확인
    int backtrack = 0;
    while (backtrack < 4 && len > 0) { // 한글은 보통 3바이트, 최대 4바이트까지 검사
        unsigned char ch = (unsigned char)dest[len - 1];

        if ((ch & 0x80) == 0) break;

        if ((ch & 0xC0) == 0xC0) {
            dest[len - 1] = '\0';
            break;
        }
        
        dest[len - 1] = '\0';
        len--;
        backtrack++;
    }
	dest[len] = '\0';
}


/*
 * BSD 표준 함수가 없는 리눅스 환경용 strlcpy, 251208
 */
#ifndef HAVE_STRLCPY /* 충돌 방지 */
size_t strlcpy(char *dst, const char *src, size_t siz)
{
    char *d = dst;
    const char *s = src;
    size_t n = siz;

    /* Copy as many bytes as will fit */
    if (n != 0) {
        while (--n != 0) {
            if ((*d++ = *s++) == '\0')
                break;
        }
    }

    /* Not enough room in dst, add NUL and traverse rest of src */
    if (n == 0) {
        if (siz != 0)
            *d = '\0';      /* NUL-terminate dst */
        while (*s++)
            ;
    }

    return (s - src - 1);   /* count does not include NUL */
}
#endif