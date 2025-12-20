/**************************************************************************
*  file: db.c , Database module.                          Part of DIKUMUD *
*  Usage: Loading/Saving chars, booting world, resetting etc.             *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete information. *
***************************************************************************/

#include "structs.h"
#include "utils.h"
#include "db.h"
#include "handler.h"
#include "limit.h"
#include "spells.h"
#include "mob_bal.c"
#include "vcs_track.h"


/****************************************************************************
*  declarations of most of the 'global' variables                         	*
*************************************************************************** */

struct room_data *world = NULL;			/* dyn alloc'ed array of rooms     	*/
int top_of_world = 0;					/* ref to the top element of world 	*/

struct zone_data *zone_table = NULL;	/* table of reset data				*/
int top_of_zone_table = 0;				/* ref to top of zone table        	*/

struct reset_q_type reset_q;			/* queue of zones to be reset       */
struct time_info_data time_info;		/* the infomation about the time   	*/
struct weather_data weather_info;		/* the infomation about the weather */

struct char_data *character_list = NULL;	/* global l-list of chars       */
struct player_index_element *player_table = NULL;	/* index to player file */
int top_of_p_table = 0;					/* ref to top of table             	*/
int top_of_p_file = 0;

struct index_data *mob_index = NULL;	/* index table for mobile file     	*/
int top_of_mobt = 0;					/* top of mobile index table       	*/
FILE *mob_f = NULL; 					/* file containing mob prototypes  	*/

struct obj_data *object_list = NULL;	/* the global linked list of obj's 	*/
struct index_data *obj_index = NULL;	/* index table for object file     	*/
int top_of_objt = 0;					/* top of object index table       	*/
FILE *obj_f = NULL;						/* obj prototypes                  	*/

char credits[MAX_STRING_LENGTH];		/* the Credits List                	*/
char news[MAX_STRING_LENGTH];			/* the news                        	*/
char imotd[MAX_STRING_LENGTH];			/* MOTD for immortals              	*/
char motd[MAX_STRING_LENGTH];			/* the messages of today           	*/
char help[MAX_STRING_LENGTH];			/* the main help page              	*/
char plan[MAX_STRING_LENGTH];			/* the info text                   	*/
char wizards[MAX_STRING_LENGTH];		/* the wizards text                	*/

struct help_index_element *help_index = NULL;	/* the help index                  */
int top_of_helpt;			/* top of help index table         */
FILE *help_fl = NULL;		/* file for help texts (HELP <kwd>) */

struct message_list fight_messages[MAX_MESSAGES];	/* fighting messages   */

/* comm.c에서 이동, Komo */
int regen_percent = 50;
int regen_time_percent = 66;
int regen_time = 200;



/*************************************************************************
*  routines for booting the system                                       *
*********************************************************************** */

/* body of the booting system */
void boot_db(void)
{
	int i;
	
	mudlog("(boot_db) Booting DB -- BEGIN.");

    mudlog("(boot_db) Resetting the game time:");
	DEBUG_LOG("VCS %s", VCS_TRACK_SHORT_HASH);

    mudlog("(boot_db)   help index done.");

    mudlog("(boot_db) Resetting the game time:");
	reset_time();

	mudlog("(boot_db) Reading news, credits, help-page, plan, wizards, and motd.");
	file_to_string(NEWS_FILE, news);
	file_to_string(CREDITS_FILE, credits);
	file_to_string(IMOTD_FILE, imotd);
	file_to_string(MOTD_FILE, motd);
	file_to_string(HELP_PAGE_FILE, help);
	file_to_string(PLAN_FILE, plan);
	file_to_string(WIZARDS_FILE, wizards);

	mudlog("(boot_db) Opening mobile, object and help files.");
    if (!(mob_f = fopen(MOB_FILE, "r"))) {
        perror("boot");
        exit(0);
    }
    mudlog("(boot_db)   Mob done");

    if (!(obj_f = fopen(OBJ_FILE, "r"))) {
        perror("boot");
        exit(0);
    }
    mudlog("(boot_db)   Obj done");
    if (!(help_fl = fopen(HELP_KWRD_FILE, "r")))
        mudlog("(boot_db)    Could not open help file.");
    else
        help_index = build_help_index(help_fl, &top_of_helpt);

    mudlog("(boot_db)   help index done.");

    mudlog("(boot_db) Initializing quest system.");
    init_quest();
    mudlog("(boot_db) Quest system initialized.");

    mudlog("(boot_db) Loading zone table.");
	boot_zones();

	mudlog("(boot_db) Loading rooms(world).");
    boot_world();
    mudlog("(boot_db) Renumbering rooms.");
    renum_world();

    mudlog("(boot_db) Generating index tables for mobile files.");
    mob_index = generate_indices(mob_f, &top_of_mobt);

    /* Quest Modernization: Pre-load mob level and act flags ---
     * --- Komo, 251102
     */
    mudlog("(boot_db) Pre-loading mob level and act flags into index.");
    { /* Create a local scope for temporary variables */
        long tmp_act;
        int tmp_level = -1;
        int i;
    
        for (i = 0; i <= top_of_mobt; i++) {
            /* Go to the file position for this monster */
            if (fseek(mob_f, mob_index[i].pos, 0) != 0) {
                char err_buf[256];
                snprintf(err_buf, sizeof(err_buf), "Quest Pre-load: fseek error on mob VNUM %d", mob_index[i].virtual);
                mudlog(err_buf);
                mob_index[i].level = -1; /* Mark as invalid */
                mob_index[i].act = 0;
                continue; /* Skip this mob */
            }
    
            /* String data (Store name, discard others) */
            mob_index[i].name = fread_string(mob_f); /* 1. name (STORE) */
            free(fread_string(mob_f)); /* 2. short_descr */
            free(fread_string(mob_f)); /* 3. long_descr */
            free(fread_string(mob_f)); /* 4. description */
    
            /* Numeric data (read and store/discard) */
            
            /* act */
            fscanf(mob_f, "%ld ", &tmp_act);
            mob_index[i].act = tmp_act;
    
            /* affected_by (discard) */
            fscanf(mob_f, " %*d ");
    
            /* alignment (discard) */
            fscanf(mob_f, " %*d \n");
    
            /* class (discard) */
            fscanf(mob_f, " %*c "); 
    
            /* level (added error handling code) */
            if (fscanf(mob_f, " %d ", &tmp_level) != 1) {
                char err_buf[256];
                snprintf(err_buf, sizeof(err_buf), "Quest Pre-load: fscanf error reading level for mob VNUM %d", mob_index[i].virtual);
                mudlog(err_buf);
                mob_index[i].level = -1;
                mob_index[i].act = 0;
                continue;
            }

			mob_index[i].level = tmp_level;
            
            mob_index[i].func = 0; 
        }
        rewind(mob_f); /* Rewind file pointer for safety */
    }
    mudlog("(boot_db) Mob index pre-load complete.");
    
    mudlog("(boot_db) Generating index tables for object files.");
	obj_index = generate_indices(obj_f, &top_of_objt);

	mudlog("(boot_db) Renumbering zone table.");
    renum_zone_table();

    mudlog("(boot_db) Generating player index.");
    build_player_index();

    mudlog("(boot_db) Loading fight messages.");
    load_messages();

    mudlog("(boot_db) Loading social messages.");
    boot_social_messages();

    mudlog("(boot_db) Assigning function pointers:");
    if (!no_specials) {
        mudlog("(boot_db)    Mobiles.");
        assign_mobiles();
        mudlog("(boot_db)    Objects.");
        assign_objects();
        mudlog("(boot_db)    Room.");
        assign_rooms();
    }

    mudlog("(boot_db)    Commands.");
    assign_command_pointers();
    mudlog("(boot_db)    Spells.");
	assign_spell_pointers();

	for (i = 0; i <= top_of_zone_table; i++) {
		fprintf(stderr,
			"Performing boot-time reset of %s (rooms %d-%d).\n",
			zone_table[i].name,
			(i ? (zone_table[i - 1].top + 1) : 0),
			zone_table[i].top);
		reset_zone(i);
	}
	reset_q.head = reset_q.tail = 0;
	mudlog("(boot_db) Booting DB -- DONE.");
}

/* reset the time in the game from file */
void reset_time(void)
{
	char buf[MAX_STRING_LENGTH];

	long beginning_of_time = 650336715;
	struct time_info_data mud_time_passed(time_t t2, time_t t1);
	time_info = mud_time_passed(time(0), beginning_of_time);

	switch (time_info.hours) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
		{
			weather_info.sunlight = SUN_DARK;
			break;
		}
	case 5:
		{
			weather_info.sunlight = SUN_RISE;
			break;
		}
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
	case 19:
	case 20:
		{
			weather_info.sunlight = SUN_LIGHT;
			break;
		}
	case 21:
		{
			weather_info.sunlight = SUN_SET;
			break;
		}
	case 22:
	case 23:
	default:
		{
			weather_info.sunlight = SUN_DARK;
			break;
		}
	}

	snprintf(buf, sizeof(buf), "   Current Gametime: %dH %dD %dM %dY.",
		time_info.hours, time_info.day,
		time_info.month, time_info.year);
	mudlog(buf);

	weather_info.pressure = 960;
	if ((time_info.month >= 7) && (time_info.month <= 12))
		weather_info.pressure += dice(1, 50);
	else
		weather_info.pressure += dice(1, 80);

	weather_info.change = 0;

	if (weather_info.pressure <= 980)
		weather_info.sky = SKY_LIGHTNING;
	else if (weather_info.pressure <= 1000)
		weather_info.sky = SKY_RAINING;
	else if (weather_info.pressure <= 1020)
		weather_info.sky = SKY_CLOUDY;
	else
		weather_info.sky = SKY_CLOUDLESS;
}

/* generate index table for the player file */
void build_player_index(void)
{
	int nr = -1, i;
	struct char_file_u dummy;
	FILE *fl;

	if (!(fl = fopen(PLAYER_FILE, "rb+"))) {
		perror("build player index.");
		exit(0);
	}
	for (; !feof(fl);) {
		fread(&dummy, sizeof(struct char_file_u), 1, fl);
		if (!feof(fl))	/* new record */
		{
			/* Create new entry in the list */
			if (nr == -1) {
				CREATE(player_table,
				       struct player_index_element, 1);
				nr = 0;
			} else {
				if (!(player_table = (struct
						      player_index_element *)
				      realloc(player_table, (++nr + 1) *
					      sizeof(struct player_index_element)))) {
					perror("generate index.");
					exit(0);
				}
			}

			player_table[nr].nr = nr;

			CREATE(player_table[nr].name, char, strlen(dummy.name) + 1);

			for (i = 0;
			     (*(player_table[nr].name + i) = tolower((unsigned char)*(dummy.name + i))); i++) ;
		}
	}

	fclose(fl);

	top_of_p_table = nr;

	top_of_p_file = top_of_p_table;
}

/* generate index table for object or monster file */
struct index_data *generate_indices(FILE * fl, int *top)
{
	int i = 0;
	struct index_data *index = NULL;
	char buf[82];

	rewind(fl);

	for (;;) {
		if (fgets(buf, 81, fl)) {
			if (*buf == '#') {
				/* allocate new cell */
				if (!i)		/* first cell */
					CREATE(index, struct index_data, 1);
				else if (!(index =
					   (struct index_data *)realloc(index,
									(i +
									1) * sizeof
									(struct index_data)))) {
					perror("(generate_indices) load indices.");
					exit(0);
				}
				sscanf(buf, "#%d", &index[i].virtual);
				index[i].pos = ftell(fl);
				index[i].number = 0;
				index[i].func = 0;
				i++;
			} else if (*buf == '$')		/* EOF */
				break;
		} else {
			perror("(generate_indices) generate indices.");
			exit(0);
		}
	}
	*top = i - 2;
	return (index);
}

