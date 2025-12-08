
/* ************************************************************************
*  file: act.comm.c , Implementation of commands.         Part of DIKUMUD *
*  Usage : Communication.                                                 *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete information. *
************************************************************************* */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "spells.h"

#include <sys/time.h>

/* extern variables */
extern struct room_data *world;
extern struct descriptor_data *descriptor_list;

/* extern functions */
int file_to_string(char *name, char *buf);  /* in db.c */
void prune_crlf(char *txt);                                     /* in utility.c 251130 */
void utf8_safe_strncpy(char *dest, const char *src, size_t n); /* in utility.c 251130 */

#define MAX_HISTORY_MSG 512  // 넉넉한 크기
#define HISTORY_SIZE 100     // 20 -> 100으로 확장, 251124 by Komo
static char history[HISTORY_SIZE][MAX_HISTORY_MSG];
static int his_start = 0, his_end = 0;
static FILE *chatlogfp = NULL;

/* 말하기 */
void do_say(struct char_data *ch, char *argument, int cmd)
{
	char buf[MAX_STRING_LENGTH];

    if (IS_SET(ch->specials.act, PLR_DUMB_BY_WIZ) && GET_LEVEL(ch) < IMO + 3) {
        return;
    }

    prune_crlf(argument); // 251130 by Komo
    for (; *argument == ' '; argument++);

    if (!*argument)
        send_to_char("Yes, but WHAT do you want to say?\n\r", ch);
    else {
        snprintf(buf, sizeof(buf), "You say '%s'\n\r", argument);
        send_to_char(buf, ch);
        snprintf(buf, sizeof(buf), "$n says '%s'", argument);
        act(buf, FALSE, ch, 0, 0, TO_ROOM);
    }
}

void do_sayh(struct char_data *ch, char *argument, int cmd)
{				/* hangul say */
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];

    if (IS_SET(ch->specials.act, PLR_DUMB_BY_WIZ) && GET_LEVEL(ch) < IMO + 3) {
        return;
    }

    prune_crlf(argument); // 251130 by Komo
    for (; *argument == ' '; argument++);

    if (!*argument)
        send_to_char_han("Yes, but WHAT do you want to say?\n\r", "예? 뭐라고 말해요?\n\r", ch);
    else {
        snprintf(buf, sizeof(buf), "You say '%s'\n\r", argument);
        snprintf(buf2, sizeof(buf2), "'%s' 하고 말합니다\n\r",argument);
        /* English - Korean display act() , english text first.. */
        send_to_char_han(buf, buf2, ch);

        snprintf(buf, sizeof(buf), "$n says '%s'", argument);
        snprintf(buf2, sizeof(buf2), "$n 님이 '%s' 하고 말합니다", argument);
        acthan(buf, buf2, FALSE, ch, 0, 0, TO_ROOM);
    }
}

/* 채팅 : 히스토리와 로그 파일에 저장하는 공통 함수 */
void save_chat_history(struct char_data *ch, char *argument)
{
    char buf[MAX_HISTORY_MSG];      // 날짜용
    char temp_msg[MAX_INPUT_LENGTH]; // 전체 메시지 조립용 임시 버퍼
    time_t tt = time(NULL);
    
    strftime(buf, sizeof(buf), "%F %H:%M", localtime(&tt));
    
    snprintf(temp_msg, sizeof(temp_msg), "%s %s> %s\n\r", &buf[5], GET_NAME(ch), argument);

    utf8_safe_strncpy(history[his_end], temp_msg, MAX_HISTORY_MSG);

    his_end = (his_end + 1) % HISTORY_SIZE;

    if (his_end == his_start) {
        his_start = (his_start + 1) % HISTORY_SIZE;
    }

    // 파일 로그 저장
    if (chatlogfp == NULL)
        chatlogfp = fopen(CHATLOG, "a");

    if (chatlogfp) {
        fprintf(chatlogfp, "%s %s> %s\n", buf, GET_NAME(ch), argument); 
        fflush(chatlogfp);
    }
}

