/* ************************************************************************
*  file: comm.h , Communication module.                   Part of DIKUMUD *
*  Usage: Prototypes for the common functions in comm.c                   *
************************************************************************* */
#ifndef _COMM_H_
#define _COMM_H_

#include "structs.h"

void send_to_all(char *messg);
void send_to_char(char *messg, struct char_data *ch);
void send_to_char_han(char *messg_eng, char *msg_han, struct char_data *ch);
void send_to_room(char *messg, int room);
void act(char *str, int hide_invisible, struct char_data *ch,
	 struct obj_data *obj, void *vict_obj, int type);
void acthan(char *seng, char *shan, int hide_invisible, struct char_data *ch,
	    struct obj_data *obj, void *vict_obj, int type);
void write_to_q(char *txt, struct txt_q *queue);
int write_to_descriptor(int desc, char *txt);
void process_color_string(const char *input, char *output, int max_out_len); // utility.c


#define TO_ROOM    0
#define TO_VICT    1
#define TO_NOTVICT 2
#define TO_CHAR    3

/* SEND_TO_Q 단순 함수 호출로 변경 */
#define SEND_TO_Q(messg, desc)  send_to_q_color((messg), (desc))

#endif /* _COMM_H_ */