/* 
	Quest Management module(source file)
		made by atre@paradise.kaist.ac.kr at 1995/11/09
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdlib.h>

#include "structs.h"
#include "utils.h"
#include "db.h"
#include "comm.h" // acthan, send_to_char_han 등

#define MaxQuest		10000
#define QUEST_FILE		"mob.quest"

/* for Challenge Room Quest System */
#define QUEST_ROOM_VNUM 3081           // 퀘스트 룸 VNUM
#define CHALLENGE_ROOM_START_VNUM 3082 // '도전의 방' 시작 VNUM
#define CHALLENGE_ROOM_END_VNUM 3089   // '도전의 방' 끝 VNUM (총 8개)
extern struct room_data *world;

int number(int from, int to);
void send_to_char_han(char *msgeng, char *msghan, struct char_data *ch);
void log(char *str);
void half_chop(char *string, char *arg1, char *arg2);
struct obj_data *get_obj_in_list_vis(struct char_data *ch, char *name,
				     struct obj_data *list);
void send_to_char(char *messg, struct char_data *ch);
void extract_obj(struct obj_data *obj);
void obj_to_char(struct obj_data *o, struct char_data *ch);

struct {
	int virtual;
	int level;
	char *name;
} QM[MaxQuest];

int topQM;

int level_quest[IMO + 4] =
{
	0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 10 */
	2, 3, 4, 5, 6, 7, 8, 9, 10, 11,		/* 20 */
	12, 13, 14, 15, 17, 19, 21, 23, 25, 27,		/* 30 */
	29, 31, 33, 35, 37, 39, 41, 43, 45, 47,		/* 40 */
	49, 51, 53, 55, 57, 59, 61, 63, 65, 67,		/* 50 */
	69, 72, 75, 78, 81, 84, 87, 90, 93, 96, 	/* 60 */
	120, 130, 140, 150
};

// Quest 보상 아이템의 종류를 명확하게 알아볼 수 있도록
enum QuestRewardTypes {
    REWARD_GOLD = 1,
    REWARD_REQ_TICKET = 3,
    REWARD_ARMOR = 5,
    REWARD_WEAPON = 6,
    REWARD_AC_TICKET = 7,
    REWARD_HR_TICKET = 8,
    REWARD_DR_TICKET = 9,
    REWARD_LIGHT = 10
};

// 각 보상과 그 확률을 묶어서 정의
struct QuestReward {
    int reward_id;
    int probability; // 1000을 기준으로 한 확률 (예: 100 = 10%)
};


extern struct index_data *mob_index;

#define ZONE_NUMBER 47
static struct {
	int number;
	char *name;
} zone_data[ZONE_NUMBER] = {

	{ 99, "the LIMBO" },
	{ 299, "the East Castle" },
	{ 1399, "the Mel's Dog-House" },
	{ 1499, "the Houses" },
	{ 1599, "Dirk's Castle" },
	{ 1799, "SHIRE" },
	{ 1899, "The Jale" },
	{ 1999, "The Lands" },
	{ 2099, "Process' Castle" },
	{ 2199, "The Wasteland" },
	{ 2300, "Dragon Tower" },
	{ 2399, "Muncie" },
	{ 2699, "The Corporation" },
	{ 2799, "The NeverLand" },
	{ 2899, "Kingdom Of Chok" },
	{ 2999, "The Keep Of MahnTor" },
	{ 3099, "Northern Midgaard MainCity" },
	{ 3199, "Southern Part Of Midgaard" },
	{ 3299, "River Of Midgaard" },
	{ 3499, "Graveyard" },
	{ 4199, "Moria" },
	{ 4330, "The Wamphyri Aerie" },
	{ 5099, "The Great Eastern Desert" },
	{ 5199, "Drow City" },
	{ 5299, "The City Of Thalos" },
	{ 6099, "HAON DOR LIGHT" },
	{ 6499, "HAON DOR DARK" },
	{ 6999, "The Dwarven Kingdom" },
	{ 7099, "SEWER" },
	{ 7199, "Second SEWER" },
	{ 7399, "SEWER MAZE" },
	{ 7899, "The Tunnels" },
	{ 7999, "Redfernes Residence" },
	{ 9099, "Arachnos" },
	{ 9499, "Arena" },
	{ 9699, "Death Kingdom" },
	{ 9771, "Galaxy" },
	{ 9851, "The Death Star" },
	{ 11151, "Dae Rim Sa" },
	{ 12099, "Easy Zone" },
	{ 13798, "Mount Olympus" },
	{ 15299, "Robot City" },
	{ 16199, "Kingdom Of Wee" },
	{ 17099, "O Kingdom" },
	{ 18039, "Moo Dang" },
	{ 23330, "Good Bad Island" },
	{ 30049, "KAIST" }
};

char *find_zone(int number)
{
	int i;

	for (i = 0; i < ZONE_NUMBER; i++) {
		if (zone_data[i].number > number)
			return zone_data[i].name;
	}

	return NULL;
}