/* 외치기 */
void do_shout(struct char_data *ch, char *argument, int cmd)
{
	char buf[MAX_STRING_LENGTH], buf1[MAX_STRING_LENGTH];
    struct descriptor_data *i;
    extern int noshoutflag;

    if (IS_SET(ch->specials.act, PLR_DUMB_BY_WIZ) && GET_LEVEL(ch) < IMO + 3) {
        send_to_char("Your mouth moves, but no sound comes out! (You have been silenced by the Gods)\n\r", ch);
        return;
    }
    if (noshoutflag && !IS_NPC(ch) && (GET_LEVEL(ch) < IMO)) {
        send_to_char("The air is still. The Gods have forbidden shouting at this moment.\n\r", ch);
        return;
    }
    if (!IS_NPC(ch))
        if (IS_SET(ch->specials.act, PLR_NOSHOUT)) {
            send_to_char("You try to shout, but your voice fails you. (The Gods have taken your ability to shout)\n\r", ch);
            return;
        }
    
    prune_crlf(argument); // 251130 by Komo
    for (; *argument == ' '; argument++);

    if (!(*argument))
        send_to_char("Shout? Yes! Fine! Shout we must, but WHAT??\n\r", ch);
    else {
        snprintf(buf, sizeof(buf), "You shout '%s'\n\r", argument);
        send_to_char(buf, ch);

        snprintf(buf1, sizeof(buf1), "$n shouts '%s'", argument);

        save_chat_history(ch, argument);

        for (i = descriptor_list; i; i = i->next) {
            if (i->character != ch && !i->connected && i->character && !IS_SET(i->character->specials.act, PLR_EARMUFFS))
                act(buf1, 0, ch, 0, i->character, TO_VICT);
        }
    }
}

/*
void do_chat(struct char_data *ch, char *argument, int cmd)
{
    struct descriptor_data *d;
    char buf[MAX_STRING_LENGTH];
    
    extern int nochatflag;

    // 예외 처리
    if (IS_SET(ch->specials.act, PLR_DUMB_BY_WIZ) && GET_LEVEL(ch) < IMO + 3) {
        send_to_char("You try to speak into the orb, but it remains dark. (You have been silenced by the Gods)\n\r", ch);
        return;
    }
    if (nochatflag) {
        send_to_char("The global telepathic channels are currently closed by the Gods.\n\r", ch);
        return;
    }
    if (IS_NPC(ch)) {
        send_to_char("몬스터는 채팅 채널을 사용할 수 없습니다.\r\n", ch);
        return;
    }
    if (ch->desc && ch->desc->original) {
        send_to_char("몬스터 몸에 있을 때에는 이 명령을 사용하실 수 없습니다.\r\n", ch);
        return;
    }

    prune_crlf(argument); // 251130 by Komo
    for (; *argument == ' '; argument++);
    
    save_chat_history(ch, argument);

    snprintf(buf, sizeof(buf), "%s> %s\n\r", GET_NAME(ch), argument);
    
    for (d = descriptor_list; d; d = d->next) {
        if (d->connected) continue;

        struct char_data *victim = d->character;
        if (victim && !IS_SET(victim->specials.act, PLR_NOCHAT)) { // 대상이 존재하고, PLR_NOCHAT 상태가 아니라면 전송
             send_to_char(buf, victim);
        }
    }
}
*/

void do_lastchat(struct char_data *ch, char *argument, int cmd)
{
    int j;

    for (j = his_start; j != his_end; j = (j + 1) % HISTORY_SIZE) {
        // 내용이 있는 경우만 출력
        if (history[j][0] != '\0')
            send_to_char(history[j], ch);
    }
    return;
}

/* 얘기하기 */
void do_tell(struct char_data *ch, char *argument, int cmd)
{
	struct char_data *vict;
	char *s, name[100], message[MAX_INPUT_LENGTH], buf[MAX_INPUT_LENGTH + 100];

	if (IS_SET(ch->specials.act, PLR_DUMB_BY_WIZ) && GET_LEVEL(ch) < IMO + 3) {
		return;
	}
	prune_crlf(argument); // 251130 by Komo
	half_chop(argument, name, message);
	if (!*name || !*message)
		send_to_char("Who do you wish to tell what??\n\r", ch);
	else if (!(vict = get_char_vis(ch, name)))
		send_to_char("No-one by that name here..\n\r", ch);
	else if (ch == vict)
		send_to_char("You try to tell yourself something.\n\r", ch);
	else if (GET_POS(vict) == POSITION_SLEEPING) {
		act("$E can't hear you.", FALSE, ch, 0, vict, TO_CHAR);
	} else if ((!IS_SET(vict->specials.act, PLR_NOTELL)) ||
		   ((GET_LEVEL(ch) >= IMO) && (GET_LEVEL(ch) > GET_LEVEL(vict)))) {
		if (IS_NPC(ch))
			s = ch->player.short_descr;
		else
			s = CAN_SEE(vict, ch) ? GET_NAME(ch) : "Someone";
		snprintf(buf, sizeof(buf), "%s tells you '%s'\n\r", s, message);
		send_to_char(buf, vict);
		snprintf(vict->specials.reply_who, sizeof(vict->specials.reply_who), "%s", GET_NAME(ch));
		snprintf(buf, sizeof(buf), "You tell %s '%s'\n\r", GET_NAME(vict), message);
		send_to_char(buf, ch);
	} else {
		act("$E isn't listening now.", FALSE, ch, 0, vict, TO_CHAR);
	}
}
void do_reply(struct char_data *ch, char *argument, int cmd)
{
	char buf[MAX_STRING_LENGTH];
	snprintf(buf, sizeof(buf), "%s %s", ch->specials.reply_who, argument);
	do_tell(ch, buf, 0);
}

