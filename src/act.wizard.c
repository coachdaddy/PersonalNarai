
/* ************************************************************************
*  file: actwiz.c , Implementation of commands.           Part of DIKUMUD *
*  Usage : Wizard Commands.                                               *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete information. *
************************************************************************* */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <stdlib.h> /* qsort, 251124 by Komo */
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "spells.h"
#include "limit.h"



#ifndef BADDOMS
#define BADDOMS 16
#endif


char history[20][MAX_STRING_LENGTH];
int his_start = 0, his_end = 0;

/* 안전한 문자열 붙이기 */
#define SAFE_PRINTF(...) \
    do { \
        if (len < size) \
            len += snprintf(buf + len, size - len, __VA_ARGS__); \
    } while (0)


void do_advance(struct char_data *ch, char *argument, int cmd)
{
	struct char_data *victim;
	char name[100], level[100];
	int i, newlevel;

	void gain_exp(struct char_data *ch, int gain);
	void init_char(struct char_data *ch);

	if (IS_NPC(ch))
		return;

	if (GET_LEVEL(ch) < (IMO + 3)) {
		send_to_char("You can only do that in a guild.\n\r", ch);
		return;
	}
	argument_interpreter(argument, name, level);
	if (*name) {
		if (!(victim = get_char_room_vis(ch, name))) {
			send_to_char("That player is not here.\n\r", ch);
			return;
		}
	} else {
		send_to_char("Advance who?\n\r", ch);
		return;
	}

	if (IS_NPC(victim)) {
		send_to_char("NO! Not on NPC's.\n\r", ch);
		return;
	}

	if (!*level) {
		send_to_char("You must supply a level number.\n\r", ch);
		return;
	} else {
		if (!isdigit(*level)) {
			send_to_char("Second argument must be a positive integer.\n\r", ch);
			return;
		}
		if ((newlevel = atoi(level)) < GET_LEVEL(victim)) {
			if (GET_LEVEL(ch) <= GET_LEVEL(victim)) {
				send_to_char("Very amusing.\n\r", ch);
				return;
			}
			victim->player.level = newlevel;
			if (newlevel < IMO) {
				for (i = 0; i < 3; ++i)
					victim->specials.conditions[i] = 0;
			}
			victim->tmpabilities = victim->abilities;
			send_to_char("The poor soul...\n\r", ch);
			send_to_char("You have been punished.\n\r", victim);
			return;
		}
	}

	if (newlevel > IMO) {
		send_to_char("Too high a level.\n\r", ch);
		return;
	}
	send_to_char("You feel generous.\n\r", ch);

	if (GET_LEVEL(victim) == 0) {
		init_char(victim);
		do_start(victim);
	} else {
		victim->points.exp = 1;
		gain_exp_regardless(victim, (titles[GET_CLASS(victim) - 1][
										  newlevel].exp)
				    - GET_EXP(victim));
	}
}

void do_at(struct char_data *ch, char *argument, int cmd)
{
	char command[MAX_INPUT_LENGTH], loc_str[MAX_INPUT_LENGTH];
	int loc_nr, location, original_loc;
	struct char_data *target_mob;
	struct obj_data *target_obj;
	
	if (IS_NPC(ch))
		return;

	half_chop(argument, loc_str, command);
	if (!*loc_str) {
		send_to_char("You must supply a room number or a name.\n\r", ch);
		return;
	}
	if (isdigit(*loc_str)) {
		loc_nr = atoi(loc_str);
		for (location = 0; location <= top_of_world; location++)
			if (world[location].number == loc_nr)
				break;
			else if (location == top_of_world) {
				send_to_char("No room exists with that number.\n\r", ch);
				return;
			}
	} else if ((target_mob = get_char_vis(ch, loc_str)))
		location = target_mob->in_room;
	else if ((target_obj = get_obj_vis(ch, loc_str)))
		if (target_obj->in_room != NOWHERE)
			location = target_obj->in_room;
		else {
			send_to_char("The object is not available.\n\r", ch);
			return;
	} else {
		send_to_char("No such creature or object around.\n\r", ch);
		return;
	}

	/* a location has been found. */
	if ((GET_LEVEL(ch) < (IMO + 3)) && (IS_SET(world[location].room_flags,
						   OFF_LIMITS))) {
		send_to_char("That room is off-limits.\n", ch);
		return;
	}
	original_loc = ch->in_room;
	char_from_room(ch);
	char_to_room(ch, location);
	command_interpreter(ch, command);

	/* check if the guy's still there */
	for (target_mob = world[location].people; target_mob; target_mob =
	     target_mob->next_in_room)
		if (ch == target_mob) {
			char_from_room(ch);
			char_to_room(ch, original_loc);
		}
}

void do_banish(struct char_data *ch, char *argument, int cmd)
{
	struct char_data *vict;
	struct obj_data *dummy;
	char buf[MAX_INPUT_LENGTH];
	int location;
	
	if (IS_NPC(ch))
		return;
	one_argument(argument, buf);
	if (!*buf)
		send_to_char("Banish whom?\n\r", ch);
	else if (!generic_find(argument, FIND_CHAR_WORLD, ch, &vict, &dummy))
		send_to_char("Couldn't find any such creature.\n\r", ch);
	else if (IS_NPC(vict))
		send_to_char("Can't do that to a beast.\n\r", ch);
	else if (GET_LEVEL(vict) >= IMO)
		send_to_char("It's pointless to banish an immortal.\n\r", ch);
	else if (IS_SET(vict->specials.act, PLR_BANISHED)) {
		REMOVE_BIT(vict->specials.act, PLR_BANISHED);
		send_to_char("You feel forgiven?\n\r", vict);
		act("$N is forgiven.", FALSE, ch, 0, vict, TO_CHAR);
	} else {
		SET_BIT(vict->specials.act, PLR_BANISHED);
		for (location = 0; location <= top_of_world; location++)
			if (world[location].number == 6999)
				break;
		if (location == top_of_world) {
			send_to_char("Death Room is gone?\n\r", ch);
		} else {
			act("$n disappears in a puff of smoke.", FALSE, vict,
			    0, 0, TO_ROOM);
			char_from_room(vict);
			char_to_room(vict, location);
			act("$n appears with an ear-splitting bang.", FALSE,
			    vict, 0, 0, TO_ROOM);
		}
		send_to_char("You smell fire and brimstone?\n\r", vict);
		act("$N is banished.", FALSE, ch, 0, vict, TO_CHAR);
	}
	send_to_char("OK.\n\r", ch);
}

/* do_chat -> act.comm.c로 이동, 251124 by Komo */

void do_demote(struct char_data *ch, char *argument, int cmd)
{
	struct char_data *victim;
	char name[100], buf[BUFSIZ];
	void init_char(struct char_data *ch);
	void do_start(struct char_data *ch);

	snprintf(buf, sizeof(buf), "%s demoted %s", GET_NAME(ch), argument);
	mudlog(buf);

	if (IS_NPC(ch))
		return;

	one_argument(argument, name);
	if (*name) {
		if (!(victim = get_char_room_vis(ch, name))) {
			send_to_char("That player is not here.\n\r", ch);
			return;
		}
	} else {
		send_to_char("Demote who?\n\r", ch);
		return;
	}
	if (IS_NPC(victim)) {
		send_to_char("NO! Not on NPC's.\n\r", ch);
		return;
	}
	if (GET_LEVEL(victim) > GET_LEVEL(ch)) {
		send_to_char("Impossible!\n\r", ch);
		return;
	}

	GET_AC(ch) += GET_LEVEL(ch);
	GET_HITROLL(ch) -= GET_LEVEL(ch);
	GET_DAMROLL(ch) -= GET_LEVEL(ch);

	init_char(victim);
	do_start(victim);
}
void do_echo(struct char_data *ch, char *argument, int cmd)
{
	struct char_data *k;
	char buf[MAX_STRING_LENGTH];

	if (IS_NPC(ch)) return;

	for (; *argument == ' '; argument++) ;

	if (!*argument) {
		send_to_char("&c[ECHO]&n &YYour divine voice needs words to reach the mortals.&n\n\r", ch);
        send_to_char("       &WUsage: echo <message>&n\n\r", ch);
	} else {
		snprintf(buf, sizeof(buf), "%s\n\r", argument);
		for (k = world[ch->in_room].people; k; k = k->next_in_room) {
            if (k != ch) {
                send_to_char(buf, k);
            }
        }
		snprintf(buf, sizeof(buf), "&c[ECHO]&n %s\n\r", argument);
        send_to_char(buf, ch);
	}
}

void do_emote(struct char_data *ch, char *argument, int cmd)
{
	char buf[MAX_STRING_LENGTH];

	if (IS_SET(ch->specials.act, PLR_DUMB_BY_WIZ) && GET_LEVEL(ch) < IMO + 3) {
		return;
	}
	if (IS_NPC(ch))	return;

	for (; *argument == ' '; argument++) ;

	if (!*argument)
		send_to_char("Yes, sir... But what?\n\r", ch);
	else {
		snprintf(buf, sizeof(buf), "$n %s", argument);
		act(buf, FALSE, ch, 0, 0, TO_ROOM);

		snprintf(buf, sizeof(buf), "You %s.\n\r", argument);
		send_to_char(buf, ch);
	}
}

void do_invis(struct char_data *ch, char *argument, int cmd)
{
	if (IS_NPC(ch) || GET_LEVEL(ch) > (IMO + 4))
		return;
	if (IS_SET(ch->specials.act, PLR_WIZINVIS)) {
		REMOVE_BIT(ch->specials.act, PLR_WIZINVIS);
		send_to_char("You are visible again.\n\r", ch);
	} else {
		SET_BIT(ch->specials.act, PLR_WIZINVIS);
		send_to_char("You vanish.\n\r", ch);
	}
	send_to_char("Ok.\n\r", ch);
}

