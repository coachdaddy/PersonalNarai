/* include/globals.h */
#pragma once

#include "structs.h"
#include "spells.h" /* for spell_info_type */

/* ========================================================
   Global Variables Declarations
   --- by Komo, 251218
*  ======================================================== */

/* --- db.c --- */
extern struct room_data *world;
extern int top_of_world;
extern struct obj_data *object_list;
extern struct char_data *character_list;
extern struct zone_data *zone_table;
extern int top_of_zone_table;
extern struct message_list fight_messages[MAX_MESSAGES];
extern struct player_index_element *player_table;
extern int top_of_p_table;

extern char credits[MAX_STRING_LENGTH];
extern char news[MAX_STRING_LENGTH];
extern char imotd[MAX_STRING_LENGTH];
extern char motd[MAX_STRING_LENGTH];
extern char help[MAX_STRING_LENGTH];
extern char plan[MAX_STRING_LENGTH];
extern char wizards[MAX_STRING_LENGTH];

extern FILE *help_fl;

extern struct index_data *mob_index;
extern struct index_data *obj_index;
extern struct help_index_element *help_index;

extern int top_of_helpt;

extern struct time_info_data time_info;
extern struct weather_data weather_info;

extern int regen_percent;
extern int regen_time_percent;
extern int regen_time;

/* --- db2.h --- */
extern struct reset_q_type reset_q;

/* --- comm.c --- */
extern struct descriptor_data *descriptor_list;
extern int baddoms;
extern char baddomain[16][32];
extern int shutdowngame;
extern int boottime;
extern unsigned long reboot_time;

extern int nochatflag;
extern int nodisarmflag;
extern int noenchantflag;
extern int nokillflag;
extern int nonewplayers;
extern int no_specials;
extern int nostealflag;
extern int noshoutflag;

/* --- constants.c --- */
extern char *weekdays[7];
extern char *month_name[17];
extern struct str_app_type str_app[31];
extern struct wis_app_type wis_app[26];
extern struct int_app_type int_app[26];
extern struct dex_app_type dex_app[26];
extern struct dex_skill_type dex_app_skill[26];
extern char *spell_wear_off_msg[];
extern char *connected_types[];
extern char *dirs[];
extern char *where[];
extern char *drinks[];
extern char *drinknames[];
extern char *color_liquid[];
extern int drink_aff[][3];
extern char *fullness[];
extern struct title_type titles[4][IMO + 4];
extern int rev_dir[];
extern int movement_loss[];
extern char *item_types[];
extern char *wear_bits[];
extern char *extra_bits[];
extern char *apply_types[];
extern char *pc_class_types[];
extern char *room_bits[];
extern char *exit_bits[];
extern char *sector_types[];
extern char *affected_bits[];
extern char *action_bits[];
extern char *player_bits[];
extern int thaco[4][IMO + 4];
extern byte backstab_mult[IMO + 4];

/* daerimsa.c */
extern struct char_data *fourth_jangro;
extern struct char_data *son_ogong;

/* --- fight.c --- */
extern struct char_data *combat_list;

/* --- guild_commands.c --- */
extern char *guild_names[];
extern int guild_skill_nums[];
extern char *police_skills[];
extern char *outlaw_skills[];
extern char *assasin_skills[];
extern int police_skill_costs[];
extern int outlaw_skill_costs[];
extern int assasin_skill_costs[];

/* --- interpreter.c --- */
extern int no_echo;
extern struct command_info cmd_info[]; // MAX_CMD_LIST -> interpreter.c
extern char *command[];


/* --- quest.c --- */
extern struct quest_mob_info QM[];
extern int level_quest[IMO + 4];

/* --- spell_parser.c --- */
extern struct spell_info_type spell_info[]; // MAX_SPL_LIST -> spell.h
extern char *spells[];
extern byte saving_throws[4][5][IMO + 10];

/* --- version.c --- */
extern char fileid[];

/* --- weather.c --- */


/* --- guild_constant.c --- */
extern char *guild_names[];
