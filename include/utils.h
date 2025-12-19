#pragma once
/* ************************************************************************
*  file: utils.h, Utility module.                         Part of DIKUMUD *
*  Usage: Utility macros                                                  *
************************************************************************* */

#include <ctype.h>
#include <string.h>


// comm.h에서 이동
/* SEND_TO_Q 단순 함수 호출로 변경 */
#define SEND_TO_Q(messg, desc)  send_to_q_color((messg), (desc))

// from daerimsa.c
#define SON_OGONG_STEP		(son_ogong->quest.solved)
#define FOURTH_JANGRO_STEP	(fourth_jangro->quest.solved)

// from db.c
#define ZCMD zone_table[zone].cmd[cmd_no]

// from interpreter.c
#define STATE(d) ((d)->connected)
#define COMMANDO(number, min_pos, pointer, min_lm, min_lc, min_lt, min_lw) { \
        cmd_info[(number)].command_pointer = (pointer);                    \
        cmd_info[(number)].minimum_position = (min_pos);                   \
        cmd_info[(number)].minimum_level[0] = (min_lm);                    \
        cmd_info[(number)].minimum_level[1] = (min_lc);                    \
        cmd_info[(number)].minimum_level[2] = (min_lt);                    \
        cmd_info[(number)].minimum_level[3] = (min_lw);                    \
}

// from limit.c
#define READ_TITLE(ch) (GET_SEX(ch) == SEX_MALE ?   \
    titles[(int)GET_CLASS(ch) - 1][(int)GET_LEVEL(ch)].title_m :  \
    titles[(int)GET_CLASS(ch) - 1][(int)GET_LEVEL(ch)].title_f)

// from mob_magic.c
#define VICT_IS_SAME_ROOM(mob)	(mob->in_room==mob->specials.fighting->in_room)

// from spec_procs.c
#define FUDGE (100 + dice(6, 20))


/* ---------- original UTILS.H ---------- */
#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

// Removed +1 kuldge
#define CREATE(result, type, number)  \
    do { \
        if (!((result) = (type *) calloc((number), sizeof(type)))) { \
            perror("malloc failure"); abort(); \
        } \
    } while(0)

#define RECREATE(result,type,number) \
    do { \
        if (!((result) = (type *) realloc((result), sizeof(type) * (number)))) { \
            perror("realloc failure"); abort(); \
        } \
    } while(0)

#define CAP(st)  (*(st) = toupper(*(st)))

#define IS_SET(flag,bit)  ((flag) & (bit))
#define IS_AFFECTED(ch,skill) ( IS_SET((ch)->specials.affected_by, (skill)) )
#define IS_DARK(room)  (!world[room].light && IS_SET(world[room].room_flags, DARK))
#define IS_LIGHT(room)  (world[room].light || !IS_SET(world[room].room_flags, DARK))
#define SET_BIT(var,bit)  ((var) = (var) | (bit))
#define REMOVE_BIT(var,bit)  ((var) = (var) & ~(bit) )

#define ISLETTER(c) (isgraph((unsigned char)(c)))
#define IF_STR(st)  ((st) ? (st) : "\0")
#define ISNEWL(ch)  ((ch) == '\n' || (ch) == '\r')

#define IS_NPC(ch)  (IS_SET((ch)->specials.act, ACT_ISNPC))
#define IS_MOB(ch)  (IS_SET((ch)->specials.act, ACT_ISNPC) && ((ch)->nr >-1))