/* load the rooms */
/* herper function,    by Komo */
/* Applied coderabbit's suggestion */
void load_rooms(FILE *fl, int zone_rnum, int *room_nr)
{
    int virtual_nr, flag, tmp;
    char *temp;
    char chk[256]; /* 크기 50 -> 256으로 변경 */
	struct extra_descr_data *new_descr;

    do {
        fscanf(fl, " #%d\n", &virtual_nr);

        temp = fread_string(fl);
        if ((flag = (*temp != '$'))) {
            allocate_room(*room_nr);

            world[*room_nr].number = virtual_nr;
            world[*room_nr].name = temp;
            world[*room_nr].description = fread_string(fl);
            
            /* 존 할당 */
            world[*room_nr].zone = zone_rnum;

            int ignore_zone;
            fscanf(fl, " %d ", &ignore_zone);

            fscanf(fl, " %d ", &tmp);
            world[*room_nr].room_flags = tmp;
            fscanf(fl, " %d ", &tmp);
            world[*room_nr].sector_type = tmp;

            world[*room_nr].funct = 0;
            world[*room_nr].contents = 0;
            world[*room_nr].people = 0;
            world[*room_nr].light = 0;
            for (tmp = 0; tmp <= 5; tmp++) world[*room_nr].dir_option[tmp] = 0;
            world[*room_nr].ex_description = 0;

            while (1) {
                fscanf(fl, " %255s \n", chk); 

                if (*chk == 'D')        /* direction field */
                    setup_dir(fl, *room_nr, atoi(chk + 1));
                else if (*chk == 'E') { /* extra description field */
                    CREATE(new_descr, struct extra_descr_data, 1);
                    new_descr->keyword = fread_string(fl);
                    new_descr->description = fread_string(fl);
                    new_descr->next = world[*room_nr].ex_description;
                    world[*room_nr].ex_description = new_descr;
                } else if (*chk == 'S') /* end of current room */
                    break;
            }
            
            (*room_nr)++;
        }
    } while (flag);

    if (temp) free(temp);
}
/* new version of boot_world */
/* modified by ares */
/* 메인 함수 re-written with helper function load_rooms(),    251121 by Komo */
void boot_world(void)
{
    FILE *map_files, *fl;
    int room_nr = 0;
    char vnum_buf[256];
    char file_name_from_list[256];
    char buf[MAX_STRING_LENGTH]; // 로그용 버퍼
    int zone_num_input;
    int z_rnum;

    world = 0;
    character_list = 0;
    object_list = 0;

    mudlog("boot_world: Loading world files...");

    if (!(map_files = fopen(ALL_WORLD_FILE, "r"))) {
        perror("boot_world: Error opening ALL_WORLD_FILE");
        exit(0);
    }

    while (1) {
        /* %255s 사용으로 버퍼 보호 */
        if (fscanf(map_files, "%255s", vnum_buf) != 1) break;
        if (*vnum_buf == '$') break;
        
        zone_num_input = atoi(vnum_buf);

        if (fscanf(map_files, "%255s", file_name_from_list) != 1) break;

        /* 존 찾기 */
        z_rnum = real_zone_by_number(zone_num_input);

        if (z_rnum == -1) {
            snprintf(buf, sizeof(buf), "boot_world: Warning - World file '%s' uses undefined zone number %d.", 
                    file_name_from_list, zone_num_input);
            mudlog(buf);
            z_rnum = 0; 
        } else {
			if (zone_table[z_rnum].wld_filename) free(zone_table[z_rnum].wld_filename);
            CREATE(zone_table[z_rnum].wld_filename, char, strlen(file_name_from_list) + 1);
            strcpy(zone_table[z_rnum].wld_filename, file_name_from_list);
        }

        if (!(fl = fopen(file_name_from_list, "r"))) {
            snprintf(buf, sizeof(buf), "boot_world: Error opening world file '%s'", file_name_from_list);
            perror(buf);
            continue;
        }

        load_rooms(fl, z_rnum, &room_nr);
        fclose(fl);
    }

    fclose(map_files);
    top_of_world = --room_nr; 
    
    snprintf(buf, sizeof(buf), "boot_world: Total %d rooms loaded.", top_of_world + 1);
    mudlog(buf);
}

#undef ALL_WORLD_FILE


void allocate_room(int new_top)
{
	struct room_data *new_world;

	if (new_top) {
		if (!(new_world = (struct room_data *)
		      realloc(world, (new_top + 1) * sizeof(struct room_data)))) {
			perror("alloc_room");
			exit(0);
		}
	} else
		CREATE(new_world, struct room_data, 1);

	world = new_world;
}

/* 특정 방(rnum)의 문자열과 데이터를 메모리 해제하는 함수, 251121 by Komo */
void clean_memory_for_room(int rnum)
{
    int i;
    struct extra_descr_data *ex_desc, *next_ex_desc;

    if (world[rnum].name) free(world[rnum].name);
    if (world[rnum].description) free(world[rnum].description);

    for (ex_desc = world[rnum].ex_description; ex_desc; ex_desc = next_ex_desc) {
        next_ex_desc = ex_desc->next;
        if (ex_desc->keyword) free(ex_desc->keyword);
        if (ex_desc->description) free(ex_desc->description);
        free(ex_desc);
    }
    world[rnum].ex_description = NULL;

    for (i = 0; i < 6; i++) {
        if (world[rnum].dir_option[i]) {
            if (world[rnum].dir_option[i]->general_description) free(world[rnum].dir_option[i]->general_description);
            if (world[rnum].dir_option[i]->keyword) free(world[rnum].dir_option[i]->keyword);
            free(world[rnum].dir_option[i]);
            world[rnum].dir_option[i] = NULL;
        }
    }
}

/* .wld 파일을 열어 기존 방들의 내용을 업데이트함 251121 by Komo */
void reload_world_file(FILE *fl, int zone_rnum)
{
    int virtual_nr, flag, tmp, rnum;
    char *temp, chk[256];
    struct extra_descr_data *new_descr;
    
    do {
        fscanf(fl, " #%d\n", &virtual_nr);
        
        temp = fread_string(fl); // 방 이름 읽기
        if ((flag = (*temp != '$'))) {
            rnum = real_room(virtual_nr);

            if (rnum != -1) {
                // 기존 데이터 청소
                clean_memory_for_room(rnum);

                // 새 데이터 연결 Swap
                world[rnum].name = temp;
                world[rnum].description = fread_string(fl);
                world[rnum].zone = zone_rnum;

				int ignore; fscanf(fl, " %d ", &ignore);

                fscanf(fl, " %d ", &tmp); world[rnum].room_flags = tmp;
                fscanf(fl, " %d ", &tmp); world[rnum].sector_type = tmp;
                
                // 출구 및 엑스트라 파싱
                while (1) {
                    fscanf(fl, " %255s \n", chk);
                    if (*chk == 'D')        setup_dir(fl, rnum, atoi(chk + 1));
                    else if (*chk == 'E') {
                        CREATE(new_descr, struct extra_descr_data, 1);
                        new_descr->keyword = fread_string(fl);
                        new_descr->description = fread_string(fl);
                        new_descr->next = world[rnum].ex_description;
                        world[rnum].ex_description = new_descr;
                    } else if (*chk == 'S') break;
                }
            } else {
                /* 존재하지 않는 방 (새로 추가된 방?): Skip */
                if (temp) free(temp);
                char *trash = fread_string(fl); free(trash);
                
                int ignore; fscanf(fl, " %d ", &ignore); // 존
                fscanf(fl, " %d ", &ignore); // 플래그
                fscanf(fl, " %d ", &ignore); // 섹터
                
                while (1) {
                    fscanf(fl, " %255s \n", chk);
                    if (*chk == 'D') { // 출구 데이터 건너뛰기
                        trash = fread_string(fl); free(trash);
                        trash = fread_string(fl); free(trash);
                        fscanf(fl, " %d ", &ignore);
                        fscanf(fl, " %d ", &ignore);
                    } else if (*chk == 'E') { // 엑스트라 건너뛰기
                        trash = fread_string(fl); free(trash);
                        trash = fread_string(fl); free(trash);
                    } else if (*chk == 'S') break;
                }
            }
        } else {
            if (temp) free(temp); // $를 만났을 때 해제
        }
    } while (flag);
}


/* read direction data */
/* with CodeRabbit, 251204 */
void setup_dir(FILE *fl, int room, int dir)
{
    int tmp;
    char err_buf[256];

    CREATE(world[room].dir_option[dir], struct room_direction_data, 1);

    world[room].dir_option[dir]->general_description = fread_string(fl);
    world[room].dir_option[dir]->keyword = fread_string(fl);

    if (fscanf(fl, " %d ", &tmp) != 1) {
        snprintf(err_buf, sizeof(err_buf), "SYSERR: Format error in setup_dir flags (Room #%d)", world[room].number);
        mudlog(err_buf);
        exit(1);
    }

    if (tmp == 1)
        world[room].dir_option[dir]->exit_info = EX_ISDOOR;
    else if (tmp == 2)
        world[room].dir_option[dir]->exit_info = EX_ISDOOR | EX_PICKPROOF;
    else if (tmp == 3)
        world[room].dir_option[dir]->exit_info = EX_ISDOOR | EX_NOPHASE;
    else if (tmp == 4)
        world[room].dir_option[dir]->exit_info = EX_ISDOOR | EX_PICKPROOF | EX_NOPHASE;
    else
        world[room].dir_option[dir]->exit_info = 0;

    if (fscanf(fl, " %d ", &tmp) != 1) {
        snprintf(err_buf, sizeof(err_buf), "SYSERR: Format error in setup_dir key (Room #%d)", world[room].number);
        mudlog(err_buf);
        exit(1);
    }
    world[room].dir_option[dir]->key = tmp;

    if (fscanf(fl, " %d ", &tmp) != 1) {
        snprintf(err_buf, sizeof(err_buf), "SYSERR: Format error in setup_dir to_room (Room #%d)", world[room].number);
        mudlog(err_buf);
        exit(1);
    }
    world[room].dir_option[dir]->to_room = tmp;
}

void renum_world(void)
{
	register int room, door;

	for (room = 0; room <= top_of_world; room++)
		for (door = 0; door <= 5; door++)
			if (world[room].dir_option[door])
				if (world[room].dir_option[door]->to_room != NOWHERE)
					world[room].dir_option[door]->to_room =
					    real_room(world[room].dir_option[door]->to_room);
}

void renum_zone_table(void)
{
	int zone, comm;

	for (zone = 0; zone <= top_of_zone_table; zone++)
		for (comm = 0; zone_table[zone].cmd[comm].command != 'S';
		     comm++)
			switch (zone_table[zone].cmd[comm].command) {
			case 'M':
				zone_table[zone].cmd[comm].arg1 =
				    real_mobile(zone_table[zone].cmd[comm].arg1);
				zone_table[zone].cmd[comm].arg3 =
				    real_room(zone_table[zone].cmd[comm].arg3);
				break;
			case 'O':
				zone_table[zone].cmd[comm].arg1 =
				    real_object(zone_table[zone].cmd[comm].arg1);
				if (zone_table[zone].cmd[comm].arg3 != NOWHERE)
					zone_table[zone].cmd[comm].arg3 =
					    real_room(zone_table[zone].cmd[comm].arg3);
				break;
			case 'G':
				zone_table[zone].cmd[comm].arg1 =
				    real_object(zone_table[zone].cmd[comm].arg1);
				break;
			case 'E':
				zone_table[zone].cmd[comm].arg1 =
				    real_object(zone_table[zone].cmd[comm].arg1);
				break;
			case 'P':
				zone_table[zone].cmd[comm].arg1 =
				    real_object(zone_table[zone].cmd[comm].arg1);
				zone_table[zone].cmd[comm].arg3 =
				    real_object(zone_table[zone].cmd[comm].arg3);
				break;
			case 'D':
				zone_table[zone].cmd[comm].arg1 =
				    real_room(zone_table[zone].cmd[comm].arg1);
				break;
			}
}