/* 보내기 */
void do_send(struct char_data *ch, char *argument, int cmd)
{
	struct char_data *vict;
	char *s, name[100], paint_name[300], message[200], buf[MAX_STRING_LENGTH],
	paint[MAX_STRING_LENGTH];

	if (IS_SET(ch->specials.act, PLR_DUMB_BY_WIZ) && GET_LEVEL(ch) < IMO + 3) {
		return;
	}
	prune_crlf(argument); // 251130 by Komo
	half_chop(argument, name, message);
	snprintf(paint_name, sizeof(paint_name), "paints/%s", message);
	if (!*name || !*message)
		send_to_char("Who do you wish to tell what??\n\r", ch);
	else if (!(vict = get_char_vis(ch, name)))
		send_to_char("No-one by that name here..\n\r", ch);
	else if (file_to_string(paint_name, paint) == -1)
		send_to_char("No such paint prepared.\n\r", ch);
	else if ((!IS_SET(vict->specials.act, PLR_NOTELL)) ||
		 ((GET_LEVEL(ch) >= IMO) && (GET_LEVEL(ch) > GET_LEVEL(vict)))) {
		send_to_char(paint, vict);
		snprintf(buf, sizeof(buf), "You send %s %s\n\r", GET_NAME(vict), message);
		send_to_char(buf, ch);
		if (IS_NPC(ch))
			s = ch->player.short_descr;
		else
			s = CAN_SEE(vict, ch) ? GET_NAME(ch) : "Someone";
		snprintf(buf, sizeof(buf), "%s sends you '%s'\n\r", s, message);
		send_to_char(buf, vict);
	} else {
		act("$E isn't listening now.", FALSE, ch, 0, vict, TO_CHAR);
	}
}
/*  그룹 말하기 */
void do_gtell(struct char_data *ch, char *argument, int cmd)
{
	struct char_data *k;
	struct follow_type *f;
	char *s, buf[200];

	if (IS_SET(ch->specials.act, PLR_DUMB_BY_WIZ) && GET_LEVEL(ch) < IMO + 3) {
		return;
	}
	prune_crlf(argument); // 251130 by Komo
	if (!(k = ch->master))
		k = ch;
	if (IS_NPC(ch))
		s = ch->player.short_descr;
	else
		s = GET_NAME(ch);
	if (k && IS_AFFECTED(k, AFF_GROUP)) {
		snprintf(buf, sizeof(buf), "**&u %s &n** '&X%s&n'\n\r", s, argument);
		send_to_char(buf, k);
	}
	for (f = k->followers; f; f = f->next) {
		if (f->follower && IS_AFFECTED(f->follower, AFF_GROUP)) {
			snprintf(buf, sizeof(buf), "**&u %s &n** '&X%s&n'\n\r", s, argument);
			send_to_char(buf, f->follower);
		}
	}
}
/* 속삭이기 */
void do_whisper(struct char_data *ch, char *argument, int cmd)
{
	struct char_data *vict;
	char name[100], message[MAX_INPUT_LENGTH], buf[MAX_INPUT_LENGTH + 100];

	if (IS_SET(ch->specials.act, PLR_DUMB_BY_WIZ) && GET_LEVEL(ch) < IMO + 3) {
		return;
	}
	prune_crlf(argument); // 251130 by Komo
	half_chop(argument, name, message);

	if (!*name || !*message)
		send_to_char("Who do you want to whisper to.. and what??\n\r", ch);
	else if (!(vict = get_char_room_vis(ch, name)))
		send_to_char("No-one by that name here..\n\r", ch);
	else if (vict == ch) {
		act("$n whispers quietly to $mself.", FALSE, ch, 0, 0, TO_ROOM);
		send_to_char(
				    "You can't seem to get your mouth close enough to your ear...\n\r",
				    ch);
	} else {
		snprintf(buf, sizeof(buf), "$n whispers to you, '%s'", message);
		act(buf, FALSE, ch, 0, vict, TO_VICT);
/*    send_to_char("Ok.\n\r", ch); */
		act("$n whispers something to $N.", FALSE, ch, 0, vict, TO_NOTVICT);
	}
}