void do_flag(struct char_data *ch, char *argument, int cmd)
{
	struct char_data *vict;
	struct obj_data *dummy;
	char buf[MAX_INPUT_LENGTH];
	int f;

	if (IS_NPC(ch))
		return;
	one_argument(argument, buf);
	if (!*buf)
		send_to_char("Flag whom?\n\r", ch);
	else if (!generic_find(argument, FIND_CHAR_WORLD, ch, &vict, &dummy))
		send_to_char("Couldn't find any such creature.\n\r", ch);
	else if (IS_NPC(vict))
		send_to_char("Can't do that to a beast.\n\r", ch);
	else if ((GET_LEVEL(vict) >= IMO) && (cmd != 231))
		send_to_char("It's pointless to flag an immortal.\n\r", ch);
	else {
		f = (cmd == 231) ? IS_SET(vict->specials.act, PLR_XYZZY) :
		    IS_SET(vict->specials.act, PLR_CRIMINAL);
		if (f) {
			if (cmd == 231) {
				REMOVE_BIT(vict->specials.act, PLR_XYZZY);
				send_to_char("Log removed.\n\r", ch);
			} else {
				if (cmd != 297) {
					REMOVE_BIT(vict->specials.act, PLR_CRIMINAL);
					send_to_char("Flag removed.\n\r", ch);
				}
			}
		} else {
			if (cmd == 231) {
				SET_BIT(vict->specials.act, PLR_XYZZY);
				send_to_char("Log set.\n\r", ch);
			} else {
				if (cmd != 297) {
					SET_BIT(vict->specials.act, PLR_CRIMINAL);
					send_to_char("Flag set.\n\r", ch);
				}
			}
		}
		if (cmd == 297)	/* wizdumb */
		{
			if (IS_SET(vict->specials.act, PLR_DUMB_BY_WIZ)) {
				REMOVE_BIT(vict->specials.act, PLR_DUMB_BY_WIZ);
				send_to_char("DUMB removed.\n\r", ch);
			} else {
				SET_BIT(vict->specials.act, PLR_DUMB_BY_WIZ);
				send_to_char("DUMB set.\n\r", ch);
			}
		}
	}
}
void do_flick(struct char_data *ch, char *argument, int cmd)
{
	struct char_data *victim;
	struct obj_data *obj;
	char victim_name[240];
	char obj_name[240];
	int eq_pos;

	argument = one_argument(argument, obj_name);
	one_argument(argument, victim_name);
	if (!(victim = get_char_vis(ch, victim_name))) {
		send_to_char("Who?\n\r", ch);
		return;
	} else if (victim == ch) {
		send_to_char("Odd?\n\r", ch);
		return;
	} else if (GET_LEVEL(ch) <= GET_LEVEL(victim)) {
		send_to_char("Bad idea.\n\r", ch);
		return;
	}
	if (!(obj = get_obj_in_list_vis(victim, obj_name, victim->carrying))) {
		for (eq_pos = 0; (eq_pos < MAX_WEAR); eq_pos++)
			if (victim->equipment[eq_pos] &&
			    (isname(obj_name, victim->equipment[eq_pos]->name))) {
				obj = victim->equipment[eq_pos];
				break;
			}
		if (!obj) {
			send_to_char("Can't find that item.\n\r", ch);
			return;
		} else {	/* It is equipment */
			obj_to_char(unequip_char(victim, eq_pos), ch);
			send_to_char("Done.\n\r", ch);
		}
	} else {		/* obj found in inventory */
		obj_from_char(obj);
		obj_to_char(obj, ch);
		send_to_char("Done.\n\r", ch);
	}
}

void do_force(struct char_data *ch, char *argument, int cmd)
{
	struct descriptor_data *i;
	struct char_data *vict;
	char name[100], to_force[100], buf[200];
	int diff;

	if (IS_NPC(ch) || GET_LEVEL(ch) > (IMO + 3))
		return;

	half_chop(argument, name, to_force);

	if (!*name || !*to_force)
		send_to_char("Who do you wish to force to do what?\n\r", ch);
	else if (str_cmp("all", name)) {
		if (!(vict = get_char_vis(ch, name)))
			send_to_char("No-one by that name here..\n\r", ch);
		else {
			diff = GET_LEVEL(ch) - GET_LEVEL(vict);
			if ((!IS_NPC(vict)) && (diff <= 0)) {
				send_to_char("Oh no you don't!!\n\r", ch);
			} else {
				if (GET_LEVEL(ch) < (IMO + 3))
					snprintf(buf, sizeof(buf),
						"$n has forced you to '%s'.", to_force);
				else
					buf[0] = 0;
				act(buf, FALSE, ch, 0, vict, TO_VICT);
				send_to_char("Ok.\n\r", ch);
				command_interpreter(vict, to_force);
			}
		}
	} else {		/* force all */
		if (GET_LEVEL(ch) < (IMO + 3)) {
			send_to_char("Force all's are a bad idea these days.\n\r", ch);
			return;
		}
		for (i = descriptor_list; i; i = i->next)
			if (i->character != ch && !i->connected) {
				vict = i->character;
				command_interpreter(vict, to_force);
			}
		send_to_char("Ok.\n\r", ch);
	}
}

void do_goto(struct char_data *ch, char *argument, int cmd)
{
	char buf[MAX_INPUT_LENGTH];
	int loc_nr, location, i, flag;
	struct char_data *target_mob, *pers;
	struct obj_data *target_obj;
	
	if (IS_NPC(ch) || GET_LEVEL(ch) > (IMO + 4))
		return;
	one_argument(argument, buf);
	if (!*buf) {
		send_to_char("You must supply a room number or a name.\n\r", ch);
		return;
	}

	if (isdigit(*buf)) {
		loc_nr = atoi(buf);
		for (location = 0; location <= top_of_world; location++)
			if (world[location].number == loc_nr)
				break;
			else if (location == top_of_world) {
				send_to_char("No room exists with that number.\n\r", ch);
				return;
			}
	} else if ((target_mob = get_char_vis(ch, buf)))
		location = target_mob->in_room;
	else if ((target_obj = get_obj_vis(ch, buf)))
		if (target_obj->in_room != NOWHERE)
			location = target_obj->in_room;
		else {
			send_to_char("The object is not available.\n\r", ch);
			return;
	} else {
		send_to_char("No such creature or object around.\n\r", ch);
		return;
	}

	/* a location has been found. */

	if (GET_LEVEL(ch) < (IMO + 2)) {
		if (IS_SET(world[location].room_flags, OFF_LIMITS)) {
			send_to_char("Sorry, off limits.\n", ch);
			return;
		}
		if (IS_SET(world[location].room_flags, PRIVATE)) {
			for (i = 0, pers = world[location].people; pers; pers =
			     pers->next_in_room, i++) ;
			if (i > 1) {
				send_to_char(
						    "There's a private conversation going on in that room.\n\r", ch);
				return;
			}
		}
	}
	flag = ((GET_LEVEL(ch) >= (IMO + 2)) &&
		IS_SET(ch->specials.act, PLR_WIZINVIS));
	if (!flag)
		act("$n disappears in a puff of smoke.", FALSE, ch, 0, 0, TO_ROOM);
	char_from_room(ch);
	char_to_room(ch, location);
	if (!flag)
		act("$n appears with an ear-splitting bang.", FALSE, ch, 0, 0, TO_ROOM);
	do_look(ch, "", 15);
}

static char *load_one_argument(char *argument, char *first_arg)
{
    if (!argument) return NULL;

    while (isspace(*argument))
        argument++;

    while (*argument && !isspace(*argument)) {
        *first_arg = *argument;
        first_arg++;
        argument++;
    }

    *first_arg = '\0'; /* 문자열의 끝을 표시 */
    
    return argument;
}

