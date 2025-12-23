/* ************************************************************************
*  file: act.informative.c , Implementation of commands.  Part of DIKUMUD *
*  Usage : Informative commands.                                          *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete information. *
************************************************************************* */

#include "structs.h"
#include "utils.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "spells.h"
#include "limit.h"
#include "guild_list.h"		/* by process */


/* intern vars */
char *news_content = NULL; // 뉴스 내용을 저장할 메모리, 251119
time_t news_last_mod = 0; // 파일이 마지막으로 수정된 시간, 251119

/* Procedures related to 'look' */

void argument_split(char *argument, char *first_arg, char *second_arg)
{
	int look_at, begin = 0;

	/* Find first non blank */
	for (; *(argument + begin) == ' '; begin++) ;

	/* Find length of first word */
	for (look_at = 0; *(argument + begin + look_at) > ' '; look_at++)
		/* Make all letters lower case, AND copy them to first_arg */
		*(first_arg + look_at) = tolower(*(argument + begin + look_at));
	*(first_arg + look_at) = '\0';
	begin += look_at;

	/* Find first non blank */
	for (; *(argument + begin) == ' '; begin++) ;

	/* Find length of second word */
	for (look_at = 0; *(argument + begin + look_at) > ' '; look_at++)
		/* Make all letters lower case, AND copy them to second_arg */
		*(second_arg + look_at) = tolower(*(argument + begin + look_at));
	*(second_arg + look_at) = '\0';
	begin += look_at;
}

struct obj_data *get_object_in_equip_vis(struct char_data *ch,
			 char *arg, struct obj_data *equipment[], int *j)
{

	for ((*j) = 0; (*j) < MAX_WEAR; (*j)++)
		if (equipment[(*j)])
			if (CAN_SEE_OBJ(ch, equipment[(*j)]))
				if (isname(arg, equipment[(*j)]->name))
					return (equipment[(*j)]);

	return (0);
}

char *find_ex_description(char *word, struct extra_descr_data *list)
{
	struct extra_descr_data *i;

	for (i = list; i; i = i->next)
		if (isname(word, i->keyword))
			return (i->description);
	return (0);
}

void show_obj_to_char(struct obj_data *object, struct char_data *ch, int mode)
{
	char buffer[MAX_STRING_LENGTH];

	buffer[0] = '\0';

	if (mode < 0 || mode > 6)
		mode = 1;
	
	if ((mode == 0) && object->description)
		strlcat(buffer, object->description, sizeof(buffer));
	else if (object->short_description && ((mode == 1) || (mode == 2) || (mode == 3) || (mode == 4)))
		strlcat(buffer, object->short_description, sizeof(buffer));
	else if (mode == 5) {
		if (object->obj_flags.type_flag == ITEM_NOTE) {
			if (object->action_description) {
				strlcat(buffer, "There is something written upon it:\n\r\n\r", sizeof(buffer));
                strlcat(buffer, object->action_description, sizeof(buffer));
				send_to_char(buffer, ch); // page_string 대신 send_to_char로 수정
			} else
				acthan("It's blank.", "비어 있습니다.", FALSE, ch, 0, 0, TO_CHAR);
			return;
		} else if ((object->obj_flags.type_flag != ITEM_DRINKCON)) {
			strlcat(buffer, "You see nothing special..", sizeof(buffer));
		} else {	/* ITEM_TYPE == ITEM_DRINKCON */
			strlcat(buffer, "It looks like a drink container.", sizeof(buffer));
		}
	}

	if (mode != 3) {
		if (IS_OBJ_STAT (object, ITEM_INVISIBLE)) {
			strlcat(buffer, "(invisible)", sizeof(buffer));
		}
		if (IS_OBJ_STAT (object, ITEM_EVIL) && ch && IS_AFFECTED(ch, AFF_DETECT_EVIL)) {
			strlcat(buffer, "..It glows red!", sizeof(buffer));
		}
		if (IS_OBJ_STAT (object, ITEM_GLOW)) {
			strlcat(buffer, "..It has a soft glowing aura!", sizeof(buffer));
		}
		if (IS_OBJ_STAT (object, ITEM_HUM)) {
			strlcat(buffer, "..It emits a faint humming sound!", sizeof(buffer));
		}
    }

	strlcat(buffer, "\n\r", sizeof(buffer));
	page_string(ch->desc, buffer, 0);
}

void list_obj_to_char(struct obj_data *list, struct char_data *ch, int mode,
		      bool show)
{
	struct obj_data *i;
	bool found;

	found = FALSE;
	for (i = list; i; i = i->next_content) {
		if (CAN_SEE_OBJ(ch, i)) {
			show_obj_to_char(i, ch, mode);
			found = TRUE;
		}
	}
	if ((!found) && (show))
		send_to_char_han("Nothing.\n\r", "없음.\n\r", ch);
}

void show_char_to_char(struct char_data *i, struct char_data *ch, int mode)
{
	char buffer[MAX_STRING_LENGTH];
	int j, found, percent;
	struct obj_data *tmp_obj;

	buffer[0] = '\0';

	if (mode == 0) {
		if (i && IS_AFFECTED(i, AFF_SHADOW_FIGURE)) {
			send_to_char("A shadow figure is here.\n\r", ch);
			return;
		}
		/* warn fix */
		if (i && (IS_AFFECTED(i, AFF_HIDE) || !CAN_SEE(ch, i))) {
			if (ch && IS_AFFECTED(ch, AFF_SENSE_LIFE))
				if (GET_LEVEL(i) < (IMO + 2))
					send_to_char_han("You sense a hidden life in the room.\n\r",
							 "방에 숨어있는 생명을 감지합니다.\n\r", ch);
			return;
		}
		if (!(i->player.long_descr) || (GET_POS(i) !=
						i->specials.default_pos)) {
			if (!IS_NPC(i)) {
				strlcat(buffer, GET_NAME(i), sizeof(buffer));
				strlcat(buffer, " ", sizeof(buffer));
				if (GET_TITLE(i))
					strlcat(buffer, GET_TITLE(i), sizeof(buffer));
			} else {
				strlcat(buffer, i->player.short_descr, sizeof(buffer));
				CAP(buffer);
			}

			if (i && IS_SET(i->specials.act, PLR_WIZINVIS) && !IS_NPC(i))
				strlcat(buffer, " (wizinvis)", sizeof(buffer));
			if (i && IS_AFFECTED(i, AFF_INVISIBLE))
				strlcat(buffer, " (invisible)", sizeof(buffer));
			switch (GET_POS(i)) {
				case POSITION_STUNNED:
					strlcat(buffer, " is lying here, stunned.", sizeof(buffer));
					break;
				case POSITION_INCAP:
					strlcat(buffer, " is lying here, incapacitated.", sizeof(buffer));
					break;
				case POSITION_MORTALLYW:
					strlcat(buffer, " is lying here, mortally wounded.", sizeof(buffer));
					break;
				case POSITION_DEAD:
					strlcat(buffer, " is lying here, dead.", sizeof(buffer));
					break;
				case POSITION_STANDING:
					strlcat(buffer, " is standing here.", sizeof(buffer));
					break;
				case POSITION_SITTING:
					strlcat(buffer, " is sitting here.", sizeof(buffer));
					break;
				case POSITION_RESTING:
					strlcat(buffer, " is resting here.", sizeof(buffer));
					break;
				case POSITION_SLEEPING:
					strlcat(buffer, " is sleeping here.", sizeof(buffer));
					break;
				case POSITION_FIGHTING:
					if (i->specials.fighting) {
						strlcat(buffer, " is here, fighting ", sizeof(buffer));
						if (i->specials.fighting == ch)
							strlcat(buffer, " YOU!", sizeof(buffer));
						else {
							if (i->in_room == i->specials.fighting->in_room)
								if (IS_NPC(i->specials.fighting)) strlcat(buffer, i->specials.fighting->player.short_descr, sizeof(buffer));
								else strlcat(buffer, GET_NAME(i->specials.fighting), sizeof(buffer));
							else
								strlcat(buffer, "someone who has already left.", sizeof(buffer));
						}
					} else		/* NIL fighting pointer */
						strlcat(buffer, " is here struggling with thin air.", sizeof(buffer));
					break;
				default:
					strlcat(buffer, " is floating here.", sizeof(buffer));
					break;
			}
			if (ch && IS_AFFECTED(ch, AFF_DETECT_EVIL)) {
				if (IS_EVIL(i))
					strlcat(buffer, " (Red Aura)", sizeof(buffer));
			}

			strlcat(buffer, "\n\r", sizeof(buffer));
			send_to_char(buffer, ch);
		} else {		/* npc with long */
			if (i && IS_AFFECTED(i, AFF_INVISIBLE))
				strlcat(buffer, "*", sizeof(buffer));
			else
				*buffer = '\0';

			if (ch && IS_AFFECTED(ch, AFF_DETECT_EVIL)) {
				if (IS_EVIL(i))
					strlcat(buffer, " (Red Aura)", sizeof(buffer));
			}

			strlcat(buffer, i->player.long_descr, sizeof(buffer));
			send_to_char(buffer, ch);
			send_to_char("\r\n", ch); // 개행문자 추가, 251014
		}

		if (i && IS_AFFECTED(i, AFF_LOVE))
			act("$n is EXCITED with glorious feeling!!!!",
			    FALSE, i, 0, ch, TO_VICT);
		if (i && IS_AFFECTED(i, AFF_RERAISE))	/* by chase */
			act("$n is calmed down with angelic comfortablity!!!",
			    FALSE, i, 0, ch, TO_VICT);
		if (i && IS_AFFECTED(i, AFF_SANCTUARY))
			act("$n glows with a bright light!", FALSE, i, 0, ch, TO_VICT);
		if (i && IS_AFFECTED(i, AFF_DEATH))	/* by process */
			act("$n is deadly UNEASE!!!!!", FALSE, i, 0, ch, TO_VICT);

	} else if (mode == 1) {
		if (i->player.description) {
			send_to_char(i->player.description, ch);
			send_to_char("\r\n", ch); // 개행문자 추가, 251014
		} else {
			act("You see nothing special about $m.", FALSE, i, 0,
			    ch, TO_VICT);
		}

		/* Show a character to another */

		if (GET_PLAYER_MAX_HIT(i) > 0)
			percent = (100 * GET_HIT(i)) / GET_PLAYER_MAX_HIT(i);
		else
			percent = -1;	/* How could MAX_HIT be < 1?? */

		if (IS_NPC(i))
			strlcat(buffer, i->player.short_descr, sizeof(buffer));
		else
			strlcat(buffer, GET_NAME(i), sizeof(buffer));

		if (percent >= 100)
			strlcat(buffer, " is in an excellent condition.\n\r", sizeof(buffer));
		else if (percent >= 90)
			strlcat(buffer, " has a few scratches.\n\r", sizeof(buffer));
		else if (percent >= 75)
			strlcat(buffer, " has some small wounds and bruises.\n\r", sizeof(buffer));
		else if (percent >= 50)
			strlcat(buffer, " has quite a few wounds.\n\r", sizeof(buffer));
		else if (percent >= 30)
			strlcat(buffer, " has some big nasty wounds and scratches.\n\r", sizeof(buffer));
		else if (percent >= 15)
			strlcat(buffer, " looks pretty hurt.\n\r", sizeof(buffer));
		else if (percent >= 0)
			strlcat(buffer, " is in an awful condition.\n\r", sizeof(buffer));
		else
			strlcat(buffer, " is bleeding awfully from big wounds.\n\r", sizeof(buffer));

		send_to_char(buffer, ch);

		found = FALSE;
		for (j = 0; j < MAX_WEAR; j++) {
			if (i->equipment[j]) {
				if (CAN_SEE_OBJ(ch, i->equipment[j])) {
					found = TRUE;
				}
			}
		}
		if (found) {
			act("\n\r$n is using:", FALSE, i, 0, ch, TO_VICT);
			for (j = 0; j < MAX_WEAR; j++) {
				if (i->equipment[j]) {
					if (CAN_SEE_OBJ(ch, i->equipment[j])) {
						send_to_char(where[j], ch);
						show_obj_to_char(i->equipment[j],
								 ch, 1);
					}
				}
			}
		}
		if (((GET_CLASS(ch) == CLASS_THIEF) && (ch != i)) ||
		    (GET_LEVEL(ch)
		     >= IMO)) {
			found = FALSE;
			send_to_char_han("\n\rYou attempt to peek at the inventory:\n\r",
					 "\n\r장비와 물건을 엿보려고 합니다.\n\r", ch);
			for (tmp_obj = i->carrying; tmp_obj;
			     tmp_obj = tmp_obj->next_content) {
				if (CAN_SEE_OBJ(ch, tmp_obj) &&
				    (number(0, 20) < GET_LEVEL(ch))) {
					show_obj_to_char(tmp_obj, ch, 1);
					found = TRUE;
				}
			}
			if (!found)
				send_to_char_han("You can't see anything.\n\r",
						 "아무 것도 없군요.\n\r", ch);
		}
	} else if (mode == 2) {
		/* Lists inventory */
		act("$n is carrying:", FALSE, i, 0, ch, TO_VICT);
		list_obj_to_char(i->carrying, ch, 1, TRUE);
	}
}