/* 헬퍼 함수: 열린 파일에서 존 데이터를 읽어 메모리에 적재 */
void parse_zone_file(FILE *fl, int zon)
{
    int cmd_no = 0, expand = 1;
    char buf[255];

    // 기본 헤더 정보 - Top, Lifespan, Reset Mode
    fscanf(fl, " %d ", &zone_table[zon].top);
    fscanf(fl, " %d ", &zone_table[zon].lifespan);
    fscanf(fl, " %d ", &zone_table[zon].reset_mode);

    // 명령어 테이블 읽기 - 기존 루프 로직 그대로
    zone_table[zon].cmd = NULL;

    cmd_no = 0;
    for (expand = 1;;) {
        if (expand) {
            if (!cmd_no)
                CREATE(zone_table[zon].cmd, struct reset_com, 1);
            else if (!(zone_table[zon].cmd = (struct reset_com *) realloc(zone_table[zon].cmd,
                            (cmd_no + 1) * sizeof(struct reset_com)))) {
                perror("reset command load");
                exit(0);
            }
        }
        expand = 1;
        fscanf(fl, " ");    /* skip blanks */
        fscanf(fl, "%c", &zone_table[zon].cmd[cmd_no].command);

        // S 명령어를 만나면 루프 종료
        if (zone_table[zon].cmd[cmd_no].command == 'S') {
            break;
        }

        // * 문자는 주석 처리
        if (zone_table[zon].cmd[cmd_no].command == '*') {
            expand = 0;
            fgets(buf, 80, fl); /* skip command */
            continue;
        }

        fscanf(fl, " %d %d %d", (int *) &zone_table[zon].cmd[cmd_no].if_flag,
                &zone_table[zon].cmd[cmd_no].arg1, &zone_table[zon].cmd[cmd_no].arg2);

        if (zone_table[zon].cmd[cmd_no].command == 'M' || zone_table[zon].cmd[cmd_no].command == 'O' ||
                zone_table[zon].cmd[cmd_no].command == 'E' || zone_table[zon].cmd[cmd_no].command == 'P' ||
                zone_table[zon].cmd[cmd_no].command == 'D')
            fscanf(fl, " %d", &zone_table[zon].cmd[cmd_no].arg3);

        fgets(buf, 80, fl); /* read comment */
        cmd_no++;
    }
}

/* new version of boot_zone : by ares */
/* read lib/zone/.zon files */
/* refactoring using Helper function, by Komo */
void load_zones(int zon)
{
    FILE *fl;
    char buf[255], *check;

    if (zon > top_of_zone_table)
        return;

    fl = fopen(zone_table[zon].filename, "r");
    if (!fl) {
        snprintf(buf, sizeof(buf), "Error in reading zone file '%s'", zone_table[zon].filename);
        mudlog(buf);
        return;
    }

    // 기존 데이터 메모리 해제
    if (zone_table[zon].name) free(zone_table[zon].name);
    if (zone_table[zon].cmd)  free(zone_table[zon].cmd);

    check = fread_string(fl);
    zone_table[zon].name = check;

    // 헬퍼 함수 호출
    parse_zone_file(fl, zon);

    fclose(fl);
}

void boot_zones(void)
{
    FILE *all_files, *fl;
    int zon = 0;
    char *check, file_name_from_list[100]; // 원래 변수명은 file_name이었음
    
    char debug_log_buffer[512]; // 디버깅용
    char current_working_dir[256]; // 디버깅용

    // 디버깅 로그, 함수 시작 및 CWD 확인 ---
    mudlog("boot_zones: Function started.");
    if (getcwd(current_working_dir, sizeof(current_working_dir)) != NULL) {
        snprintf(debug_log_buffer, sizeof(debug_log_buffer), "boot_zones: Current working directory: [%s]", current_working_dir);
        mudlog(debug_log_buffer);
    } else {
        mudlog("boot_zones: Error getting current working directory.");
    }

    if (!(all_files = fopen(ALL_ZONE_FILE, "r"))) { // ALL_ZONE_FILE은 "zone/zone_files.new"
        perror("boot_zones(ALL_ZONE_FILE)");
        snprintf(debug_log_buffer, sizeof(debug_log_buffer), "boot_zones: CRITICAL - Failed to open ALL_ZONE_FILE: %s", ALL_ZONE_FILE);
        mudlog(debug_log_buffer);
        exit(0);
    }
    snprintf(debug_log_buffer, sizeof(debug_log_buffer), "boot_zones: Successfully opened ALL_ZONE_FILE: %s", ALL_ZONE_FILE);
    mudlog(debug_log_buffer);

    while (1) {
        char vnum_buf[256]; // 존 번호를 임시로 읽을 버퍼
        int vnum_input;     // 정수로 변환된 존 번호

        /* 첫 번째 존 번호 또는 '$' 읽기 */
        if (fscanf(all_files, "%s", vnum_buf) != 1) {
            mudlog("boot_zones: Error reading zone file list (Unexpected EOF).");
            break;
        }

        /* '$' 문자를 만나면 루프 종료 */
        if (*vnum_buf == '$') {
            mudlog("boot_zones: Done reading zone file list.");
            break;
        }

        /* 읽은 문자열을 숫자로 변환 */
        vnum_input = atoi(vnum_buf);

        /* 두 번째 토큰 - 파일 경로 읽기 */
        if (fscanf(all_files, "%s", file_name_from_list) != 1) {
            mudlog("boot_zones: Error reading filename after zone number.");
            break;
        }

        /* 개별 존 파일 열기 */
        if (!(fl = fopen(file_name_from_list, "r"))) {
            char error_buf[256];
            snprintf(error_buf, sizeof(error_buf), "boot_zones: Error opening zone file '%s'", file_name_from_list);
            perror(error_buf);
            continue; // 파일을 못 열었어도 다음 존을 계속 읽기 위해 루프를 유지
        }

        check = fread_string(fl);
		if(!check) {
				char error_buf[256];
				snprintf(error_buf, sizeof(error_buf), "boot_zones: NULL from fread_stirng for zone file %s", file_name_from_list);
				mudlog(error_buf);
				fclose(fl);
				continue; // 파일을 못 열었어도 다음 존을 계속 읽기 위해 루프를 유지
		}

        if (!zon)
            CREATE(zone_table, struct zone_data, 1);
        else if (!(zone_table = (struct zone_data *) realloc(zone_table, (zon + 1) * sizeof(struct zone_data)))) {
            perror("boot_zones realloc");
            exit(0);
        }

        zone_table[zon].name = check;
        zone_table[zon].number = vnum_input; // 존 번호 저장
        
        CREATE(zone_table[zon].filename, char, strlen(file_name_from_list) + 1);
        strcpy(zone_table[zon].filename, file_name_from_list);

        /* 헬퍼 함수 호출하여 나머지 데이터 파싱 */
        parse_zone_file(fl, zon);

        fclose(fl);
        zon++;
    }
    top_of_zone_table = --zon;
    fclose(all_files);
    mudlog("boot_zones: Function finished.");
}
#undef ALL_ZONE_FILE

/*************************************************************************
*  procedures for resetting, both play-time and boot-time      *
*********************************************************************** */

/* read a mobile from MOB_FILE */
struct char_data *read_mobile(int nr, int type)
{
	int i;
	struct char_data *mob;
	long tmp, tmp2, tmp3;
	char letter;
	int level, level2, level3, abil, class;
	char buf[256];
	int r_num;

	i = nr;
	if (type == VIRTUAL) {
        r_num = real_mobile(nr);
        if (r_num < 0) {
            snprintf(buf, sizeof(buf), "read_mobile: Invalid virtual number %d. Mobile not loaded.", nr);
            mudlog(buf);
            return(0);
        }
    } else {
        r_num = nr;
    }

	fseek(mob_f, mob_index[r_num].pos, 0); // nr 대신 r_num 사용
	CREATE(mob, struct char_data, 1);
	clear_char(mob);

	/***** String data *** */
	mob->player.name = fread_string(mob_f);
	mob->player.short_descr = fread_string(mob_f);
	mob->player.long_descr = fread_string(mob_f);
	mob->player.description = fread_string(mob_f);
	mob->player.title = 0;

	/* *** Numeric data *** */
	fscanf(mob_f, "%ld ", &tmp);
	mob->specials.act = tmp;
	SET_BIT(mob->specials.act, ACT_ISNPC);

	fscanf(mob_f, " %ld ", &tmp);
	mob->specials.affected_by = tmp;

	fscanf(mob_f, " %ld \n", &tmp);
	mob->specials.alignment = tmp;

	/* set mob's class */
	fscanf(mob_f, " %c ", &letter);
	switch (letter) {
	case 'M':
		GET_CLASS(mob) = CLASS_MAGIC_USER;
		break;
	case 'C':
		GET_CLASS(mob) = CLASS_CLERIC;
		break;
	case 'T':
		GET_CLASS(mob) = CLASS_THIEF;
		break;
	case 'W':
		GET_CLASS(mob) = CLASS_WARRIOR;
		break;
	default:
		/* random selection */
		GET_CLASS(mob) = number(1, 4);
	}
	class = GET_CLASS(mob);

	/* set mob's level */
	fscanf(mob_f, " %ld ", &tmp);
	level = MIN(43, tmp);
	level2 = level * level;
	level3 = level2 * level;

	GET_LEVEL(mob) = level;

	/* level dependent data */
	abil = level / 4 + 5;
	switch (class) {
	case CLASS_MAGIC_USER:
		mob->abilities.str = number(abil >> 1, abil);
		mob->abilities.str_add = 0;
		mob->abilities.intel = number(abil, 18);
		mob->abilities.wis = number(abil, 17);
		mob->abilities.dex = number(abil >> 1, abil);
		mob->abilities.con = number(abil >> 1, abil);
		break;
	case CLASS_CLERIC:
		mob->abilities.str = number(abil >> 1, abil);
		mob->abilities.str_add = 0;
		mob->abilities.intel = number(abil, 17);
		mob->abilities.wis = number(abil, 18);
		mob->abilities.dex = number(abil >> 1, abil);
		mob->abilities.con = number(abil >> 1, abil);
		break;
	case CLASS_THIEF:
		mob->abilities.str = number(abil, 18);
		mob->abilities.str_add = 0;
		mob->abilities.intel = number(abil >> 1, abil);
		mob->abilities.wis = number(abil >> 1, abil);
		mob->abilities.dex = number(abil, 18);
		mob->abilities.con = number(abil, 17);
		break;
	case CLASS_WARRIOR:
		mob->abilities.str = 18;
		mob->abilities.str_add = number(1, MAX(abil << 3, 100));
		mob->abilities.intel = number(abil >> 1, abil);
		mob->abilities.wis = number(abil >> 1, abil);
		mob->abilities.dex = number(abil, 17);
		mob->abilities.con = number(abil, 18);
		break;
	}
	mob->points.mana = mob->points.max_mana =
	    number(level2, level * mob->abilities.wis * mob->abilities.intel);
	mob->points.move = mob->points.max_move =
	    number(level2, level * mob->abilities.dex * mob->abilities.dex);

	/* set hit */
	fscanf(mob_f, " %ld ", &tmp);
	if (tmp < 2) {		/* tmp == 1 */
		mob->points.hit = mob->points.max_hit =
		    number(level3, mob_bal_hit[level - 1][0]);
	} else if (tmp > 20) {
		mob->points.hit = mob->points.max_hit =
		    number(mob_bal_hit[level - 1][19], level3 * tmp);
	} else {		/* 1 < tmp <= 20 */
		mob->points.hit = mob->points.max_hit =
		    number(mob_bal_hit[level - 1][tmp - 2], mob_bal_hit[level
									- 1][tmp
									     - 1]);
	}

