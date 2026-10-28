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

#define GREAT_POTION 18001
#define TAICHI_ORB 18002
#define YANG_HALF 18003
#define YIN_HALF 18004
#define YANG_ORB 2839			// HWATA - YANG_ORB  
#define YANG_SEED 17060			// WOOGIL - YANG_SEED 
#define YIN_ORB 11135			// Juji - YIN_ORB
#define YIN_SEED 16112			// JWAJA - YIN_SEED

int jang_sambong_func(struct char_data *ch, int cmd, char *arg)
{
	struct obj_data *i, *obj;
	struct obj_data *seed, *orb;
	struct obj_data *yin, *yang;
	
	struct char_data *victim;

	DEBUG_LOG("TAICHI Orb start");

		switch (TAICHI_STEP) {
			case 0:
				if (number(1, 5) == 1) {
					acthan("Sambong thinks deeply about taichi", 
							"태극의 원리에 대해 궁리하고 있습니다.!",
				 	     	FALSE, ch, 0, 0, TO_ROOM);
					return 1;
				}
			case 1:
				for (i = jangsambong->carrying; i; i = i->next_content) {
					if (obj_index[i->item_number].virtual == YIN_SEED ) {
						acthan( "Jangsambong says \"Hmm.. It's missing bright part. !!!\".",
							 "장삼봉이 양의 성질이 빠져 있다고 말합니다. ", 
							 FALSE, jangsambong, 0, 0, TO_ROOM);
							TAICHI_STEP = 2;
					}
				}
			case 2:
				seed = orb = NULL;
				for (i = jangsambong->carrying; i; i = i->next_content) { 
					if (obj_index[i->item_number].virtual == YIN_SEED)
						seed = i;
					if (obj_index[i->item_number].virtual == YANG_ORB)
						orb = i;
				}
			
				if (obj_index[seed->item_number].virtual == YIN_SEED 
					&& obj_index[orb->item_number].virtual == YANG_ORB ) {
					extract_obj(seed);
					extract_obj(orb);
					obj = read_object(YIN_HALF, VIRTUAL);
					obj_to_room(obj, jangsambong->in_room);
					TAICHI_STEP = 3;
				}
			case 3:
				for (i = jangsambong->carrying; i; i = i->next_content) {
					if (obj_index[i->item_number].virtual == YIN_SEED ) {
						acthan( "Jangsambong says \"Hmm.. It's missing dark part. !!!\".",
			 					"장삼봉이 음의 성질이 빠져 있다고 말합니다. ", 
								FALSE, jangsambong, 0, 0, TO_ROOM);
						TAICHI_STEP = 4;
					}
				}
			case 4:
				seed = orb = NULL;
				for (i = jangsambong->carrying; i; i = i->next_content) {
					if (obj_index[i->item_number].virtual == YIN_SEED)
						seed = i;
					if (obj_index[i->item_number].virtual == YANG_ORB)
						orb = i;
				}

				if (obj_index[seed->item_number].virtual == YANG_SEED 
					&& obj_index[orb->item_number].virtual == YIN_ORB ) {
					extract_obj(seed);
					extract_obj(orb);
					obj = read_object(YANG_HALF, VIRTUAL);
					obj_to_room(obj, jangsambong->in_room);
					TAICHI_STEP = 5;
				}	
			case 5:
			 	yin = yang = NULL;
				for (i = jangsambong->carrying; i; i = i->next_content) {
					if (obj_index[i->item_number].virtual == YANG_HALF)
						yang = i;
					if (obj_index[i->item_number].virtual == YIN_HALF)
						yin = i;
				}

				if (obj_index[yang->item_number].virtual == YANG_HALF 
					&& obj_index[yin->item_number].virtual == YIN_HALF ) {
					extract_obj(yang);
					extract_obj(yin);
					obj = read_object(TAICHI_ORB, VIRTUAL);
				}
				break;
		}
 	return 0;
}

int great_potion(struct char_data *ch, int cmd, char *arg)
{
	char buf[100];
	struct obj_data *obj;
	int pumping  = 0;

	if (cmd != 206)
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

int yang_half(struct char_data *ch, int cmd, char *arg)
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

	pumping = GET_MAX_MANA(ch) + 5 ;
	GET_MANA(ch) -= pumping;

	DEBUG_LOG("Player %s quaf great potion to get %d ",  GET_NAME(ch), pumping);

	act("신비한 기운이 흐릅니다. ...", TRUE, ch, 0, 0, TO_ROOM);
	send_to_char("갑자기 모든 기운이 사라집니다...\n\r", ch);

	extract_obj(obj);

	return TRUE;
}

int yin_half(struct char_data *ch, int cmd, char *arg)
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

	pumping = GET_MAX_MOVE(ch) + 5 ;
	GET_MOVE(ch) -= pumping;

	DEBUG_LOG("Player %s quaf great potion to get %d ",  GET_NAME(ch), pumping);

	act("신비한 기운이 흐릅니다. ...", TRUE, ch, 0, 0, TO_ROOM);
	send_to_char("온 몸을 움직일 수 없습니다.\n\r", ch);

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

	ch->points.max_hit += 1000;
	ch->points.max_mana += 1000;
	ch->points.max_move += 1000;

	DEBUG_LOG("Player %s eat TAICHI Orb",  GET_NAME(ch));

	act("뭔가 미묘한 맛입니다...", TRUE, ch, 0, 0, TO_ROOM);
	send_to_char("갑자기 새로운 힘이 느껴집니다.\n\r", ch);

	extract_obj(obj);

	return TRUE;
}