#define CARRY_WEIGHT_BASE	500
#define GET_POS(ch)     ((ch)->specials.position)
#define GET_COND(ch, i) ((ch)->specials.conditions[(i)])
#define GET_NAME(ch)    ((ch)->player.name)
#define GET_TITLE(ch)   ((ch)->player.title)
#define GET_LEVEL(ch)   ((ch)->player.level)
#define GET_CLASS(ch)   ((ch)->player.class)
#define GET_AGE(ch)     (age(ch).year)
#define GET_STR(ch)     ((ch)->tmpabilities.str)
#define GET_ADD(ch)     ((ch)->tmpabilities.str_add)
#define GET_DEX(ch)     ((ch)->tmpabilities.dex)
#define GET_INT(ch)     ((ch)->tmpabilities.intel)
#define GET_WIS(ch)     ((ch)->tmpabilities.wis)
#define GET_CON(ch)     ((ch)->tmpabilities.con)
#define GET_REMORTAL(ch)   ((ch)->player.remortal)
#define GET_AC(ch)      ((ch)->points.armor)
#define GET_HIT(ch)     ((ch)->points.hit)
#define GET_MAX_HIT(ch) (hit_limit(ch))
#define GET_MOVE(ch)    ((ch)->points.move)
#define GET_MAX_MOVE(ch) (move_limit(ch))
#define GET_MANA(ch)    ((ch)->points.mana)
#define GET_MAX_MANA(ch) (mana_limit(ch))
#define GET_GOLD(ch)    ((ch)->points.gold)
#define GET_EXP(ch)     ((ch)->points.exp)
#define GET_HEIGHT(ch)  ((ch)->player.height)
#define GET_WEIGHT(ch)  ((ch)->player.weight)
#define GET_SEX(ch)     ((ch)->player.sex)
#define GET_HITROLL(ch) ((ch)->points.hitroll)
#define GET_DAMROLL(ch) ((ch)->points.damroll)
#define GET_REGENERATION(ch) ((ch)->regeneration)
#define GET_PLAYER_MAX_HIT(ch) ((ch)->points.max_hit)
#define GET_PLAYER_MAX_MOVE(ch) ((ch)->points.max_move)
#define GET_PLAYER_MAX_MANA(ch) ((ch)->points.max_mana)
#define AWAKE(ch) (GET_POS(ch) > POSITION_SLEEPING)
#define WAIT_STATE(ch, cycle)  ((ch)->desc ? (ch)->desc->wait = (cycle) : 0)

#define STRENGTH_APPLY_INDEX(ch) \
        ( ((GET_ADD(ch) == 0) || (GET_STR(ch) != 18)) ? GET_STR(ch) : \
          (GET_ADD(ch) <= 50) ? 26 :( \
          (GET_ADD(ch) <= 75) ? 27 :( \
          (GET_ADD(ch) <= 90) ? 28 :( \
          (GET_ADD(ch) <= 99) ? 29 :  30 ) ) )                   \
        )

/* Can subject see character "obj"? */
#define OMNI(sub) (!IS_NPC(sub) && (GET_LEVEL(sub) >= IMO))

#define X99(sub, obj)  ( ((!IS_AFFECTED((obj), AFF_INVISIBLE) || \
        IS_AFFECTED((sub), AFF_DETECT_INVISIBLE)) && !IS_AFFECTED((sub), AFF_BLIND) ) && \
        (IS_NPC(obj) || !IS_SET((obj)->specials.act,PLR_WIZINVIS)) && \
        (OMNI(sub) || IS_LIGHT(sub->in_room) || IS_AFFECTED(sub,AFF_INFRAVISION)) )

#define CAN_SEE(sub,obj) (X99(sub,obj)||(OMNI(sub) && (IS_NPC(obj) || \
   (GET_LEVEL(sub) >= GET_LEVEL(obj)))))

/* Object And Carry related macros */
#define X98(sub, obj)                                    \
  ( (( !IS_SET((obj)->obj_flags.extra_flags, ITEM_INVISIBLE) ||   \
       IS_AFFECTED((sub),AFF_DETECT_INVISIBLE) ) &&               \
       !IS_AFFECTED((sub),AFF_BLIND)) &&    \
     ( IS_LIGHT(sub->in_room) || IS_AFFECTED((sub),AFF_INFRAVISION)) )

#define CAN_SEE_OBJ(sub,obj) (X98(sub,obj)||OMNI(sub))

#define CAN_WEAR(obj, part) (IS_SET((obj)->obj_flags.wear_flags,part))
#define CAN_CARRY_W(ch) (str_app[STRENGTH_APPLY_INDEX(ch)].carry_w + CARRY_WEIGHT_BASE)
#define CAN_CARRY_N(ch) (5+GET_DEX(ch)/2+GET_LEVEL(ch)/2)
#define CAN_CARRY_OBJ(ch,obj)  \
   (((IS_CARRYING_W(ch) + GET_OBJ_WEIGHT(obj)) <= CAN_CARRY_W(ch)) &&   \
    ((IS_CARRYING_N(ch) + 1) <= CAN_CARRY_N(ch)))
