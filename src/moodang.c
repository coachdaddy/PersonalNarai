/* ************************************************************************
*  file: moodang.c , Special module.                  					  *
*  Usage: Procedures handling special procedures for moodang 			  *
************************************************************************* */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "structs.h"
#include "utils.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "spells.h"
#include "limit.h"

struct char_data *jangsambong;

#define JANG_SAMBONG	18000

#define TAICHI_STEP		(jangsambong->quest.solved)

#define GREAT_POTION 18001
#define TAICHI_ORB 18002
#define YANG_ORB 2839
#define YANG_SEED 17060
#define YIN_ORB 11135
#define YIN_SEED 16112

int jang_sambong_func(struct char_data *ch, int cmd, char *arg)
{
	struct obj_data *i, *obj;
	struct char_data *victim, *mob;
// Step
// WOOGIL - YANG_SEED 
// JWAJA - YIN_SEED
// HWATA - YANG_ORB  
// Juji - YIN_ORB
	switch (TAICHI_STEP) {
		case 0:
		case 1:
		case 2:
		// hold TAICHI_ORB
	}

 	return 0;
}

int great_potion(struct char_data *ch, int cmd, char *arg)
{
	char buf[100];
	struct obj_data *obj;
	int pumping  = 0;

	if (cmd != 12)
		return FALSE;

	/* eat */
	one_argument(arg, buf);
	if (!(obj = get_obj_in_list_vis(ch, buf, ch->carrying)))
		return FALSE;

	if (obj_index[obj->item_number].virtual != GREAT_POTION)
		return FALSE;

	pumping = GET_MAX_HIT(ch) * 2 ;
	GET_HIT(ch) += pumping;

	DEBUG_LOG("Player %s quaf great potion to get %d ",  GET_NAME(ch), pumping);

	act("신비한 기운이 흐릅니다. ...", TRUE, ch, 0, 0, TO_ROOM);
	send_to_char("갑자기 새로운 힘이 느껴집니다.\n\r", ch);

	extract_obj(obj);

	return TRUE;
}

int taichi_orb(struct char_data *ch, int cmd, char *arg)
{
	char buf[100];
	struct obj_data *obj;

	if (cmd != 12)
		return FALSE;

	/* eat */
	one_argument(arg, buf);
	if (!(obj = get_obj_in_list_vis(ch, buf, ch->carrying)))
		return FALSE;

	if (obj_index[obj->item_number].virtual != TAICHI_ORB)
		return FALSE;

	GET_MAX_HIT(ch) += 1000;
	GET_MAX_MANA(ch) += 1000;
	GET_MAX_MOVE(ch) += 1000;

	DEBUG_LOG("Player %s eat TAICHI Orb",  GET_NAME(ch));

	act("뭔가 미묘한 맛입니다...", TRUE, ch, 0, 0, TO_ROOM);
	send_to_char("갑자기 새로운 힘이 느껴집니다.\n\r", ch);

	extract_obj(obj);

	return TRUE;
}

