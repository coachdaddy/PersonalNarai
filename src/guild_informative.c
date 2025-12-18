/* ************************************************************************
*  file: guild_informative.c , Implementation of guild commands.  		*
*  Usage : guild Informative commands.					  *
*  Made by Shin Won-dong  in KAIST                                        *
************************************************************************* */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <assert.h>

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "spells.h"
#include "limit.h"
#include "guild_list.h"		/* process */


void do_cant(struct char_data *ch, char *argument, int cmd)
{

	struct descriptor_data *i;
	struct char_data *victim;
	char buf[100];

	int j;

	if (IS_NPC(ch))
		return;
	if (IS_SET(ch->specials.act, PLR_DUMB_BY_WIZ) && GET_LEVEL(ch) < IMO +
	    3) {
		return;
	}
	if (ch->player.guild < 1 || ch->player.guild > MAX_GUILD_LIST) {
		send_to_char("You must join any guild first.\n\r", ch);
		return;
	}
	if (strcmp(argument, " /who") == 0) {
		j = 0;
		for (i = descriptor_list; i; i = i->next)
			if (!i->connected) {
				if (i->original)
					continue;
				victim = i->character;
				if (ch->player.guild == victim->player.guild
				    && GET_LEVEL
				    (victim) < IMO) {
					j++;
					snprintf(buf, sizeof(buf),
						"<%2d> %s <%5ld,%5ld,%5ld> %s \n\r",
						GET_LEVEL(victim),
						GET_NAME(victim),
						GET_PLAYER_MAX_HIT(victim),
						GET_PLAYER_MAX_MANA(victim),
						GET_PLAYER_MAX_MOVE(victim),
						world[victim->in_room].name);
					send_to_char(buf, ch);
				}
			}
		snprintf(buf, sizeof(buf), "You can see %d member(s) of %s guild.\n\r",
			j, guild_names[(int)ch->player.guild]);
		send_to_char(buf, ch);
	} else {
		snprintf(buf, sizeof(buf),
			"(%s) %s >>> %s\n\r",
			guild_names[(int)ch->player.guild],
			ch->player.name,
			argument);

		for (i = descriptor_list; i; i = i->next) {
			if (!i->connected) {
				if (i->original)
					continue;
				victim = i->character;
				if (ch->player.guild == victim->player.guild
				    || GET_LEVEL
				    (victim) > IMO) {
					send_to_char(buf, victim);
				}
			}
		}
	}
}

void do_query(struct char_data *ch, char *argument, int cmd)
{
	struct char_data *victim;
	char victim_name[240];
	char buf[MAX_STRING_LENGTH];

	one_argument(argument, victim_name);

	if (!(victim = get_char_room_vis(ch, victim_name))) {
		send_to_char("QUERY who?\n\r", ch);
		return;
	} else if (victim == ch) {
		send_to_char("The better idea is to type \"sc\".\n\r", ch);
		return;
	}
	if (victim->player.guild >= 0 && victim->player.guild <= MAX_GUILD_LIST) {
		snprintf(buf, sizeof(buf),
			"%s(%d) is a member of %s guild\n\r",
			GET_NAME(victim),
			GET_LEVEL(victim),
			guild_names[(int)victim->player.guild]);
		send_to_char(buf, ch);
	}

	/* modified by ares */
	if (GET_LEVEL(ch) >= GET_LEVEL(victim))
		snprintf(buf, sizeof(buf), "%s's hit is %ld.\n\r", GET_NAME(victim),
			GET_HIT(victim));
	else if (GET_HIT(victim) > GET_HIT(ch))
		snprintf(buf, sizeof(buf), "%s's hit is higher than you.\n\r", GET_NAME(victim));
	else
		snprintf(buf, sizeof(buf), "%s's hit is lower than you.\n\r", GET_NAME(victim));
	send_to_char(buf, ch);

	if (GET_LEVEL(ch) >= GET_LEVEL(victim))
		snprintf(buf, sizeof(buf), "%s's mana is %ld.\n\r", GET_NAME(victim),
			GET_MANA(victim));
	else if (GET_MANA(victim) > GET_MANA(ch))
		snprintf(buf, sizeof(buf), "%s's mana is higher than you.\n\r", GET_NAME(victim));
	else
		snprintf(buf, sizeof(buf), "%s's mana is lower than you.\n\r", GET_NAME(victim));
	send_to_char(buf, ch);

	if (GET_LEVEL(ch) >= GET_LEVEL(victim))
		snprintf(buf, sizeof(buf), "%s's move is %ld.\n\r", GET_NAME(victim),
			GET_MOVE(victim));
	else if (GET_MOVE(victim) > GET_MOVE(ch))
		snprintf(buf, sizeof(buf), "%s's move is higher than you.\n\r", GET_NAME(victim));
	else
		snprintf(buf, sizeof(buf), "%s's move is lower than you.\n\r", GET_NAME(victim));
	send_to_char(buf, ch);

	if (GET_LEVEL(ch) >= GET_LEVEL(victim))
		snprintf(buf, sizeof(buf), "%s's hitroll is %d.\n\r", GET_NAME(victim),
			GET_HITROLL(victim));
	else if (GET_HITROLL(victim) > GET_HITROLL(ch))
		snprintf(buf, sizeof(buf), "%s's hitroll is higher than you.\n\r", GET_NAME(victim));
	else
		snprintf(buf, sizeof(buf), "%s's hitroll is lower than you.\n\r", GET_NAME(victim));
	send_to_char(buf, ch);

	if (GET_LEVEL(ch) >= GET_LEVEL(victim))
		snprintf(buf, sizeof(buf), "%s's damroll is %d.\n\r", GET_NAME(victim),
			GET_DAMROLL(victim));
	else if (GET_DAMROLL(victim) > GET_DAMROLL(ch))
		snprintf(buf, sizeof(buf), "%s's damroll is higher than you.\n\r", GET_NAME(victim));
	else
		snprintf(buf, sizeof(buf), "%s's damroll is lower than you.\n\r", GET_NAME(victim));
	send_to_char(buf, ch);

	act("$n QUERYS $N.", TRUE, ch, 0, victim, TO_ROOM);
	GET_MANA(ch) -= 10;
}