	/* set armor */
	fscanf(mob_f, " %ld ", &tmp);
	if (tmp < 2) {		/* tmp == 1 */
		tmp2 = 100 - mob_bal_ac[level - 1][0];
		mob->points.armor = 100 - number(level, tmp2);
	} else if (tmp > 20) {	/* tmp > 20 */
		tmp2 = 100 - mob_bal_ac[level - 1][19];
		mob->points.armor = 100 - number(tmp2, level * tmp);
	} else {		/* 1 < tmp <= 20 */
		tmp2 = 100 - mob_bal_ac[level - 1][tmp - 2];
		tmp3 = 100 - mob_bal_ac[level - 1][tmp - 1];
		mob->points.armor = 100 - number(tmp2, tmp3);
	}

	/* set hitroll */
	fscanf(mob_f, " %ld ", &tmp);
	if (tmp < 2) {		/* tmp == 1 */
		mob->points.hitroll = number(level, mob_bal_hr[level - 1][0]);
	} else if (tmp > 20) {
		mob->points.hitroll =
		    number(mob_bal_hr[level - 1][19], level * tmp);
	} else {		/* 1 < tmp <= 20 */
		mob->points.hitroll =
		    number(mob_bal_hr[level - 1][tmp - 2], mob_bal_hr[level -
			   1][tmp - 1]);
	}

	/* set damdice */
	mob->specials.damnodice = number(level / 3, level / 2) + 1;
	mob->specials.damsizedice = number(level / 4, level / 3) + 1;

	/* set damroll */
	fscanf(mob_f, " %ld \n", &tmp);
	if (tmp < 2) {		/* tmp == 1 */
		mob->points.damroll = number(level, mob_bal_dr[level - 1][0]);
	} else if (tmp > 20) {
		mob->points.damroll =
		    number(mob_bal_dr[level - 1][19], level * tmp);
	} else {		/* 1 < tmp <= 20 */
		mob->points.damroll =
		    number(mob_bal_dr[level - 1][tmp - 2], mob_bal_dr[level -
			   1][tmp - 1]);
	}

	/* set gold */
	fscanf(mob_f, " %ld ", &tmp);
	mob->points.gold = tmp;

	/* set XP */
	fscanf(mob_f, " %ld ", &tmp);
	if (tmp < 2) {			/* tmp == 1 */
		mob->points.exp = number(level3, mob_bal_exp[level - 1][0]);
	} else if (tmp > 20) {
		mob->points.exp =
		    number(mob_bal_exp[level - 1][19], level3 * level * tmp / 10);
	} else {				/* 1 < tmp <= 20 */
		mob->points.exp =
		    number(mob_bal_exp[level - 1][tmp - 2], mob_bal_exp[level
									- 1][tmp
									     - 1]);
	}

	/* set position */
	fscanf(mob_f, " %ld ", &tmp);
	mob->specials.default_pos = mob->specials.position = tmp;

	/* set sex */
	fscanf(mob_f, " %c \n", &letter);
	switch (letter) {
	case 'N':
		mob->player.sex = 0;
		break;
	case 'M':
		mob->player.sex = 1;
		break;
	case 'F':
		mob->player.sex = 2;
		break;
	default:
		mob->player.sex = number(0, 2);
	}

	mob->player.guild = 0;
	mob->player.pk_num = 0;
	mob->player.pked_num = 0;

	mob->player.weight = number(100, 200);
	mob->player.height = number(100, 200);

	for (i = 0; i < 3; i++)
		GET_COND(mob, i) = -1;

	mob->specials.apply_saving_throw[SAVING_PARA] =
	    100 - number(GET_LEVEL(mob) >> 1, GET_LEVEL(mob));
	mob->specials.apply_saving_throw[SAVING_PETRI] =
	    100 - number(GET_LEVEL(mob) >> 1, GET_LEVEL(mob));
	mob->specials.apply_saving_throw[SAVING_BREATH] =
	    100 - number(GET_LEVEL(mob) >> 1, GET_LEVEL(mob));

	switch (class) {
	case CLASS_MAGIC_USER:
	case CLASS_CLERIC:
		mob->specials.apply_saving_throw[SAVING_HIT_SKILL] =
		    100 - number(GET_LEVEL(mob) >> 1, GET_LEVEL(mob));
		mob->specials.apply_saving_throw[SAVING_SPELL] =
		    100 - number(GET_LEVEL(mob), GET_LEVEL(mob) << 1);
		break;
	case CLASS_THIEF:
	case CLASS_WARRIOR:
		mob->specials.apply_saving_throw[SAVING_HIT_SKILL] =
		    100 - number(GET_LEVEL(mob), GET_LEVEL(mob) << 1);
		mob->specials.apply_saving_throw[SAVING_SPELL] =
		    100 - number(GET_LEVEL(mob) >> 1, GET_LEVEL(mob));
		break;
	}

	mob->tmpabilities = mob->abilities;

	mob->player.time.birth = time(0);
	mob->player.time.played = 0;
	mob->player.time.logon = time(0);

	for (i = 0; i < MAX_WEAR; i++)	/* Initialisering Ok */
		mob->equipment[i] = 0;

	mob->nr = r_num; // nr에서 r_num으로 수정
	mob->desc = 0;

	for (i = 0; i < MAX_SKILLS; i++) {
		if (spell_info[i].min_level[class - 1] <= level) {
			mob->skills[i].learned
			    = (level > 40) ? 99 : number(level,
							 spell_info[i].max_skill[class
										 - 1]);
		} else {
			mob->skills[i].learned = 0;
		}
		// FiXME : level 60 balancing
		mob->skills[i].skilled = (level > 40) ? level : 0;
		mob->skills[i].recognise = 0;
	}

	if (level > 40)
		mob->regeneration = level << 4;
	else if (level > 35)
		mob->regeneration = level << 2;
	else
		mob->regeneration = level << 1;

	/* quest */
	mob->quest.type = 0;
	mob->quest.data = 0;
	mob->quest.solved = 0;

	/* insert in list */
	mob->next = character_list;
	character_list = mob;

	mob_index[r_num].number++; // nr에서 r_num으로 수정

    if (mob_index[r_num].virtual == SON_OGONG) // nr에서 r_num으로 수정
        son_ogong = mob;
    else if (mob_index[r_num].virtual == FOURTH_JANGRO) // nr에서 r_num으로 수정
		fourth_jangro = mob;

	mob->regened = 0;
	mob->specials.fighting = 0;

	return (mob);
}

/* read an object from OBJ_FILE */
struct obj_data *read_object(int nr, int type)
{
	struct obj_data *obj;
	int tmp, i;
	char chk[50], buf[100];
	struct extra_descr_data *new_descr;
	int r_num;

	i = nr;
	if (type == VIRTUAL) {
        r_num = real_object(nr);
        if (r_num < 0) {
            snprintf(buf, sizeof(buf), "read_object: Invalid virtual number %d. Object not loaded.", nr);
            mudlog(buf);
            return(0);
		}
    } else {
        r_num = nr; // 타입이 REAL이면 nr을 그대로 사용
    }
	fseek(obj_f, obj_index[r_num].pos, 0); // nr 대신 r_num으로 인덱스 접근

	CREATE(obj, struct obj_data, 1);

	clear_object(obj);

	/* *** string data *** */
	obj->name = fread_string(obj_f);
	obj->short_description = fread_string(obj_f);
	obj->description = fread_string(obj_f);
	obj->action_description = fread_string(obj_f);

	/* *** numeric data *** */
	fscanf(obj_f, " %d ", &tmp);
	obj->obj_flags.type_flag = tmp;
	fscanf(obj_f, " %d ", &tmp);
	obj->obj_flags.extra_flags = tmp;
	fscanf(obj_f, " %d ", &tmp);
	obj->obj_flags.wear_flags = tmp;
	fscanf(obj_f, " %d ", &tmp);
	obj->obj_flags.value[0] = tmp;
	fscanf(obj_f, " %d ", &tmp);
	obj->obj_flags.value[1] = tmp;
	fscanf(obj_f, " %d ", &tmp);
	obj->obj_flags.value[2] = tmp;
	fscanf(obj_f, " %d ", &tmp);
	obj->obj_flags.value[3] = tmp;
	fscanf(obj_f, " %d ", &tmp);
	obj->obj_flags.weight = tmp;
	fscanf(obj_f, " %d \n", &tmp);
	obj->obj_flags.cost = tmp;
	fscanf(obj_f, " %d \n", &tmp);
	obj->obj_flags.gpd = tmp;

	/* *** extra descriptions *** */
	obj->ex_description = 0;

	while (fscanf(obj_f, " %s \n", chk), *chk == 'E') {
		CREATE(new_descr, struct extra_descr_data, 1);
		new_descr->keyword = fread_string(obj_f);
		new_descr->description = fread_string(obj_f);
		new_descr->next = obj->ex_description;
		obj->ex_description = new_descr;
	}

	for (i = 0; (i < MAX_OBJ_AFFECT) && (*chk == 'A'); i++) {
		fscanf(obj_f, " %d ", &tmp);
		obj->affected[i].location = tmp;
		fscanf(obj_f, " %d \n", &tmp);
		obj->affected[i].modifier = tmp;
		fscanf(obj_f, " %s \n", chk);
	}

	for (; (i < MAX_OBJ_AFFECT); i++) {
		obj->affected[i].location = APPLY_NONE;
		obj->affected[i].modifier = 0;
	}

	obj->in_room = NOWHERE;
	obj->next_content = 0;
	obj->carried_by = 0;
	obj->in_obj = 0;
	obj->contains = 0;
	obj->item_number = r_num; // nr 대신 r_num

	obj->next = object_list;
	object_list = obj;

	obj_index[r_num].number++; // nr 대신 r_num

	return (obj);
}


/* update zone ages, queue for reset if necessary, and dequeue when possible */
void zone_update(void)
{
	int i;
	struct reset_q_element *update_u, *temp;

	/* enqueue zones */
	for (i = 0; i <= top_of_zone_table; i++) {
		if (zone_table[i].age < (zone_table[i].lifespan *
					 regen_time_percent /
					 100) &&
		    zone_table[i].reset_mode)
			(zone_table[i].age)++;
		else if (zone_table[i].age < ZO_DEAD && zone_table[i].reset_mode) {
			/* enqueue zone */
			CREATE(update_u, struct reset_q_element, 1);
			update_u->zone_to_reset = i;
			update_u->next = 0;

			if (!reset_q.head)
				reset_q.head = reset_q.tail = update_u;
			else {
				reset_q.tail->next = update_u;
				reset_q.tail = update_u;
			}

			zone_table[i].age = ZO_DEAD;
		}
	}

	/* dequeue zones (if possible) and reset */
	for (update_u = reset_q.head; update_u; update_u = update_u->next) {
		if (zone_table[update_u->zone_to_reset].reset_mode == 2 ||
		    is_empty(update_u->zone_to_reset)) {
			reset_zone(update_u->zone_to_reset);

			/* dequeue */
			if (update_u == reset_q.head)
				reset_q.head = reset_q.head->next;
			else {
				for (temp = reset_q.head; temp->next != update_u;
				     temp = temp->next) ;

				if (!update_u->next)
					reset_q.tail = temp;

				temp->next = update_u->next;
			}

			free(update_u);
			break;
		}
	}
}

/*
	index : real number
*/
struct char_data *get_mobile_index(int index)
{
	struct char_data *ch;

	for (ch = character_list; ch; ch = ch->next) {
		if (IS_NPC(ch) && ch->nr == index) {
			return ch;
		}
	}

	/* error */
	return NULL;
}