void list_char_to_char(struct char_data *list, struct char_data *ch,
		       int mode)
{
	struct char_data *i;

	for (i = list; i; i = i->next_in_room) {
		if (ch && i) {
			if ((ch != i) && (IS_AFFECTED(ch, AFF_SENSE_LIFE) ||
					  (CAN_SEE(ch, i) && !IS_AFFECTED(i, AFF_HIDE))))
				show_char_to_char(i, ch, 0);
		}
	}
}

void do_look(struct char_data *ch, char *argument, int cmd)
{
	char buffer[MAX_STRING_LENGTH];
	char buf[MAX_STRING_LENGTH];
	char arg1[MAX_STRING_LENGTH];
	char arg2[MAX_STRING_LENGTH];
	int keyword_no;
	int j, bits, temp;
	bool found;
	struct obj_data *tmp_object, *found_object;
	struct char_data *tmp_char;
	char *tmp_desc;
	static char *keywords[] =
	{
		"north",
		"east",
		"south",
		"west",
		"up",
		"down",
		"in",
		"at",
		"",		/* Look at '' case */
		"\n"};

	if (!ch->desc)
		return;

	if (GET_POS(ch) < POSITION_SLEEPING)
		send_to_char_han("You can't see anything but stars!\n\r",
				 "별들이 아름답게 빛나고 있습니다.\n\r", ch);
	else if (GET_POS(ch) == POSITION_SLEEPING)
		send_to_char_han("You can't see anything, you're sleeping!\n\r",
				 "아마도 지금 꿈을 꾸는게 아닌지요 ??\n\r", ch);
	else if (ch && IS_AFFECTED(ch, AFF_BLIND))
		send_to_char_han("You can't see a damn thing, you're blinded!\n\r",
				 "앞을 볼 수가 없습니다! 눈이 멀었습니다!\n\r", ch);
	else if (GET_LEVEL(ch) < IMO && IS_DARK(ch->in_room) && !OMNI(ch) && ch
		 && !IS_AFFECTED(ch, AFF_INFRAVISION))
		send_to_char_han("It is pitch black...\n\r",
				 "너무 깜깜합니다..\n\r", ch);
	else {
		argument_split(argument, arg1, arg2);
		if (!*arg1)
			keyword_no = 8;
		else
			keyword_no = search_block(arg1, keywords, FALSE);	/* Partial Match */

		if ((keyword_no == -1) && *arg1) {
			keyword_no = 7;
			arg2[0] = '\0';
			strlcat(arg2, arg1, sizeof(arg2));	/* Let arg2 become the target object (arg1) */
		}

		found = FALSE;
		tmp_object = 0;
		tmp_char = 0;
		tmp_desc = 0;

		buffer[0] = '\0';

		switch (keyword_no) {
			/* look <dir> */
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			{

				if (EXIT(ch, keyword_no)) {

					if (EXIT(ch, keyword_no)->general_description) {
						send_to_char(EXIT(ch,
							     keyword_no)->
							     general_description, ch);
					} else {
						send_to_char_han("You see nothing special.\n\r",
								 "별다른 것이 없습니다.\n\r", ch);
					}

					if (IS_SET(EXIT(ch,
						   keyword_no)->exit_info,
					    EX_CLOSED) &&
					    (EXIT(ch, keyword_no)->keyword)) {
						snprintf(buffer, sizeof(buffer),
							"The %s is closed.\n\r",
							fname(EXIT(ch,
							      keyword_no)->keyword));
						send_to_char(buffer, ch);
					} else {
						if (IS_SET(EXIT(ch,
							   keyword_no)->exit_info,
						    EX_ISDOOR) &&
						    EXIT(ch, keyword_no)->keyword) {
							snprintf(buffer, sizeof(buffer),
								"The %s is open.\n\r",
								fname(EXIT(ch,
								      keyword_no)->keyword));
							send_to_char(buffer, ch);
						}
					}
				} else {
					send_to_char_han("Nothing special there...\n\r",
							 "거기엔 특별한 것이 없습니다 ... \n\r", ch);
				}
			}
			break;

			/* look 'in'  */
		case 6:
			{
				if (*arg2) {
					/* Item carried */

					bits = generic_find(arg2, FIND_OBJ_INV
							    | FIND_OBJ_ROOM |
							    FIND_OBJ_EQUIP,
							    ch, &tmp_char, &tmp_object);

					if (bits) {	/* Found something */
						if (GET_ITEM_TYPE(tmp_object)
						    == ITEM_DRINKCON) {
							if
							    (tmp_object->obj_flags.value[1]
							    <= 0) {
								acthan("It is empty.",
								       "비어 있습니다.",
								       FALSE,
								       ch, 0,
								       0, TO_CHAR);
							} else {
								temp =
									((tmp_object->obj_flags.value[1]
									 * 3) /
									tmp_object->obj_flags.value[0]);
								snprintf(buffer, sizeof(buffer),
									"It's %sfull of a %s liquid.\n\r",
									fullness[temp],
									color_liquid[tmp_object->obj_flags.value[2]]);
								send_to_char(buffer, ch);
							}
						} else if
							   (GET_ITEM_TYPE(tmp_object)
							   == ITEM_CONTAINER) {
							if
							    (!IS_SET(tmp_object->obj_flags.value[1],
							     CONT_CLOSED)) {
								send_to_char(fname(tmp_object->name), ch);
								switch (bits) {
									case
								FIND_OBJ_INV:
									send_to_char(" (carried) : \n\r", ch);
									break;
									case
								FIND_OBJ_ROOM:
									send_to_char(" (here) : \n\r", ch);
									break;
									case
								FIND_OBJ_EQUIP:
									send_to_char(" (used) : \n\r", ch);
									break;
								}
								list_obj_to_char(tmp_object->contains,
										 ch,
										 2, TRUE);
							} else
								send_to_char_han("It is closed.\n\r",
										 "닫혔군요.\n\r", ch);
						} else {
							send_to_char_han("That is not a container.\n\r",
									 "이것은 물건을 담을 용기가 아니군요.\n\r", ch);
						}
					} else {	/* wrong argument */
						send_to_char_han("You do not see that item here.\n\r",
								 "그런 아이템은 없어요.\n\r", ch);
					}
				} else {	/* no argument */
					send_to_char_han("Look in what?!\n\r",
							 "무엇의 안을 들여다 본다구요?\n\r", ch);
				}
			}
			break;

			/* look 'at'  */
		case 7:
			{
				if (*arg2) {
					bits = generic_find(arg2, FIND_OBJ_INV
							    | FIND_OBJ_ROOM |
							    FIND_OBJ_EQUIP |
							    FIND_CHAR_ROOM, ch,
							    &tmp_char, &found_object);

					if (tmp_char) {
						show_char_to_char(tmp_char,
								  ch, 1);
						if (ch != tmp_char) {
							acthan("$n looks at you.",
							       "$n님이 당신을 바라봅니다.",
							       TRUE, ch, 0,
							       tmp_char, TO_VICT);
							acthan("$n looks at $N.",
							       "$n님이 $N님을 바라봅니다.",
							       TRUE, ch, 0,
							       tmp_char, TO_NOTVICT);
						}
						return;
					}

					/* Search for Extra Descriptions in room and items */

					/* Extra description in room?? */
					if (!found) {
						tmp_desc = find_ex_description(arg2,
									       world[ch->in_room].ex_description);
						if (tmp_desc) {
							page_string(ch->desc,
								    tmp_desc, 0);
							return;		/* RETURN SINCE IT WAS A ROOM DESCRIPTION */
						}
					}
					/* Search for extra descriptions in items */
					/* Equipment Used */
					if (!found) {
						for (j = 0; j < MAX_WEAR && !found; j++) {
							if (ch->equipment[j]) {
								if
								    (CAN_SEE_OBJ(ch,
										 ch->equipment[j])) {
									tmp_desc
									    =
									    find_ex_description(arg2,
												ch->equipment[j]->ex_description);
									if (tmp_desc) {
										page_string(ch->desc, tmp_desc, 0);
										found = TRUE;
									}
								}
							}
						}
					}
					/* In inventory */
					if (!found) {
						for (tmp_object = ch->carrying;
						     tmp_object && !found;
						     tmp_object = tmp_object->next_content) {
							if (CAN_SEE_OBJ(ch, tmp_object)) {
								tmp_desc =
								    find_ex_description(arg2,
											tmp_object->ex_description);
								if (tmp_desc) {
									page_string(ch->desc, tmp_desc, 0);
									found = TRUE;
								}
							}
						}
					}

					/* Object In room */

					if (!found) {
						for (tmp_object =
						     world[ch->in_room].contents;
						     tmp_object && !found;
						     tmp_object = tmp_object->next_content) {
							if (CAN_SEE_OBJ(ch, tmp_object)) {
								tmp_desc =
								    find_ex_description(arg2,
											tmp_object->ex_description);
								if (tmp_desc) {
									page_string(ch->desc, tmp_desc, 0);
									found = TRUE;
								}
							}
						}
					}
					/* wrong argument */

					if (bits) {	/* If an object was found */
						if (!found)
							show_obj_to_char(found_object,
									 ch, 5);	/* Show no-description */
						else
							show_obj_to_char(found_object,
									 ch, 6);	/* Find hum, glow etc */
					} else if (!found) {
						send_to_char_han("You do not see that here.\n\r",
								 "그런 것은 여기에 없습니다.\n\r", ch);
					}
				} else {
					/* no argument */

					send_to_char_han("Look at what?\n\r",
							 "무엇을 바라보라고요 ?\n\r", ch);
				}
			}
			break;

			/* look ''    */
		case 8:
			{
				if (GET_LEVEL(ch) >= IMO) {
					snprintf(buffer, sizeof(buffer), "%s  [%5d] [ ",
						world[ch->in_room].name,
						world[ch->in_room].number);
					sprintbit((long)world[ch->in_room].room_flags,
						  room_bits, buf);
					strlcat(buffer, buf, sizeof(buffer));
					strlcat(buffer, "]\n\r", sizeof(buffer));
				} else
					snprintf(buffer, sizeof(buffer), "%s\n\r",
						world[ch->in_room].name);
				send_to_char(buffer, ch);

				if (!IS_SET(ch->specials.act, PLR_BRIEF)) {
					send_to_char(world[ch->in_room].description, ch);
					send_to_char("\r\n", ch);
				}
				snprintf(buffer, sizeof(buffer), "[ EXITS : ");
				
				if (EXIT(ch, 0)) 
					IS_SET(EXIT(ch, 0)->exit_info, EX_CLOSED) ? strlcat(buffer, "(N) ", sizeof(buffer)) : strlcat(buffer, "N ", sizeof(buffer));
			   	if (EXIT(ch, 1)) 
					IS_SET(EXIT(ch, 1)->exit_info, EX_CLOSED) ? strlcat(buffer, "(E) ", sizeof(buffer)) : strlcat(buffer, "E ", sizeof(buffer));
				if (EXIT(ch, 2)) 
					IS_SET(EXIT(ch, 2)->exit_info, EX_CLOSED) ? strlcat(buffer, "(S) ", sizeof(buffer)) : strlcat(buffer, "S ", sizeof(buffer));
				if (EXIT(ch, 3)) 
					IS_SET(EXIT(ch, 3)->exit_info, EX_CLOSED) ? strlcat(buffer, "(W) ", sizeof(buffer)) : strlcat(buffer, "W ", sizeof(buffer));
				if (EXIT(ch, 4)) 
					IS_SET(EXIT(ch, 4)->exit_info, EX_CLOSED) ? strlcat(buffer, "(U) ", sizeof(buffer)) : strlcat(buffer, "U ", sizeof(buffer));
				if (EXIT(ch, 5)) 
					IS_SET(EXIT(ch, 5)->exit_info, EX_CLOSED) ? strlcat(buffer, "(D) ", sizeof(buffer)) : strlcat(buffer, "D ", sizeof(buffer));
				
				strlcat(buffer, " ]\n\r", sizeof(buffer));
				send_to_char(buffer, ch);
				list_obj_to_char(world[ch->in_room].contents,
						 ch, 0, FALSE);
				list_char_to_char(world[ch->in_room].people,
						  ch, 0);
			}
			break;

			/* wrong arg  */
		case -1:
			send_to_char_han("Sorry, I didn't understand that!\n\r",
					 "미안합니다. 이해를 못하겠네요.\n\r", ch);
			break;
		}
	}
}