int get_quest(struct char_data *ch)
{
	static int width[8] =
	{
		0,		/* 0 */ /* 10 level */
		88,		/* 10 */ /* 4215 16 boris */
		130,		/* 20 */ /* 9727 20 pisces */
		222,		/* 30 */ /* 15009 31 mine robocop */
		251,		/* 33 */ /* 30016 33 poor nimpus */
		347,		/* 37 */ /* 1465 37 roy slade */
		437,		/* 40 */ /* 15117 40 super magnet */
		526,		/* 60 */ /* 18010 40 2nd apprenciate mudang SongCheongSeo */
	};
#define END_QUEST_MOBILE 635

	int low, high;
	int t;
	int num;

	if (GET_LEVEL(ch) == 60) {
		low = 345;	/* 9531 36 son adle second */
		high = END_QUEST_MOBILE;
	} else if (GET_LEVEL(ch) > 39 && GET_LEVEL(ch) < 50) {
		low = 347;	/* 1465 37 roy slade    */
		high = 437;	/* 15117 40 super magnet        */
	} else if (GET_LEVEL(ch) > 49 && GET_LEVEL(ch) < 60) {
		low = 367;	/* 15092 38 sick robot */
		high = 539;	/* 13784 41 zeus god    */
	} else if (ch->quest.solved >= 70) {
		low = 448;	/* 2928 40 amyrok minotaur mage master */
		high = 528;	/* 1990 41 brontosaurus bronto          */
	} else {
		t = ch->quest.solved / 10;
		low = width[t];
		high = width[t + 1];
	}

	do {
		num = number(low, high);
	}
	while (num == ch->quest.data);

	return num;
}

void do_request(struct char_data *ch, char *arg, int cmd)
{
	int num;
	char buf1[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];

	if (GET_LEVEL(ch) >= IMO) {	/* IMO */
		send_to_char_han("&CQUEST&n : &YYou can do anything.&n\n\r", "&CQUEST&n : &Y당신은 무엇이든 할 수 있습니다.&n\n\r", ch);
		return;
	}

	if (IS_MOB(ch)) {	/* MOBILE */
		return;
	}

	/* request */
	if (ch->quest.type > 0) {

		/*      if error occur, can do request. */
		if (ch->quest.data == NUL) {
			ch->quest.type = 0;
			return;
		}

		/* All remotal player cann't do request.    */
		if ((ch->player.level >= (IMO - 1)) && (ch->player.remortal >= 15)) {
			send_to_char_han("&CQUEST&n : &YYou can't request.&n\n\r", "&CQUEST&n : &Y다른 임무를 맡을 수 없습니다.&n\n\r", ch);
			return;
		}

		ch->quest.type = 0;
		if (ch->quest.solved > 0) {
			(ch->quest.solved)--;
		} else {
			/* 단군의 request penalty */
			int xp = number(5000000, 10000000);

			if (GET_EXP(ch) > xp) {
				GET_EXP(ch) -= xp;
			} else {
				send_to_char_han("&CQUEST&n : &YYou can't request.&n\n\r", "&CQUEST&n : &Y다른 임무를 맡을 수 없습니다.&n\n\r", ch);
				return;
			}
		}
	}

	if (ch->quest.type == 0) {
		/* initialize quest */
		num = get_quest(ch);

		snprintf(buf1, sizeof(buf1), "&CQUEST&n : &YQM proposes you should kill &U%s&Y.&n\n\r",
			QM[num].name);
		snprintf(buf2, sizeof(buf2), "&CQUEST&n : &YQM은 당신이 &U%s&Y을 죽일 것을 제안합니다.&n\n\r",
			QM[num].name);
		send_to_char_han(buf1, buf2, ch);

		ch->quest.data = num;
		ch->quest.type = 1;

		return;
	}

	/* ch solved quest */
	if (ch->quest.type < 0) {
		send_to_char_han("&CQUEST&n : &YCongratulations, You made! Go to QM.&n\n\r", 
                         "&CQUEST&n : &Y성공했군요, 축하합니다. QM에게 가보세요.&n\n\r", ch);
		return;
	}
}