void reset_zone(int zone)
{
    int cmd_no;
    char last_cmd = 1;
    struct char_data *mob = NULL;
    struct obj_data *obj, *obj_to;
    char buf[256];

    /* item regen !!! */
    /* if (regen == 1) then regen all items. */
    int regen;
    int real_load;
    int zone_lifespan = zone_table[zone].lifespan; // .zon 파일에 정의된 리셋 주기(분)
    int regen_chance_denominator; // 확률 계산의 분모

    if (zone_lifespan <= 0) { /* 0으로 나누기 방지 */
        regen = 0; 
    } else {
        regen_chance_denominator = regen_time / zone_lifespan; /* 분모 먼저 계산 */
        if (regen_chance_denominator < 1) { /* 최소 1/2 (50%) 확률 보장 */
            regen_chance_denominator = 1; 
        }

        regen = (number(0, regen_chance_denominator) == 0);
    }

    real_load = 0;
    for (cmd_no = 0;; cmd_no++) {
        if (ZCMD.command == 'S')
            break;

        if (last_cmd || !ZCMD.if_flag) {
            switch (ZCMD.command) {
                case 'M':		/* read a mobile */
                    if (mob_index[ZCMD.arg1].number < ZCMD.arg2) {
                        mob = read_mobile(ZCMD.arg1, REAL);
                        char_to_room(mob, ZCMD.arg3);
                        last_cmd = 'M';
                        if (regen)
                            mob->regened = 1;
                        real_load = 1;
                    } else {
                        mob = get_mobile_index (ZCMD.arg1);
                        if (mob == NULL) 
                            last_cmd = 0;
                        else {
                            if (mob->regened)
                                last_cmd = 0;
                            else {
                                last_cmd = 'M';
                                if (regen)
                                    mob->regened = 1;
                                real_load = 0;
                            }
                        }
                    }
                    break;

                case 'O':		/* read an object */
                    if (ZCMD.arg3 >= 0) {
                        if (!get_obj_in_list_num (ZCMD.arg1, world[ZCMD.arg3].contents)) {
                            obj = read_object(ZCMD.arg1, REAL);
                            if ((obj->obj_flags.type_flag == ITEM_KEY) || (!IS_SET(obj->obj_flags.extra_flags, ITEM_EQ_LVL_LIMIT)) || (regen == 1)) {
                                obj_to_room(obj, ZCMD.arg3);
                                last_cmd = 'O';
                            } else
                                extract_obj(obj);
                        }
                    } else {
                        obj = read_object(ZCMD.arg1, REAL);
                        if ((obj->obj_flags.type_flag == ITEM_KEY) || (!IS_SET(obj->obj_flags.extra_flags, ITEM_EQ_LVL_LIMIT)) || (regen == 1)) {
                            obj->in_room = NOWHERE;
                            last_cmd = 'O';
                        } else
                            extract_obj(obj);
                    }
                    break;

                case 'P':		/* object to object */
                    if ( (last_cmd == 'O' || last_cmd == 'P' || last_cmd == 'E' || last_cmd == 'G')) {
                        obj = read_object(ZCMD.arg1, REAL);
                        if ((obj->obj_flags.type_flag == ITEM_KEY) || (!IS_SET(obj->obj_flags.extra_flags, ITEM_EQ_LVL_LIMIT)) || (regen == 1)) {
                            obj_to = get_obj_num(ZCMD.arg3);
                            if (obj_to) {
                                obj_to_obj(obj, obj_to);
                                last_cmd = 'P';
                            } else
                                extract_obj(obj);
                        } else
                            extract_obj(obj);
                    }
                    break;

                case 'G':    /* obj_to_char */
                    if ( (last_cmd == 'M' || last_cmd == 'G' || last_cmd == 'E')) {
                        obj = read_object (ZCMD.arg1, REAL);

                        if ((obj->obj_flags.type_flag == ITEM_KEY) || (!IS_SET (obj->obj_flags.extra_flags, ITEM_EQ_LVL_LIMIT))) {
                            if (mob && real_load) {
                                obj_to_char(obj, mob);
                                last_cmd = 'G';
                            } else
                                extract_obj (obj);
                        } else if (IS_SET (obj->obj_flags.extra_flags, ITEM_EQ_LVL_LIMIT)) {
                            if (mob && real_load) {
                                obj_to_char(obj, mob);
                                last_cmd = 'G';
                            } else
                                extract_obj (obj);
                        } else
                            extract_obj (obj);
                    }
                    break;

                case 'E':		/* object to equipment list */
                    if ( (last_cmd == 'M' || last_cmd == 'G' || last_cmd == 'E')) {
                        obj = read_object(ZCMD.arg1, REAL);
                        if (obj->obj_flags.type_flag == ITEM_KEY) {
                            if (mob && real_load) {
                                equip_char(mob, obj, ZCMD.arg3);
                                last_cmd = 'E';
                            } else
                                extract_obj(obj);
                        } else if (regen == 1) {
                            if (mob) {
                                equip_char(mob, obj, ZCMD.arg3);
                                last_cmd = 'E';
                            } else
                                extract_obj(obj);
                        } else
                            extract_obj(obj);
                    }
                    break;

                case 'D':		/* set state of door */
                    switch (ZCMD.arg3) {
                        case 0:
                            REMOVE_BIT(world[ZCMD.arg1].dir_option[ZCMD.arg2]->exit_info, EX_LOCKED);
                            REMOVE_BIT(world[ZCMD.arg1].dir_option[ZCMD.arg2]->exit_info, EX_CLOSED);
                            break;
                        case 1:
                            SET_BIT(world[ZCMD.arg1].dir_option[ZCMD.arg2]->exit_info, EX_CLOSED);
                            REMOVE_BIT(world[ZCMD.arg1].dir_option[ZCMD.arg2]->exit_info, EX_LOCKED);
                            break;
                        case 2:
                            SET_BIT(world[ZCMD.arg1].dir_option[ZCMD.arg2]->exit_info, EX_LOCKED);
                            SET_BIT(world[ZCMD.arg1].dir_option[ZCMD.arg2]->exit_info, EX_CLOSED);
                            break;
                    }
                    last_cmd = 'D';
                    break;

                default:
                    snprintf(buf, sizeof(buf), "Undefd cmd in reset table; zone %d cmd %d.\n\r", zone, cmd_no);
                    mudlog(buf);
                    exit(0);
                    break;
            }
        } else
            last_cmd = 0;
    }

    zone_table[zone].age = 0;
}

#undef ZCMD

/* for use in reset_zone; return TRUE if zone 'nr' is free of PC's  */
int is_empty(int zone_nr)
{
	struct descriptor_data *i;

	for (i = descriptor_list; i; i = i->next)
		if (!i->connected)
			if (world[i->character->in_room].zone == zone_nr)
				return (0);

	return (1);
}

/*************************************************************************
*  stuff related to the save/load player system                  *
*********************************************************************** */

/* Load a char, TRUE if loaded, FALSE if not */
int load_char(char *name, struct char_file_u *char_element)
{
	FILE *fl;
	int player_i;

	int find_name(char *name);

	if ((player_i = find_name(name)) >= 0) {
		if (!(fl = fopen(PLAYER_FILE, "r"))) {
			perror("Opening player file for reading. (db.c, load_char)");
			exit(0);
		}
		fseek(fl, (long)(player_table[player_i].nr *
				 sizeof(struct char_file_u)), 0);
		fread(char_element, sizeof(struct char_file_u), 1, fl);
		fclose(fl);
		return (player_i);
	} else
		return (-1);
}

/* copy data from the file structure to a char struct */
/* COPY FILE to DATA */
void store_to_char(struct char_file_u *st, struct char_data *ch)
{
	int i;

	GET_SEX(ch) = st->sex;
	GET_CLASS(ch) = st->class;
	GET_LEVEL(ch) = st->level;
	ch->player.short_descr = 0;
	ch->player.long_descr = 0;
	if (*st->title) {
		CREATE(ch->player.title, char, strlen(st->title) + 1);
		strcpy(ch->player.title, st->title);
	} else
		GET_TITLE(ch) = 0;
	if (*st->description) {
		CREATE(ch->player.description, char, strlen(st->description) + 1);
		strcpy(ch->player.description, st->description);
	} else
		ch->player.description = 0;

	ch->player.pk_num = st->pk_num;		/* by process */
	ch->player.pked_num = st->pked_num;	/* by process */
	ch->player.guild = st->guild;	/* by process */
	for (i = 0; i < MAX_GUILD_SKILLS; i++)	/* by process */
		ch->player.guild_skills[i] = st->guild_skills[i];

	ch->player.time.birth = st->birth;
	ch->player.time.played = st->played;
	ch->player.time.logon = time(0);
	ch->player.weight = st->weight;
	ch->player.height = st->height;
	ch->abilities = st->abilities;
	ch->tmpabilities = st->abilities;
	ch->points = st->points;
	for (i = 0; i <= MAX_SKILLS - 1; i++) {
		ch->skills[i].learned = st->skills[i].learned;
		ch->skills[i].skilled = st->skills[i].skilled;
		ch->skills[i].recognise = st->skills[i].recognise;
	}
	ch->specials.spells_to_learn = st->spells_to_learn;
	ch->specials.alignment = st->alignment;
	ch->specials.act = st->act;
	ch->specials.carry_weight = 0;
	ch->specials.carry_items = 0;

	ch->points.armor = st->points.armor;
	ch->points.hitroll = st->points.hitroll;
	ch->points.damroll = st->points.damroll;

	/* quest */
	ch->quest.type = st->quest.type;
	ch->quest.data = st->quest.data;
	ch->quest.solved = st->quest.solved;

	/* hand dice */
#ifdef INIT_BARE_HAND
	ch->specials.damnodice = 1;
	ch->specials.damsizedice = 2;
#else
	ch->specials.damnodice = st->damnodice;
	ch->specials.damsizedice = st->damsizedice;
#endif

	/* wimpyness */
	ch->specials.wimpyness = st->wimpyness;

	/* remortal */
	ch->player.remortal = st->remortal;

	ch->regeneration = 0;
	CREATE(GET_NAME(ch), char, strlen(st->name) + 1);
	strcpy(GET_NAME(ch), st->name);
	for (i = 0; i <= 4; i++)
		ch->specials.apply_saving_throw[i] = 0;
	for (i = 0; i <= 2; i++)
		GET_COND(ch, i) = st->conditions[i];
	/* Add all spell effects */
	for (i = 0; i < MAX_AFFECT; i++) {
		if (st->affected[i].type)
			affect_to_char(ch, &st->affected[i]);
	}
	ch->in_room = st->load_room;
	affect_total(ch);
	ch->bank = st->bank;
	GET_HIT(ch) = MIN(GET_HIT(ch) + (time(0) - st->last_logon) /
			  SECS_PER_MUD_HOUR * 5, GET_MAX_HIT(ch));
	GET_MANA(ch) = MIN(GET_MANA(ch) + (time(0) - st->last_logon) /
			   SECS_PER_MUD_HOUR * 3, GET_MAX_MANA(ch));
	GET_MOVE(ch) = MIN(GET_MOVE(ch) + (time(0) - st->last_logon) /
			   SECS_PER_MUD_HOUR * 10, GET_MAX_MOVE(ch));
}				/* store_to_char */