/* end of look */

void do_bank(struct char_data *ch, char *argument, int cmd)
{
	send_to_char_han("You can only do that at the bank.\n\r",
			 "그것은 은행에서만 가능한 일이에요.\n\r", ch);
}

void do_read(struct char_data *ch, char *argument, int cmd)
{
	char buf[100];

	/* This is just for now - To be changed later! */
	snprintf(buf, sizeof(buf), "at %s", argument);
	do_look(ch, buf, 15);
}

void do_examine(struct char_data *ch, char *argument, int cmd)
{
	char name[100], buf[100];
	struct char_data *tmp_char;
	struct obj_data *tmp_object;

	snprintf(buf, sizeof(buf), "at %s", argument);
	do_look(ch, buf, 15);

	one_argument(argument, name);

	if (!*name) {
		send_to_char_han("Examine what?\n\r",
				 "무엇을 조사하시려구요?\n\r", ch);
		return;
	}

	generic_find(name, FIND_OBJ_INV | FIND_OBJ_ROOM |
				  FIND_OBJ_EQUIP, ch, &tmp_char, &tmp_object);

	if (tmp_object) {
		if ((GET_ITEM_TYPE(tmp_object) == ITEM_DRINKCON) ||
		    (GET_ITEM_TYPE(tmp_object) == ITEM_CONTAINER)) {
			send_to_char_han("When you look inside, you see:\n\r",
					 "안을 들여다 보니 이런 것이 있군요.", ch);
			snprintf(buf, sizeof(buf), "in %s", argument);
			do_look(ch, buf, 15);
		} else {
			if (GET_ITEM_TYPE(tmp_object) == ITEM_FIREWEAPON) {
				snprintf(buf, sizeof(buf), "There are %d shots left.\n\r",
					tmp_object->obj_flags.value[0]);
				send_to_char(buf, ch);
			}
		}
	}
}

void do_report(struct char_data *ch, char *argument, int cmd)
{
	char buf[128];
	snprintf(buf, sizeof(buf), "%s %ld/%ld hp, %ld/%ld mn, %ld/%ld mv", GET_NAME(ch),
						GET_HIT(ch), GET_PLAYER_MAX_HIT(ch), GET_MANA(ch),
						GET_PLAYER_MAX_MANA(ch),
						GET_MOVE(ch), GET_PLAYER_MAX_MOVE(ch));
	act(buf, FALSE, ch, 0, 0, TO_ROOM);
	send_to_char("&c[REPORT]&n You've reported your state.\n\r",ch); // uncommented by Komo, 251022
}

void do_title(struct char_data *ch, char *argument, int cmd)
{
	char buf[MAX_STRING_LENGTH];
	
	for (; *argument == ' '; argument++);

	if (!*argument) {
        snprintf(buf, sizeof(buf), "&c[TITLE]&n You are '%s %s' NOW.\n\r", GET_NAME(ch), GET_TITLE(ch) ? GET_TITLE(ch) : "None");
        send_to_char(buf, ch);
        return;
    }

	if (GET_TITLE(ch)) {
        free(GET_TITLE(ch));
        GET_TITLE(ch) = NULL;
    }

    if (strlen(argument) > 80) { /* 길이 제한 여기서 */
        send_to_char("&c[TITLE]&n Title is too long! (Max 80 bytes)\n\r", ch);
        return;
    }

    GET_TITLE(ch) = strdup(argument);

    snprintf(buf, sizeof(buf), "&c[TITLE]&n Ok. Your Title has been set to [ %s ] now.\n\r", GET_TITLE(ch));
    send_to_char(buf, ch);
}

void do_exits(struct char_data *ch, char *argument, int cmd)
{
	int door;
	char buf[MAX_STRING_LENGTH];
	char *p = buf;
    int remaining_space = MAX_STRING_LENGTH;
    int written_chars;
    int found_any_exit = 0;

	static char *exits[] = {
        "North", "East ", "South", "West ", "Up   ", "Down "
    };

	*p = '\0';

	send_to_char_han("Obvious exits:\r\n", "명백한 출구는:\r\n", ch);

	for (door = 0; door <= 5; door++) { 
		written_chars = 0;

		if (!EXIT(ch, door)) {
			continue;
		}

		found_any_exit = 1;

		if (EXIT(ch, door)->to_room == NOWHERE) {
			written_chars = snprintf(p, remaining_space, "%-7s - Leads nowhere\r\n", exits[door]);
		} else {
            // 괄호 모양을 결정할 변수, 기본값은 정렬을 위한 공백
            const char *left_bracket = " ";
            const char *right_bracket = " ";

            if (IS_SET(EXIT(ch, door)->exit_info, EX_LOCKED)) { // 잠겨있는지
                left_bracket = "{";
                right_bracket = "}";
            }
            else if (IS_SET(EXIT(ch, door)->exit_info, EX_CLOSED)) { // 잠겨있지 않다면, 닫혀있는지
                left_bracket = "(";
                right_bracket = ")";
            }

            if (IS_DARK(EXIT(ch, door)->to_room) && !OMNI(ch)) { // 어두워서 안 보이는 경우
                written_chars = snprintf(p, remaining_space, "%-7s - Too dark to tell\r\n", exits[door]);
            } else {
                // IMO인지 확인
                if (GET_LEVEL(ch) >= IMO) {
                    written_chars = snprintf(p, remaining_space, "%s%-5s%s - [%5d] %s\r\n",
                        left_bracket, exits[door], right_bracket,
                        world[EXIT(ch, door)->to_room].number,
                        world[EXIT(ch, door)->to_room].name);
                } else {
                    // 일반 플레이어
                    written_chars = snprintf(p, remaining_space, "%s%-5s%s - %s [%5d] \r\n",
                        left_bracket, exits[door], right_bracket,
                        world[EXIT(ch, door)->to_room].name, 
						world[EXIT(ch, door)->to_room].number);
                }
            }

            if (written_chars > 0 && written_chars < remaining_space) {
                p += written_chars;
                remaining_space -= written_chars;
            } else if (written_chars >= remaining_space) {
				mudlog("do_exits: Buffer full, exit list truncated.");
				break;
			}
        }
	}

	if (found_any_exit)
        send_to_char(buf, ch);
    else
        send_to_char("None.\r\n", ch);
}
/*
static char ac_msg[13][44] = {
  "You are naked.\n\r",
  "You are almost naked.\n\r",
  "You are barely covered.\n\r",
  "You are somewhat covered.\n\r",
  "You are pretty covered.\n\r",
  "You are pretty well covered.\n\r",
  "You are pretty well armored.\n\r",
  "You are heavily armored.\n\r",
  "You are very heavily armored.\n\r",
  "You are extremely heavily armored.\n\r",
  "You are almost an armored tank.\n\r",
  "You are an armored tank.\n\r",
  "You are a walking fortress.\n\r"
};
*/
static char align_msg[13][44] =
{
	"You are a saint.\n\r",
	"You feel like being a saint.\n\r",
	"You are good.\n\r",
	"You are slightly good.\n\r",
	"You are almost good.\n\r",
	"You are going to be good.\n\r",
	"You are neutral.\n\r",
	"You are going to be evil.\n\r",
	"You are almost evil.\n\r",
	"You are slightly evil.\n\r",
	"You are evil.\n\r",
	"You feel like being a devil.\n\r",
	"You are a devil.\n\r"
};