#define CAN_GET_OBJ(ch, obj)   \
   (CAN_WEAR((obj), ITEM_TAKE) && CAN_CARRY_OBJ((ch),(obj)) &&          \
    CAN_SEE_OBJ((ch),(obj)))
#define GET_ITEM_TYPE(obj) ((obj)->obj_flags.type_flag)
#define GET_OBJ_WEIGHT(obj) ((obj)->obj_flags.weight)
#define IS_CARRYING_W(ch) ((ch)->specials.carry_weight)
#define IS_CARRYING_N(ch) ((ch)->specials.carry_items)
#define IS_OBJ_STAT(obj,stat) (IS_SET((obj)->obj_flags.extra_flags,stat))


#define OUTSIDE(ch) (!IS_SET(world[(ch)->in_room].room_flags,INDOORS))
#define EXIT(ch, door)  (world[(ch)->in_room].dir_option[door])
#define CAN_GO(ch, door) (EXIT(ch,door)  &&  (EXIT(ch,door)->to_room != NOWHERE) \
                          && !IS_SET(EXIT(ch, door)->exit_info, EX_CLOSED))
#define GET_ALIGNMENT(ch) ((ch)->specials.alignment)

#define IS_GOOD(ch)    (GET_ALIGNMENT(ch) >= 350)
#define IS_EVIL(ch)    (GET_ALIGNMENT(ch) <= -350)
#define IS_NEUTRAL(ch) (!IS_GOOD(ch) && !IS_EVIL(ch))

/* quest */
#define GET_QUEST_TYPE(ch) (ch->quest.type)
#define GET_QUEST_DATA(ch) (ch->quest.data)
#define GET_QUEST_SOLVED(ch) (ch->quest.solved)

/* skilled */
#define GET_LEARNED(ch, sk_no)		((ch)->skills[(int)(sk_no)].learned)
#define GET_SKILLED(ch, sk_no)		((ch)->skills[(int)(sk_no)].skilled)

/* max skilled = 100 */
/* jhpark, skilled increase시 같은 길드멤버가 아닌지 확인 */
#define INCREASE_SKILLED(ch, victim, sk_no)	\
    do { \
        if((IS_NPC(victim) || ch == victim) || (ch->player.guild != victim->player.guild)) {	\
            if(!number(0,99+(GET_SKILLED(ch,sk_no)<<1))) {    \
                if(GET_SKILLED(ch,sk_no)<100) {         \
                    GET_SKILLED(ch,sk_no)=GET_SKILLED(ch,sk_no)+1;       \
                    print_increased_skilled(ch,sk_no);   \
                }	\
            }	\
        } \
    } while(0)

#define INCREASE_SKILLED2(ch, victim, sk_no)	\
    do { \
        if((IS_NPC(victim) || ch == victim) || (ch->player.guild != victim->player.guild)) {    \
            if(!number(0,29+(GET_SKILLED(ch,sk_no)))) {       \
                if(GET_SKILLED(ch,sk_no)<100) {          \
                    GET_SKILLED(ch,sk_no)=GET_SKILLED(ch,sk_no)+1;	\
                    print_increased_skilled(ch,sk_no);           \
                }	\
            }	\
        } \
    } while(0)

#define INCREASE_SKILLED1(ch, victim, sk_no) \
    do { \
        if((IS_NPC(victim) || ch == victim) || (ch->player.guild != victim->player.guild)) {    \
            if(!number(0,250+(GET_SKILLED(ch,sk_no)<<2))) {   \
                if(GET_SKILLED(ch,sk_no)<100) {          \
                    GET_SKILLED(ch,sk_no)=GET_SKILLED(ch,sk_no)+1;       \
                    print_increased_skilled(ch,sk_no);   \
                }	\
            }	\
        } \
    } while(0)

/* Unit Macros, 251218 */
#define K(x) ((long)(x) * 1000)         /* Kilo, 1천 */
#define M(x) ((long)(x) * 1000000)      /* Mega, 백만 */
#define G(x) ((long)(x) * 1000000000)   /* Giga, 10억 */
