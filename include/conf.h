#pragma once
/* ************************************************************************
*  file: Conf.h, 설정값 모음(게임 밸런스, 환경 설정, 튜닝값 etc)    *
*  비트벡터나 flag 등은 structs.h에 다수 유지됨                           *
*  251218   *
************************************************************************* */

/* ============================================================ *
 * Server Configuration                                         *
** ============================================================ */

#ifndef DFLT_DIR
#   define DFLT_DIR    "lib"   /* default port */
#endif  /* DFLT_DIR */
#ifndef DFLT_PORT
#   define DFLT_PORT   5101    /* default port */
#endif  /* DFLT_PORT */

#define MAXFDALLOWED    200

#define MID_HELPER	"narai"

#define BADDOMS 16

#define REBOOT_TIME	(4*86400)
#define REBOOT_WHEN	(9*60-5)
#define TIME_ZONE	(9*3600)

#define PULSE_MOBILE    39
#define PULSE_MOBILE2   13
#define PULSE_ZONE      240
#define PULSE_VIOLENCE  12

#define WAIT_SEC    4
#define WAIT_ROUND  4

#define OPT_USEC        200000 /* time delay corresponding to 5 passes/sec */
#define A_DAY           86400
#define MINUTES(m)      (60 * (m))
#define TICS_PER_SEC    5
#define SAVE_PERIOD_MINUTES 10 /* 10분마다 플레이어 저장 */
#define PLAYER_SAVE_PERIOD  (MINUTES(SAVE_PERIOD_MINUTES) * TICS_PER_SEC)

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

/* ============================================================ *
 * Data Limits                                                  *
** ============================================================ */

#define IMO 61

#define MAX_NAME_LENGTH 15
#define MAX_HOSTNAME    256

#define MAX_STRING_LENGTH   4096
#define MAX_OUTPUT_LENGTH   5000
#define MAX_INPUT_LENGTH    2048
#define MAX_MESSAGES        61
#define MAX_ITEMS           153

// from act.comm.c
#define MAX_HISTORY_MSG 512
#define HISTORY_SIZE    100 // 20 -> 100으로 확장, 251124 by Komo
#define MAX_NOTE_LENGTH MAX_STRING_LENGTH	/* also was in board.c */

// from board.c & modify.c
#define MAX_MSGS        100
#define WRITER_NAME_LEN 20
#define BOARD_DIR       "boards"

// from db.c
#define MAX_RENT_ITEM 1000

// from fight.c
#define MAX_NPC_CORPSE_TIME 15
#define MAX_PC_CORPSE_TIME  40

// from guild_command.c
#define LIMIT_LOAD  10
#define LOCKER_COST 10000

// from interpreter.c
#define MAX_CMD_LIST    350	/*  max command list modified by chase */

// from limits.c
#define SPELL_LEARN_MAX	120

// from mail.c
#define MAX_MAILS   200
#define MBOARD_DIR  "mboxes"

// from mob_magic.c
#define NUMBER_OF_MAX_VICTIM	8
#define NUMBER_OF_MAX_MOB	8

// from modify.c
#define TP_MOB      0
#define TP_OBJ      1
#define TP_ERROR    2

// from quest.c
#define MaxQuest		10000
#define QUEST_FILE		"mob.quest"
#define ZONE_NUMBER     47
#define END_QUEST_MOB   631

// from shop.c
#define SHOP_FILE "tinyworld.shp"
#define MAX_TRADE 5
#define MAX_PROD 5

// from spell.h
#define MAXSPELL        150
#define MAX_TYPES       70
#define MAX_SPL_LIST    200

/* ============================================================ *
 * Game Mechanics                                               *
** ============================================================ */
/* from db.c, for 대림사 */
/* also defined in daerimsa.c & spec_assign.c (now deleted) */
#define SON_OGONG			11101
#define FOURTH_JANGRO		11132
// FROM daerimsa.c
#define SON_OGONG_MIRROR	11141   /* also in mobact.c*/
#define DAERIMSA_PEN		11123
#define DAERIMSA_PAPER		11124
#define DAERIMSA_SCROLL		11132
#define GOLDEN_RIM			11127
#define SON_OGONG_BONG		11126
#define DARK_ROOM_IN_DAERIMSA		11134
#define TOWER_IN_DAERIMSA			11161
#define SAINT_WATER         11134


// from gbisland.c
#define GBISLAND_SEASHORE			23302
#define GBISLAND_MIRROR_SAINT		23321
#define GBISLAND_MAGIC_BARRIER_OUT	23331
#define GBISLAND_BOTTLE				23312
#define GBISLAND_MAGIC_PAPER1		23306
#define GBISLAND_MAGIC_PAPER2		23307
#define GBISLAND_MAGIC_PAPER		23308
#define GBISLAND_SEED_EVIL_POWER	23309

// from guild_command.c
#define TO_KAIST    3014
#define TO_PROCESS  3502

// from magic2.c
#define REAL	0
#define VIRTUAL	1



// from spec_procs2.c
#define RESCUER_VICTIM  5



/* ============================================================ *
 * VNUMs : Virtual Number Definitions                           *
 * Naming Rules Suggested:                                      *
 * - Rooms      : VNUM_ROOM_NAME                                *
 * - Mobiles    : VNUM_MOB_NAME                                 *
 * - Objects    : VNUM_OBJ_NAME                                 *
** ============================================================ */
#define VNUM_ROOM_VOID      0       /* The Void  */
#define VNUM_ROOM_LIMBO     1       /* Limbo */
#define VNUM_ROOM_MID       3001    /* Temple of Midgaard */

#define JALE_ROOM	1800    /* guild_skill.c */

#define VNUM_ROOM_QUESTROOM         3081    // 퀘스트룸
#define VNUM_ROOM_CHALLENGE_START   3082    // '도전의 방' 시작 VNUM
#define VNUM_ROOM_CHALLENGE_END     3089    // '도전의 방' 끝 VNUM (총 8개)

    /* GoodBadIsland */
#define VNUM_MOB_MIRROR_SAINT   23304
#define VNUM_MOB_LANESSA        23303
#define VNUM_MOB_CARPIE         23320


#define VNUM_OBJ_WINGS_1    2700    /* feather 깃털 */
#define VNUM_OBJ_WINGS_2    9703    /* wings of pegasus */