static char *align_msg_han[13] =
{
	"당신은 성인 군자십니다.\n\r",
	"당신은 성이 군자가 되어가고 있습니다.\n\r",
	"당신은 선하십니다.\n\r",
	"당신은 약간 선합니다.\n\r",
	"당신은 거의 선한 경지에 이르렀습니다.\n\r",
	"당신은 선한쪽으로 변하고 있습니다.\n\r",
	"당신은 아주 평범한 성향입니다.\n\r",
	"당신은 악한 쪽으로 변해가고 있습니다.\n\r",
	"당신은 거의 악해졌습니다.\n\r",
	"당신은 약간 악한 성향을 띄고 있습니다.\n\r",
	"당신은 악하십니다.\n\r",
	"당신은 거의 악마가 되어갑니다.\n\r",
	"당신은 악마가 되셨습니다.\n\r"
};

void do_score(struct char_data *ch, char *argument, int cmd)
{
	struct affected_type *aff;
	struct time_info_data playing_time;
	char buf[200], buf2[200];
	int tmp;
	
	snprintf(buf, sizeof(buf), "You are %d years old.\n\r", GET_AGE(ch));
	snprintf(buf2, sizeof(buf2), "당신은 %d 살 입니다.\n\r", GET_AGE(ch));
	send_to_char_han(buf, buf2, ch);

	/* alignment message */
	tmp = GET_ALIGNMENT(ch) / 50;
	if (tmp > 17)
		send_to_char_han(align_msg[0], align_msg_han[0], ch);
	else if (tmp > 12)
		send_to_char_han(align_msg[1], align_msg_han[1], ch);
	else if (tmp > 7)
		send_to_char_han(align_msg[2], align_msg_han[2], ch);
	else if (tmp > 6)
		send_to_char_han(align_msg[3], align_msg_han[3], ch);
	else if (tmp > 5)
		send_to_char_han(align_msg[4], align_msg_han[4], ch);
	else if (tmp > 2)
		send_to_char_han(align_msg[5], align_msg_han[5], ch);
	else if (tmp > -3)
		send_to_char_han(align_msg[6], align_msg_han[6], ch);
	else if (tmp > -6)
		send_to_char_han(align_msg[7], align_msg_han[7], ch);
	else if (tmp > -7)
		send_to_char_han(align_msg[8], align_msg_han[8], ch);
	else if (tmp > -8)
		send_to_char_han(align_msg[9], align_msg_han[9], ch);
	else if (tmp > -13)
		send_to_char_han(align_msg[10], align_msg_han[10], ch);
	else if (tmp > -18)
		send_to_char_han(align_msg[11], align_msg_han[11], ch);
	else
		send_to_char_han(align_msg[12], align_msg_han[12], ch);

	snprintf(buf, sizeof(buf), "Your ac is %d.\n\r", ch->points.armor);
	snprintf(buf2, sizeof(buf2), "당신의 무장정도는 %d 입니다.\n\r", ch->points.armor);
	send_to_char_han(buf, buf2, ch);

	if (GET_COND(ch, DRUNK) > 10)
		send_to_char_han("You are intoxicated.\n\r",
				 "당신은 취해 있습니다.\n\r", ch);
	snprintf(buf, sizeof(buf),
		"You have %ld(%ld) hit, %ld(%ld) mana and %ld(%ld) movement points.\n\r",
		GET_HIT(ch), GET_PLAYER_MAX_HIT(ch),
		GET_MANA(ch), GET_PLAYER_MAX_MANA(ch),
		GET_MOVE(ch), GET_PLAYER_MAX_MOVE(ch));
	snprintf(buf2, sizeof(buf2),
		"당신은 %ld(%ld) hit, %ld(%ld) mana 와 %ld(%ld) movement 를 가지고 있습니다.\n\r",
		GET_HIT(ch), GET_PLAYER_MAX_HIT(ch),
		GET_MANA(ch), GET_PLAYER_MAX_MANA(ch),
		GET_MOVE(ch), GET_PLAYER_MAX_MOVE(ch));
	send_to_char_han(buf, buf2, ch);

	if (GET_LEVEL(ch) > 12) {
		snprintf(buf, sizeof(buf),
			"Your stat: str %d/%d wis %d int %d dex %d con %d.\n\r",
			GET_STR(ch), GET_ADD(ch), GET_WIS(ch), GET_INT(ch),
			GET_DEX(ch), GET_CON(ch));
		snprintf(buf2, sizeof(buf2),
			"당신의 체질: str %d/%d wis %d int %d dex %d con %d.\n\r",
			GET_STR(ch), GET_ADD(ch), GET_WIS(ch), GET_INT(ch),
			GET_DEX(ch), GET_CON(ch));
		send_to_char_han(buf, buf2, ch);

		snprintf(buf, sizeof(buf), "Your hitroll is %d , and damroll is %d.\n\r",
			GET_HITROLL(ch), GET_DAMROLL(ch));
		snprintf(buf2, sizeof(buf2),
			"당신의 hitroll 은 %d , damroll 은 %d입니다.\n\r",
			GET_HITROLL(ch), GET_DAMROLL(ch));
		send_to_char_han(buf, buf2, ch);	/* changed by shin won dong */

		/* bare hand dice */
		snprintf(buf, sizeof(buf), "Your Bare Hand Dice is %dD%d.\n\r",
			ch->specials.damnodice, ch->specials.damsizedice);
		snprintf(buf2, sizeof(buf2), "당신의 맨손 위력은 %dD%d입니다.\n\r",
			ch->specials.damnodice, ch->specials.damsizedice);
		send_to_char_han(buf, buf2, ch);	/* changed by shin won dong */

		snprintf(buf, sizeof(buf),
			"You save para : %d, hit skill : %d, breath : %d, spell : %d.\n\r",
			IS_NPC(ch) ?
			ch->specials.apply_saving_throw[SAVING_PARA] :
			ch->specials.apply_saving_throw[SAVING_PARA] +
			saving_throws[GET_CLASS(ch) -
								      1][SAVING_PARA][GET_LEVEL(ch)
								      - 1],
			IS_NPC(ch) ?
			ch->specials.apply_saving_throw[SAVING_HIT_SKILL] :
			ch->specials.apply_saving_throw[SAVING_HIT_SKILL] +
			saving_throws[GET_CLASS(ch) - 1][SAVING_HIT_SKILL][GET_LEVEL
									   (ch)
									   - 1],
			IS_NPC(ch) ?
			ch->specials.apply_saving_throw[SAVING_BREATH] :
			ch->specials.apply_saving_throw[SAVING_BREATH] +
			saving_throws[GET_CLASS(ch) - 1][SAVING_BREATH][GET_LEVEL
									(ch) - 1],
			IS_NPC(ch) ?
			ch->specials.apply_saving_throw[SAVING_SPELL] :
			ch->specials.apply_saving_throw[SAVING_SPELL] +
			saving_throws[GET_CLASS(ch) - 1][SAVING_SPELL][GET_LEVEL
								       (ch) - 1]);
		snprintf(buf2, sizeof(buf2),
			"당신의 안전도는 para : %d, hit skill : %d, breath : %d, spell : %d입니다.\n\r",
			IS_NPC(ch) ?
			ch->specials.apply_saving_throw[SAVING_PARA] :
			ch->specials.apply_saving_throw[SAVING_PARA] +
			saving_throws[GET_CLASS(ch) -
								      1][SAVING_PARA][GET_LEVEL(ch)
								      - 1],
			IS_NPC(ch) ?
			ch->specials.apply_saving_throw[SAVING_HIT_SKILL] :
			ch->specials.apply_saving_throw[SAVING_HIT_SKILL] +
			saving_throws[GET_CLASS(ch) - 1][SAVING_HIT_SKILL][GET_LEVEL
									   (ch)
									   - 1],
			IS_NPC(ch) ?
			ch->specials.apply_saving_throw[SAVING_BREATH] :
			ch->specials.apply_saving_throw[SAVING_BREATH] +
			saving_throws[GET_CLASS(ch) - 1][SAVING_BREATH][GET_LEVEL
									(ch) - 1],
			IS_NPC(ch) ?
			ch->specials.apply_saving_throw[SAVING_SPELL] :
			ch->specials.apply_saving_throw[SAVING_SPELL] +
			saving_throws[GET_CLASS(ch) - 1][SAVING_SPELL][GET_LEVEL
								       (ch) - 1]);
		send_to_char_han(buf, buf2, ch);

		snprintf(buf, sizeof(buf), "Your regeneration is %d.\n\r", GET_REGENERATION(ch));
		snprintf(buf2, sizeof(buf2), "당신의 회복 정도는 %d 입니다.\n\r",
			GET_REGENERATION(ch));
		send_to_char_han(buf, buf2, ch);
	}

	snprintf(buf, sizeof(buf), "You have scored %lld exp, and have %lld gold coins.\n\r",
		GET_EXP(ch), GET_GOLD(ch));
	snprintf(buf2, sizeof(buf2),
		"당신은 %lld 의 경험치와 %lld 원의 돈을 가지고 있습니다.\n\r",
		GET_EXP(ch), GET_GOLD(ch));
	send_to_char_han(buf, buf2, ch);

	playing_time = real_time_passed((time(0) - ch->player.time.logon) +
					ch->player.time.played, 0);
	snprintf(buf, sizeof(buf), "You have been playing for %d days and %d hours.\n\r",
		playing_time.day, playing_time.hours);
	snprintf(buf2, sizeof(buf2),
		"당신은 이 안에서 %d 일 %d 시간동안 있었습니다.\n\r",
		playing_time.day, playing_time.hours);
	send_to_char_han(buf, buf2, ch);
	snprintf(buf, sizeof(buf), "This ranks you as %s %s (level %d).\n\r",
		GET_NAME(ch), GET_TITLE(ch), GET_LEVEL(ch));
	snprintf(buf2, sizeof(buf2), "당신은 %s %s (레벨 %d) 입니다.\n\r",
		GET_NAME(ch), GET_TITLE(ch), GET_LEVEL(ch));
	send_to_char_han(buf, buf2, ch);
	if (GET_LEVEL(ch) < IMO) {
		if (titles[GET_CLASS(ch) - 1][GET_LEVEL(ch) + 1].exp < GET_EXP(ch)) {
			strcpy(buf, "You have enough experience to advance.\n\r");
			strcpy(buf2, "레벨을 올릴만큼 충분한 경험치가 쌓였습니다.\n\r");
		} else {
			snprintf(buf, sizeof(buf), "You need %lld experience to advance.\n\r",
				titles[GET_CLASS(ch) - 1][GET_LEVEL(ch) +
				1].exp -
				GET_EXP(ch));
			snprintf(buf2, sizeof(buf2),
				"다음 레벨까지 %lld 만큼의 경험치가 필요합니다.\n\r",
				titles[GET_CLASS(ch) - 1][GET_LEVEL(ch) +
				1].exp -
				GET_EXP(ch));
		}
		send_to_char_han(buf, buf2, ch);
	}

	switch (GET_POS(ch)) {
	case POSITION_DEAD:
		send_to_char_han("You are DEAD!\n\r", "당신은 죽으셨습니다!\n\r", ch);
		break;
	case POSITION_MORTALLYW:
		send_to_char("You are mortally wounded!, you should seek help!\n\r",
			     ch);
		break;
	case POSITION_INCAP:
		send_to_char("You are incapacitated, slowly fading away.\n\r", ch);
		break;
	case POSITION_STUNNED:
		send_to_char("You are stunned! You can't move!\n\r", ch);
		break;
	case POSITION_SLEEPING:
		send_to_char_han("You are sleeping.\n\r", "당신은 자고 있습니다.\n\r", ch);
		break;
	case POSITION_RESTING:
		send_to_char_han("You are resting.\n\r", "당신은 쉬고 있습니다.\n\r", ch);
		break;
	case POSITION_SITTING:
		send_to_char_han("You are sitting.\n\r", "당신은 앉아 있습니다.\n\r", ch);
		break;
	case POSITION_FIGHTING:
		if (ch->specials.fighting)
			acthan("You are fighting $N.\n\r", "당신은 $N님과 싸우고 있습니다.\n\r",
			       FALSE, ch, 0, ch->specials.fighting, TO_CHAR);
		else
			send_to_char("You are fighting thin air.\n\r", ch);
		break;
	case POSITION_STANDING:
		send_to_char_han("You are standing.\n\r", "당신은 서있습니다.\n\r", ch);
		break;
	default:
		send_to_char("You are floating.\n\r", ch);
		break;
	}

	if (ch->affected) {
		send_to_char_han("Affecting Spells:\n\r", "걸려있는 마법들:\n\r", ch);
		for (aff = ch->affected; aff; aff = aff->next) {
			snprintf(buf, sizeof(buf), "%s: %d hrs\n\r", spells[aff->type - 1], aff->duration);
			snprintf(buf2, sizeof(buf2), "%s: %d 시간\n\r", spells[aff->type -
				1], aff->duration);
			send_to_char_han(buf, buf2, ch);
		}
	}

	snprintf(buf, sizeof(buf), "\n\rYou have killed #%d player(s).", ch->player.pk_num);
	snprintf(buf2, sizeof(buf2), "\n\r당신은 지금까지 %d명을 죽였습니다.", ch->player.pk_num);
	send_to_char_han(buf, buf2, ch);
	snprintf(buf, sizeof(buf), "\n\rYou have been killed #%d time(s).", ch->player.pked_num);
	snprintf(buf2, sizeof(buf2), "\n\r당신은 지금까지 %d번 죽었습니다.", ch->player.pked_num);
	send_to_char_han(buf, buf2, ch);
	snprintf(buf, sizeof(buf), "\n\rYou have made  #%ld QUEST(s).", ch->quest.solved);
	snprintf(buf2, sizeof(buf2),
		"\n\r당신은 지금까지 %ld번 QUEST를 풀었습니다.", ch->quest.solved);
	send_to_char_han(buf, buf2, ch);
	if (ch->player.guild >= 1 && ch->player.guild <= MAX_GUILD_LIST) {
		snprintf(buf, sizeof(buf), "\n\rYou are a member of %s guild.\n\r",
			guild_names[(int)ch->player.guild]);
		snprintf(buf2, sizeof(buf2),
			"\n\r당신은 %s 길드의 회원입니다. \n\r",
			guild_names[(int)ch->player.guild]);
		send_to_char_han(buf, buf2, ch);
	} else {
		send_to_char("\n\r", ch);
	}
}