/* rework by Komo, 251118 */
void do_load(struct char_data *ch, char *argument, int cmd)
{
    struct char_data *mob = NULL;
    struct obj_data *obj = NULL;
    
    char arg1[MAX_INPUT_LENGTH] = {0};
    char arg2[MAX_INPUT_LENGTH] = {0};
    char arg3[MAX_INPUT_LENGTH] = {0};
    char log_buf[MAX_STRING_LENGTH] = {0};		/* 로그용 버퍼 */
    char feedback_buf[MAX_STRING_LENGTH] = {0};	/* 관리자 피드백용 버퍼 */

    int vnum = -1, r_num = -1, quantity = 1, i; /* 수량 변수 추가 */

    if (IS_NPC(ch) || GET_LEVEL(ch) > (IMO + 4)) {
        return;
    }

    argument = load_one_argument(argument, arg1); /* type: char/obj */
    argument = load_one_argument(argument, arg2); /* vnum */
	argument = load_one_argument(argument, arg3); /* quantity (optional) */


    /* 도움말 개선 */
    if (!*arg1 || !*arg2) { /* 타입이나 번호가 입력되지 않은 경우 */
        send_to_char("&WUsage: load <type> <vnum> [quantity]&n\n\r\n\r", ch);
        send_to_char("  'load' 명령은 지정한 몬스터(char)나 아이템(obj)을 생성합니다.\n\r", ch);
        send_to_char("  <type>     : 'char' (몬스터) 또는 'obj' (아이템) 중 하나\n\r", ch);
        send_to_char("  <vnum>     : 생성할 대상의 가상 번호 (Virtual Number)\n\r", ch);
        send_to_char("  [quantity] : (optional) 생성할 수량(default: 1). 최대 char 20, obj 100\n\r\n\r", ch);
        send_to_char("Examples:\n\r", ch);
        send_to_char("  load char 3001    (3001번 몬스터 1마리 생성)\n\r", ch);
        send_to_char("  load obj 1201 10  (1201번 아이템 10개 생성)\n\r", ch);
        return;
    }

    if (!is_number(arg2)) { /* 입력값 검증 */
        send_to_char_han("The 'vnum' must be a number.\n\r", "VNum은 숫자로만 입력해야 합니다.\n\r", ch);
        return;
    }
    
    vnum = atoi(arg2);

    if (*arg3) { /* 수량 인수가 있다면 */
        if (!is_number(arg3)) {
            send_to_char_han("The 'quantity' must be a number.\n\r", "수량은 숫자로만 입력해야 합니다.\n\r", ch);
            return;
        }
        quantity = atoi(arg3);
        if (quantity < 1) {
            quantity = 1;
        }
        
        if (quantity > 100) { 
            send_to_char_han("You can create up to 100 objects at a time.\n\r", "한 번에 100개까지만 생성할 수 있습니다.\n\r", ch);
            quantity = 100;
        }
    }

    if (vnum < 0) { /* 기존 로직 */
        send_to_char("What did you mean by A NEGATIVE number??\n\r", ch);
        return;
    }

    /* --- 몬스터(char) 생성 --- */
    if (is_abbrev(arg1, "char")) {
        if ((r_num = real_mobile(vnum)) < 0) {
            send_to_char("There is no monster with that number (vnum).\n\r", ch);
            return;
        }

        if (quantity > 20) { /* 몬스터 수량 제한 */
            send_to_char_han("You can create up to 20 mobiles at a time.\n\r", "몬스터는 한 번에 20명까지만 생성할 수 있습니다.\n\r", ch);
            quantity = 20;
        }

		for (i = 0; i < quantity; i++) {
            mob = read_mobile(r_num, REAL);
            char_to_room(mob, ch->in_room);
        }

        const char *mob_name = mob->player.short_descr;
        const char *mob_keywords = mob->player.name;

        act("$n makes a quaint, magical gesture with one hand.", TRUE, ch, 0, 0, TO_ROOM);

        /* 사용자에게 정보 피드백 */
        if (quantity == 1) {
            act("$n has created $N!", FALSE, ch, 0, mob, TO_ROOM);
            snprintf(feedback_buf, sizeof(feedback_buf), "You've created 1 mobile - '%s' (%s, VNum: %d).\n\r",
                     mob_name, mob_keywords, vnum);
        } else {
            act("$n has created multiple monsters!", FALSE, ch, 0, 0, TO_ROOM);
            snprintf(feedback_buf, sizeof(feedback_buf),  "You've created %d mobiles - '%s' (%s, VNum: %d).\n\r",
                     quantity, mob_name, mob_keywords, vnum);
        }
        send_to_char(feedback_buf, ch);

        snprintf(log_buf, sizeof(log_buf), "%s loaded %d x char %d (%s)", ch->player.name, quantity, vnum, mob_name);
        log(log_buf);

    /* --- 아이템(obj) 생성 --- */
    } else if (is_abbrev(arg1, "obj")) {
        if ((r_num = real_object(vnum)) < 0) {
            send_to_char_han("There is no object with that number (vnum).\n\r", "해당 번호(VNum)를 가진 아이템이 없습니다.\n\r", ch);
            return;
        }

		obj = read_object(r_num, REAL);
		
		if (IS_SET(obj->obj_flags.wear_flags, ITEM_TAKE)) { /* create 1 first */
            obj_to_char(obj, ch);
        } else {
            obj_to_room(obj, ch->in_room);
        }

		if (!IS_SET(obj->obj_flags.wear_flags, ITEM_TAKE) && quantity > 1) {
            send_to_char_han("You can make only 1 at a time for this object.\n\r", "이 아이템은 한 번에 1개만 만들 수 있습니다.\n\r", ch);
			extract_obj(obj); 
            return;
        }
		
        for (i = 1; i < quantity; i++) { /* i = 1 -> create (quantity - 1) */
            obj = read_object(r_num, REAL);
            obj_to_char(obj, ch);
        }

		/* 'obj' 변수에는 마지막으로 생성된 아이템의 포인터가 남아있음 */
		const char *obj_name = obj->short_description;
        
        act("$n makes a strange magical gesture.", TRUE, ch, 0, 0, TO_ROOM);

        if (quantity == 1) {
            if (IS_SET(obj->obj_flags.wear_flags, ITEM_TAKE)) {
                act("$n snares $p!", FALSE, ch, obj, 0, TO_ROOM);
            } else {
                act("$n has created $p!", FALSE, ch, obj, 0, TO_ROOM);
            }
            snprintf(feedback_buf, sizeof(feedback_buf), "You've created 1 object -'%s' (VNum: %d).\n\r", obj_name, vnum);
        } else {
            if (IS_SET(obj->obj_flags.wear_flags, ITEM_TAKE)) {
                act("$n grabs a handful of items!", FALSE, ch, 0, 0, TO_ROOM);
            } else {
                act("$n has created a pile of items!", FALSE, ch, 0, 0, TO_ROOM);
            }
            snprintf(feedback_buf, sizeof(feedback_buf),
                     "You've created %d objects - '%s' (VNum: %d).\n\r", quantity, obj_name, vnum);
        }
        send_to_char(feedback_buf, ch);

        snprintf(log_buf, sizeof(log_buf), "%s loaded %d x object %d (%s)", ch->player.name, quantity, vnum, obj_name);
        log(log_buf);

    } else {
        send_to_char("That'll have to be either 'char' or 'obj'.\n\r", ch);
    }
}

void do_noaffect(struct char_data *ch, char *argument, int cmd)
{
	struct char_data *vict;
	struct obj_data *dummy;
	struct affected_type *hjp;
	char buf[MAX_INPUT_LENGTH];

	if (IS_NPC(ch)) return;

	one_argument(argument, buf);
	if (!*buf) {
		send_to_char("Remove affects from whom?\n\r", ch);
		return;
	} else {
		if (!generic_find(argument, FIND_CHAR_WORLD, ch, &vict, &dummy))
			send_to_char("Couldn't find any such creature.\n\r", ch);
		else if (IS_NPC(vict) && vict->in_room != ch->in_room)
			send_to_char("Only can do that to a mob in same room.\n\r", ch);
		else if (!IS_NPC(vict) && GET_LEVEL(vict) > GET_LEVEL(ch))
			act("$E might object to that.. better not.", 0, ch, 0,
			    vict, TO_CHAR);
		else {
			send_to_char("You are normal again.\n\r", vict);
			for (hjp = vict->affected; hjp; hjp = hjp->next)
				affect_remove(vict, hjp);
		}
	}
	send_to_char("Ok.\n\r", ch);
}
void do_noshout(struct char_data *ch, char *argument, int cmd)
{
	struct char_data *vict;
	struct obj_data *dummy;
	char buf[MAX_INPUT_LENGTH];

	if (IS_NPC(ch))
		return;
	one_argument(argument, buf);
	if (!*buf) {
		if (IS_SET(ch->specials.act, PLR_EARMUFFS)) {
			send_to_char("You can now hear shouts again.\n\r", ch);
			REMOVE_BIT(ch->specials.act, PLR_EARMUFFS);
		} else {
			send_to_char("From now on, you won't hear shouts.\n\r", ch);
			SET_BIT(ch->specials.act, PLR_EARMUFFS);
		}
		return;
	}
	if (GET_LEVEL(ch) < IMO)
		return;
	if (!generic_find(argument, FIND_CHAR_WORLD, ch, &vict, &dummy))
		send_to_char("Couldn't find any such creature.\n\r", ch);
	else if (IS_NPC(vict))
		send_to_char("Can't do that to a mobile.\n\r", ch);
	else if (GET_LEVEL(vict) > GET_LEVEL(ch))
		act("$E might object to that.. better not.", 0, ch, 0, vict, TO_CHAR);
	else if (IS_SET(vict->specials.act, PLR_NOSHOUT)) {
		send_to_char("You can shout again.\n\r", vict);
		send_to_char("NOSHOUT removed.\n\r", ch);
		REMOVE_BIT(vict->specials.act, PLR_NOSHOUT);
	} else {
		send_to_char("The gods take away your ability to shout!\n\r", vict);
		send_to_char("NOSHOUT set.\n\r", ch);
		SET_BIT(vict->specials.act, PLR_NOSHOUT);
	}
}

/* clean a room of all mobiles and objects */
/* modified by Komo */
void do_purge(struct char_data *ch, char *argument, int cmd)
{
	struct char_data *vict, *next_v;
	struct obj_data *obj, *next_o;
	int i;
	char name[100], buf[100], confirm[100];

	if (IS_NPC(ch) || GET_LEVEL(ch) > (IMO + 3))
		return;
	half_chop(argument, name, confirm);

	if (!*name) { // 인자가 없으면 사용법 안내
		send_to_char("&WUsage: purge <character | object>&n\n\r", ch);
		send_to_char("&W       purge room&n\n\r", ch);
		send_to_char("\n\r", ch);
		send_to_char("WARNING: Purging a PLAYER is permanent and irreversible.\n\r", ch);
		send_to_char("To purge a player, type: &Wpurge <player_name> confirm&n\n\r", ch);
		return;
	}

	// "room" 인자를 받았는지 확인
	if (str_cmp(name, "room") == 0)
	{
		if (IS_NPC(ch)) {
			send_to_char("Don't... You would only kill yourself..\n\r", ch);
			return;
		}
		act("$n gestures... You are surrounded by scorching flames!", FALSE, ch, 0, 0, TO_ROOM);
		send_to_room("The world seems a little cleaner.\n\r", ch->in_room);

		for (vict = world[ch->in_room].people; vict; vict = next_v) {
			next_v = vict->next_in_room;
			if (IS_NPC(vict)) {
				DEBUG_LOG("act.wizard.c purge(%s)", vict->player.name);
				extract_char(vict, TRUE);
			}
		}

		for (obj = world[ch->in_room].contents; obj; obj = next_o) {
			next_o = obj->next_content;
			extract_obj(obj);
		}
		return;
	}

	// 대상 찾기 - char
	if ((vict = get_char_room_vis(ch, name))) {
		if (!IS_NPC(vict)) {
			if (GET_LEVEL(ch) < GET_LEVEL(vict)) {
				snprintf(buf, sizeof(buf), "%s tried to purge you.\n\r", ch->player.name);
				send_to_char(buf, vict);
				send_to_char("You cannot purge someone of a higher level.\n\r", ch); // 사용자에게 피드백
				return;
			}

			// 2단계 확인 절차
			if (str_cmp(confirm, "confirm") != 0) {
				snprintf(buf, sizeof(buf), "WARNING: You are about to PERMANENTLY DELETE player '%s'.\n\r", vict->player.name);
				send_to_char(buf, ch);
				send_to_char("This action CANNOT be undone.\n\r", ch);
				send_to_char("To proceed, type: purge <player_name> confirm\n\r", ch);
				return;
			}
			
			// "confirm"이 입력된 경우, 플레이어 삭제 절차 진행
			snprintf(buf, sizeof(buf), "PURGE: %s permanently deleted player %s.", ch->player.name, vict->player.name);
			log(buf); // 로그 기록

			stash_char(vict);
			move_stashfile_safe(vict->player.name); 
			for (i = 0; i < MAX_WEAR; i++)
				if (vict->equipment[i]) {
					extract_obj(unequip_char(vict, i));
					vict->equipment[i] = 0;
				}
			wipe_obj(vict->carrying);
			vict->carrying = 0;
			if (vict->desc)
				close_socket(vict->desc);
			extract_char(vict, TRUE);
			send_to_char("Player purged.\n\r", ch); // 실행자에게 완료 알림
			return;
		}
		
		// 대상이 NPC인 경우
		act("$n disintegrates $N.", FALSE, ch, 0, vict, TO_NOTVICT);
		if (IS_NPC(vict)) {
			extract_char(vict, TRUE);
		}
	}

	if (*name) {
		if ((obj = get_obj_in_list_vis(ch, name, world[ch->in_room].contents))) {
			act("$n destroys $p.", FALSE, ch, obj, 0, TO_ROOM);
			extract_obj(obj);
		}
		else {
			send_to_char("I don't know anyone or anything by that name.\n\r", ch);
			return;
		}
		send_to_char("Ok.\n\r", ch);
	}
}