void do_hint(struct char_data *ch, char *arg, int cmd)
{
	int num;
	char buf1[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
	char *zone;

	if (GET_LEVEL(ch) >= IMO) {	/* IMO */
		send_to_char_han("&CQUEST&n : &YYou can do anything, Sir!&n\n\r", "&CQUEST&n : &Y당신은 무엇이든 할 수 있습니다.&n\n\r", ch);
		return;
	}

	if (IS_MOB(ch)) {	/* MOBILE */
		return;
	}

	/* not initialized */
	if (ch->quest.type == 0) {
		send_to_char_han("&CQUEST&n : &YFirst, you should type quest.&n\n\r", "&CQUEST&n : &Y먼저 quest라고 해 보세요.&n\n\r", ch);
		return;
	}

	/* ch solved quest */
	if (ch->quest.type < 0) {
		send_to_char_han("&CQUEST&n : &YCongratulations, You made! Go to QM.&n\n\r",
                         "&CQUEST&n : &Y성공했군요, 축하합니다. QM에게 가보세요.&n\n\r", ch);
		return;
	}

	num = ch->quest.data;

	zone = find_zone(QM[num].virtual);

	if(strcspn( QM[num].name, "\n\r")) 
	{
		char buf[100];
		strncpy(buf, QM[num].name, strlen(QM[num].name));
		buf[strlen(QM[num].name)-1] = '\0';
		strncpy(QM[num].name, buf, strlen(buf));

		log("QUEST : zone name has newline");
	}

	if (!zone) {
		snprintf(buf1, sizeof(buf1), "&CQUEST&n : &YWhere &U%s&Y is, I don't know, either.&n\n\r",
			QM[num].name);
		snprintf(buf2, sizeof(buf2), "&CQUEST&n : &U%s&Y? 어디 있는 걸까? 모르겠는데...&n\n\r",
			QM[num].name);
		log("QUEST : INVALID mobile (or zone not found).");
	} else {
		snprintf(buf1, sizeof(buf1), "&CQUEST&n : &U%s&Y is in &#%s&Y probably.&n\n\r",
			QM[num].name, zone);
		snprintf(buf2, sizeof(buf2), "&CQUEST&n : &Y아마도 &U%s&Y는 &#%s&Y에 있을 걸요.&n\n\r",
			QM[num].name, zone);
	}

	send_to_char_han(buf1, buf2, ch);
}

void do_quest(struct char_data *ch, char *arg, int cmd)
{
	int num;
	char buf1[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];

	if (GET_LEVEL(ch) >= IMO) {	/* IMO */
		send_to_char_han("&CQUEST&n : &YYou can do anything, Sir!&n\n\r", "&CQUEST&n : &Y당신은 무엇이든 할 수 있습니다.&n\n\r", ch);
		return;
	}

	if (IS_MOB(ch)) {	/* MOBILE */
		return;
	}

	/* not initialized */
	if (ch->quest.type == 0) {
		/* initialize quest */
		num = get_quest(ch);

		snprintf(buf1, sizeof(buf1), "&CQUEST&n : &YQM proposes you should kill &U%s&Y.&n\n\r",
			QM[num].name);
		snprintf(buf2, sizeof(buf2), "&CQUEST&n : &YQM은 당신이 &U%s&Y을 죽일 것을 제안합니다.&n\n\r",
			QM[num].name);
		send_to_char_han(buf1, buf2, ch);

		ch->quest.data = num;
		ch->quest.type = 1;

		return;
	}

	/* ch solved quest */
	if (ch->quest.type < 0) {
		send_to_char_han("&CQUEST&n : &YCongratulations, You made! Go to QM.&n\n\r", 
                         "&CQUEST&n : &Y성공했군요, 축하합니다. QM에게 가보세요.&n\n\r", ch);
		return;
	}

	num = ch->quest.data;

	snprintf(buf1, sizeof(buf1), "&CQUEST&n : &YQM proposes you should kill &U%s&Y.&n\n\r", QM[num].name);
	snprintf(buf2, sizeof(buf2), "&CQUEST&n : &YQM은 당신이 &U%s&Y을 죽일 것을 제안합니다.&n\n\r",
		QM[num].name);

	send_to_char_han(buf1, buf2, ch);
}

void init_quest(void)
{
	FILE *fp;
	char buf[MAX_STRING_LENGTH];
	int num, size;

	if (!(fp = fopen(QUEST_FILE, "r"))) {
		log("init quest (quest_file)");
		exit(0);
	}

	topQM = 0;
	while (1) {
		fscanf(fp, "%s", buf);
		if (buf[0] == '$') {	/* end of file */
			fclose(fp);
			return;
		}

		num = atoi(buf);
		QM[topQM].virtual = num;

		fscanf(fp, " %d ", &(QM[topQM].level));

		fgets(buf, MAX_STRING_LENGTH - 1, fp);
		size = strlen(buf) + 1;
		buf[size - 1] = 0;
		CREATE(QM[topQM].name, char, size);
		strcpy(QM[topQM].name, buf);

		topQM++;

		if (topQM > MaxQuest) {
			log("Quest Mobiles are overflow.");
			fclose(fp);
			return;
		}

	}
}
/* 기존 퀘스트 보상 산정, 알아보기가 어렵다...
int give_gift_for_quest(int level)
{
	static int gift_array[100] =
	{
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 6,
		1, 1, 1, 1, 1, 1, 1, 1, 3, 3,
		1, 1, 1, 1, 1, 1, 1, 1, 5, 3,
		1, 1, 1, 1, 1, 3, 1, 1, 1, 3,
		1, 1, 1, 1, 1, 3, 1, 1, 1, 8,
		1, 1, 1, 1, 1, 1, 1, 1, 3, 7,
		1, 1, 1, 1, 1, 1, 1, 1, 8, 9,
		5, 5, 5, 5, 6, 6, 7, 6, 9, 1,
	};
	int num;

	num = number(level * 3 / 2, 99);

	return (gift_array[num]);
}
 */

int give_reward_for_quest(int level)
{
    // 레벨 11-19 구간의 보상 테이블
    static struct QuestReward low_level_rewards[] = {
        {REWARD_GOLD, 300},       // 30%
        {REWARD_LIGHT, 300},      // 30%
        {REWARD_ARMOR, 200},      // 20%
        {REWARD_WEAPON, 100},     // 10%
        {REWARD_REQ_TICKET, 100}, // 10%
        {-1, 0}                   // 테이블의 끝
    };

    // 레벨 20-39 구간의 보상 테이블
    static struct QuestReward mid_level_rewards[] = {
        {REWARD_GOLD, 500},       // 50%
        {REWARD_ARMOR, 250},      // 25%
        {REWARD_WEAPON, 150},     // 15%
        {REWARD_REQ_TICKET, 100}, // 10%
        {-1, 0}};

    // 레벨 40~59 구간의 보상 테이블
    static struct QuestReward high_level_rewards[] = {
        {REWARD_GOLD, 300},       // 30%
        {REWARD_ARMOR, 170},      // 17%
        {REWARD_WEAPON, 100},     // 10%
        {REWARD_REQ_TICKET, 120}, // 12%
        {REWARD_HR_TICKET, 110},  // 11%
        {REWARD_DR_TICKET, 100},  // 10%
        {REWARD_AC_TICKET, 100},  // 10%
        {-1, 0}};

    // 레벨 60의 보상 테이블
    static struct QuestReward top_level_rewards[] = {
        {REWARD_GOLD, 360},      // 36%
        {REWARD_ARMOR, 170},     // 17%
        {REWARD_WEAPON, 110},    // 11%
        {REWARD_HR_TICKET, 130}, // 13%
        {REWARD_DR_TICKET, 120}, // 12%
        {REWARD_AC_TICKET, 110}, // 11%
        {-1, 0}};

    struct QuestReward *reward_table;
    int dice, cumulative_prob = 0, i;

    // 플레이어 레벨에 맞는 보상 테이블 선택
    if (level < 20) {
        reward_table = low_level_rewards;
    } else if (level < 40) {
        reward_table = mid_level_rewards;
    } else if (level < 60) {
        reward_table = high_level_rewards;
    } else {
        reward_table = top_level_rewards;
    }

    // 1부터 1000까지의 주사위를 굴리고, 확률에 따라 어떤 보상을 줄지 결정
    dice = number(1, 1000);
    for (i = 0; reward_table[i].reward_id != -1; i++) {
        cumulative_prob += reward_table[i].probability;
        if (dice <= cumulative_prob) {
            return reward_table[i].reward_id;
        }
    }

    return REWARD_GOLD; // 기본 보상은 골드로 주자.
}


/* 
   check ch's quest number with real mobile num
   */
void check_quest_mob_die(struct char_data *ch, int mob)
{
	int num;

	num = ch->quest.data;

	if (ch->quest.type < 1) {
		return;
	}

	num = real_mobile(QM[num].virtual);
	if (num == mob) {
		ch->quest.type = -1;
		send_to_char_han("&CQUEST&n : &YCongratulations, You made! Go to QM.&n\n\r", 
                         "&CQUEST&n : &Y성공했군요, 축하합니다. QM에게 가보세요.&n\n\r", ch);
	}
}

/* 기존 코드를 부분 수정 */
int quest_room(struct char_data *ch, int cmd, char *arg)
{
	int fnum; // quest 보상이 armor일 때 wear flag
    int r_num; // real number
	char buf[MAX_STRING_LENGTH];
	char buf1[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
	struct obj_data *obj;
	static char *weapon[] = {"STAFF", "CLUB", "DAGGER", "SWORD"};
    static int weapon_type[] = {7, 7, 11, 3};
    static char *wear[] = {
        "RING",     "PENDANT",  "ROBE",     "HELMET",
        "LEGGINGS", "BOOTS",    "GLOVES",   "ARMPLATES",
        "SHIELD",   "CLOAK",    "BELT",     "BRACELET",
        "MASCOT",   "KNEEBAND", "PLATE"
    };

    static int wear_flags[] = {
        2,          4,          8,          16,
        32,         64,         128,        256,
        512,        1024,       2048,       4096,
        16384,      65536,      131072
    };

	struct obj_data *tmp_obj;
	extern struct index_data *obj_index;

	/* quest(302) or use(172)  */
	if (cmd != 302 && cmd != 172) {
		return FALSE;
	}

	if (cmd == 172) {
		half_chop(arg, buf, buf2);
		if (*buf) {
			if (strncmp("tickets", buf, strlen(buf))) {
				/* not ticket */
				return FALSE;
			}

			tmp_obj = get_obj_in_list_vis(ch, buf, ch->carrying);
			if (tmp_obj) {
				if ((ch->player.level >= (IMO - 1)) &&
				    (ch->player.remortal >= 15))
					send_to_char("You can't use that ticket .\n\r", ch);
				else if
					    (obj_index[tmp_obj->item_number].virtual
					     == 7994) {
					ch->quest.type = 0;
					do_quest(ch, arg, 302);
					extract_obj(tmp_obj);
				} else {
					send_to_char("You can't use that ticket here.\n\r", ch);
				}
			} else {
				send_to_char("You do not have that item.\n\r", ch);
			}

			return TRUE;
		}

		return FALSE;
	}

	if (GET_LEVEL(ch) >= IMO) {	/* IMO */
		send_to_char_han("&CQUEST&n : &YYou can do anything, Sir!&n\n\r", "&CQUEST&n : &Y당신은 무엇이든 할 수 있습니다.&n\n\r", ch);
		return TRUE;
	}

	if (ch->quest.type < 0) {
//		switch (give_gift_for_quest(GET_LEVEL(ch))) {
		switch (give_reward_for_quest(GET_LEVEL(ch))) {
		case 1:	/* some gold */
			GET_GOLD(ch) += number(100000, (500000 *
							GET_LEVEL(ch)) >> 2);
			send_to_char_han("&CQUEST&n : &YQM gives some coins for your success.&n\n\r",
                             "&CQUEST&n : &Y당신의 성공을 축하하며 QM이 돈을 줍니다.&n\n\r", ch);
			break;
		case 3:	/* request ticket */
			obj = read_object(7994, VIRTUAL);
			obj_to_char(obj, ch);
			send_to_char_han("&CQUEST&n : &YQM gives a ticket for request for your success.&n\n\r",
                             "&CQUEST&n : &Y당신의 성공을 축하하며 QM이 REQUEST용 티켓을 줍니다.&n\n\r", ch);
			break;
		case 5:	/* armor */
			obj = read_object(7998, VIRTUAL);

			/* wear flags */
			fnum = number(0, 14);
			obj->obj_flags.wear_flags = wear_flags[fnum] + 1;

			/* name */
			free(obj->name);
			sprintf(buf1, "%s QUEST ARMOR", wear[fnum]);
			CREATE(obj->name, char, strlen(buf1) + 1);
			strcpy(obj->name, buf1);

			/* short description */
			free(obj->short_description);
			sprintf(buf1, "%s's QUEST %s", GET_NAME(ch), wear[fnum]);
			CREATE(obj->short_description, char, strlen(buf1) + 1);
			strcpy(obj->short_description, buf1);

			/* armor class */
			obj->obj_flags.value[0] = number(10, GET_LEVEL(ch) / 2);

			/* affected */
			obj->affected[0].location = 18;
			obj->affected[0].modifier = number(10, GET_LEVEL(ch) / 2);
			obj->affected[1].location = 19;
			obj->affected[1].modifier = number(10, GET_LEVEL(ch) / 2);

			obj_to_char(obj, ch);
			send_to_char_han("&CQUEST&n : &YQM gives an armor for your success.&n\n\r",
                             "&CQUEST&n : &Y당신의 성공을 축하하며 QM이 갑옷을 줍니다.&n\n\r", ch);
			break;
		case 6:	/* weapon */
			obj = read_object(7997, VIRTUAL);

			/* name */
			free(obj->name);
			snprintf(buf1, sizeof(buf1), "%s's QUEST WEAPON", weapon[GET_CLASS(ch) - 1]);
			CREATE(obj->name, char, strlen(buf1) + 1);
			strcpy(obj->name, buf1);

			/* short description */
			free(obj->short_description);
			snprintf(buf1, sizeof(buf1), "%s's QUEST %s", GET_NAME(ch), weapon[GET_CLASS(ch) - 1]);
			CREATE(obj->short_description, char, strlen(buf1) + 1);
			strcpy(obj->short_description, buf1);

			/* dice 
			   obj->obj_flags.value[1] = number(5+(GET_LEVEL(ch)>>3),5+(GET_LEVEL(ch)>>1));
			   obj->obj_flags.value[2] = number(5+(GET_LEVEL(ch)>>3),5+(GET_LEVEL(ch)>>1));
			 */
			/* dice */
			obj->obj_flags.value[1] = number(GET_LEVEL(ch) / 2, GET_LEVEL(ch) / 10);
			obj->obj_flags.value[2] = number(5 + (GET_LEVEL(ch) >> 3), 5 + (GET_LEVEL(ch) >> 1));

			/* weapon type */
			obj->obj_flags.value[3] = weapon_type[GET_CLASS(ch) - 1];

			/* affected */
			obj->affected[0].location = 18;
			obj->affected[0].modifier = number(10, 5 + GET_LEVEL(ch) / 2);
			obj->affected[1].location = 19;
			obj->affected[1].modifier = number(10, 5 + GET_LEVEL(ch) / 2);

			obj_to_char(obj, ch);
			send_to_char_han("&CQUEST&n : &YQM gives a weapon for your success.&n\n\r",
                             "&CQUEST&n : &Y당신의 성공을 축하하며 QM이 무기를 줍니다.&n\n\r", ch);
			break;
		case 7:	/* ticket for AC */
			obj = read_object(7991, VIRTUAL);
			obj_to_char(obj, ch);
			send_to_char_han("&CQUEST&n : &YQM gives a ticket for meta(AC) for your success.&n\n\r",
                             "&CQUEST&n : &Y당신의 성공을 축하하며 QM이 메타용 AC 티켓을 줍니다.&n\n\r", ch);
			break;
		case 8:	/* ticket for HR */
			obj = read_object(7992, VIRTUAL);
			obj_to_char(obj, ch);
			send_to_char_han("&CQUEST&n : &YQM gives a ticket for meta(HR) for your success.&n\n\r",
                             "&CQUEST&n : &Y당신의 성공을 축하하며 QM이 메타용 HR 티켓을 줍니다.&n\n\r", ch);
			break;
		case 9:	/* ticket for DR */
			obj = read_object(7993, VIRTUAL);
			obj_to_char(obj, ch);
			send_to_char_han(
						"QUEST : QM gives a ticket for meta(DR) for your success.\n\r",
						"QUEST : 당신의 성공을 축하하며 QM이 메타용 DR 티켓을 줍니다.\n\r", ch);
			break;
            case 10: /* light source for lower levels */ /* 영원히 타오르는 퀘스트 횃불 */
                obj = read_object(2319, VIRTUAL); // 2319번 아이템을 기본틀로 (rod of dark light)

                // 아이템 이름 설정
                snprintf(buf, sizeof(buf), "Quest Light Source Torch");
                free(obj->name);
                CREATE(obj->name, char, strlen(buf) + 1);
                strcpy(obj->name, buf);

                // 아이템 설명 설정
                snprintf(buf, sizeof(buf), "%s's Everlasting Quest Torch", GET_NAME(ch));
                free(obj->short_description);
                CREATE(obj->short_description, char, strlen(buf) + 1);
                strcpy(obj->short_description, buf);

                obj->obj_flags.type_flag = ITEM_LIGHT; // 아이템 타입 광원으로
                obj->obj_flags.value[2] = -1; // 광원의 지속시간 무한(-1)으로

                // light source 하나는 계속 갖게 두자... NO_RENT 플래그가 있다면 제거
                if (IS_SET(obj->obj_flags.extra_flags, ITEM_NORENT)) {
                    REMOVE_BIT(obj->obj_flags.extra_flags, ITEM_NORENT);
                }

                obj_to_char(obj, ch);
                send_to_char_han("&CQUEST&n : &YQM gives you an everlasting torch for your success.&n\n\r",
                                "&CQUEST&n : &Y당신의 성공을 축하하며 QM이 영원히 타오르는 횃불을 줍니다.&n\n\r", ch);
                break;
		case 0:
		case 2:
		case 4:
		default:
			send_to_char_han("&CQUEST&n : &YHmm. I can't find a gift for you this time.&n\n\r",
                             "&CQUEST&n : &Y음. 이번에는 줄만한 선물이 없네요.&n\n\r", ch);
		}
		ch->quest.type = 0;
		ch->quest.solved++;
		return TRUE;
	}

	if (ch->quest.type == 0) {
		/* initialize quest */
		r_num = get_quest(ch);

		snprintf(buf1, sizeof(buf1), "&CQUEST&n : &YQM proposes you should kill &U%s&Y.&n\n\r",
			QM[r_num].name);
		snprintf(buf2, sizeof(buf2), "&CQUEST&n : &YQM은 당신이 &U%s&Y을 죽일 것을 제안합니다.&n\n\r",
			QM[r_num].name);
		send_to_char_han(buf1, buf2, ch);

		ch->quest.type = 1;
		ch->quest.data = r_num;

		return TRUE;
	}

	/* not made */
	send_to_char_han("&CQUEST&n : &YCome to me, if you made your quest!&n\n\r", "&CQUEST&n : &Yquest를 마치고 제게 오세요!&n\n\r", ch);
	return TRUE;
}


/* For Challenge Room Quest System by Komo, 251017 */
void do_challenge(struct char_data *ch)
{
    int i, room_rnum;
    bool found_empty_room = FALSE;
    struct follow_type *f;
    struct char_data *challenger, *group_leader;

    challenger = ch; // challenger는 항상 명령어를 실행한 ch

    /* challenger 자격 조건 검사 */
    if (IS_NPC(challenger))
        return;
    if (GET_LEVEL(challenger) < 10 || GET_LEVEL(challenger) >= 40) {
        send_to_char_han("&CQUEST&n : &YThe Room of Challenge is restricted to those between level 10 and 40.&n\n\r",
                         "&CQUEST&n : &Y레벨 10 미만과 레벨 40 이상은 도전의 방을 이용할 수 없습니다.&n\n\r", challenger);
        return;
    }

    if (world[challenger->in_room].number != QUEST_ROOM_VNUM) {
        send_to_char_han("&CQUEST&n : &YYou cannot begin the challenge from here.&n\n\r",
                         "&CQUEST&n : &Y이곳에서는 도전을 시작할 수 없습니다.&n\n\r", challenger);
        return;
    }

    if (challenger->quest.type <= 0) { // 퀘스트가 없거나 이미 완료한 경우
        send_to_char_han("&CQUEST&n : &YYou must accept a QUEST first.&n\n\r", "&CQUEST&n : &Y먼저 퀘스트를 받아야 합니다.&n\n\r", challenger);
        return;
    }

    if (challenger->specials.challenge_room_vnum > 0) { // 본인이 이미 도전 중인지 확인
        send_to_char_han("&CQUEST&n : &YYou are already on a challenge.&n\n\r",
                         "&CQUEST&n : &Y당신은 이미 다른 도전을 진행 중입니다.&n\n\r", challenger);
        return;
    }

    group_leader = (challenger->master ? challenger->master : challenger);

    /* 포인터 유효성 검사용 디버깅 코드 삭제, 문제 없는 듯... */

    /* 비어있는 도전의 방 찾기 */
    for (i = CHALLENGE_ROOM_START_VNUM; i <= CHALLENGE_ROOM_END_VNUM; i++) {
        room_rnum = real_room(i);
        if (room_rnum != NOWHERE && world[room_rnum].people == NULL) {

            int original_room_rnum = challenger->in_room;
            int original_vnum = world[original_room_rnum].number; // 이동 전 VNUM 저장

            /* 자격이 되는 그룹원 선별하여 이동 */
            // 도전자(challenger)는 항상 먼저 이동
            acthan("&cCHALLENGE&n : &yAs if having made a decision, $n disappears.&n", 
                   "&cCHALLENGE&n : &y$n이(가) 결심한 듯 어디론가 사라집니다.&n", FALSE, challenger, 0, 0, TO_ROOM);
            char_from_room(challenger);
            char_to_room(challenger, room_rnum);

            // 이동 성공 여부 확인 후 상태 설정
            if (challenger->in_room == room_rnum) { // 이동에 성공했다면
                // 이동 성공 시에만 상태를 설정
                challenger->specials.challenge_room_vnum = i;
                challenger->specials.return_room_vnum = original_vnum;
                send_to_char_han("&cCHALLENGE&n : &yEntering the Room of Challenge. Type '&Wbegin&n&y' to begin your challenge.&n\n\r\n\r",
                                 "&cCHALLENGE&n : &y도전의 방으로 이동합니다. '&Wbegin&n&y'을 입력해 몬스터를 소환하세요.&n\n\r\n\r", challenger);
                
                do_look(challenger, "", 15);

                // 리더의 자격 조건을 검사 후 이동 (리더가 도전자가 아닐 경우)
                if (group_leader != challenger && group_leader->in_room == original_room_rnum) {
                    if (GET_LEVEL(group_leader) >= 10 && GET_LEVEL(group_leader) < 40) {
                        acthan("&cCHALLENGE&n : &yYou follow $N into the Room of Challenge.&n", 
                               "&cCHALLENGE&n : &y당신은 $N을 따라 도전의 방으로 들어갑니다.&n", FALSE, group_leader, 0, challenger, TO_CHAR);
                        char_from_room(group_leader);
                        char_to_room(group_leader, room_rnum);
                        do_look(group_leader, "", 15);
                    }
                }

                // 나머지 그룹원들의 자격 조건을 개별 검사 후 이동
                for (f = group_leader->followers; f; f = f->next) {
                    if (f->follower == challenger || f->follower == group_leader)
                        continue;

                    if (f->follower->in_room == original_room_rnum && IS_AFFECTED(f->follower, AFF_GROUP)) {
                        if (GET_LEVEL(f->follower) >= 10 && GET_LEVEL(f->follower) < 40) {
                            acthan("&cCHALLENGE&n : &yYou follow $N into the Room of Challenge.&n", 
                                   "&cCHALLENGE&n : &y당신은 $N을 따라 도전의 방으로 들어갑니다.&n", FALSE, f->follower, 0, challenger, TO_CHAR);
                            char_from_room(f->follower);
                            char_to_room(f->follower, room_rnum);
                            do_look(f->follower, "", 15);
                        }
                    }
                }
            } else { // char_to_room 내부 문제 등으로 이동에 실패했다면...
                // 만약의 경우를 대비해 원래 방으로 돌려보냄 (char_from_room은 이미 된 상태일 수 있음)
                if (challenger->in_room != original_room_rnum) {  // 이미 다른 방이라면
                    char_to_room(challenger, original_room_rnum); // 원래 방으로 강제 이동
                }
                send_to_char("&cCHALLENGE&n : &y오류: &R도전의 방으로 이동하는 데 실패했습니다.&n\n\r", challenger);
                found_empty_room = TRUE; // 루프 탈출 위해 설정 (다른 방 찾기 중단)
                break;
            }
            /* [디버깅 코드 삭제] 함수 성공 및 종료 */
            
            return; // 방을 찾았으므로 함수 종료
        }
    } /* end of for : 비어있는 도전의 방 찾기*/

    if (!found_empty_room) {
        send_to_char_han("&cCHALLENGE&n : &yAll Rooms of Challenge are currently in use. Please try again in a moment.&n\n\r",
                         "&cCHALLENGE&n : &y지금은 모든 도전의 방이 사용 중입니다. 잠시 후에 다시 시도해주세요.&n\n\r", ch);
    }
}


void do_begin(struct char_data *ch)
{
    struct char_data *mob, *existing_mob;
    int mob_rnum, mob_vnum = 0;
    bool mob_already_exists = FALSE; // 몬스터 존재 여부
    int target_rnum;

    if (IS_NPC(ch))
        return;

    if (ch->specials.challenge_room_vnum == 0 || world[ch->in_room].number != ch->specials.challenge_room_vnum) {
        send_to_char_han("&cCHALLENGE&n : &yYou can't BEGIN your challenge here.&n\n\r", "&cCHALLENGE&n : &y이곳에서는 도전을 시작할 수 없습니다.&n\n\r", ch);
        return;
    }

    if (ch->quest.type <= 0) {
        send_to_char_han("&cCHALLENGE&n : &yYou don't have any quest to perform. Returning to where you were.&n\n\r\n\r",
                         "&cCHALLENGE&n : &y수행할 퀘스트가 없습니다. 원래 장소로 돌아갑니다.&n\n\r\n\r", ch);
        char_from_room(ch);
        char_to_room(ch, real_room(ch->specials.return_room_vnum));
        ch->specials.challenge_room_vnum = 0;
        ch->specials.return_room_vnum = 0;
        do_look(ch, "", 15);
        return;
    }

    target_rnum = ch->quest.data; // 퀘스트 목표의 real number

    // r_num 유효성 검사
    if (target_rnum < 0) {
        send_to_char_han("&cCHALLENGE&n : &yThe information on Qeust Monster can't be found (Invalid Index). Contact the GM or Wizard.&n\n\r",
                         "&CQUEST&n : &y퀘스트 몬스터 정보를 찾을 수 없습니다 (잘못된 인덱스). 관리자에게 문의해주세요.&n\n\r", ch);
        return;
    }

    mob_vnum = mob_index[target_rnum].virtual; // real number로 VNUM 획득
    if (mob_vnum <= 0) { // 혹시 모를 방어 코드
        send_to_char_han("&cCHALLENGE&n : &yThe information on Qeust Monster can't be found (Invalid VNUM). Contact the GM or Wizard.&n\n\r",
                         "&CQUEST&n : &y퀘스트 몬스터 정보를 찾을 수 없습니다 (잘못된 VNUM). 관리자에게 문의해주세요.&n\n\r", ch);
        return;
    }

    // 현재 방에 있는 모든 캐릭터(몹 포함)를 검사하여 이미 퀘스트 몬스터가 존재하는지 확인
    for (existing_mob = world[ch->in_room].people; existing_mob; existing_mob = existing_mob->next_in_room) {
        // 만약 NPC이고, VNUM이 소환하려는 몬스터의 VNUM과 같다면
        if (IS_NPC(existing_mob) && mob_index[existing_mob->nr].virtual == mob_vnum) {
            mob_already_exists = TRUE; // 이미 존재한다고 표시
            break;                     // 더 이상 검사할 필요 없음
        }
    }

    // mob이 이미 존재한다면 메시지를 출력하고 함수 종료
    if (mob_already_exists) {
        send_to_char_han("&cCHALLENGE&n : &yYour Quest monster has been already summoned!&n\n\r",
                         "&cCHALLENGE&n : &y퀘스트 몬스터는 이미 소환되어 있습니다.&n\n\r", ch);
        return;
    }

    /* ----- 퀘스트 몬스터 소환 ----- */
    mob_rnum = real_mobile(mob_vnum);
    if (mob_rnum == NOBODY) {
        send_to_char_han("&cCHALLENGE&n : &yThe information on Qeust Monster can't be found. Contact the GM or Wizard.&n\n\r",
                         "&cCHALLENGE&n : &y퀘스트 몬스터 정보를 찾을 수 없습니다. 관리자에게 문의해주세요.&n\n\r", ch);
        return;
    }

    mob_rnum = target_rnum; // mob_rnum은 이미 유효한 real number
    mob = read_mobile(mob_rnum, REAL);

    if (IS_SET(mob->specials.act, ACT_WIMPY)) {
        REMOVE_BIT(mob->specials.act, ACT_WIMPY); // 소환된 몬스터에 wimpy 플래그가 있다면 제거
    }
    SET_BIT(mob->specials.act, ACT_SENTINEL); // 소환된 몬스터 이동 방지

    ch->specials.challenge_quest_mob_vnum = mob_vnum; // 도전자(ch)에게 퀘스트 몬스터의 VNUM을 저장
    char_to_room(mob, ch->in_room);

    char buf_eng[MAX_STRING_LENGTH];
    char buf_han[MAX_STRING_LENGTH];
    snprintf(buf_eng, sizeof(buf_eng), 
             "&cCHALLENGE&n : &yA swirl of light coalesces in the air, taking the form of [&h %s&y ]!!&n\n\r", GET_NAME(mob));
    snprintf(buf_han, sizeof(buf_han), 
             "&cCHALLENGE&n : &y허공에 빛무리가 뭉치더니 이내 [&h %s&y ](으)로 변합니다!!&n\n\r", GET_NAME(mob));
    send_to_char_han(buf_eng, buf_han, ch);
    acthan("&cCHALLENGE&n : &y$n utters an incantation, and [&h $N &y] materializes in the center of the room!&n",
           "&cCHALLENGE&n : &y$n이(가) 주문을 외우자, [&h $N &y]이(가) 방 한가운데 나타납니다!&n", FALSE, ch, 0, mob, TO_ROOM);
}


void do_rejoin(struct char_data *ch)
{
    struct char_data *group_leader;
    struct follow_type *f;
    int target_room_rnum;

    /* 명령어 사용자(ch)의 기본 조건 검사 */
    if (IS_NPC(ch))
        return;
    if (GET_LEVEL(ch) < 10 || GET_LEVEL(ch) >= 40) {
        send_to_char("&cCHALLENGE&n : &y이 레벨에서는 도전에 합류할 수 없습니다.&n\n\r", ch);
        return;
    }
    if (world[ch->in_room].number != QUEST_ROOM_VNUM) {
        send_to_char("&cCHALLENGE&n : &y이곳에서는 도전에 다시 합류할 수 없습니다.&n\n\r", ch);
        return;
    }

    /* 사용자(ch)가 현재 유효한 도전 상태인지 확인 */
    if (ch->specials.challenge_room_vnum <= 0 || (target_room_rnum = real_room(ch->specials.challenge_room_vnum)) == NOWHERE) {
        send_to_char("&cCHALLENGE&n : &y현재 진행 중인 유효한 도전 정보가 없습니다.&n\n\r", ch);
        // 만약을 위해 상태 초기화 : 안전장치
        ch->specials.challenge_room_vnum = 0;
        ch->specials.return_room_vnum = 0;
        return;
    }

    /* 사용자(ch)를 도전의 방으로 이동 */
    acthan("&cCHALLENGE&n : &yTo continue a Qeust Challenge, $n disappears.&n", 
           "&cCHALLENGE&n : &y$n이(가) 진행 중인 도전을 이어가기 위해 사라집니다.&n", FALSE, ch, 0, 0, TO_ROOM);
    char_from_room(ch);
    char_to_room(ch, target_room_rnum);
    send_to_char("&cCHALLENGE&n : &y진행 중인 도전에 다시 합류합니다!&n\n\r", ch);
    do_look(ch, "", 15);

    /* 같은 그룹원 중 조건 맞는 사람 이동 */
    group_leader = (ch->master ? ch->master : ch); // 그룹 리더 찾기

    // 리더 이동 - 리더가 명령 사용자(ch)가 아니고, 퀘스트 룸에 있다면
    if (group_leader != ch && group_leader->in_room == real_room(QUEST_ROOM_VNUM)) {
        if (GET_LEVEL(group_leader) >= 10 && GET_LEVEL(group_leader) < 40) {
            acthan("&cCHALLENGE&n : &yYou follow $N into the Room of Challenge.&n", 
                   "&cCHALLENGE&n : &y당신은 $N을 따라 도전의 방으로 들어갑니다.&n", FALSE, group_leader, 0, ch, TO_CHAR);
            char_from_room(group_leader);
            char_to_room(group_leader, target_room_rnum);
            do_look(group_leader, "", 15);
        }
    }

    // 나머지 그룹원 이동 - 퀘스트 룸에 있으면...
    for (f = group_leader->followers; f; f = f->next) {
        if (f->follower == ch || f->follower == group_leader)
            continue;

        if (f->follower->in_room == real_room(QUEST_ROOM_VNUM) && IS_AFFECTED(f->follower, AFF_GROUP)) {
            if (GET_LEVEL(f->follower) >= 10 && GET_LEVEL(f->follower) < 40) {
                acthan("&cCHALLENGE&n : &yYou follow $N into the Room of Challenge.&n",
                       "&cCHALLENGE&n : &y당신은 $N을 따라 도전의 방으로 들어갑니다.&n", FALSE, f->follower, 0, ch, TO_CHAR);
                char_from_room(f->follower);
                char_to_room(f->follower, target_room_rnum);
                do_look(f->follower, "", 15);
            }
        }
    }
}