void store_to_char_for_transform(struct char_file_u *st, struct char_data *ch)
{
	int i;

	GET_SEX(ch) = st->sex;
	GET_CLASS(ch) = st->class;
	GET_LEVEL(ch) = st->level;
	ch->player.short_descr = 0;
	ch->player.long_descr = 0;
	if (*st->title) {
		RECREATE(ch->player.title, char, strlen(st->title) + 1);
		strcpy(ch->player.title, st->title);
	} else
		GET_TITLE(ch) = 0;
	if (*st->description) {
		RECREATE(ch->player.description, char, strlen(st->description) + 1);
		strcpy(ch->player.description, st->description);
	} else
		ch->player.description = 0;
	ch->player.time.birth = st->birth;
	ch->player.time.played = st->played;
	ch->player.time.logon = time(0);
	ch->player.weight = st->weight;
	ch->player.height = st->height;
	ch->abilities = st->abilities;
	ch->tmpabilities = st->abilities;
	ch->points = st->points;
	for (i = 0; i <= MAX_SKILLS - 1; i++)
		ch->skills[i] = st->skills[i];
	ch->specials.spells_to_learn = st->spells_to_learn;
	ch->specials.alignment = st->alignment;
	ch->specials.act = st->act;
	ch->specials.carry_weight = 0;
	ch->specials.carry_items = 0;
	ch->points.armor = 101 - st->points.armor;
	ch->points.hitroll = 0;
	ch->points.damroll = 0;

	RECREATE(GET_NAME(ch), char, strlen(st->name) + 1);
	strcpy(GET_NAME(ch), st->name);
	for (i = 0; i <= 4; i++)
		ch->specials.apply_saving_throw[i] = 0;
	for (i = 0; i <= 2; i++)
		GET_COND(ch, i) = st->conditions[i];
	/* Add all spell effects */
	for (i = 0; i < MAX_AFFECT; i++) {
		if (st->affected[i].type)
			affect_to_char(ch, &st->affected[i]);
	}
	affect_total(ch);
	ch->bank = st->bank;
}				/* store_to_char_for_transform */

/* copy vital data from a players char-structure to the file structure */
void char_to_store(struct char_data *ch, struct char_file_u *st)
{
	int i;
	struct affected_type *af;
	struct obj_data *char_eq[MAX_WEAR];

	/* Unaffect everything a character can be affected by */

	for (i = 0; i < MAX_WEAR; i++) {
		if (ch->equipment[i])
			char_eq[i] = unequip_char(ch, i);
		else
			char_eq[i] = 0;
	}

	for (af = ch->affected, i = 0; i < MAX_AFFECT; i++) {
		if (af) {
			st->affected[i] = *af;
			st->affected[i].next = 0;
			/* subtract effect of the spell or the effect will be doubled */
			affect_modify(ch, st->affected[i].location,
				      st->affected[i].modifier,
				      st->affected[i].bitvector, FALSE);
			af = af->next;
		} else {
			st->affected[i].type = 0;	/* Zero signifies not used */
			st->affected[i].duration = 0;
			st->affected[i].modifier = 0;
			st->affected[i].location = 0;
			st->affected[i].bitvector = 0;
			st->affected[i].next = 0;
		}
	}
	if ((i >= MAX_AFFECT) && af && af->next)
		mudlog("WARNING: OUT OF STORE ROOM FOR AFFECTED TYPES!!!");
	ch->tmpabilities = ch->abilities;
	st->birth = ch->player.time.birth;
	st->played = ch->player.time.played;
	st->played += (long)(time(0) - ch->player.time.logon);
	st->last_logon = time(0);
	ch->player.time.played = st->played;
	ch->player.time.logon = time(0);
	st->weight = GET_WEIGHT(ch);
	st->height = GET_HEIGHT(ch);
	st->sex = GET_SEX(ch);
	st->class = GET_CLASS(ch);
	st->level = GET_LEVEL(ch);
	st->abilities = ch->abilities;
	st->points = ch->points;
	st->alignment = ch->specials.alignment;
	st->spells_to_learn = ch->specials.spells_to_learn;
	st->act = ch->specials.act;
	st->points.armor = ch->points.armor;
	st->points.hitroll = ch->points.hitroll;
	st->points.damroll = ch->points.damroll;
	if (GET_TITLE(ch))
		strcpy(st->title, GET_TITLE(ch));
	else
		*st->title = '\0';
	if (ch->player.description)
		strcpy(st->description, ch->player.description);
	else
		*st->description = '\0';
	st->pk_num = ch->player.pk_num;		/* by process */
	st->pked_num = ch->player.pked_num;	/* by process */
	st->guild = ch->player.guild;	/* by process */
	for (i = 0; i < MAX_GUILD_SKILLS; i++)
		st->guild_skills[i] = ch->player.guild_skills[i];	/* by process */

	for (i = 0; i <= MAX_SKILLS - 1; i++) {
		st->skills[i].learned = ch->skills[i].learned;
		st->skills[i].skilled = ch->skills[i].skilled;
		st->skills[i].recognise = ch->skills[i].recognise;
	}

	/* quest */
	st->quest.type = ch->quest.type;
	st->quest.data = ch->quest.data;
	st->quest.solved = ch->quest.solved;

	/* bare hand dice */
	st->damnodice = ch->specials.damnodice;
	st->damsizedice = ch->specials.damsizedice;

	st->wimpyness = ch->specials.wimpyness;

	/* remortal */
	st->remortal = ch->player.remortal;

	if (GET_NAME(ch))
		strcpy(st->name, GET_NAME(ch));
	for (i = 0; i <= 2; i++)
		st->conditions[i] = GET_COND(ch, i);
	for (af = ch->affected, i = 0; i < MAX_AFFECT; i++) {
		if (af) {
			/* Add effect of the spell or it will be lost */
			/* When saving without quitting               */
			affect_modify(ch, st->affected[i].location,
				      st->affected[i].modifier,
				      st->affected[i].bitvector, TRUE);
			af = af->next;
		}
	}
	for (i = 0; i < MAX_WEAR; i++) {
		if (char_eq[i])
			equip_char(ch, char_eq[i], i);
	}
	affect_total(ch);
	st->bank = ch->bank;
}				/* Char to store */

/* create a new entry in the in-memory index table for the player file */
int create_entry(char *name)
{
	int i;

	if (top_of_p_table == -1) {
		CREATE(player_table, struct player_index_element, 1);
		top_of_p_table = 0;
	} else if (!(player_table = (struct player_index_element *)
		     realloc(player_table, sizeof(struct player_index_element) *
			      (++top_of_p_table + 1)))) {
		perror("create entry");
		exit(1);
	}
	CREATE(player_table[top_of_p_table].name, char, strlen(name) + 1);
	/* copy lowercase equivalent of name to table field */
	for (i = 0;
	     (*(player_table[top_of_p_table].name + i) = tolower(*(name + i)));
	     i++) ;
	player_table[top_of_p_table].nr = top_of_p_table;
	return (top_of_p_table);
}

/* save_char_nocon is made for coin copy bug fixing by dsshin */

void save_char_nocon(struct char_data *ch, sh_int load_room)
{
	FILE *fl;
	int player_i;
	struct char_file_u st;

	int find_name(char *name);

	if ((player_i = find_name(GET_NAME(ch))) >= 0) {
		if (!(fl = fopen(PLAYER_FILE, "r+"))) {
			perror("Opening player file for reading. (db.c, save_char_nocon)");
			exit(0);
		}
		fseek(fl, (long)(player_table[player_i].nr *
				 sizeof(struct char_file_u)), 0);
		fread(&st, sizeof(struct char_file_u), 1, fl);
		char_to_store(ch, &st);
		st.load_room = load_room;

		fseek(fl, (long)(player_table[player_i].nr *
				 sizeof(struct char_file_u)), 0);
		fwrite(&st, sizeof(struct char_file_u), 1, fl);

		fclose(fl);
	} else
		return;
}

/* write the vital data of a player to the player file */
void save_char(struct char_data *ch, sh_int load_room)
{
	struct char_file_u st;
	FILE *fl;
	char mode[4];
	int expand;

	if (IS_NPC(ch) || !ch->desc)
		return;

	if ((expand = (ch->desc->pos > top_of_p_file))) {
		strcpy(mode, "a");
		top_of_p_file++;
	} else
		strcpy(mode, "r+");
	char_to_store(ch, &st);
	st.load_room = load_room;
	if (ch->desc->pwd[0] != NUL)
		strcpy(st.pwd, ch->desc->pwd);
	if (!(fl = fopen(PLAYER_FILE, mode))) {
		perror("save char");
		exit(1);
	}
	if (!expand)
		fseek(fl, ch->desc->pos * sizeof(struct char_file_u), 0);
	fwrite(&st, sizeof(struct char_file_u), 1, fl);
	fclose(fl);
}

/* rewrite player file to delete one character */
void delete_char(struct char_data *ch)
{
	struct char_file_u element;
	FILE *old, *new;
	int i, skip;
	char fname[256];

	if (IS_NPC(ch) || !ch->desc)
		return;

	if (!(old = fopen(PLAYER_FILE, "r")))
		return;
	strcpy(fname, PLAYER_FILE);
	strcat(fname, ".tmp");
	if (!(new = fopen(fname, "w")))
		return;

	skip = ch->desc->pos;
	for (i = 0; i < top_of_p_file; i++) {
		fread(&element, sizeof(struct char_file_u), 1, old);
		if (i != skip)
			fwrite(&element, sizeof(struct char_file_u), 1, new);
	}
	top_of_p_file--;

	fclose(old);
	fclose(new);
	unlink(PLAYER_FILE);
	rename(fname, PLAYER_FILE);
}

int remove_entry(struct char_data *ch)
{
	int i;
	int need_copy;
	struct player_index_element *tmp_player_table;
	struct char_data *tmp_ch;

	/* only exist imple */
	if (top_of_p_table == 0) {
		free(player_table);
		player_table = 0;
	} else {
		/* create new entry */
		CREATE(tmp_player_table, struct player_index_element, top_of_p_table);
		if (!tmp_player_table)
			return 0;
		/* now copy entry */
		need_copy = ch->desc->pos;
		for (i = 0; i < need_copy; i++)
			tmp_player_table[i] = player_table[i];
		for (; i < top_of_p_table; i++) {
			player_table[i + 1].nr--;
			tmp_player_table[i] = player_table[i + 1];
		}
		/* update already playing player's pos */
		for (tmp_ch = character_list; tmp_ch; tmp_ch = tmp_ch->next) {
			if (tmp_ch->desc && tmp_ch->desc->pos > need_copy)
				tmp_ch->desc->pos--;
		}
		/* remove old entry */
		free(player_table);

		/* assign new entry */
		player_table = tmp_player_table;
	}
	top_of_p_table--;
	return 1;
}

int compare(struct player_index_element *arg1, struct player_index_element *arg2)
{
	return (str_cmp(arg1->name, arg2->name));
}

/************************************************************************
*  procs of a (more or less) general utility nature      *
********************************************************************** */

/* read and allocate space for a '~'-terminated string from a given file */
/* ~ 문자로 끝나는 문자열을 파일에서 읽고 앞뒤 공백을 제거한 후 메모리를 할당
 *  by Komo, 251121                              */
char *fread_string(FILE *fl)
{
    char buf[MAX_STRING_LENGTH] = {0}; 
    char tmp[512];
    char *rslt;
    char *point;
    char *start_ptr; 
    int flag = 0;
    
    if (fl == NULL) {
        mudlog("fread_string: CRITICAL - Passed NULL file pointer.");
        exit(1);
    }

    do {
        if (!fgets(tmp, sizeof(tmp), fl)) {
            
            if (ferror(fl) && errno == EINTR) {
                clearerr(fl); // 에러 플래그 초기화
                continue;     // 루프 재시작 (다시 읽기)
            }

            /* 진짜 에러이거나 EOF인 경우 */
            mudlog("fread_string: fgets failed (EOF or Error).");
            perror("fread_string");
            exit(1);
        }

        /* 버퍼 오버플로우 방지 검사 */
        if (strlen(buf) + strlen(tmp) >= MAX_STRING_LENGTH) {
            mudlog("SYSERR: fread_string: string too large (db.c)");
            strncat(buf, tmp, MAX_STRING_LENGTH - strlen(buf) - 1);
            flag = 1; 
        } else {
            strcat(buf, tmp);
        }

        /* 문자열 끝에서 '~' 문자 검색 */
        if ((point = strchr(buf, '~')) != NULL) {
            *point = '\0';
            flag = 1;
        }

    } while (!flag);

    // 뒤쪽 공백 제거
    point = buf + strlen(buf) - 1;
    while (point >= buf && isspace(*point)) {
        *point-- = '\0';
    }

    // 앞쪽 공백 건너뛰기
    start_ptr = buf;
    while (*start_ptr && isspace(*start_ptr)) {
        start_ptr++;
    }

    /* 메모리 할당 및 복사 */
    if (strlen(start_ptr) > 0) {
        CREATE(rslt, char, strlen(start_ptr) + 1);
        strcpy(rslt, start_ptr);
    } else {
        CREATE(rslt, char, 1);
        rslt[0] = '\0';
    }

    return rslt;
}