void do_reroll(struct char_data *ch, char *argument, int cmd)
{
	struct char_data *victim;
	char buf[100];

	if (IS_NPC(ch) || GET_LEVEL(ch) > (IMO + 3))
		return;

	one_argument(argument, buf);
	if (!*buf)
		send_to_char("Who do you wish to reroll?\n\r", ch);
	else if (!(victim = get_char(buf)))
		send_to_char("No-one by that name in the world.\n\r", ch);
	else {
		roll_abilities(victim);
		snprintf(buf, sizeof(buf), "Rerolled: %d/%d %d %d %d %d\n", victim->abilities.str,
			victim->abilities.str_add,
			victim->abilities.intel,
			victim->abilities.wis,
			victim->abilities.dex,
			victim->abilities.con);
		send_to_char(buf, ch);
	}
}

void do_restore(struct char_data *ch, char *argument, int cmd)
{
	struct char_data *victim;
	char buf[100];
	int i;

	void update_pos(struct char_data *victim);

	if (IS_NPC(ch) || GET_LEVEL(ch) > (IMO + 3))
		return;
	one_argument(argument, buf);
	if (!*buf)
		send_to_char("Who do you wish to restore?\n\r", ch);
	else if (!(victim = get_char_vis(ch, buf)))
		send_to_char("No-one by that name in the world.\n\r", ch);
	else {
		GET_MANA(victim) = GET_PLAYER_MAX_MANA(victim);
		GET_HIT(victim) = GET_PLAYER_MAX_HIT(victim);
		GET_MOVE(victim) = GET_PLAYER_MAX_MOVE(victim);

		if (GET_LEVEL(victim) >= IMO) {
			for (i = 0; i < MAX_SKILLS; i++) {
				victim->skills[i].learned = 100;
				victim->skills[i].recognise = 0;
			}

			if (GET_LEVEL(victim) >= (IMO + 3)) {
				victim->abilities.str_add = 100;
				victim->abilities.intel = 25;
				victim->abilities.wis = 25;
				victim->abilities.dex = 25;
				victim->abilities.str = 25;
				victim->abilities.con = 25;
			}
			victim->tmpabilities = victim->abilities;

		}
		update_pos(victim);
		send_to_char("Done.\n\r", ch);
		act("You have been fully healed by $N!", FALSE, victim, 0, ch, TO_CHAR);
	}
}

void do_return(struct char_data *ch, char *argument, int cmd)
{
	if (!ch->desc)
		return;

	if (!ch->desc->original) {
		send_to_char("Eh?\n\r", ch);
		return;
	} else {
		send_to_char("You return to your originaly body.\n\r", ch);

		ch->desc->character = ch->desc->original;
		ch->desc->original = 0;

		ch->desc->character->desc = ch->desc;
		ch->desc = 0;
	}
}

