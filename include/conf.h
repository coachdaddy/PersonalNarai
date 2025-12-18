#pragma once

/* ========================================================= */
/* Server Configuration                                      */
/* ========================================================= */
#define BADDOMS 16

#define MID_HELPER	"narai"

#define REBOOT_TIME	(4*86400)

#define TIME_ZONE	(9*3600)
#define REBOOT_WHEN	(9*60-5)

#define IMO 61

#define PULSE_MOBILE 39
#define PULSE_MOBILE2 13

#define PULSE_ZONE 240
#define PULSE_VIOLENCE 12
#define WAIT_SEC 4
#define WAIT_ROUND 4

#define MAX_STRING_LENGTH 4096
#define MAX_OUTPUT_LENGTH 5000
#define MAX_INPUT_LENGTH 2048
#define MAX_MESSAGES 61
#define MAX_ITEMS 153

#define MESS_ATTACKER 1
#define MESS_VICTIM 2
#define MESS_ROOM 3

#define SECS_PER_REAL_MIN 60
#define SECS_PER_REAL_HOUR (60 * SECS_PER_REAL_MIN)
#define SECS_PER_REAL_DAY (24 * SECS_PER_REAL_HOUR)
#define SECS_PER_REAL_YEAR (365 * SECS_PER_REAL_DAY)

#define SECS_PER_MUD_HOUR 60
#define SECS_PER_MUD_DAY (24 * SECS_PER_MUD_HOUR)
#define SECS_PER_MUD_MONTH (35 * SECS_PER_MUD_DAY)
#define SECS_PER_MUD_YEAR (17 * SECS_PER_MUD_MONTH)

#define MAX_OBJ_AFFECT 2	/* Used in OBJ_FILE_ELEM *DO*NOT*CHANGE* */
#define OBJ_NOTIMER    -7000000

#define NOWHERE -1	/* nil reference for room-database    */
#define NOBODY  -1  /* nil reference for mobile-database (추가, 251017) */

/* ========================================================= */
/* Data Limits                                               */
/* ========================================================= */


/* ========================================================== */
/* Game Mechanics                                             */
/* ========================================================== */