/* release memory allocated for a char struct */
/* modified by Komo */
void free_char(struct char_data *ch)
{
    struct affected_type *af, *next_af;
    
	if (GET_NAME(ch)) {
        free(GET_NAME(ch));
    }

    if (ch->player.title) {
        free(ch->player.title);
    }
    if (ch->player.short_descr){
        free(ch->player.short_descr);
    }
    if (ch->player.long_descr){
        free(ch->player.long_descr);
    }
    if(ch->player.description){
        free(ch->player.description);
    }
    for (af = ch->affected; af; af = next_af) {
        next_af = af->next; // 다음 노드 미리 저장 (안전한 루프)
        affect_remove(ch, af); 
    }

    free(ch);
}

/* release memory allocated for an obj struct */
void free_obj(struct obj_data *obj)
{
	struct extra_descr_data *this, *next_one;

	free(obj->name);
	if (obj->description)
		free(obj->description);
	if (obj->short_description)
		free(obj->short_description);
	if (obj->action_description)
		free(obj->action_description);

	for (this = obj->ex_description; (this != 0); this = next_one) {
		next_one = this->next;
		if (this->keyword)
			free(this->keyword);
		if (this->description)
			free(this->description);
		free(this);
		this = NULL;
	}

	free(obj);
}

/* read contents of a text file, and place in buf */
int file_to_string(char *name, char *buf)
{
	FILE *fl;
	char tmp[200];
	char log_buf[256]; // 로그를 위한 버퍼 추가

	*buf = '\0';

	snprintf(log_buf, sizeof(log_buf), "Attempting to read data file: %s", name);
    mudlog(log_buf);

	if (!(fl = fopen(name, "r"))) {
		perror(name);
		*buf = '\0';
		return (-1);
	}

	do {
		fgets(tmp, 199, fl);

		if (!feof(fl)) {
			if (strlen(buf) + strlen(tmp) + 2 > MAX_STRING_LENGTH) {
				mudlog("fl->strng: string too big (db.c, file_to_string)");
				buf[25] = '\0';
				mudlog(buf);
				*buf = '\0';
				return (-1);
			}

			strcat(buf, tmp);
			*(buf + strlen(buf) + 1) = '\0';
			*(buf + strlen(buf)) = '\r';
		}
	} while (!feof(fl));

	fclose(fl);

	return (0);
}

/* clear some of the the working variables of a char */
void reset_char(struct char_data *ch)
{
	int i;

	for (i = 0; i < MAX_WEAR; i++)	/* Initializing */
		ch->equipment[i] = 0;
	ch->followers = 0;
	ch->master = 0;
	ch->carrying = 0;
	ch->next = 0;
	ch->next_fighting = 0;
	ch->next_in_room = 0;
	ch->specials.fighting = 0;
	ch->specials.position = POSITION_STANDING;
	ch->specials.default_pos = POSITION_STANDING;
	ch->specials.carry_weight = 0;
	ch->specials.carry_items = 0;
	if (GET_HIT(ch) <= 0)
		GET_HIT(ch) = 1;
	if (GET_MOVE(ch) <= 0)
		GET_MOVE(ch) = 1;
	if (GET_MANA(ch) <= 0)
		GET_MANA(ch) = 1;
}

/* clear ALL the working variables of a char and do NOT free any space alloc'ed*/
void clear_char(struct char_data *ch)
{
	bzero(ch, sizeof(struct char_data));

	ch->in_room = NOWHERE;
	ch->specials.was_in_room = NOWHERE;
	ch->specials.position = POSITION_STANDING;
	ch->specials.default_pos = POSITION_STANDING;
	GET_AC(ch) = 1;		/* Basic Armor */
}

void clear_object(struct obj_data *obj)
{
	bzero(obj, sizeof(struct obj_data));

	obj->item_number = -1;
	obj->in_room = NOWHERE;
}

/* initialize a new character only if class is set */
void init_char(struct char_data *ch)
{
	int i;
	int remortal[4] = {1, 2, 4, 8};

	/* *** if this is our first player --- he be God *** */
	if (top_of_p_table < 0) {
		GET_EXP(ch) = 1;
		GET_LEVEL(ch) = IMO + 3;
		void set_title(struct char_data *ch);
		set_title(ch);
	} else
		GET_LEVEL(ch) = 0;

	ch->player.short_descr = 0;
	ch->player.long_descr = 0;
	ch->player.description = 0;

	ch->player.time.birth = time(0);
	ch->player.time.played = 0;
	ch->player.time.logon = time(0);

	roll_abilities(ch);

	ch->player.remortal = remortal[GET_CLASS(ch) - 1];

	GET_HIT(ch) = GET_PLAYER_MAX_HIT(ch) = 0;
	GET_MANA(ch) = GET_PLAYER_MAX_MANA(ch) = 0;
	GET_MOVE(ch) = GET_PLAYER_MAX_MOVE(ch) = 0;

	/* make favors for sex */
	if (ch->player.sex == SEX_MALE) {
		ch->player.weight = number(120, 180);
		ch->player.height = number(160, 200);
	} else {
		ch->player.weight = number(100, 160);
		ch->player.height = number(150, 180);
	}

	/* quest */
	ch->quest.type = 0;
	ch->quest.data = 0;
	ch->quest.solved = 0;

	/* wimpyness */
	ch->specials.wimpyness = 0;

	/* initial bonus */
	ch->points.gold = 1000;

	for (i = 0; i <= MAX_SKILLS - 1; i++) {
		if (GET_LEVEL(ch) < IMO + 3) {
			ch->skills[i].learned = 0;
			ch->skills[i].skilled = 0;
			ch->skills[i].recognise = 0;
		} else {
			ch->skills[i].learned = 100;
			ch->skills[i].skilled = 0;
			ch->skills[i].recognise = 0;
		}
	}

	ch->specials.affected_by = 0;
	ch->specials.spells_to_learn = 0;
	for (i = 0; i < 5; i++)
		ch->specials.apply_saving_throw[i] = 0;
	for (i = 0; i < 3; i++)
		GET_COND(ch, i) = (GET_LEVEL(ch) == (IMO + 3) ? -1 : 24);
}

/* returns the real number of the room with given virtual number */
int real_room(int virtual)
{
	int bot, top, mid;

	bot = 0;
	top = top_of_world;

	/* perform binary search on world-table */
	while (bot <= top) {
		mid = bot + (top - bot) / 2;
		if ((world + mid)->number == virtual)
			return mid;
		if ((world + mid)->number > virtual)
			top = mid - 1;
		else
			bot = mid + 1;
	}
	fprintf(stderr, "Room %d does not exist in database\n", virtual);
	return -1;
}

/* returns the real number of the monster with given virtual number */
int real_mobile(int virtual)
{
	int bot, top, mid;

	bot = 0;
	top = top_of_mobt;

	/* perform binary search on mob-table */
	while (bot <= top) {
		mid = bot + (top - bot) / 2;

		if ((mob_index + mid)->virtual == virtual)
			return mid;
		if ((mob_index + mid)->virtual > virtual)
			top = mid - 1;
		else
			bot = mid + 1;
	}
	fprintf(stderr, "Mob %d does not exist in database\n", virtual);
	return -1;
}

/* returns the real number of the object with given virtual number */
int real_object(int virtual)
{
	int bot, top, mid;

	bot = 0;
	top = top_of_objt;

	/* perform binary search on obj-table */
	while (bot <= top) {
		mid = bot + (top - bot) / 2;

		if ((obj_index + mid)->virtual == virtual)
			return mid;
		if ((obj_index + mid)->virtual > virtual)
			top = mid - 1;
		else
			bot = mid + 1;
	}
	fprintf(stderr, "Object %d does not exist in database\n", virtual);
	return -1;
}

/*
 * 존의 고유 번호(VNUM)를 입력받아 zone_table 배열의 인덱스(RNUM)를 반환
 * 찾지 못하면 -1을 반환. by Komo
 */
int real_zone_by_number(int virtual)
{
    int i;

    // 존 테이블 전체를 순회
    for (i = 0; i <= top_of_zone_table; i++) {
        if (zone_table[i].number == virtual)
            return i;
    }

    return -1;
}


// re-work by komoyon@gmail.com, 251016
/* * 안전하게 stash 파일 이동 - 성공 시 0, 실패 시 -1 반환
 */
int move_stashfile_safe(const char *victim)
{
    char sf1[256], sf2[256], name[100];
    int i;
    char log_buf[1024];

    if (!victim || victim[0] == '\0') {
        mudlog("move_stashfile_safe error: victim name is NULL or empty.");
        return -1;
    }

    if (strlen(victim) >= sizeof(name)) {
        snprintf(log_buf, sizeof(log_buf), "move_stashfile_safe error: victim name '%s' is too long.", victim);
        mudlog(log_buf);
        return -1;
    }
    strncpy(name, victim, sizeof(name) - 1);
    name[sizeof(name) - 1] = '\0'; // 항상 NULL 문자로 끝나도록

    /* 경로 조작 방지 */
    for (i = 0; name[i]; ++i) {
        if (!isalnum((unsigned char)name[i])) {
            snprintf(log_buf, sizeof(log_buf), "move_stashfile_safe error: victim name '%s' contains invalid characters.", victim);
            mudlog(log_buf);
            return -1;
        }
        name[i] = tolower((unsigned char)name[i]);
    }
    
    snprintf(sf1, sizeof(sf1), "%s/%c/%s.x", STASH, name[0], name);
    snprintf(sf2, sizeof(sf2), "%s/%c/%s.x.y", STASH, name[0], name);

    /* 오류 처리 */
    if (rename(sf1, sf2) != 0) {
        snprintf(log_buf, sizeof(log_buf), "move_stashfile_safe error: Failed to rename '%s' to '%s'", sf1, sf2);
        mudlog(log_buf);
        perror("move_stashfile_safe system error");
        return -1; // 실패 반환
    }

    snprintf(log_buf, sizeof(log_buf), "move_stashfile_safe: Successfully renamed '%s' to '%s'", sf1, sf2);
    mudlog(log_buf);
    
    return 0; // 성공 반환
}