/* modified by Komo */
void do_set(struct char_data *ch, char *argument, int cmd)
{
	struct char_data *victim;
	char mess[MAX_STRING_LENGTH] = {0};
    char buf[MAX_INPUT_LENGTH] = {0};
    char buf2[MAX_INPUT_LENGTH] = {0};
    char buf3[MAX_INPUT_LENGTH] = {0};
    char buf4[MAX_INPUT_LENGTH] = {0};
    char buf5[MAX_INPUT_LENGTH] = {0};
    char buf6[MAX_INPUT_LENGTH] = {0};
	int i, j;
	unsigned int k;
	LONGLONG kk;

	bool target_ok = FALSE;
	struct obj_data *tar_obj = NULL;

	if (IS_NPC(ch))
		return;
	if (GET_LEVEL(ch) < (IMO + 3))
		return;

	half_chop(argument, buf, buf2);
	if (!*buf) {
        send_to_char("&WUsage: set <option> <value>       &n  (for global settings)\n\r", ch);
        send_to_char("&W       set <target> <stat> <value>&n\n\r\n\r", ch);
        send_to_char("============= Current Global Settings =============\n\r", ch);

        snprintf(mess, sizeof(mess),
                 " nokill: %-3d   nosteal: %-3d   freeze: %-3d\n\r"
                 " nochat: %-3d   noshout: %-3d   nodisarm: %-3d\n\r"
                 " regen_percent: %-3d%%  regen_time: %4d  regen_time_percent: %d%%\n\r"
                 " reboot_time: %ld (seconds after boot)\n\r"
                 " baddomain: %s\n\r",
                 nokillflag, nostealflag, nonewplayers,
                 nochatflag, noshoutflag, nodisarmflag,
                 regen_percent, regen_time, regen_time_percent, reboot_time,
				 *baddomain[0] ? baddomain[0] : "<NONE>");
        send_to_char(mess, ch);
        
        send_to_char("\n\r------- Options for Character Settings -------\n\r", ch);
        send_to_char(" exp, lev, hit, mana, move, ac, hr, dr, quest, pra\n\r", ch);
		send_to_char(" str, str_add, wis, int, dex, con, handno, handsize,\n\r", ch);
		send_to_char(" remortal, hunger, thirsty, drunk, align, bank, gold, \n\r", ch);
        send_to_char(" skill (all), recognise (all), class, sex, age, passwd.\n\r", ch);
        
        send_to_char("\n\r------- Options for Object Settings -------\n\r", ch);
        send_to_char(" value1 ~ value4, weight, type, extra, wear, cost, magic, affect1, affect2.\n\r", ch);
        
        send_to_char("\n\rExample: set nokill 1\n\r", ch);
        send_to_char("Example: set (player_name) lev 40\n\r", ch);
        send_to_char("Example: set (object_name) value2 10\n\r", ch);
		send_to_char("&WSEE ALSO : stat&n\n\r", ch);
        return;
	} else {
		if (strcmp(buf, "nokill") == 0) {
            one_argument(buf2, buf3);
            if (*buf3)
                nokillflag = atoi(buf3);
            snprintf(mess, sizeof(mess), "Global 'nokill' flag set to %d.\n\r", nokillflag);
            send_to_char(mess, ch);
            return;
        }
        if (strcmp(buf, "baddomain") == 0) {
            one_argument(buf2, buf3);
            if (*buf3) {
                strncpy(baddomain[0], buf3, sizeof(baddomain[0]) - 1);
				baddomain[0][sizeof(baddomain[0]) - 1] = '\0';
            }
            snprintf(mess, sizeof(mess), "Global 'baddomain' is now '%s'.\n\r", baddomain[0]);
            send_to_char(mess, ch);
            return;
        }
        if (strcmp(buf, "nosteal") == 0) {
            one_argument(buf2, buf3);
            if (*buf3)
                nostealflag = atoi(buf3);
            snprintf(mess, sizeof(mess), "Global 'nosteal' flag set to %d.\n\r", nostealflag);
            send_to_char(mess, ch);
            return;
        }
        if (strcmp(buf, "nodisarm") == 0) {
            one_argument(buf2, buf3);
            if (*buf3)
                nodisarmflag = atoi(buf3);
            snprintf(mess, sizeof(mess), "Global 'nodisarm' flag set to %d.\n\r", nodisarmflag);
            send_to_char(mess, ch);
            return;
        }
        if (strcmp(buf, "freeze") == 0) {
            one_argument(buf2, buf3);
            if (*buf3)
                nonewplayers = atoi(buf3);
            snprintf(mess, sizeof(mess), "Global 'freeze' (nonewplayers) flag set to %d.\n\r", nonewplayers);
            send_to_char(mess, ch);
            return;
        }
        if (strcmp(buf, "noshout") == 0) {
            one_argument(buf2, buf3);
            if (*buf3)
                noshoutflag = atoi(buf3);
            snprintf(mess, sizeof(mess), "Global 'noshout' flag set to %d.\n\r", noshoutflag);
            send_to_char(mess, ch);
            return;
        }
        if (strcmp(buf, "nochat") == 0) {
            one_argument(buf2, buf3);
            if (*buf3)
                nochatflag = atoi(buf3);
            snprintf(mess, sizeof(mess), "Global 'nochat' flag set to %d.\n\r", nochatflag);
            send_to_char(mess, ch);
            return;
        }
        if (strcmp(buf, "regen_percent") == 0) {
            one_argument(buf2, buf3);
            if (*buf3)
                regen_percent = atoi(buf3);
            snprintf(mess, sizeof(mess), "Global 'regen_percent' set to %d%%.\n\r", regen_percent);
            send_to_char(mess, ch);
            return;
        }
        if (strcmp(buf, "regen_time") == 0) {
            one_argument(buf2, buf3);
            if (*buf3)
                regen_time = atoi(buf3);
            snprintf(mess, sizeof(mess), "Global 'regen_time' (equipment) set to %d.\n\r", regen_time);
            send_to_char(mess, ch);
            return;
        }
        if (strcmp(buf, "regen_time_percent") == 0) {
            one_argument(buf2, buf3);
            if (*buf3)
                regen_time_percent = atoi(buf3);
            snprintf(mess, sizeof(mess), "Global 'regen_time_percent' set to %d%%.\n\r", regen_time_percent);
            send_to_char(mess, ch);
            return;
        }
        if (strcmp(buf, "reboot_time") == 0) {
            one_argument(buf2, buf3);
            if (*buf3)
                reboot_time = atoi(buf3);
            snprintf(mess, sizeof(mess), "Global 'reboot_time' set to %ld seconds after boot.\n\r", reboot_time);
            send_to_char(mess, ch);
            return;
        }

		/* obj */
		if (!(victim = get_char(buf))) {
			if (!target_ok)
                if ((tar_obj = get_obj_in_list_vis(ch, buf, ch->carrying)))
                    target_ok = TRUE;
            if (!target_ok)
                if ((tar_obj = get_obj_in_list_vis(ch, buf, world[ch->in_room].contents)))
                    target_ok = TRUE;
            if (!target_ok)
                if ((tar_obj = get_obj_vis(ch, buf)))
                    target_ok = TRUE;
            if (!target_ok)
                for (i = 0; i < MAX_WEAR && !target_ok; i++)
                    if (ch->equipment[i] && str_cmp(buf, ch->equipment[i]->name) == 0) {
                        tar_obj = ch->equipment[i];
                        target_ok = TRUE;
                    }

			if (!target_ok) {
				snprintf(mess, sizeof(mess), "Could not find a character or object named '%s'.\n\r", buf);
                send_to_char(mess, ch);
				return;
			}

			half_chop(buf2, buf3, buf4); /* buf3 = stat, buf4 = value */
			k = atoi(buf4);
			if (GET_LEVEL(ch) < (IMO + 3))
				return;
			if (strcmp(buf3, "value1") == 0)
				tar_obj->obj_flags.value[0] = k;
			else if (strcmp(buf3, "value2") == 0)
				tar_obj->obj_flags.value[1] = k;
			else if (strcmp(buf3, "value3") == 0)
				tar_obj->obj_flags.value[2] = k;
			else if (strcmp(buf3, "value4") == 0)
				tar_obj->obj_flags.value[3] = k;
			else if (strcmp(buf3, "weight") == 0)
				tar_obj->obj_flags.weight = k;
			else if (strcmp(buf3, "type") == 0)
				tar_obj->obj_flags.type_flag = k;
			else if (strcmp(buf3, "extra") == 0)
				tar_obj->obj_flags.extra_flags = k;
			else if (strcmp(buf3, "wear") == 0)
				tar_obj->obj_flags.wear_flags = k;
			else if (strcmp(buf3, "cost") == 0)
				tar_obj->obj_flags.cost = k;
			else if (strcmp(buf3, "magic") == 0)
				tar_obj->obj_flags.gpd = k;
			else if (strcmp(buf3, "affect1") == 0) {
				half_chop(buf4, buf5, buf6); /* buf4=value -> "location modifier" */
				k = atoi(buf5);
				j = atoi(buf6);
				tar_obj->affected[0].location = k;
				tar_obj->affected[0].modifier = j;
			} else if (strcmp(buf3, "affect2") == 0) {
				half_chop(buf4, buf5, buf6);
				k = atoi(buf5);
				j = atoi(buf6);
				tar_obj->affected[1].location = k;
				tar_obj->affected[1].modifier = j;
			} else {
				snprintf(mess, sizeof(mess), "Unknown property '%s' for object '%s'.\n\r", buf3, tar_obj->short_description);
                send_to_char(mess, ch);
                return; /* "Huh?" 대신 함수 종료 */
			}
			snprintf(mess, sizeof(mess), "Successfully set '%s' on object '%s'.\n\r", buf3, tar_obj->short_description);
            send_to_char(mess, ch);
		} 

		/* char */
		else {
			half_chop(buf2, buf3, buf4); /* buf3 = stat, buf4 = value */
			k = atoi(buf4);
			kk = atoll(buf4); /* exp, gold, bank용 */

			if ((GET_LEVEL(ch) < (IMO + 2)) && (strcmp(buf3, "gold")))
				return;

			if (strcmp(buf3, "exp") == 0 && (GET_LEVEL(ch) > (IMO + 2)))
                victim->points.exp = kk;
            else if (strcmp(buf3, "skill") == 0) {
                for (i = 0; i < MAX_SKILLS; i++)
                    victim->skills[i].learned = k;
            }
            else if (strcmp(buf3, "recognise") == 0) {
                for (i = 0; i < MAX_SKILLS; i++)
                    victim->skills[i].recognise = k;
            }
            else if (strcmp(buf3, "lev") == 0)
                GET_LEVEL(victim) = k;
            else if (strcmp("hit", buf3) == 0)
                victim->points.hit = victim->points.max_hit = k;
            else if (strcmp("age", buf3) == 0) {
                victim->player.time.birth = time(0);
                victim->player.time.played = 0;
                victim->player.time.logon = time(0);
            }
            else if (strcmp("mana", buf3) == 0)
                victim->points.mana = victim->points.max_mana = k;
            else if (strcmp("move", buf3) == 0)
                victim->points.move = victim->points.max_move = k;
            else if (strcmp("bank", buf3) == 0)
                victim->bank = kk;
            else if (strcmp("gold", buf3) == 0)
                victim->points.gold = kk;
            else if (strcmp("align", buf3) == 0)
                victim->specials.alignment = k;
            else if (strcmp("str", buf3) == 0)
                victim->abilities.str = k;
            else if (strcmp("str_add", buf3) == 0)
                victim->abilities.str_add = k;
            else if (strcmp("dex", buf3) == 0)
                victim->abilities.dex = k;
            else if (strcmp("wis", buf3) == 0)
                victim->abilities.wis = k;
            else if (strcmp("con", buf3) == 0)
                victim->abilities.con = k;
            else if (strcmp("int", buf3) == 0)
                victim->abilities.intel = k;
            else if (strcmp("pra", buf3) == 0)
                victim->specials.spells_to_learn = k;
            else if (strcmp("hunger", buf3) == 0)
                victim->specials.conditions[FULL] = k;
            else if (strcmp("thirsty", buf3) == 0)
                victim->specials.conditions[THIRST] = k;
            else if (strcmp("drunk", buf3) == 0)
                victim->specials.conditions[0] = k;
            else if (strcmp("dr", buf3) == 0)
                victim->points.damroll = k;
            else if (strcmp("hr", buf3) == 0)
                victim->points.hitroll = k;
            else if (strcmp("handno", buf3) == 0)
                victim->specials.damnodice = k;
            else if (strcmp("handsize", buf3) == 0)
                victim->specials.damsizedice = k;
            else if (strcmp("quest", buf3) == 0)
                victim->quest.solved = k;
            else if (strcmp("sex", buf3) == 0) {
                if (strncmp("m", buf4, 1) == 0 || strncmp("M", buf4, 1) == 0)
                    victim->player.sex = 1;
                else if (strncmp("f", buf4, 1) == 0 || strncmp("F", buf4, 1) == 0)
                    victim->player.sex = 2;
                else
                    send_to_char("Invalid sex (m/f).\n\r", ch);
            }
            else if (strcmp("ac", buf3) == 0)
                victim->points.armor = k;
            else if (strcmp("remortal", buf3) == 0) {
                if (strncmp("w", buf4, 1) == 0 || strncmp("W", buf4, 1) == 0) {
                    victim->player.remortal |= REMORTAL_WARRIOR;
                    send_to_char("REMORTAL_WARRIOR set.\n\r", ch);
                } else if (strncmp("c", buf4, 1) == 0 || strncmp("C", buf4, 1) == 0) {
                    victim->player.remortal |= REMORTAL_CLERIC;
                    send_to_char("REMORTAL_CLERIC set.\n\r", ch);
                } else if (strncmp("m", buf4, 1) == 0 || strncmp("M", buf4, 1) == 0) {
                    victim->player.remortal |= REMORTAL_MAGIC_USER;
                    send_to_char("REMORTAL_MAGIC_USER set.\n\r", ch);
                } else if (strncmp("t", buf4, 1) == 0 || strncmp("T", buf4, 1) == 0) {
                    victim->player.remortal |= REMORTAL_THIEF;
                    send_to_char("REMORTAL_THIEF set.\n\r", ch);
                } else {
                    victim->player.remortal = 0;
                    send_to_char("Invalid class. remortal is resetted.\n\r", ch);
                }
            }
            else if (strcmp("class", buf3) == 0) {
                if (strncmp("w", buf4, 1) == 0 || strncmp("W", buf4, 1) == 0)
                    victim->player.class = 4;
                else if (strncmp("c", buf4, 1) == 0 || strncmp("C", buf4, 1) == 0)
                    victim->player.class = 2;
                else if (strncmp("m", buf4, 1) == 0 || strncmp("M", buf4, 1) == 0)
                    victim->player.class = 1;
                else if (strncmp("t", buf4, 1) == 0 || strncmp("T", buf4, 1) == 0)
                    victim->player.class = 3;
                else
                    send_to_char("Invalid class (w/c/m/t).\n\r", ch);
            }
			else if (strcmp("passwd", buf3) == 0) {
				if (!victim->desc) {
                    send_to_char("You can only set passwords on players who are currently online.\n\r", ch);
                    return;
                }
                
                strncpy(victim->desc->pwd, buf4, sizeof(victim->desc->pwd) - 1); /* by Komo */
                victim->desc->pwd[sizeof(victim->desc->pwd) - 1] = '\0';
                
                send_to_char("Password changed. (Note: Password was set in PLAINTEXT).\n\r", ch);
		        save_char(ch, world[ch->in_room].number);
			}
			else {
                snprintf(mess, sizeof(mess), "Unknown property '%s' for character '%s'.\n\r", buf3, GET_NAME(victim));
                send_to_char(mess, ch);
                return; /* "Huh?" 대신 함수 종료 */
            }
            
            snprintf(mess, sizeof(mess), "Successfully set '%s' on character '%s'.\n\r", buf3, GET_NAME(victim));
            send_to_char(mess, ch);

            victim->tmpabilities = victim->abilities;
		}
	}
}

void do_shutdow(struct char_data *ch, char *argument, int cmd)
{
	send_to_char("If you want to shut something down - say so!\n\r", ch);
}