/* From msd mud : cyb */
void do_attribute(struct char_data *ch, char *argument, int cmd)
{
	char buf[MAX_STRING_LENGTH];
	struct affected_type *aff;
	
	snprintf(buf, sizeof(buf),
		"You are %d years and %d months, %d cms, and you weigh %d lbs.\n\r",
		GET_AGE(ch), age(ch).month,
		ch->player.height,
		ch->player.weight);

	send_to_char(buf, ch);

	snprintf(buf, sizeof(buf), "You are carrying %d lbs of equipment.\n\r",
		IS_CARRYING_W(ch));
	send_to_char(buf, ch);

/*snprintf(buf, sizeof(buf),"Your armor is %d (-100 to 100).\n\r",ch->points.armor);
  send_to_char(buf,ch); */

	/* if ((GET_LEVEL(ch) > 15) || (GET_CLASS(ch))) {    cyb */
	if ((GET_LEVEL(ch) > 15)) {
		if ((GET_STR(ch) == 18) && (GET_CLASS(ch))) {
			snprintf(buf, sizeof(buf),
				"You have %d/%d STR, %d INT, %d WIS, %d DEX, %d CON\n\r",
				GET_STR(ch), GET_ADD(ch), GET_INT(ch), GET_WIS(ch),
				GET_DEX(ch), GET_CON(ch));
			send_to_char(buf, ch);
		} else {
			snprintf(buf, sizeof(buf),
				"You have %d STR, %d INT, %d WIS, %d DEX, %d CON\n\r",
				GET_STR(ch), GET_INT(ch), GET_WIS(ch), GET_DEX(ch),
				GET_CON(ch));
			send_to_char(buf, ch);
		}
	}

	snprintf(buf, sizeof(buf),
		"Your hitroll and damroll are %d and %d respectively.\n\r",
		GET_HITROLL(ch), GET_DAMROLL(ch));
	send_to_char(buf, ch);

	/*
	   **   by popular demand -- affected stuff
	 */
	if (ch->affected) {
		send_to_char("\n\rAffecting Spells:\n\r--------------\n\r", ch);
		for (aff = ch->affected; aff; aff = aff->next) {
			switch (aff->type) {
			case SKILL_SNEAK:
			case SPELL_POISON:
			case SPELL_CURSE:
				break;
			default:
				snprintf(buf, sizeof(buf), "Spell : '%s'\n\r",
									spells[aff->type - 1]);
				send_to_char(buf, ch);
				break;
			}
		}
	}
}  /* end of Attribute Module... */

void do_time(struct char_data *ch, char *argument, int cmd)
{
	char buf[100], *suf;
	int weekday, day;
	
	snprintf(buf, sizeof(buf), "It is %d o'clock %s, on ",
		((time_info.hours % 12 == 0) ? 12 : ((time_info.hours) % 12)),
		((time_info.hours >= 12) ? "pm" : "am"));

	weekday = ((35 * time_info.month) + time_info.day + 1) % 7;	/* 35 days in a month */

	strlcat(buf, weekdays[weekday], sizeof(buf));
	strlcat(buf, "\n\r", sizeof(buf));
	send_to_char(buf, ch);

	day = time_info.day + 1;	/* day in [1..35] */

	if (day == 1)
		suf = "st";
	else if (day == 2)
		suf = "nd";
	else if (day == 3)
		suf = "rd";
	else if (day < 20)
		suf = "th";
	else if ((day % 10) == 1)
		suf = "st";
	else if ((day % 10) == 2)
		suf = "nd";
	else if ((day % 10) == 3)
		suf = "rd";
	else
		suf = "th";

	snprintf(buf, sizeof(buf), "The %d%s Day of the %s, Year %d.\n\r",
		day,
		suf,
		month_name[(int)time_info.month],
		time_info.year);

	send_to_char(buf, ch);
}

void do_weather(struct char_data *ch, char *argument, int cmd)
{
	char buf[100], buf2[100];
	char static *sky_look[4] =
	{
		"cloudless",
		"cloudy",
		"rainy",
		"lit by flashes of lightning"
	};
	char static *sky_look_han[4] =
	{
		"하늘이 구름 한점 없이 맑습니다.",
		"하늘에 구름이 조금 있습니다.",
		"비가 오고 있습니다.",
		"천둥 번개가 칩니다."
	};

	one_argument(argument, buf2);
	if ((GET_LEVEL(ch) > (IMO + 2)) && buf2[0] != NUL) {
		if (strcmp(buf2, "cloud") == 0)
			weather_change(1);
		else if (strcmp(buf2, "rain") == 0)
			weather_change(2);
		else if (strcmp(buf2, "cloudless") == 0)
			weather_change(3);
		else if (strcmp(buf2, "lightning") == 0)
			weather_change(4);
		return;
	}
	if (OUTSIDE(ch)) {
		snprintf(buf, sizeof(buf), "The sky is %s and %s.\n\r", sky_look[weather_info.sky],
			(weather_info.change >= 0 ?
			 "you feel a warm wind from south" :
			 "your foot tells you bad weather is due"));
		snprintf(buf2, sizeof(buf2), "%s %s.\n\r", sky_look_han[weather_info.sky],
			(weather_info.change >= 0 ?
			 "서편하늘부터 구름이 개이고 있습니다" :
			 "더 궂은 날이 되것 같습니다"));
		send_to_char_han(buf, buf2, ch);
	} else
		send_to_char_han("You have no feeling about the weather at all.\n\r",
				 		 "여기선 날씨가 어떤지 알 수 없습니다.\n\r", ch);
}

