/* ************************************************************************
*  file: db.h , Database module.                          Part of DIKUMUD *
*  Usage: Loading/Saving chars booting world.                             *
************************************************************************* */

#include <errno.h>

#include "structs.h"

/* data files used by the game system */

/*  default directory is defined in comm.c : usually lib */

#define WORLD_FILE        "tinyworld.wld"	/* room definitions           */
#define MOB_FILE          "tinyworld.mob"	/* monster prototypes         */
#define OBJ_FILE          "tinyworld.obj"	/* object prototypes          */
#define ZONE_FILE         "tinyworld.zon"	/* zone defs & command tables */
#define CREDITS_FILE      "credits"	/* for the 'credits' command  */
#define NEWS_FILE         "news"	/* for the 'news' command     */
#define IMOTD_FILE		  "imotd"	/* MOTD for immortals                 */
#define MOTD_FILE         "motd"	/* messages of today          */
#define PLAYER_FILE       "players"	/* the player database        */
#define TIME_FILE         "time"	/* game calendar information  */
#define MESS_FILE         "messages"	/* damage message             */
#define SOCMESS_FILE      "actions"	/* messgs for social acts     */
#define HELP_KWRD_FILE    "help_table"	/* for HELP <keywrd>          */
#define HELP_PAGE_FILE    "help"	/* for HELP <CR>              */
#define PLAN_FILE         "plan"	/* for god's plan             */
#define WIZARDS_FILE      "wizards"	/* for wizards command : cyb  */
#define STASH             "stash"
#define CHATLOG           "log/chatlog.log"

/* db.c에서 옮겨옴 */
#define KJHRENT      66666	/* kjh number to tell new rent format */

#define NEW_ZONE_SYSTEM
#define ALL_WORLD_FILE "world/world_files.new" /* by Komo, 251124 */
#define ALL_ZONE_FILE "zone/zone_files.new"   /* by Komo, 251120 */
#define ZO_DEAD  999

/* public procedures in db.c */
void boot_db(void);
void save_char(struct char_data *ch, sh_int load_room);
int create_entry(char *name);
void zone_update(void);
void init_char(struct char_data *ch);
void clear_char(struct char_data *ch);
void clear_object(struct obj_data *obj);
void reset_char(struct char_data *ch);
void free_char(struct char_data *ch);
int real_room(int virtual);
char *fread_string(FILE * fl);
int real_object(int virtual);
int real_mobile(int virtual);
int real_zone_by_number(int virtual);

#define REAL 0
#define VIRTUAL 1

struct obj_data *read_object(int nr, int type);
struct char_data *read_mobile(int nr, int type);


/* 
#define GREETINGS \
"\n\r\n\r\
                     	NaraiMUD (PERSONAL)            \n\r\n\r\
                      A derivative of KIT-MUD           \n\r\n\r\
                        Continued from EVE             \n\r\
                    Restarted at 2010. 12. 20.         \n\r\
                    Revisited at 2025. 10. 14. ........\n\r\n\r"
 */

#define GREETINGS \
"\n\r\n\r\
&Y   _   _    _    ____     _    ._.  &C  __  __  _   _  ____   &n\n\r\
&Y  | \\ | |  / \\  |  _ \\   / \\   | |  &C |  \\/  || | | ||  _ \\  &n\n\r\
&Y  |  \\| | / _ \\ | |_) | / _ \\  | |  &C | |\\/| || | | || | \\ \\ &n\n\r\
&C  | |\\  |/ ___ \\|  _ < / ___ \\ | |  &Y | |  | || \\_/ || |_/ / &n\n\r\
&C  |_| \\_/_/   \\_\\_| \\_\\_/   \\_\\|_|  &Y |_|  |_|\\_____/|____/  &n\n\r\n\r\
&W                     NaraiMUD (PERSONAL)            &n\n\r\n\r\
&D                   A derivative of KIT-MUD          &n\n\r\
&A                      Continued from EVE            &n\n\r\
&g                 Restarted at 2010. 12. 20.         &n\n\r\
&G                 Revisited at 2025. 10. 14. .....   &n\n\r\n\r"


#define MENU         \
"\n\rGOOD luck to NaraiMUDDER\n\r\n\
0) Get away from NaraiMUD.\n\r\
1) Into the NaraiMUD.\n\r\
2) Modify description.\n\r\
3) Change secret key.\n\r\
** Delete character is removed .\n\r\n\r\
   &YWhat number? : &n"


#define WELC_MESSG \
"\n\rWelcome to the land of Kkang-PaeMUD. May your visit here be... Interesting.\
\n\rIf you are new player, type 'info' or 'help' or 'NEWS'\n\r"

#define STORY     \
"Once upon a time ....Daystar doesn't say more. You'd better enter the Game.\n\r\n\r"

/* structure for the reset commands */
struct reset_com {
	char command;		/* current command                      */
	bool if_flag;		/* if TRUE: exe only if preceding exe'd */
	int arg1;		/*                                      */
	int arg2;		/* Arguments to the command             */
	int arg3;		/*                                      */

	/* 
	   *  Commands:              *
	   *  'M': Read a mobile     *
	   *  'O': Read an object    *
	   *  'G': Give obj to mob   *
	   *  'P': Put obj in obj    *
	   *  'G': Obj to char       *
	   *  'E': Obj to char equip *
	   *  'D': Set state of door *
	 */
};

/* zone definition structure. for the 'zone-table'   */
struct zone_data {
	int number;         /* 존 고유 번호 (VNUM), 251120 */
	char *name;		/* name of this zone                  */
	char *filename;		/* zone file name                                         */
	int lifespan;		/* how long between resets (minutes)  */
	char *wld_filename; /* 이 존에 매핑된 .wld 파일 이름, 251121 */
    int age;		/* current age of this zone (minutes) */
	int top;		/* upper limit for rooms in this zone */

	int reset_mode;		/* conditions for reset (see below)   */
	struct reset_com *cmd;	/* command table for reset                */

	/*
	   *  Reset mode:                              *
	   *  0: Don't reset, and don't update age.    *
	   *  1: Reset if no PC's are located in zone. *
	   *  2: Just reset.                           *
	 */
};

/* element in monster and object index-tables   */
struct index_data {
	int virtual;		/* virtual number of this mob/obj           */
	long pos;		/* file position of this field              */
	int number;		/* number of existing units of this mob/obj     */
	int (*func) ();		/* special procedure for this mob/obj       */

	/* 퀘스트 시스템 개편을 위해 추가 */
    char *name;      /* Monster name */
    int level;       /* Monster level */
    long act;        /* Monster act flags */
};

struct player_index_element {
	char *name;
	int nr;
};

struct help_index_element {
	char *keyword;
	long pos;
};