void do_shutdown(struct char_data *ch, char *argument, int cmd)
{
	char arg[MAX_INPUT_LENGTH];

	if (IS_NPC(ch) || GET_LEVEL(ch) > (IMO + 3))
		return;
	one_argument(argument, arg);
	if (!*arg) {
		send_to_all("Shutting down immediately.\n\r");
		shutdowngame = 1;
	} else
		send_to_char("Go shut down someone your own size.\n\r", ch);
}
void do_snoop(struct char_data *ch, char *argument, int cmd)
{
	char arg[MAX_STRING_LENGTH];
	struct char_data *victim;
	int diff;

	if (!ch->desc)
		return;
	if (IS_NPC(ch))
		return;
	one_argument(argument, arg);
	if (!*arg) {
		victim = ch;
	} else if (!(victim = get_char_vis(ch, arg))) {
		send_to_char("No such person around.\n\r", ch);
		return;
	}
	if (!victim->desc) {
		send_to_char("There's no link.. nothing to snoop.\n\r", ch);
		return;
	}
	if (victim == ch) {
		send_to_char("Ok, you just snoop yourself.\n\r", ch);
		if (ch->desc->snoop.snooping) {
			ch->desc->snoop.snooping->desc->snoop.snoop_by = 0;
			ch->desc->snoop.snooping = 0;
		}
		return;
	}
	if (victim->desc->snoop.snoop_by) {
		send_to_char("Busy already. \n\r", ch);
		return;
	}
	diff = GET_LEVEL(victim) - GET_LEVEL(ch);
	if (diff >= 0) {
		send_to_char("You failed.\n\r", ch);
		return;
	}
	send_to_char("Ok. \n\r", ch);
	if (ch->desc->snoop.snooping)
		ch->desc->snoop.snooping->desc->snoop.snoop_by = 0;
	ch->desc->snoop.snooping = victim;
	victim->desc->snoop.snoop_by = ch;
	return;
}

void do_start(struct char_data *ch)
{
	void advance_level(struct char_data * ch, int level_up);

	GET_LEVEL(ch) = 1;
	GET_EXP(ch) = 1;
	set_title(ch);
	advance_level(ch, 1);

	GET_HIT(ch) = GET_PLAYER_MAX_HIT(ch);
	GET_MANA(ch) = GET_PLAYER_MAX_MANA(ch);
	GET_MOVE(ch) = GET_PLAYER_MAX_MOVE(ch);

	GET_COND(ch, THIRST) = 24;
	GET_COND(ch, FULL) = 24;
	GET_COND(ch, DRUNK) = 0;

	ch->specials.spells_to_learn = 3;
	ch->player.time.played = 0;
	ch->player.time.logon = time(0);
}


/* 
 * do_stat refactoring, 251125 by Komo
 * --- 1. 방 정보 출력 함수 --- 
 */
void perform_stat_room(struct char_data *ch, struct room_data *rm, char *buf, size_t size)
{
    size_t len = 0;
    int i;
    struct extra_descr_data *desc;
    struct char_data *k;
    struct obj_data *j;
    char sbuf[256], sbuf2[256];

    SAFE_PRINTF("Room: %s, Zone: %d. V-Num: %d, R-num: %d, Light: %d.\n\r", 
                rm->name, rm->zone, rm->number, ch->in_room, rm->light);

    sprinttype(rm->sector_type, sector_types, sbuf2);
    SAFE_PRINTF("Sector type : %s\n\r", sbuf2);
    
    SAFE_PRINTF("Special procedure : %s", (rm->funct) ? "Exists\n\r" : "No\n\r");

    sprintbit((long)rm->room_flags, room_bits, sbuf);
    SAFE_PRINTF("Room flags: %s\n\r", sbuf);

    SAFE_PRINTF("Description:\n\r%s", rm->description);

    SAFE_PRINTF("Extra description keywords(s): ");
    if (rm->ex_description) {
        SAFE_PRINTF("\n\r");
        for (desc = rm->ex_description; desc; desc = desc->next)
            SAFE_PRINTF("%s\n\r", desc->keyword);
        SAFE_PRINTF("\n\r");
    } else {
        SAFE_PRINTF("None\n\r");
    }

    SAFE_PRINTF("Chars present:\n\r");
    for (k = rm->people; k; k = k->next_in_room) {
        if (CAN_SEE(ch, k)) {
            SAFE_PRINTF("%s%s", GET_NAME(k),
                (!IS_NPC(k) ? "(PC)\n\r" : (!IS_MOB(k) ? "(NPC)\n\r" : "(MOB)\n\r")));
        }
    }
    SAFE_PRINTF("\n\r");

    SAFE_PRINTF("Contents:\n\r");
    for (j = rm->contents; j; j = j->next_content) {
        SAFE_PRINTF("%s\n\r", j->name);
    }
    SAFE_PRINTF("\n\r");

    SAFE_PRINTF("Exits:\n\r");
    for (i = 0; i <= 5; i++) {
        if (rm->dir_option[i]) {
            SAFE_PRINTF("Direction %s. Keyword : %s\n\r", dirs[i], rm->dir_option[i]->keyword);
            SAFE_PRINTF("Description:\n\r  %s", 
                rm->dir_option[i]->general_description ? rm->dir_option[i]->general_description : "UNDEFINED\n\r");
            
            sprintbit(rm->dir_option[i]->exit_info, exit_bits, sbuf2);
            SAFE_PRINTF("Exit flag: %s\n\rKey #: %d\n\r", sbuf2, rm->dir_option[i]->key);
            SAFE_PRINTF("To room(R-Num): %d\n\r", rm->dir_option[i]->to_room);
        }
    }
}

/* 
 * do_stat refactoring, 251125 by Komo
 * --- 2. 아이템 정보 출력 함수
 */
void perform_stat_object(struct char_data *ch, struct obj_data *j, char *buf, size_t size)
{
    size_t len = 0;
    int i, virtual;
    struct extra_descr_data *desc;
    char sbuf[256], sbuf2[256];
    
    virtual = (j->item_number >= 0) ? obj_index[j->item_number].virtual : 0;

    SAFE_PRINTF("Object name: [%s], R-num: [%d], V-number: [%d]\n\r", j->name, j->item_number, virtual);
    
    sprinttype(GET_ITEM_TYPE(j), item_types, sbuf2);
    SAFE_PRINTF("Item type: %s\n\r", sbuf2);

    SAFE_PRINTF("Short desc: %s\n\rLong desc:\n\r%s\n\r",
                ((j->short_description) ? j->short_description : "None"),
                ((j->description) ? j->description : "None"));

    SAFE_PRINTF("\n\rExtra desc keyword(s):\n\r");
    if (j->ex_description) {
        for (desc = j->ex_description; desc; desc = desc->next)
            SAFE_PRINTF("%s\n\r", desc->keyword);
    } else {
        SAFE_PRINTF("None\n\r");
    }

    sprintbit(j->obj_flags.wear_flags, wear_bits, sbuf);
    SAFE_PRINTF("\n\rCan be worn on : %s\n\r", sbuf);

    sprintbit(j->obj_flags.extra_flags, extra_bits, sbuf);
    SAFE_PRINTF("Extra flags: %s\n\r\n\r", sbuf);

    SAFE_PRINTF("Weight: %d, Value: %d, Timer: %d\n\r", j->obj_flags.weight, j->obj_flags.cost, j->obj_flags.timer);
    SAFE_PRINTF("Values 0-3 : [%d] [%d] [%d] [%d]\n\r",
                j->obj_flags.value[0], j->obj_flags.value[1],
                j->obj_flags.value[2], j->obj_flags.value[3]);

    SAFE_PRINTF("Can affect char :\n\r");
    for (i = 0; i < MAX_OBJ_AFFECT; ++i) {
        sprinttype(j->affected[i].location, apply_types, sbuf2);
        SAFE_PRINTF("  Affects: %s by %d\n\r", sbuf2, j->affected[i].modifier);
    }
}

/* 
 * do_stat refactoring, 251125 by Komo
 * --- 3. 캐릭터 정보 출력 함수 --- 
 * */