void do_help(struct char_data *ch, char *argument, int cmd)
{
	int chk, bot, top, mid, minlen;
	char buf[MAX_STRING_LENGTH], buffer[MAX_STRING_LENGTH];

	if (!ch->desc)
		return;
	for (; isspace(*argument); argument++) ;
	if (*argument) {
		if (!help_index) {
			send_to_char("No help available.\n\r", ch);
			return;
		}
		bot = 0;
		top = top_of_helpt;

		for (;;) {
			mid = (bot + top) / 2;
			minlen = strlen(argument);

			if (!(chk = strn_cmp(argument, help_index[mid].keyword, minlen))) {
				fseek(help_fl, help_index[mid].pos, 0);
				*buffer = '\0';
				for (;;) {
					if (fgets(buf, sizeof(buf), help_fl) == NULL) // 기존: fgets(buf, 80, help_fl);
						break;
					
					if (*buf == '#')
						break;
					
					strlcat(buffer, buf, sizeof(buffer));
					strlcat(buffer, "\r", sizeof(buffer));
				}
				page_string(ch->desc, buffer, 0);
				return;
			} else if (bot >= top) {
				send_to_char("There is no help on that word.\n\r", ch);
				return;
			} else if (chk > 0)
				bot = ++mid;
			else
				top = --mid;
		}
		return;
	}
	send_to_char(help, ch);
}

void do_spells(struct char_data *ch, char *argument, int cmd)
{
	char buf[80 * MAX_SKILLS], tmp[MAX_STRING_LENGTH];
	int no, i;
	
	if (IS_NPC(ch))
		return;
	snprintf(buf, sizeof(buf),
		"NUM     SPELL NAME       MU CL TH WA    MU  CL  TH  WA    MIN_MANA\n\r");
	if (!*argument) {
		for (i = 0; *spells[i] != '\n'; i++) {
			if (*spells[i] == 0)
				continue;
			snprintf(tmp, sizeof(tmp),
				"%3d %-20s%3d%3d%3d%3d  %4d%4d%4d%4d%8d\n\r",
				i,
				spells[i], spell_info[i + 1].min_level[0],
				spell_info[i +
					   1].min_level[1],
				spell_info[i + 1].min_level[2], spell_info[i + 1].min_level[3],
				spell_info[i + 1].max_skill[0], spell_info[i + 1].max_skill[1],
				spell_info[i + 1].max_skill[2], spell_info[i + 1].max_skill[3],
				spell_info[i + 1].min_usesmana);
			strlcat(buf, tmp, sizeof(buf));
		}
		page_string(ch->desc, buf, 1);
	} else {
		no = old_search_block(argument, 1, strlen(argument), spells, FALSE);
		if (no == -1) {
			send_to_char("There's no such skill.\n\r", ch);
			return;
		}
		send_to_char(buf, ch);
		snprintf(tmp, sizeof(tmp), "%3d %-20s%3d%3d%3d%3d  %4d%4d%4d%4d%8d\n\r",
			no - 1,
			spells[no - 1], spell_info[no].min_level[0],
			spell_info[no].min_level[1],
			spell_info[no].min_level[2], spell_info[no].min_level[3],
			spell_info[no].max_skill[0], spell_info[no].max_skill[1],
			spell_info[no].max_skill[2], spell_info[no].max_skill[3],
			spell_info[no].min_usesmana);
		send_to_char(tmp, ch);
	}
}

void do_wizhelp(struct char_data *ch, char *argument, int cmd)
{
	char buf[MAX_STRING_LENGTH];
	int no, i, j, is_wizcmd;
	size_t len = 0;
	
	if (IS_NPC(ch))
		return;
	send_to_char("The following privileged commands are available:\n\r", ch);
	*buf = '\0';

	len += snprintf(buf + len, sizeof(buf) - len, "The following privileged commands are available:\n\r");

	for (no = 1, i = 0; *command[i] != '\n'; i++) {
		is_wizcmd = 1;
		for (j = 0; j < 4; j++) {
			if ((GET_LEVEL(ch) < cmd_info[i + 1].minimum_level[j]) ||
			    (cmd_info[i + 1].minimum_level[j] < IMO)) {
				is_wizcmd = 0;
				break;
			}
		}

		if (is_wizcmd) {
			if (len < sizeof(buf)) {
                len += snprintf(buf + len, sizeof(buf) - len, "%-15s", command[i]);
            }

            if (!(no % 5)) {
                if (len < sizeof(buf)) {
                    len += snprintf(buf + len, sizeof(buf) - len, "\n\r");
                }
            }
			no++;
		}
	}
	if (len < sizeof(buf)) {
        snprintf(buf + len, sizeof(buf) - len, "\n\r");
    }
	page_string(ch->desc, buf, 1);
}

void do_who(struct char_data *ch, char *argument, int cmd)
{
	struct descriptor_data *d;
	char buf[256], buf2[256], class;
	int num_player = 0, tlev = 0, lev = 0, tz = 0, z = 0, flag = 0, condtype;
	int is_korean;
	char page_buffer[25600];
	char *name = NULL;

	buf[0] = buf2[0] = page_buffer[0] = '\0';

	condtype = 0;
	if (argument) {
		while (argument[0] == ' ')
			++argument;

		if (!*argument)
			condtype = 0;
		else if (isalpha(*argument)) {
			/* name search */
			condtype = 5;
			name = argument;
		} else {
			switch (argument[0]) {
			case '+':
				condtype = 1;
				++argument;
				break;
			case '-':
				condtype = 2;
				++argument;
				break;
			case '=':
				condtype = 3;
				++argument;
				break;
			case '*':
				condtype = 4;
				++argument;
				z = world[ch->in_room].zone;
				break;
			default:
				condtype = 1;
			}
		}
		if (condtype > 0 && condtype < 4)
			lev = atoi(argument);
	}

	is_korean = IS_SET(ch->specials.act, PLR_KOREAN);

	snprintf(page_buffer, sizeof(page_buffer), "%s", is_korean ? "사람들\n\r-------\n\r" :
		"Players\n\r-------\n\r");

	for (d = descriptor_list; d; d = d->next) {
		if (!d->connected && CAN_SEE(ch, d->character)) {
			if (condtype) {
				if (condtype < 4)
					tlev = d->original ?
					    GET_LEVEL(d->original) :
					    GET_LEVEL(d->character);
				else if (condtype == 4)
					tz = d->original ?
					    world[d->original->in_room].zone :
					    world[d->character->in_room].zone;
				else {	/* name find */
					name = d->original ?
					    GET_NAME(d->original) :
					    GET_NAME(d->character);
				}

				switch (condtype) {
				case 1:
					flag = (tlev >= lev);
					break;
				case 2:
					flag = (tlev <= lev);
					break;
				case 3:
					flag = (tlev == lev);
					break;
				case 4:
					flag = (tz == z);
					break;
				case 5:
					flag = (isname(argument, name));
				}
				if (!flag)
					continue;
			}
			if (d->original) {	/* If switched */
				switch (GET_CLASS(d->original)) {
				case CLASS_MAGIC_USER:
					class = 'M';
					break;
				case CLASS_CLERIC:
					class = 'C';
					break;
				case CLASS_THIEF:
					class = 'T';
					break;
				case CLASS_WARRIOR:
					class = 'W';
					break;
				default:
					class = 'U';
				}
				if (d->original) {
					if (is_korean)
						snprintf(buf, sizeof(buf),
							"< %c %2d > %s %s", class,
							GET_LEVEL(d->original),
							GET_NAME(d->original), d->original->player.title);
					else
						snprintf(buf, sizeof(buf),
							"< %c %2d > %s %s", class,
							GET_LEVEL(d->original),
							GET_NAME(d->original), d->original->player.title);
				}
			} else {
				switch (GET_CLASS(d->character)) {
				case CLASS_MAGIC_USER:
					class = 'M';
					break;
				case CLASS_CLERIC:
					class = 'C';
					break;
				case CLASS_THIEF:
					class = 'T';
					break;
				case CLASS_WARRIOR:
					class = 'W';
					break;
				default:
					class = 'U';
				}
				if (d->character) {
					if (is_korean)
						snprintf(buf, sizeof(buf),
							"< %c %2d > %s %s", class,
							GET_LEVEL(d->character),
							GET_NAME(d->character), d->character->player.title);
					else
						snprintf(buf, sizeof(buf),
							"< %c %2d > %s %s", class,
							GET_LEVEL(d->character),
							GET_NAME(d->character), d->character->player.title);
					if (IS_SET(d->character->specials.act, PLR_CRIMINAL))
						strlcat(buf, " (CRIMINAL)", sizeof(buf));
				}
				if (GET_LEVEL(ch) < IMO) {
					snprintf(buf2, sizeof(buf2), " PK#(%d)\n\r",
						d->character->player.pk_num);
					strlcat(buf, buf2, sizeof(buf));
				} else if (d->character->player.guild >= 0 &&
					   d->character->player.guild <= MAX_GUILD_LIST) {
					snprintf(buf2, sizeof(buf2), " PK#(%d) %s\n\r",
						d->character->player.pk_num,
						guild_names[(int)d->character->player.guild]);
					strlcat(buf, buf2, sizeof(buf));
				}
			}
			if (!IS_AFFECTED(d->character, AFF_SHADOW_FIGURE)) {
				strlcat(page_buffer, buf, sizeof(page_buffer));
			} else {
				if (GET_LEVEL(ch) < IMO)
					strlcat(page_buffer, "A shadow figure\n\r", sizeof(page_buffer));
				else
					strlcat(page_buffer, buf, sizeof(page_buffer));
			}
			num_player++;
		}
	}
	snprintf(buf, sizeof(buf), is_korean ?
		"\n\r%d 명이 있습니다.\n\r" :
		"\n\rYou can see %d players.\n\r", num_player);
	strlcat(page_buffer, buf, sizeof(page_buffer));
	page_string(ch->desc, page_buffer, 1);
}