void stash_char(struct char_data *ch)
{
	struct obj_data *p;
	char stashfile[256], name[100];
	FILE *fl;
	int i;
	sigset_t mask, orig_mask; /* 251121 by Komo */
	void stash_contents(FILE * fl, struct obj_data *p, int wear_flag);
	char buf[MAX_OUTPUT_LENGTH];

	sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGBUS);
    sigaddset(&mask, SIGSEGV);
    sigaddset(&mask, SIGPIPE);
    sigaddset(&mask, SIGALRM);
    sigaddset(&mask, SIGTERM);
    sigaddset(&mask, SIGURG);
    sigaddset(&mask, SIGXCPU);
    sigaddset(&mask, SIGHUP);

	if (!ch)
		return;
	if (IS_NPC(ch) || !ch->desc)
		return;
	if (GET_NAME(ch))
		strcpy(name, GET_NAME(ch));
	else
		return;

	for (i = 0; name[i]; ++i)
		if (isupper(name[i]))
			name[i] = tolower(name[i]);
	// 최종 파일(.x.y) 대신 임시 파일(.x)의 경로 생성, 251016
    snprintf(stashfile, sizeof(stashfile), "%s/%c/%s.x", STASH, name[0], name); // 확장자를 .x 로 변경

    snprintf (buf, sizeof(buf), "Saving character data to stash : %s", stashfile);
	mudlog(buf);

	sigprocmask(SIG_BLOCK, &mask, &orig_mask); /* by Komo */
	if (!(fl = fopen(stashfile, "w"))) {		/* remove all data if exist */
		perror("saving PC's stash");
		sigprocmask(SIG_SETMASK, &orig_mask, NULL);
		return;
	}

	fprintf(fl, "%d\n", KJHRENT);
	for (i = 0; i < MAX_WEAR; ++i)
		if ((p = ch->equipment[i])) {
			if (p != NULL)
				stash_contents(fl, p, i);
		}
	if (ch->carrying)
		stash_contents(fl, ch->carrying, -1);

	fflush(fl);
	fclose(fl);
	sigprocmask(SIG_SETMASK, &orig_mask, NULL);
}

/* Read stash file and load objects to player. and remove stash files */
/* save one item into stashfile. if item has contents, save it also */
/* old version */
void stash_contents(FILE * fl, struct obj_data *p, int wear_flag)
{
	struct obj_data *pc;
	int j;

	if (!fl)
		return;
	if (p == 0)
		return;

	if (p->obj_flags.type_flag != ITEM_KEY && !IS_OBJ_STAT(p, ITEM_NORENT)) {
		if ((pc = p->contains))
			stash_contents(fl, pc, wear_flag >= 0 ? -2 : wear_flag
				       - 1);

		fprintf(fl, "%d", obj_index[p->item_number].virtual);
		fprintf(fl, " %d", wear_flag);
		for (j = 0; j < 4; ++j)
			fprintf(fl, " %d", p->obj_flags.value[j]);
		for (j = 0; j < 2; j++)
			fprintf(fl, " %d %d", p->affected[j].location,
				p->affected[j].modifier);
		// #ifdef SYPARK
		fprintf(fl, " %d %d", p->obj_flags.extra_flags, p->obj_flags.gpd);
		fprintf(fl, "\n");
		fprintf(fl, "%s\n", p->name);
		fprintf(fl, "%s\n", p->short_description);
		fprintf(fl, "%s\n", p->description);
		// #endif
	}

	if ((pc = p->next_content))
		stash_contents(fl, pc, wear_flag);
}

/* Read stash file and load objects to player. and remove stash files */
/* old version */
void unstash_char(struct char_data *ch, char *filename)
{
	void wipe_stash(char *filename);
	struct obj_data *obj;
	char stashfile[256], name[100];
	FILE *fl;
	int i, n, tmp[4];
	char tmp_str[255], *str;
	char buf[MAX_OUTPUT_LENGTH];

	/* for Knife rent */
	struct obj_data *item_stackp[MAX_RENT_ITEM];
	int item_stack[MAX_RENT_ITEM];
	int stack_count = 0;
	int where;
	sigset_t mask, orig_mask; /* by Komo */
	static int loc_to_where[22] =
	{
		0, 1, 1, 2, 2, 3, 4, 5, 6, 7,
		8, 14, 9, 10, 11, 11, 12, 13, 15, 15,
		16, 17};
#undef MAX_RENT_ITEM

	sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGBUS);
    sigaddset(&mask, SIGSEGV);
    sigaddset(&mask, SIGPIPE);
    sigaddset(&mask, SIGALRM);
    sigaddset(&mask, SIGTERM);
    sigaddset(&mask, SIGURG);
    sigaddset(&mask, SIGXCPU);
    sigaddset(&mask, SIGHUP);

	if (!ch)
		return;
	if (IS_NPC(ch) || !ch->desc)
		return;
	if (GET_NAME(ch))
		strcpy(name, filename ? filename : GET_NAME(ch));
	else
		return;
	for (i = 0; name[i]; ++i)
		if (isupper(name[i]))
			name[i] = tolower(name[i]);

	sigprocmask(SIG_BLOCK, &mask, &orig_mask);
	snprintf(stashfile, sizeof(stashfile), "%s/%c/%s.x.y", STASH, name[0], name);
	if (!(fl = fopen(stashfile, "r"))) {
		snprintf(stashfile, sizeof(stashfile), "%s/%c/%s.x", STASH, name[0], name);
		if (!(fl = fopen(stashfile, "r"))) {
			sigprocmask(SIG_SETMASK, &orig_mask, NULL);
			return;
		}
	}

	snprintf(buf, sizeof(buf), "Unstash : %s", stashfile);
	mudlog(buf);

	fscanf(fl, "%d", &n);
	if (n != KJHRENT) {
		mudlog("File format error in unstash_char. (db.c)");
		sigprocmask(SIG_SETMASK, &orig_mask, NULL);
		fclose(fl);
		return;
	}

	for (;;) {
		if (fscanf(fl, "%d", &n) <= 0)
			break;

		fscanf(fl, "%d", &where);
		for (i = 0; i < 4; ++i)
			fscanf(fl, "%d", &tmp[i]);
		obj = read_object(n, VIRTUAL);
		if (obj == 0)
			continue;
		for (i = 0; i < 4; ++i)
			obj->obj_flags.value[i] = tmp[i];
		for (i = 0; i < 4; i++)
			fscanf(fl, "%d", &tmp[i]);
		for (i = 0; i < 2; i++) {
			obj->affected[i].location = tmp[i * 2];
			obj->affected[i].modifier = tmp[i * 2 + 1];
		}
		// #ifdef   SYPARK
		fscanf(fl, "%d", &tmp[0]);
		if (tmp[0] != -1)
			obj->obj_flags.extra_flags = tmp[0];
		fscanf(fl, "%d", &tmp[0]);
		if (tmp[0] != -1)
			obj->obj_flags.gpd = tmp[0];
		fgets(tmp_str, 255, fl);
		fgets(tmp_str, 255, fl);
		tmp_str[strlen(tmp_str) - 1] = 0;
		if (strlen(tmp_str) != 0) {
			str = malloc(strlen(tmp_str) + 1);
			strcpy(str, tmp_str);
			free(obj->name);
			obj->name = str;
		}
		fgets(tmp_str, 255, fl);
		tmp_str[strlen(tmp_str) - 1] = 0;
		if (strlen(tmp_str) != 0) {
			str = malloc(strlen(tmp_str) + 1);
			strcpy(str, tmp_str);
			free(obj->short_description);
			obj->short_description = str;
		}
		fgets(tmp_str, 255, fl);
		tmp_str[strlen(tmp_str) - 1] = 0;
		if (strlen(tmp_str) != 0) {
			str = malloc(strlen(tmp_str) + 1);
			strcpy(str, tmp_str);
			free(obj->description);
			obj->description = str;
		}
		// #endif
		while (stack_count && item_stack[stack_count - 1] < -1 &&
		       item_stack[stack_count - 1] < where) {
			stack_count--;
			obj_to_obj(item_stackp[stack_count], obj);
		}
		item_stackp[stack_count] = obj;
		item_stack[stack_count] = where;
		stack_count++;
	}
	while (stack_count > 0) {
		stack_count--;
		obj_to_char(item_stackp[stack_count], ch);
		if (item_stack[stack_count] >= 0) {
			where = loc_to_where[item_stack[stack_count]];
			wear(ch, item_stackp[stack_count], where);
		}
	}

	fclose(fl);
	sigprocmask(SIG_SETMASK, &orig_mask, NULL);
}

void wipe_stash(char *filename)	/* delete id.x and id.x.y */
{
	char stashfile[256], name[50];
	int i;

	for (i = 0; filename[i]; ++i)
		name[i] = isupper(filename[i]) ? tolower(filename[i]) : filename[i];
	name[i] = 0;
	snprintf(stashfile, sizeof(stashfile), "%s/%c/%s.x", STASH, name[0], name);
	unlink(stashfile);
	snprintf(stashfile, sizeof(stashfile), "%s/%c/%s.x.y", STASH, name[0], name);
	unlink(stashfile);
}

void do_checkrent(struct char_data *ch, char *argument, int cmd)
{
	char stashfile[MAX_STRING_LENGTH], name[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
	char str[256];
	FILE *fl;
	int i, j, n;
	
	one_argument(argument, name);
	if (!*name) {
        send_to_char("Check whose rent file?\n\r", ch);
        return;
    }
	for (i = 0; name[i]; ++i)
		if (isupper(name[i]))
			name[i] = tolower(name[i]);
	snprintf(stashfile, sizeof(stashfile), "%s/%c/%s.x.y", STASH, name[0], name);
	if (!(fl = fopen(stashfile, "r"))) {
		snprintf(buf, sizeof(buf), "%s has nothing in rent.\n\r", name);
		send_to_char(buf, ch);
		return;
	}
	buf[0] = '\0';
	fscanf(fl, "%d", &n);
	for (i = j = 0;;) {
		if (fscanf(fl, "%d", &n) <= 0)
			break;
		if (n < 1000)
			continue;
		if (n > 99999)
			continue;
		++j;
		snprintf(buf + i, sizeof(buf) - i, "%5d%c", n, (j == 10) ? '\n' : ' ');
        if (j == 10)
            j = 0;
        i += 5;
        fgets(str, 255, fl);
        fgets(str, 255, fl);
        fgets(str, 255, fl);
        fgets(str, 255, fl);
	}
	fclose(fl);
	strlcat(buf, "\n\r", sizeof(buf));
	send_to_char(buf, ch);
	return;
}
void do_extractrent(struct char_data *ch, char *argument, int cmd)
{
	char name[MAX_INPUT_LENGTH], buf[MAX_OUTPUT_LENGTH];

	one_argument(argument, name);
	if (!*name)
		return;
	unstash_char(ch, name);
	send_to_char("OK.\n\r", ch);
	snprintf(buf, sizeof(buf), "%s grabbed rent for %s", GET_NAME(ch), name);
	mudlog(buf);
}
void do_replacerent(struct char_data *ch, char *argument, int cmd)
{
	char name[MAX_INPUT_LENGTH], buf[MAX_OUTPUT_LENGTH];

	one_argument(argument, name);
	if (!*name)
		return;
	stash_char(ch);
	move_stashfile_safe(name);
	send_to_char("OK.\n\r", ch);
	snprintf(buf, sizeof(buf), "%s replaced rent for %s", GET_NAME(ch), name);
	mudlog(buf);
}

void do_rent(struct char_data *ch, char *arg, int cmd)
{
	// sh_int save_room;
	int i;
	void wipe_obj(struct obj_data *obj);

	if (IS_NPC(ch))
		return;
	if (cmd) {
		if (!IS_SET(world[ch->in_room].room_flags, RENT)) {
			send_to_char("You cannot rent here.\n\r", ch);
			return;
		}
		send_to_char("You retire for the night.\n\r", ch);
		act("$n retires for the night.", FALSE, ch, 0, 0, TO_NOTVICT);
	}
	stash_char(ch);		/* clear file.x and save into file.x */
	move_stashfile_safe(ch->player.name);	/* move file.x to file.x.y */
	wipe_obj(ch->carrying);
	for (i = 0; i < MAX_WEAR; i++)
		if (ch->equipment[i]) {
			extract_obj(unequip_char(ch, i));
			ch->equipment[i] = 0;
		}
	wipe_obj(ch->carrying);
	ch->carrying = 0;
	
	DEBUG_LOG("db.c purge(%s)", ch->player.name);
	extract_char(ch, TRUE);
	return;
}

void wipe_obj(struct obj_data *obj)
{
	if (obj) {
		wipe_obj(obj->contains);
		wipe_obj(obj->next_content);
		if (obj->in_obj)
			obj_from_obj(obj);
		extract_obj(obj);
	}
}