void perform_stat_char(struct char_data *ch, struct char_data *k, char *buf, size_t size)
{
    size_t len = 0;
    int i, i2, t;
    struct follow_type *fol;
    struct affected_type *aff;
    char sbuf[256], sbuf2[256];
    struct obj_data *j;

    SAFE_PRINTF("Sex: %d, %s, Name: %s\n\r", 
        k->player.sex, (!IS_NPC(k) ? "PC" : (!IS_MOB(k) ? "NPC" : "MOB")), GET_NAME(k));

    if (IS_NPC(k)) {
        SAFE_PRINTF("V-Number [%d]\n\r", mob_index[k->nr].virtual);
        SAFE_PRINTF("Short desc: %s\n\r", (k->player.short_descr ? k->player.short_descr : "None"));
        SAFE_PRINTF("Long desc: %s", (k->player.long_descr ? k->player.long_descr : "None.\n\r"));
    } else {
        SAFE_PRINTF("Title: %s\n\r", (k->player.title ? k->player.title : "None"));
    }

    sprinttype(k->player.class, pc_class_types, sbuf2);
    SAFE_PRINTF("Class: %s  Level [%d] Alignment[%d]\n\r", sbuf2, k->player.level, k->specials.alignment);

    /* 시간 출력 안전하게 처리 */
    t = k->player.time.birth;
    SAFE_PRINTF("Birth: %.24s, Logon: ", ctime((time_t *)&t));
    t = k->player.time.logon;
    SAFE_PRINTF("%.24s", ctime((time_t *)&t));

    t = k->player.time.played;
    i = t % 86400;
    SAFE_PRINTF("\n\rPlayed: %d days, %d:%02d\n\r", t / 86400, i / 3600, (i + 30) % 60);

    SAFE_PRINTF("Age: %d Y, %d M, %d D, %d H.  Ht: %d cm, Wt: %d lbs\n\r",
        age(k).year, age(k).month, age(k).day, age(k).hours, GET_HEIGHT(k), GET_WEIGHT(k));

    SAFE_PRINTF("Str:[%d/%d]  Int:[%d]  Wis:[%d]  Dex:[%d]  Con:[%d]\n\r",
                GET_STR(k), GET_ADD(k), GET_INT(k), GET_WIS(k), GET_DEX(k), GET_CON(k));

    SAFE_PRINTF("Mana: %ld/%ld+%d, Hits: %ld/%ld+%d, Moves: %ld/%ld+%d\n\r",
                GET_MANA(k), mana_limit(k), mana_gain(k),
                GET_HIT(k), hit_limit(k), hit_gain(k),
                GET_MOVE(k), move_limit(k), move_gain(k));

    SAFE_PRINTF("AC: %d/10, Hitroll: %d, Damroll: %d, Regen: %d\n\r",
                GET_AC(k), k->points.hitroll, k->points.damroll, k->regeneration);
    
    SAFE_PRINTF("Gold: %lld, Bank: %lld, Exp: %lld\n\r", GET_GOLD(k), k->bank, GET_EXP(k));

    if (IS_NPC(k)) {
        SAFE_PRINTF("Special: %s", (mob_index[k->nr].func ? "Exists\n\r" : "None\n\r"));
    }

    /* Remortal 표시 */
    SAFE_PRINTF("Remortal : ");
    if (k->player.remortal & REMORTAL_MAGIC_USER) SAFE_PRINTF("M");
    if (k->player.remortal & REMORTAL_CLERIC)     SAFE_PRINTF("C");
    if (k->player.remortal & REMORTAL_THIEF)      SAFE_PRINTF("T");
    if (k->player.remortal & REMORTAL_WARRIOR)    SAFE_PRINTF("W");
    SAFE_PRINTF("\n\r");

    SAFE_PRINTF("Bare Hand Damage %dd%d.\n\r", k->specials.damnodice, k->specials.damsizedice);
    SAFE_PRINTF("Carried weight: %d   Carried items: %d\n\r", IS_CARRYING_W(k), IS_CARRYING_N(k));

    for (i = 0, j = k->carrying; j; j = j->next_content, i++);
    SAFE_PRINTF("Items in inv: %d, ", i);

    for (i = 0, i2 = 0; i < MAX_WEAR; i++)
        if (k->equipment[i]) i2++;
    SAFE_PRINTF("Items in equ: %d\n\r", i2);

    if (k->desc) {
        sprinttype(k->desc->connected, connected_types, sbuf2);
        SAFE_PRINTF("Connected: %s %s (%d)\n\r", sbuf2, k->desc->host, k->desc->descriptor);
    }

    if (IS_NPC(k)) {
        sprintbit(k->specials.act, action_bits, sbuf2);
        SAFE_PRINTF("NPC flags: %s\n\r", sbuf2);
    } else {
        sprintbit(k->specials.act, player_bits, sbuf2);
        SAFE_PRINTF("PC flags: %s\n\r", sbuf2);
    }

    if (!IS_NPC(k)) {
        SAFE_PRINTF("Thirst: %d, Hunger: %d, Drunk: %d\n\r",
            k->specials.conditions[THIRST], k->specials.conditions[FULL], k->specials.conditions[DRUNK]);
        SAFE_PRINTF("Practices: %d\n\r", k->specials.spells_to_learn);
    }

    SAFE_PRINTF("Master is '%s'\n\r", ((k->master) ? GET_NAME(k->master) : "NOBODY"));
    
    SAFE_PRINTF("Followers are:\n\r");
    for (fol = k->followers; fol; fol = fol->next) {
        SAFE_PRINTF("  %s", GET_NAME(fol->follower));
    }
    
    sprintbit(k->specials.affected_by, affected_bits, sbuf);
    SAFE_PRINTF("\n\rAffected by: %s\n\r", sbuf);

    if (k->affected) {
        SAFE_PRINTF("Affecting Spells:\n\r");
        for (aff = k->affected; aff; aff = aff->next) {
            const char *spell_name = "Unknown"; /* spells 배열 인덱스 검사 */
            if (aff->type > 0 && aff->type <= MAX_SPL_LIST)
                spell_name = spells[aff->type - 1];
            
            sprintbit(aff->bitvector, affected_bits, sbuf2);
            SAFE_PRINTF("%s: %s by %d, %d hrs, bits: %s\n\r", 
                spell_name, apply_types[(int)aff->location], aff->modifier, aff->duration, sbuf2);
        }
    }
}

/* --- 메인 do_stat 함수 --- */
void do_stat(struct char_data *ch, char *argument, int cmd)
{
    char arg1[MAX_INPUT_LENGTH];
    char page_buffer[16384]; /* page_buffer를 static으로 선언하지 않고, 넉넉하게 16KB 할당 */
    struct room_data *rm = NULL;
    struct char_data *k = NULL;
    struct obj_data *j = NULL;
    struct char_data *get_specific_vis(struct char_data * ch, char *name, int type);

    if (IS_NPC(ch)) return;

    one_argument(argument, arg1);

    /* 도움말 추가 */
    if (!*arg1) {
        send_to_char("&W[STAT] Usage: stat <room | object | character name>&n\n\r", ch);
        send_to_char("&W[STAT]   stat room      : 현재 있는 방의 정보 확인&n\n\r", ch);
        send_to_char("&W[STAT]   stat <name>    : PC/NPC/아이템 정보 확인&n\n\r", ch);
        return;
    }

    /* 초기화 */
    page_buffer[0] = '\0';

    /* ROOM 처리 */
    if ((cmd != 232) && (cmd != 233) && (!str_cmp("room", arg1))) {
        rm = &world[ch->in_room];
        perform_stat_room(ch, rm, page_buffer, sizeof(page_buffer));
    }
    /* OBJECT 처리 */
    else if ((cmd != 232) && (cmd != 233) && (j = get_obj_vis(ch, arg1))) {
        perform_stat_object(ch, j, page_buffer, sizeof(page_buffer));
    }
    /* CHAR 처리 */
    else {
        if (cmd == 233)      k = get_specific_vis(ch, arg1, 0); /* PC only */
        else if (cmd == 232) k = get_specific_vis(ch, arg1, 1); /* Mob only */
        else                 k = get_char_vis(ch, arg1);        /* Both */

        if (k) {
            perform_stat_char(ch, k, page_buffer, sizeof(page_buffer));
        } else {
            send_to_char("[STAT] No mobile or object by that name in the world.\n\r", ch);
            return;
        }
    }

    /* 최종 출력 */
    page_string(ch->desc, page_buffer, 1);
}


void do_switch(struct char_data *ch, char *argument, int cmd)
{
	char arg[MAX_STRING_LENGTH];
	struct char_data *victim;

	if (IS_NPC(ch))
		return;
	one_argument(argument, arg);
	if (!*arg) {
		send_to_char("&W[SWITCH]&n Switch with who?\n\r", ch);
	} else {
		if (!(victim = get_char(arg)))
			send_to_char("&W[SWITCH]&n They aren't here.\n\r", ch);
		else {
			if (ch == victim) {
				send_to_char("&W[SWITCH]&n He he he... We are jolly funny today, eh?\n\r", ch);
				return;
			}
			if (!ch->desc || ch->desc->snoop.snoop_by ||
			    ch->desc->snoop.snooping) {
				send_to_char("&W[SWITCH]&n You can't do that, the body is already in use.\n\r", ch);
				return;
			}
			if (victim->desc || (!IS_NPC(victim))) {
				if (GET_LEVEL(victim) > GET_LEVEL(ch))
					send_to_char("&W[SWITCH]&n They aren't here.\n\r", ch);
				else
					send_to_char("&W[SWITCH]&n You can't do that, the body is already in use!\n\r", ch);
			} else {
				send_to_char("&W[SWITCH]&n Ok.\n\r", ch);
				ch->desc->character = victim;
				ch->desc->original = ch;
				victim->desc = ch->desc;
				ch->desc = 0;
			}
		}
	}
}

void do_sys(struct char_data *ch, char *argument, int cmd)
{
	struct rusage xru;
	char buffer[256];
	struct char_data *i;
	struct obj_data *k;
	struct descriptor_data *d;
	static int nits, nics, nids;

	getrusage(0, &xru);
	snprintf(buffer, sizeof(buffer),
		"sys time: %d secs\n\rusr time: %d secs\n\rrun time: %d secs\n\r",
		(int)xru.ru_stime.tv_sec, (int)xru.ru_utime.tv_sec, (int)(time
									  (0)
									  - boottime));
	send_to_char(buffer, ch);
	if (GET_LEVEL(ch) >= (IMO + 2)) {
		nits = 0;
		for (k = object_list; k; k = k->next)
			++nits;
		nics = 0;
		for (i = character_list; i; i = i->next)
			++nics;
		nids = 0;
		for (d = descriptor_list; d; d = d->next)
			++nids;
	}
	snprintf(buffer, sizeof(buffer), " objects: %d\n\r   chars: %d\n\r players: %d\n\r",
		nits, nics, nids);
	send_to_char(buffer, ch);
}

/* send_to_room_except 대체 및 출력 방식 등 수정, by Komo */
void do_trans(struct char_data *ch, char *argument, int cmd)
{
	struct descriptor_data *i;
	struct char_data *victim;
	char buf[100];
	int target;

	if (IS_NPC(ch))
		return;

	one_argument(argument, buf);
	if (!*buf)
		send_to_char("Who do you wich to transfer?\n\r", ch);
	else if (str_cmp("all", buf)) {
		if (!(victim = get_char_vis(ch, buf)))
			send_to_char("No-one by that name around.\n\r", ch);
		else {
			if (GET_LEVEL(ch) < IMO + 3 && GET_LEVEL(victim) > GET_LEVEL
			    (ch)) {
				send_to_char("That might not be appreciated.\n\r", ch);
				return;
			}
			act("$n disappears in a mushroom cloud.", FALSE,
			    victim, 0, 0, TO_ROOM);
			target = ch->in_room;
			char_from_room(victim);
			char_to_room(victim, target);
			act("$n arrives from a puff of smoke.", FALSE, victim,
			    0, 0, TO_ROOM);
			act("$n has transferred you!", FALSE, ch, 0, victim, TO_VICT);
			do_look(victim, "", 15);
			send_to_char("Ok.\n\r", ch);
		}
	} else {		/* Trans All */
		if (ch->player.level >= (IMO + 3))
			for (i = descriptor_list; i; i = i->next)
				if (i->character != ch && !i->connected) {
					target = ch->in_room;
					victim = i->character;
					char_from_room(victim);
					char_to_room(victim, target);
					act("$n arrives from a puff of smoke.",
					    FALSE, victim, 0, 0, TO_ROOM);
					act("$n has transferred you!", FALSE,
					    ch, 0, victim, TO_VICT);
					do_look(victim, "", 15);
				}
		send_to_char("Ok.\n\r", ch);
	}
}

void do_transform(struct char_data *ch, char *argument, int cmd)
{
	struct char_data *tmp_ch;
	struct char_file_u tmp;
	int i_player;
	char buf[200];

	return;
	if (IS_NPC(ch))
		return;
	one_argument(argument, buf);
	if (*buf)
		return;
	for (tmp_ch = character_list; tmp_ch; tmp_ch = tmp_ch->next) {
		if (!strcmp(buf, GET_NAME(tmp_ch)) && !IS_NPC(tmp_ch)) {
			send_to_char("Body already in game.\n\r", ch);
			return;
		}
	}
	if ((i_player = load_char(buf, &tmp)) < 0) {
		send_to_char("No such player\n\r", ch);
		return;
	}
	store_to_char_for_transform(&tmp, ch->desc->original);
	store_to_char_for_transform(&tmp, ch);
	ch->desc->pos = player_table[i_player].nr;
	unstash_char(ch, 0);
	stash_char(ch);
}

