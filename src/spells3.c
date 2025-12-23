
/* ************************************************************************
*  file: spells3.c , Implementation of magic.             Part of NARAIMUD*
*  Usage : All the non-offensive magic handling routines.                 *
************************************************************************* */

#include "structs.h"
#include "utils.h"
#include "db.h"
#include "interpreter.h"
#include "spells.h"
#include "handler.h"



/*
cast 'spell block' victim
by process
*/
void cast_spell_block(byte level, struct char_data *ch, char *arg,
		      int type, struct char_data *tar_ch, struct obj_data *tar_obj)
{
	switch (type) {
	case SPELL_TYPE_SPELL:
		if (number(1, 10) > 5)
			spell_spell_block(level, ch, tar_ch, 0);
		break;
	default:
		break;
	}
}
void cast_dumb(byte level, struct char_data *ch, char *arg,
	       int type, struct char_data *tar_ch, struct obj_data *tar_obj)
{
	switch (type) {
	case SPELL_TYPE_SPELL:
		spell_dumb(level, ch, tar_ch, 0);
		break;
	default:
		break;
	}
}
void cast_reflect_damage(byte level, struct char_data *ch, char *arg,
			 int type, struct char_data *tar_ch, struct obj_data *tar_obj)
{
	switch (type) {
	case SPELL_TYPE_SPELL:
		spell_reflect_damage(level, ch, tar_ch, 0);
		break;
	default:
		break;
	}
}
void cast_holy_shield(byte level, struct char_data *ch, char *arg,
		      int type, struct char_data *tar_ch, struct obj_data *tar_obj)
{
	switch (type) {
	case SPELL_TYPE_SPELL:
		spell_holy_shield(level, ch, tar_ch, 0);
		break;
	default:
		break;
	}
}
/*
cast 'mana transfer' victim
by process
*/
void cast_mana_transfer(byte level, struct char_data *ch, char *arg,
			int type, struct char_data *tar_ch, struct obj_data *tar_obj)
{
	switch (type) {
	case SPELL_TYPE_SPELL:
		spell_mana_transfer(level, ch, tar_ch, 0);
		break;
	default:
		break;
	}
}
/*
cast 'kiss of process' no_victim
by process
*/
void cast_kiss_of_process(byte level, struct char_data *ch, char *arg,
			  int type, struct char_data *tar_ch, struct obj_data *tar_obj)
{
	switch (type) {
	case SPELL_TYPE_SPELL:
		if (GET_MANA(ch) < 30000) {
			send_to_char("You do not have enough mana\n\r", ch);
			return;
		}
		GET_HIT(ch) = 1;
		GET_MANA(ch) = 1;
		GET_MOVE(ch) = 1;
	case SPELL_TYPE_WAND:
	case SPELL_TYPE_STAFF:
	case SPELL_TYPE_SCROLL:
		spell_kiss_of_process(level, ch, 0, 0);
		break;
	default:
		break;
	}
}

void cast_thunder_bolt(byte level, struct char_data *ch, char *arg,
		       int type, struct char_data *tar_ch, struct obj_data *tar_obj)
{
	switch (type) {
	case SPELL_TYPE_SPELL:
		spell_thunder_bolt(level, ch, tar_ch, 0);
		break;
	default:
		break;
	}
}