void do_users(struct char_data *ch, char *argument, int cmd)
{
	char line[MAX_STRING_LENGTH], line2[MAX_STRING_LENGTH];
	struct descriptor_data *d;
	int total_cnt = 0;    /* m: 전체 접속자 수 */
    int visible_cnt = 0;  /* n: 내 눈에 보이는 접속자 수 */
    int flag, uptime;
    size_t len = 0;       /* 현재 버퍼에 써진 길이 추적용 */
	static int most = 0;
	
	one_argument(argument, line);
	flag = ((GET_LEVEL(ch) < IMO) || (strcmp("-t", line) == 0));
	line[0] = '\0';
    len = 0;
	for (d = descriptor_list; d; d = d->next) {
		total_cnt++;
		if (flag) continue;
		if (d->original) {
			if (!CAN_SEE(ch, d->original)) continue;

			if (len < sizeof(line)) {
                len += snprintf(line + len, sizeof(line) - len, 
                        "%3d%2d:%-14s%2d ", 
                        d->descriptor, 
                        d->original->specials.timer,
                        GET_NAME(d->original), 
                        GET_LEVEL(d->original));
            }
		} else if (d->character) {
			if (!CAN_SEE(ch, d->character)) continue;
			if (len < sizeof(line)) {
                len += snprintf(line + len, sizeof(line) - len, 
                        "%3d%2d:%-14s%2d ", 
                        d->descriptor, 
                        d->character->specials.timer,
                        (d->connected == CON_PLAYING) ? GET_NAME(d->character) : "Not in game",
                        GET_LEVEL(d->character));
            }
		} else {
			if (len < sizeof(line)) {
                len += snprintf(line + len, sizeof(line) - len, 
                        "%3d%9s%10s ", d->descriptor, " UNDEF ", 
                        connected_types[d->connected]);
            }
		}
			
		if (len < sizeof(line)) {
            len += snprintf(line + len, sizeof(line) - len, "%-15s", d->host);
        }
		if (!(visible_cnt % 2)) {
            if (len < sizeof(line)) {
                len += snprintf(line + len, sizeof(line) - len, " | ");
            }
        } else { /* 홀수(1, 3...)번째 사람은 줄바꿈 후 전송 */
            if (len < sizeof(line)) {
                len += snprintf(line + len, sizeof(line) - len, "\n\r");
            }
            send_to_char(line, ch);
            
            /* 전송했으니 버퍼 리셋 */
            line[0] = '\0';
            len = 0;
        }
        visible_cnt++;
	}
	if ((!flag) && (visible_cnt % 2)) {
        if (len < sizeof(line)) {
            len += snprintf(line + len, sizeof(line) - len, "\n\r");
        }
        send_to_char(line, ch);
    }

    /* 최고 접속자 갱신 */
    if (total_cnt > most) most = total_cnt;

    /* 통계 출력 */
    snprintf(line, sizeof(line), "%s%d/%d active connections\n\r", (visible_cnt % 2) ? "\n\r" : "", total_cnt, most);
             
    if (GET_LEVEL(ch) > IMO + 2)
        send_to_char(line, ch);

    /* 업타임 출력 */
    uptime = 30 + time(0) - boottime;
    snprintf(line, sizeof(line), "Running time %d:%02d\n\r", uptime / 3600, (uptime % 3600) / 60);
    snprintf(line2, sizeof(line2), "현재 %d시간 %02d분 지났습니다.\n\r", uptime / 3600, (uptime % 3600) / 60);
	send_to_char_han(line, line2, ch);
}
void do_inventory(struct char_data *ch, char *argument, int cmd)
{
	send_to_char_han("You are carrying:\n\r",
			 "가지고 있는 물건들:\n\r", ch);
	list_obj_to_char(ch->carrying, ch, 1, TRUE);
}
void do_equipment(struct char_data *ch, char *argument, int cmd)
{
	int j;
	bool found;

	send_to_char_han("You are using:\n\r", "쓰고 있는 물건들:\n\r", ch);
	found = FALSE;
	for (j = 0; j < MAX_WEAR; j++) {
		if (ch->equipment[j]) {
			if (CAN_SEE_OBJ(ch, ch->equipment[j])) {
				send_to_char(where[j], ch);
				show_obj_to_char(ch->equipment[j], ch, 1);
				found = TRUE;
			} else {
				send_to_char(where[j], ch);
				send_to_char("Something.\n\r", ch);
				found = TRUE;
			}
		}
	}
	if (!found) {
		send_to_char_han(" Nothing.\n\r",
				 " 아무것도 없네요.\n\r", ch);
	}
}

void do_credits(struct char_data *ch, char *argument, int cmd)
{
	page_string(ch->desc, credits, 0);
}


/* Hot Reloading test on NEWS */
void load_news_if_changed() {
    struct stat file_info;
    FILE *fl;
    char filename[] = "news";

    // 파일의 상태 확인
    if (stat(filename, &file_info) != 0) {
        // 파일이 없으면 에러 처리
        if (news_content) free(news_content);
        news_content = strdup("뉴스가 없습니다.\r\n");
        return;
    }

    // 마지막으로 읽은 시간과 파일의 수정 시간(st_mtime) 비교
    if (news_content != NULL && news_last_mod == file_info.st_mtime) {
        return;
    }

    // 파일이 바뀌었다면 다시 읽음
    if (!(fl = fopen(filename, "r"))) {
        mudlog("SYSERR: 뉴스 파일을 열 수 없습니다.");
        return;
    }

    if (news_content) free(news_content);

    news_content = (char *)malloc(file_info.st_size + 1);
    
    fread(news_content, 1, file_info.st_size, fl);
    news_content[file_info.st_size] = '\0'; // 문자열 끝 처리

    news_last_mod = file_info.st_mtime;
    
    fclose(fl);
    mudlog("INFO: 뉴스 파일이 갱신되어 새로 로딩했습니다.");
}

void do_news(struct char_data *ch, char *argument, int cmd)
{
	load_news_if_changed(); // 파일 확인 및 로딩 (변경된 경우에만 실제로 읽음)

	if(news_content) 
		page_string(ch->desc, news_content, 0); // send_to_char(news_content, ch);
	else
		page_string(ch->desc, news, 0); // send_to_char(news_content, ch);

}

void do_plan(struct char_data *ch, char *argument, int cmd)
{
	page_string(ch->desc, plan, 0);
}
void do_wizards(struct char_data *ch, char *argument, int cmd)
{
	page_string(ch->desc, wizards, 0);
}
void do_where(struct char_data *ch, char *argument, int cmd)
{
	char name[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH], buf2[256];
	struct char_data *i;
	int j, n;
	struct obj_data *k;
	struct descriptor_data *d;

	one_argument(argument, name);
	n = 0;
	if (!*name) {
		if (GET_LEVEL(ch) < IMO) {
			for (d = descriptor_list; d; d = d->next) {
				if (d->character && (d->connected ==
				    CON_PLAYING) &&
				    (d->character->in_room != NOWHERE))
					if (CAN_SEE(ch, d->character) &&
					    world[d->character->in_room].zone ==
					    world[ch->in_room].zone) {
						snprintf(buf, sizeof(buf),
							"%-20s - %s [%d] \n\r",
							d->character->player.name,
							world[d->character->in_room].name,
							world[d->character->in_room].number);
						
						if (d->character &&
						    (!IS_AFFECTED(d->character,
								  AFF_SHADOW_FIGURE) ||
						     d->character == ch) &&
						    (!IS_AFFECTED(d->character,
						     AFF_HIDE) ||
						     d->character == ch))
							send_to_char(buf, ch);
					}
			}
			return;
		} else {
			for (d = descriptor_list; d; d = d->next) {
				if (d->character && (d->connected ==
				    CON_PLAYING) &&
				    (d->character->in_room != NOWHERE))
					if (CAN_SEE(ch, d->character)) {
						if (d->original)	/* If switched */
							snprintf(buf, sizeof(buf),
								"%-20s - %s [%d] In body of %s\n\r",
								d->original->player.name,
								world[d->character->in_room].name,
								world[d->character->in_room].number,
								fname(d->character->player.name));
						else
							snprintf(buf, sizeof(buf),
								"%-20s - %s [%d]\n\r",
								d->character->player.name,
								world[d->character->in_room].name,
								world[d->character->in_room].number);
						if (d->character)
							send_to_char(buf, ch);
					}
			}
			return;
		}
	}
	*buf = '\0';
	for (i = character_list; i; i = i->next)
		if (isname(name, i->player.name) && CAN_SEE(ch, i)) {
			if ((i->in_room != NOWHERE) && ((GET_LEVEL(ch) >= IMO) ||
							(world[i->in_room].zone ==
							 world[ch->in_room].zone))) {
				if (IS_NPC(i))
					snprintf(buf, sizeof(buf), "%2d: %-30s- %s ", ++n, i->player.short_descr,
						world[i->in_room].name);
				else
					snprintf(buf, sizeof(buf), "%2d: %-30s- %s ", ++n, i->player.name,
						world[i->in_room].name);
				if (GET_LEVEL(ch) >= IMO)
					snprintf(buf2, sizeof(buf2), "[%d]\n\r",
						world[i->in_room].number);
				else
					strcpy(buf2, "\n\r");
				strcat(buf, buf2);
				if (i && (!IS_AFFECTED(i, AFF_SHADOW_FIGURE)
					  || i == ch ||
					  GET_LEVEL(ch) >= IMO))
					send_to_char(buf, ch);
				if (GET_LEVEL(ch) < IMO)
					break;
			}
		}
	if (GET_LEVEL(ch) >= IMO) {
		for (i = character_list; i; i = i->next)
			for (j = 0; j < MAX_WEAR; ++j)
				if (i->equipment[j])
					if (isname(name, i->equipment[j]->name)) {
						snprintf(buf, sizeof(buf),
							"%2d: %s worn by %s\n\r",
							++n,
							i->equipment[j]->short_description,
							GET_NAME(i));
						send_to_char(buf, ch);
					}
		for (k = object_list; k; k = k->next)
			if (isname(name, k->name)) {
				if (k->carried_by) {
					snprintf(buf, sizeof(buf),
						"%2d: %s carried by %s.\n\r",
						++n, k->short_description,
						get_char_name(k->carried_by, ch));
					send_to_char(buf, ch);
				} else if (k->in_obj) {
					snprintf(buf, sizeof(buf), "%2d: %s in %s", ++n, k->short_description,
						k->in_obj->short_description);
					if (k->in_obj->carried_by) {
						snprintf(buf2, sizeof(buf2),
							" carried by %s\n\r", get_char_name
							(k->in_obj->carried_by, ch));
						strcat(buf, buf2);
					} else
						strcat(buf, "\n\r");
					send_to_char(buf, ch);
				} else if (k->in_room != NOWHERE) {
					snprintf(buf, sizeof(buf), "%2d: %s in %s [%d]\n\r",
						++n, k->short_description,
						world[k->in_room].name,
						world[k->in_room].number);
					send_to_char(buf, ch);
				}
			}
	}
	if (!*buf)
		send_to_char("Couldn't find any such thing.\n\r", ch);
}