void do_wall(struct char_data *ch, char *argument, int cmd)
{
	char buf[MAX_STRING_LENGTH];

	if (IS_NPC(ch) || (!*argument) || GET_LEVEL(ch) > (IMO + 3))
		return;
	snprintf(buf, sizeof(buf), "%s\n\r", argument + 1);
	send_to_all(buf);
	send_to_char("[WALL] Ok.\n\r", ch);
}

void do_wiznet(struct char_data *ch, char *argument, int cmd)
{
	struct descriptor_data *d;
	struct char_data *victim;
	char buf[MAX_STRING_LENGTH];

	if (IS_NPC(ch))
		return;
	snprintf(buf, sizeof(buf), "[WIZNET]&T %s: %s&n\n\r", ch->player.name, argument);
	send_to_char(buf, ch);

	for (d = descriptor_list; d; d = d->next) {
		if (!d->connected || d->original) continue;
		victim = d->character;
		if(victim == ch) continue;

		if ((GET_LEVEL(victim) >= IMO) && (GET_LEVEL(victim) <= (IMO + 4)))
			send_to_char(buf, victim);
	}
}
// qsort 정렬을 위한 비교 함수 - 내림차순
int compare_to_descending_order(const void *a, const void *b) {
    return (*(ubyte *)b - *(ubyte *)a);
}

/* Give pointers to the five abilities */
// FIXME: db.c t
void roll_abilities(struct char_data *ch)
{
    int i, result;
    ubyte table[5];

    // 능력치 5개 생성
    for (i = 0; i < 5; i++) {
        /* 1~7 사이의 숫자 4개를 더하는 원래의 로직을 dice 함수로 수정(4D7), by Komo */
        result = dice(4, 7);
        
        if (result > 18) result = 18;
        
        table[i] = (ubyte)result;
    }

    // 가장 높은 값이 table[0]에 오도록 내림차순 정렬, qsort here:
    qsort(table, 5, sizeof(ubyte), compare_to_descending_order);

    ch->abilities.str_add = 0;

    // 직업별로 가장 높은 스탯부터 중요도 순으로 할당
    switch (GET_CLASS(ch)) {
        case CLASS_MAGIC_USER:
            ch->abilities.intel = table[0]; // 1순위
            ch->abilities.wis   = table[1];
            ch->abilities.dex   = table[2];
            ch->abilities.str   = table[3];
            ch->abilities.con   = table[4]; // 5순위
            break;

        case CLASS_CLERIC:
            ch->abilities.wis   = table[0];
            ch->abilities.intel = table[1];
            ch->abilities.str   = table[2];
            ch->abilities.dex   = table[3];
            ch->abilities.con   = table[4];
            break;

        case CLASS_THIEF:
            ch->abilities.dex   = table[0];
            ch->abilities.str   = table[1];
            ch->abilities.con   = table[2];
            ch->abilities.intel = table[3];
            ch->abilities.wis   = table[4];
            break;

        case CLASS_WARRIOR:
            ch->abilities.str   = table[0];
            ch->abilities.dex   = table[1];
            ch->abilities.con   = table[2];
            ch->abilities.wis   = table[3];
            ch->abilities.intel = table[4];

            // W이고 힘이 18이면 str_add도 랜덤 부여
            if (ch->abilities.str == 18)
                ch->abilities.str_add = number(0, 100);
            break;
            
        default:
            // 혹시 모를 예외 처리 (W 세팅으로)
            ch->abilities.str = table[0];
            ch->abilities.dex = table[1];
            ch->abilities.con = table[2];
            ch->abilities.wis = table[3];
            ch->abilities.intel = table[4];
            break;
    }

    ch->tmpabilities = ch->abilities;
}


/*
 * zreload 명령어: 특정 존 파일을 다시 로드
 * by Komo
 */
void do_zreload(struct char_data *ch, char *argument, int cmd)
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int target_zone_num;
    int i, zone_rnum = -1;

    one_argument(argument, arg);

    // 인자 확인
    if (!*arg) {
        send_to_char("&c[ZRELOAD]&n &W사용법: zreload <존 번호>&n\r\n"
                     "&c[ZRELOAD]&n &W예시: zreload 30 (미드가르드), zreload 300 (카이스트)&n\r\n", ch);
        return;
    }

    // 숫자 확인
    if (!isdigit(*arg)) {
        send_to_char("&c[ZRELOAD]&n 존 번호는 숫자여야 합니다.\r\n", ch);
        return;
    }

    target_zone_num = atoi(arg);

    for (i = 0; i <= top_of_zone_table; i++) {
        if (zone_table[i].number == target_zone_num) {
            zone_rnum = i; // 찾았다!
            break;
        }
    }

    if (zone_rnum == -1) { // 찾지 못한 경우
        snprintf(buf, sizeof(buf), "&c[ZRELOAD]&n 존 번호 %d번을 찾을 수 없습니다. '%s' 목록을 확인하세요.\r\n", target_zone_num, ALL_ZONE_FILE);
        send_to_char(buf, ch);
        return;
    }

    snprintf(buf, sizeof(buf), "[ZRELOAD] (GC) %s reloaded Zone %d (%s).", 
            GET_NAME(ch), target_zone_num, zone_table[zone_rnum].filename);
    log(buf);

    load_zones(zone_rnum);

    snprintf(buf, sizeof(buf), "&c[ZRELOAD]&n 존 %d번 '%s' (%s) Reload 완료.\r\n", 
            zone_table[zone_rnum].number, zone_table[zone_rnum].name, zone_table[zone_rnum].filename);
    send_to_char(buf, ch);
}


/*
 * wreload 명령어: 특정 wld 파일을 다시 로드
 * --- by Komo
 */
void do_wreload(struct char_data *ch, char *argument, int cmd)
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int target_zone_num, zone_rnum;
    FILE *fl;

    one_argument(argument, arg);

    if (!*arg) {
        send_to_char("&c[WRELOAD]&n 존 번호를 기준으로 특정 wld 파일을 다시 읽어옵니다.\r\n", ch);
		send_to_char("&c[WRELOAD]&n &W사용법: wreload <존 번호>&n\r\n", ch);
        return;
    }

    if (!isdigit(*arg)) {
        send_to_char("&c[WRELOAD]&n 존 번호는 숫자여야 합니다.\r\n", ch);
        return;
    }

    target_zone_num = atoi(arg);
    zone_rnum = real_zone_by_number(target_zone_num);

    if (zone_rnum == -1) {
        send_to_char("&c[WRELOAD]&n 존재하지 않는 존 번호입니다.\r\n", ch);
        return;
    }

    /* .wld 파일이 정의되어 있는지 확인 */
    if (!zone_table[zone_rnum].wld_filename) {
        send_to_char("&c[WRELOAD]&n 이 존에는 연결된 World 파일(.wld)이 없습니다.\r\n", ch);
        return;
    }

    /* 파일 열기 */
    if (!(fl = fopen(zone_table[zone_rnum].wld_filename, "r"))) {
        snprintf(buf, sizeof(buf), "&c[WRELOAD]&n 파일 열기 실패: %s\r\n", zone_table[zone_rnum].wld_filename);
        send_to_char(buf, ch);
        return;
    }

    /* 핵심 로직 호출 */
    reload_world_file(fl, zone_rnum);
    fclose(fl);

    /* 로그 및 메시지 */
    snprintf(buf, sizeof(buf), "[WRELOAD] (GC) %s reloaded World file for Zone %d.", GET_NAME(ch), target_zone_num);
    log(buf);
    
    snprintf(buf, sizeof(buf), "&c[WRELOAD]&n 존 %d번 월드 데이터(%s) 업데이트 완료.\r\n"
				 "&c[WRELOAD]&n 동기화를 위해 zreload 실행을 권장합니다.\r\n"
                 "&c[WRELOAD]&n 주의: 새로 추가된 방은 반영되지 않으며, 기존 방의 정보만 갱신되었습니다.\r\n", 
            target_zone_num, zone_table[zone_rnum].wld_filename);
    send_to_char(buf, ch);
}


void do_zonelist(struct char_data *ch, char *argument, int cmd)
{
    int i;
    char *buf;            /* 동적 할당할 큰 버퍼 포인터 */
    char wld_status[64];
    char zon_name_fmt[64];
    char zon_file_display[64];
    
    size_t len = 0;       /* 현재 버퍼에 채워진 길이 */
    size_t buf_size = 65536; /* 64KB */

    CREATE(buf, char, buf_size);

    /* 헤더 출력 */
    len += snprintf(buf + len, buf_size - len,
        " IDX    VNUM    TOP   %-30s %-25s %-25s\r\n"
        "---------------------------------------------------------------------------------------------------------\r\n",
        "Zone Name", "Zone File", "World File");

    for (i = 0; i <= top_of_zone_table; i++) {
        
        snprintf(zon_name_fmt, sizeof(zon_name_fmt), "%-30.30s", zone_table[i].name ? zone_table[i].name : "<No Name>");

        if (zone_table[i].wld_filename && *zone_table[i].wld_filename) {
            char *fname = strrchr(zone_table[i].wld_filename, '/');
            snprintf(wld_status, sizeof(wld_status), "%-25.25s", fname ? fname + 1 : zone_table[i].wld_filename);
        } else {
            snprintf(wld_status, sizeof(wld_status), "%-25s", "---");
        }

        char *zname = strrchr(zone_table[i].filename, '/');
        snprintf(zon_file_display, sizeof(zon_file_display), "%-25.25s", zname ? zname + 1 : zone_table[i].filename);

        len += snprintf(buf + len, buf_size - len, 
            "[%3d] [%5d] [%5d] %s %s %s\r\n",
            i,
            zone_table[i].number,
            zone_table[i].top,
            zon_name_fmt,
            zon_file_display,
            wld_status
        );

        if (len >= buf_size - 256) {
            strcat(buf, "** LIST TRUNCATED (Buffer really full) **\r\n");
            break;
        }
    }

    page_string(ch->desc, buf, 1);

    free(buf);
}
