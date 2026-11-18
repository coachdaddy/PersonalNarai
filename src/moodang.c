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

#define JANG_SAMBONG	18000

#define GREAT_POTION 18001
#define TAICHI_ORB 18002
#define YANG_HALF 18003
#define YIN_HALF 18004
#define YANG_ORB 2839			// HWATA - YANG_ORB  
#define YANG_SEED 17060			// WOOGIL - YANG_SEED 
#define YIN_ORB 11135			// Juji - YIN_ORB
#define YIN_SEED 16112			// JWAJA - YIN_SEED

/*
 * 장삼봉의 태극 구슬 만들기 (2026-11-18 Komo, coachdaddy의 처음 코드에서 의도를 추정해 고침)
 *
 *   음 씨앗(YIN_SEED)  + 양 구슬(YANG_ORB) -> 음 반쪽(YIN_HALF)
 *   양 씨앗(YANG_SEED) + 음 구슬(YIN_ORB)  -> 양 반쪽(YANG_HALF)
 *   양 반쪽 + 음 반쪽                       -> 태극 구슬(TAICHI_ORB)
 * 만든 것은 바닥에 내려놓는다. 씨앗만 있고 짝이 되는 구슬이 없으면 무엇이 빠졌는지 알려 준다.
 *
 * 처음 코드에서 고친 것
 * - 전역 포인터 jangsambong에 값을 넣는 곳이 없어 항상 NULL이었다. 장삼봉 방에서 누가 아무 명령이나
 *   치면 NULL을 읽어 서버가 죽었다. 대림사(daerimsa_self)처럼 같은 방의 장삼봉을 찾아 쓴다.
 * - case에 break가 없어 단계가 이어서 실행되고, 짝을 못 찾으면(seed·orb NULL) NULL을 읽었다.
 * - 양 반쪽 단계가 음 씨앗·양 구슬을 찾고 나서 양 씨앗·음 구슬인지 비교해 절대 만들어지지 않았다.
 * - 태극 구슬을 만들기만 하고 어디에도 두지 않았다.
 * - 1/5 확률로 플레이어 명령을 삼켰다(return 1).
 * 단계 변수 없이 들고 있는 물건만 보고 판단하므로 순서와 상관없이 짝이 맞으면 만든다.
 */
static struct char_data *jang_sambong_self(struct char_data *ch, int cmd)
{
	struct char_data *k;

	if (!ch)
		return NULL;
	if (!cmd)
		return ch;	/* 몹 AI 호출이면 ch가 장삼봉 */
	if (ch->in_room == NOWHERE)
		return NULL;
	for (k = world[ch->in_room].people; k; k = k->next_in_room)
		if (IS_NPC(k) && k->nr >= 0 && mob_index[k->nr].virtual == JANG_SAMBONG)
			return k;
	return NULL;
}

static struct obj_data *carried_vnum(struct char_data *ch, int vnum)
{
	struct obj_data *i;

	for (i = ch->carrying; i; i = i->next_content)
		if (i->item_number >= 0 && obj_index[i->item_number].virtual == vnum)
			return i;
	return NULL;
}

/* a와 b를 없애고 vnum 물건을 만들어 장삼봉 방 바닥에 둔다 */
static void taichi_combine(struct char_data *mob, struct obj_data *a, struct obj_data *b, int vnum)
{
	struct obj_data *obj;

	if (!(obj = read_object(vnum, VIRTUAL))) {
		DEBUG_LOG("SYSERR: (jang_sambong) object %d does not exist.", vnum);
		return;
	}
	extract_obj(a);
	extract_obj(b);
	obj_to_room(obj, mob->in_room);

	acthan("JangSamBong mixes them deeply, and puts $p on the floor.",
	       "장삼봉이 둘을 깊이 어우르더니 $p 하나를 바닥에 내려놓습니다.",
	       FALSE, mob, obj, 0, TO_ROOM);
	DEBUG_LOG("(jang_sambong) made object %d", vnum);
}

static void taichi_work(struct char_data *mob)
{
	struct obj_data *seed, *orb, *yin, *yang;

	/* 음 씨앗 + 양 구슬 -> 음 반쪽 */
	if ((seed = carried_vnum(mob, YIN_SEED))) {
		if ((orb = carried_vnum(mob, YANG_ORB)))
			taichi_combine(mob, seed, orb, YIN_HALF);
		else
			acthan("Jangsambong says \"Hmm.. It's missing bright part. !!!\".",
			       "장삼봉이 양의 성질이 빠져 있다고 말합니다.",
			       FALSE, mob, 0, 0, TO_ROOM);
	}

	/* 양 씨앗 + 음 구슬 -> 양 반쪽 */
	if ((seed = carried_vnum(mob, YANG_SEED))) {
		if ((orb = carried_vnum(mob, YIN_ORB)))
			taichi_combine(mob, seed, orb, YANG_HALF);
		else
			acthan("Jangsambong says \"Hmm.. It's missing dark part. !!!\".",
			       "장삼봉이 음의 성질이 빠져 있다고 말합니다.",
			       FALSE, mob, 0, 0, TO_ROOM);
	}

	/* 양 반쪽 + 음 반쪽 -> 태극 구슬 */
	yang = carried_vnum(mob, YANG_HALF);
	yin = carried_vnum(mob, YIN_HALF);
	if (yang && yin)
		taichi_combine(mob, yang, yin, TAICHI_ORB);
}

int jang_sambong_func(struct char_data *ch, int cmd, char *arg)
{
	struct char_data *jangsambong;

	/* 플레이어 명령은 give(72)만 본다. 다른 명령은 그대로 통과시킨다. */
	if (cmd && cmd != 72)
		return FALSE;
	if (!(jangsambong = jang_sambong_self(ch, cmd)))
		return FALSE;

	if (!cmd) {
		/* 몹 AI 호출(ACT_SPEC이 켜져 있을 때만): 가끔 궁리하고, 들고 있는 것으로 만들어 본다 */
		if (jangsambong->specials.fighting)
			return FALSE;
		if (number(1, 5) == 1)
			acthan("Sambong thinks deeply about taichi",
			       "장삼봉이 태극의 원리에 대해 궁리하고 있습니다.",
			       FALSE, jangsambong, 0, 0, TO_ROOM);
		taichi_work(jangsambong);
		return FALSE;
	}

	/* give: 먼저 주는 것을 처리하고, 받은 뒤 곧바로 만들어 본다(손오공 wear와 같은 방식) */
	if (ch == jangsambong)
		return FALSE;
	do_give(ch, arg, cmd);
	taichi_work(jangsambong);
	return TRUE;
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

	if (obj->item_number < 0 || obj_index[obj->item_number].virtual != GREAT_POTION)
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

	/* 2026-11-18: 처음에는 GREAT_POTION 번호를 확인해서 양 반쪽을 먹어도 아무 일이 없었다 */
	if (obj->item_number < 0 || obj_index[obj->item_number].virtual != YANG_HALF)
		return FALSE;

	pumping = GET_MAX_MANA(ch) + 5 ;
	GET_MANA(ch) -= pumping;

	DEBUG_LOG("Player %s eat YANG half-orb, mana -%d",  GET_NAME(ch), pumping);

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

	if (obj->item_number < 0 || obj_index[obj->item_number].virtual != YIN_HALF)
		return FALSE;

	pumping = GET_MAX_MOVE(ch) + 5 ;
	GET_MOVE(ch) -= pumping;

	DEBUG_LOG("Player %s eat YIN half-orb, move -%d",  GET_NAME(ch), pumping);

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

	if (obj->item_number < 0 || obj_index[obj->item_number].virtual != TAICHI_ORB)
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