void do_levels(struct char_data *ch, char *argument, int cmd)
{
	int i;
	char buf[MAX_STRING_LENGTH];
	
	if (IS_NPC(ch)) {
		send_to_char("You ain't nothin' but a hound-dog.\n\r", ch);
		return;
	}
	*buf = '\0';
	for (i = 1; i < IMO; i++) {
		sprintf(buf + strlen(buf), "%2d: %11ld to %11ld: ", i,
			titles[GET_CLASS(ch) - 1][i].exp,
			titles[GET_CLASS(ch) - 1][i + 1].exp);
		switch (GET_SEX(ch)) {
		case SEX_MALE:
			strcat(buf, titles[GET_CLASS(ch) - 1][i].title_m);
			break;
		case SEX_FEMALE:
			strcat(buf, titles[GET_CLASS(ch) - 1][i].title_f);
			break;
		default:
			send_to_char("Oh dear.\n\r", ch);
			break;
		}
		strcat(buf, "\n\r");
	}
	send_to_char(buf, ch);
}

void do_consider(struct char_data *ch, char *argument, int cmd)
{
	struct char_data *victim;
	char name[256];
	int diff;

	one_argument(argument, name);

	if (!(victim = get_char_room_vis(ch, name))) {
		send_to_char("Consider killing who?\n\r", ch);
		return;
	}

	if (victim == ch) {
		send_to_char("Easy! Very easy indeed!\n\r", ch);
		return;
	}

	if (!IS_NPC(victim)) {
		send_to_char("Would you like to borrow a cross and a shovel?\n\r", ch);
		return;
	}

	diff = (GET_LEVEL(victim) - GET_LEVEL(ch));

	if (diff <= -10)
		send_to_char("Now where did that chicken go?\n\r", ch);
	else if (diff <= -5)
		send_to_char("You could do it with a needle!\n\r", ch);
	else if (diff <= -2)
		send_to_char("Easy.\n\r", ch);
	else if (diff <= -1)
		send_to_char("Fairly easy.\n\r", ch);
	else if (diff == 0)
		send_to_char("The perfect match!\n\r", ch);
	else if (diff <= 1)
		send_to_char("You would need some luck!\n\r", ch);
	else if (diff <= 2)
		send_to_char("You would need a lot of luck!\n\r", ch);
	else if (diff <= 3)
		send_to_char("You would need a lot of luck and great equipment!\n\r", ch);
	else if (diff <= 5)
		send_to_char("Do you feel lucky, punk?\n\r", ch);
	else if (diff <= 10)
		send_to_char("Are you mad!?\n\r", ch);
	else if (diff <= 100)
		send_to_char("You ARE mad!\n\r", ch);
}

void do_police(struct char_data *ch, char *argument, int cmd)
{
	char name[200];
	struct descriptor_data *d;
	int i, target;
	void stash_char(struct char_data *ch);
	void wipe_obj(struct obj_data *obj);
	struct obj_data *unequip_char(struct char_data *ch, int pos);

	one_argument(argument, name);
	if (!*argument)
		return;
	target = atoi(name);
	for (d = descriptor_list; d; d = d->next) {
		if (target == d->descriptor) {
			snprintf(name, sizeof(name), "Policed: %d\n", d->descriptor);
			mudlog(name);
			if ((d->connected == CON_PLAYING) && (d->character)) {
				if (d->character->player.level < ch->player.level) {
					stash_char(d->character);
					move_stashfile_safe(d->character->player.name);
					for (i = 0; i < MAX_WEAR; i++)
						if (d->character->equipment[i]) {
							extract_obj(unequip_char(d->character, i));
							d->character->equipment[i]
							    = 0;
						}
					if (d->character->carrying)
						wipe_obj(d->character->carrying);
					d->character->carrying = 0;
					close_socket(d);
					DEBUG_LOG("act.informative.c extract char(%s)", d->character);
					extract_char(d->character, TRUE);
				}
			} else {
				close_socket(d);
			}
		}
	}
}


/*
 * 함수명: do_siteban (구 do_wizlock), 251121 by Komo
* */
void do_siteban(struct char_data *ch, char *argument, int cmd)
{
    char buf[MAX_INPUT_LENGTH];
    char out_buf[MAX_STRING_LENGTH];
    int i, j;

    buf[0] = '\0';
    out_buf[0] = '\0';
    one_argument(argument, buf);

    if (*buf) {
        j = -1;
        
        for (i = 0; i < baddoms; ++i)
            if (strcmp(baddomain[i], buf) == 0) {
                j = i;
                break;
            }

        if (j >= 0) {
            baddoms--;
            if (j != baddoms) {
                memmove(baddomain[j], baddomain[baddoms], sizeof(baddomain[0]));
                baddomain[j][sizeof(baddomain[0]) - 1] = '\0';
            }
            
            snprintf(out_buf, sizeof(out_buf), "Domain '%s' removed from siteban list.\n\r", buf);
            send_to_char(out_buf, ch);
        } else {
            if (baddoms < BADDOMS) {
                strncpy(baddomain[baddoms], buf, sizeof(baddomain[0]) - 1);
                baddomain[baddoms][sizeof(baddomain[0]) - 1] = '\0';
                baddoms++;
				
				snprintf(out_buf, sizeof(out_buf), "Domain '%s' added to siteban list.\n\r", buf);
                send_to_char(out_buf, ch);
            } else {
                send_to_char("Siteban list is full.\n\r", ch);
            }
        }
    } else { // 인자가 없다면 -> 현재 리스트 보여주기
        send_to_char("Currently sitebanned domains:\n\r", ch);
        if (baddoms == 0) {
            send_to_char("  None.\n\r", ch);
            return;
        }
        
        for (i = 0; i < baddoms; ++i) {
            snprintf(out_buf, sizeof(out_buf), "  %s\n\r", baddomain[i]);
            send_to_char(out_buf, ch);
		}
	}
}
void do_nochat(struct char_data *ch, char *argument, int cmd)
{
	if (IS_NPC(ch))
		return;

	if (IS_SET(ch->specials.act, PLR_NOCHAT)) {
		send_to_char("You can now hear chats again.\n\r", ch);
		REMOVE_BIT(ch->specials.act, PLR_NOCHAT);
	} else {
		send_to_char("From now on, you won't hear chats.\n\r", ch);
		SET_BIT(ch->specials.act, PLR_NOCHAT);
	}
}
void do_notell(struct char_data *ch, char *argument, int cmd)
{
	if (IS_NPC(ch))
		return;

	if (IS_SET(ch->specials.act, PLR_NOTELL)) {
		send_to_char("You can now hear tells again.\n\r", ch);
		REMOVE_BIT(ch->specials.act, PLR_NOTELL);
	} else {
		send_to_char("From now on, you won't hear tells.\n\r", ch);
		SET_BIT(ch->specials.act, PLR_NOTELL);
	}
}
void do_data(struct char_data *ch, char *argument, int cmd)
{
	struct descriptor_data *d;
	struct char_data *victim;
	char buf[256], name[256], fmt[16];
	int i = 0, k, t = 0, nc;
	LONGLONG n = 0;

	one_argument(argument, name);
	if (strcmp(name, "exp") == 0)
		k = 1;
	else if (strcmp("hit", name) == 0)
		k = 2;
	else if (strcmp("gold", name) == 0)
		k = 3;
	else if (strcmp("armor", name) == 0)
		k = 4;
	else if (strcmp("age", name) == 0) {
		k = 5;
		t = time(0);
	} else if (strcmp("com", name) == 0) {
		k = 6;
		t = time(0);
	} else if (strcmp("flags", name) == 0)
		k = 7;
	else if (strcmp("bank", name) == 0)
		k = 8;
	else if (strcmp("des", name) == 0)
		k = 9;
	else
		k = 10;
	if ((k == 1) || (k == 3) || (k == 7) || (k == 8)) {
		nc = 3;
		strcpy(fmt, "%-12s%13lld%s");
	} else {
		nc = 4;
		strcpy(fmt, "%-12s%6lld%s");
	}
	for (d = descriptor_list; d; d = d->next) {
		if (!d->connected && CAN_SEE(ch, d->character)) {
			++i;
			if (d->original)	/* If switched */
				victim = d->original;
			else
				victim = d->character;
			switch (k) {
			case 1:
				n = victim->points.exp;
				break;
			case 2:
				n = victim->points.max_hit;
				break;
			case 3:
				n = victim->points.gold;
				break;
			case 4:
				n = victim->points.armor;
				break;
			case 5:
				n = (t - victim->player.time.birth) / 86400;
				break;
			case 6:
				n = (60 * victim->desc->ncmds) / (1 + t -
								  victim->desc->contime);
				break;
			case 7:
				n = victim->specials.act;
				break;
			case 8:
				n = victim->bank;
				break;
			case 9:
				n = d->descriptor;
				break;
			case 10:
				n = victim->player.level;
			}
			snprintf(buf, sizeof(buf), fmt, GET_NAME(victim), n, (i % nc) ? "|"
				: "\n\r");
			send_to_char(buf, ch);
		}
	}
	if (i % nc)
		send_to_char("\n\r", ch);
}

void do_hangul(struct char_data *ch, char *argument, int cmd)
{
	if (IS_NPC(ch))
		return;

	if (IS_SET(ch->specials.act, PLR_KOREAN)) {
		send_to_char("English command message mode.\n\r", ch);
		REMOVE_BIT(ch->specials.act, PLR_KOREAN);
	} else {
		send_to_char("You can now see hangul command messages.\n\r", ch);
		SET_BIT(ch->specials.act, PLR_KOREAN);
	}
}

void do_solo(struct char_data *ch, char *argument, int cmd)
{
	if (IS_NPC(ch))
		return;
	if (IS_SET(ch->specials.act, PLR_SOLO)) {
		send_to_char("Now,others can follow you.", ch);
		REMOVE_BIT(ch->specials.act, PLR_SOLO);
	} else {
		send_to_char("Now,others cannot follow you.", ch);
		SET_BIT(ch->specials.act, PLR_SOLO);
	}
}
