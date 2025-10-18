
/* ************************************************************************
*  file: limits.h , Limit/Gain control module             Part of DIKUMUD *
*  Usage: declaration of title type                                       *
************************************************************************* */

/* Public Procedures */
long int mana_limit(struct char_data *ch);
long int hit_limit(struct char_data *ch);
long int move_limit(struct char_data *ch);

struct title_type {
	char *title_m;
	char *title_f;
	long int exp;
};