/* 물어보기 */
void do_ask(struct char_data *ch, char *argument, int cmd)
{
	struct char_data *vict;
	char name[100], message[MAX_INPUT_LENGTH], buf[MAX_INPUT_LENGTH + 100];

	if (IS_SET(ch->specials.act, PLR_DUMB_BY_WIZ) && GET_LEVEL(ch) < IMO + 3) {
		return;
	}
	prune_crlf(argument); // 251130 by Komo
	half_chop(argument, name, message);

	if (!*name || !*message)
		send_to_char("Who do you want to ask something.. and what??\n\r", ch);
	else if (!(vict = get_char_room_vis(ch, name)))
		send_to_char("No-one by that name here..\n\r", ch);
	else if (vict == ch) {
		act("$n quietly asks $mself a question.", FALSE, ch, 0, 0, TO_ROOM);
		send_to_char("You think about it for a while...\n\r", ch);
	} else {
		snprintf(buf, sizeof(buf), "$n asks you '%s'", message);
		act(buf, FALSE, ch, 0, vict, TO_VICT);
/*    send_to_char("Ok.\n\r", ch); */
		act("$n asks $N a question.", FALSE, ch, 0, vict, TO_NOTVICT);
	}
}

#define MAX_NOTE_LENGTH MAX_STRING_LENGTH	/* arbitrary */
/* 쓰기 */
void do_write(struct char_data *ch, char *argument, int cmd)
{
	struct obj_data *paper = 0, *pen = 0;
	char papername[MAX_INPUT_LENGTH], penname[MAX_INPUT_LENGTH], buf[MAX_INPUT_LENGTH + 100];

	if (IS_SET(ch->specials.act, PLR_DUMB_BY_WIZ) && GET_LEVEL(ch) < IMO + 3) {
		return;
	}
	argument_interpreter(argument, papername, penname);

	if (!ch->desc)
		return;

	if (!*papername)	/* nothing was delivered */
	{
		send_to_char(
				    "Write? with what? ON what? what are you trying to do??\n\r", ch);
		return;
	}
	if (*penname)		/* there were two arguments */
	{
		if (!(paper = get_obj_in_list_vis(ch, papername, ch->carrying))) {
			snprintf(buf, sizeof(buf), "You have no %s.\n\r", papername);
			send_to_char(buf, ch);
			return;
		}
		if (!(pen = get_obj_in_list_vis(ch, penname, ch->carrying))) {
			snprintf(buf, sizeof(buf), "You have no %s.\n\r", penname);
			send_to_char(buf, ch);
			return;
		}
	} else
		/* there was one arg.let's see what we can find */
	{
		if (!(paper = get_obj_in_list_vis(ch, papername, ch->carrying))) {
			snprintf(buf, sizeof(buf), "There is no %s in your inventory.\n\r", papername);
			send_to_char(buf, ch);
			return;
		}
		if (paper->obj_flags.type_flag == ITEM_PEN)	/* oops, a pen.. */
		{
			pen = paper;
			paper = 0;
		} else if (paper->obj_flags.type_flag != ITEM_NOTE) {
			send_to_char("That thing has nothing to do with writing.\n\r", ch);
			return;
		}

		/* one object was found. Now for the other one. */
		if (!ch->equipment[HOLD]) {
			snprintf(buf, sizeof(buf), "You can't write with a %s alone.\n\r", papername);
			send_to_char(buf, ch);
			return;
		}
		if (!CAN_SEE_OBJ(ch, ch->equipment[HOLD])) {
			send_to_char("The stuff in your hand is invisible! Yeech!!\n\r", ch);
			return;
		}

		if (pen)
			paper = ch->equipment[HOLD];
		else
			pen = ch->equipment[HOLD];
	}

	/* ok.. now let's see what kind of stuff we've found */
	if (pen->obj_flags.type_flag != ITEM_PEN) {
		act("$p is no good for writing with.", FALSE, ch, pen, 0, TO_CHAR);
	} else if (paper->obj_flags.type_flag != ITEM_NOTE) {
		act("You can't write on $p.", FALSE, ch, paper, 0, TO_CHAR);
	} else if (paper->action_description)
		send_to_char("There's something written on it already.\n\r", ch);
	else {
		/* we can write - hooray! */

		send_to_char("Ok.. go ahead and write.. end the note with a @.\n\r",
			     ch);
		act("$n begins to jot down a note.", TRUE, ch, 0, 0, TO_ROOM);
		ch->desc->str = &paper->action_description;
		ch->desc->max_str = MAX_NOTE_LENGTH;
		ch->desc->title = 0;
	}
}
