int get_quest(struct char_data *ch)
{
    // 퀘스트 후보 몹의 real number(인덱스)를 저장할 임시 배열
    static int candidate_list[MAX_QUEST_CANDIDATES];
    int candidate_count = 0;

    int player_level = GET_LEVEL(ch);
    int min_mob_level = 0;
    int max_mob_level = 0;

    int i; // mob_index 순회용
    int r_num; // 최종 선택된 real number

    // 레벨 및 완료 횟수 연동 - 플레이어 레벨 구간에 따라 몬스터 레벨 범위 설정
    if (player_level >= 10 && player_level <= 20) {
        min_mob_level = player_level - 7;
        max_mob_level = player_level;
    } else if (player_level >= 21 && player_level <= 30) {
        min_mob_level = player_level - 6;
        max_mob_level = player_level;
    } else if (player_level >= 31 && player_level <= 40) {
        min_mob_level = player_level - 5;
        max_mob_level = player_level;
    } else if (player_level >= 41 && player_level <= 50) {
        min_mob_level = player_level - 10;
        max_mob_level = player_level + 1;
    } else if (player_level >= 51 && player_level <= 60) {
        min_mob_level = player_level - 20;
        max_mob_level = player_level + 3;
    } else {
        return -1;
    }

    // 레벨 하한선은 1로
    if (min_mob_level < 1) min_mob_level = 1;

    for (i = 0; i <= top_of_mobt; i++) {
        // 레벨 범위 확인
        if (mob_index[i].level < min_mob_level || mob_index[i].level > max_mob_level)
            continue;
        
        // 지역 필터링? 필요?
        char *zone_name = find_zone(mob_index[i].virtual);
        if (zone_name) {
            if (strcmp(zone_name, "the LIMBO") == 0 || strcmp(zone_name, "Arena") == 0) {
               /*  strcmp(zone_name, "Northern Midgaard MainCity") == 0 ||
                strcmp(zone_name, "Southern Part Of Midgaard") == 0 ||
                strcmp(zone_name, "River Of Midgaard") == 0 || */

                continue;
            }
        }

        candidate_list[candidate_count] = i;
        candidate_count++;

        if (candidate_count >= MAX_QUEST_CANDIDATES)
            break;
    }

    // 최종 선택
    if (candidate_count == 0) {
        return -1; // 적합한 퀘스트 몹 없음
    }

    // 무작위로 하나 선택
    r_num = candidate_list[number(0, candidate_count - 1)];

    // 이전 퀘스트와 동일한 몹인지 확인
    // ch->quest.data에 real number가 저장됨
    if (candidate_count > 1 && r_num == ch->quest.data) {
        // 다른 몹 다시 뽑기
        int new_r_num = candidate_list[number(0, candidate_count - 1)];
        if (new_r_num != r_num) {
            r_num = new_r_num;
        }
    }
    
    return r_num; // real number 반환
}

/* 기존 코드에 변경된 내용 적용 */
void do_request(struct char_data *ch, char *arg, int cmd)
{
    int r_num; // 몹 real number
    char buf1[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];

    if (GET_LEVEL(ch) >= IMO) { /* IMO */
        send_to_char_han("&CQUEST&n : &YYou can do anything.&n\n\r", "&CQUEST&n : &Y당신은 무엇이든 할 수 있습니다.&n\n\r", ch);
        return;
    }

    if (IS_MOB(ch)) { /* MOBILE */
        return;
    }

    /* request */
    if (ch->quest.type > 0) {
        /* if error occur, can do request. */
        // ch->quest.data가 유효한 r_num인지 확인 (0보다 작은지)
        if (ch->quest.data < 0 || ch->quest.data > top_of_mobt) {
            ch->quest.type = 0;
            // (오류 로그) log("Quest data error, resetting quest for player.");
            return;
        }

        /* All remotal players can't do request.    */
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
        r_num = get_quest(ch); // 새로운 get_quest 호출

        if (r_num < 0) { // 적합한 몹을 찾지 못함
            send_to_char_han("&CQUEST&n : &YQM can't find a suitable quest for you right now.&n\n\r",
                             "&CQUEST&n : &Y지금은 당신에게 맞는 임무를 찾을 수가 없네요.&n\n\r", ch);
            return;
        }

        // QM[num].name 대신 mob_index[r_num].name 사용
        snprintf(buf1, sizeof(buf1), "&CQUEST&n : &YQM proposes you should kill &U%s&Y.&n\n\r", mob_index[r_num].name);
        snprintf(buf2, sizeof(buf2), "&CQUEST&n : &YQM은 당신이 &U%s&Y을 죽일 것을 제안합니다.&n\n\r", mob_index[r_num].name);
        send_to_char_han(buf1, buf2, ch);

        ch->quest.data = r_num; // ch->quest.data에 real number 저장
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

/* 기존 코드에 변경된 내용 적용 */
void do_hint(struct char_data *ch, char *arg, int cmd)
{
    int r_num; // real number
    int vnum;  // virtual number
    char *mob_name;
    char buf1[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
    char *zone;

    if (GET_LEVEL(ch) >= IMO) { /* IMO */
        send_to_char_han("&CQUEST&n : &YYou can do anything, Sir!&n\n\r", "&CQUEST&n : &Y당신은 무엇이든 할 수 있습니다.&n\n\r", ch);
        return;
    }

    if (IS_MOB(ch)) { /* MOBILE */
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

    r_num = ch->quest.data; // ch->quest.data는 real number

    // r_num 유효성 검사
    if (r_num < 0 || r_num > top_of_mobt) {
        send_to_char_han("&CQUEST&n : &YYour quest data seems corrupted. Please request a new one.&n\n\r",
                         "&CQUEST&n : &Y퀘스트 정보가 손상된 것 같습니다. 'request'로 새 임무를 받으세요.&n\n\r", ch);
        ch->quest.type = 0;
        ch->quest.data = -1;
        return;
    }

    vnum = mob_index[r_num].virtual;
    mob_name = mob_index[r_num].name;
    zone = find_zone(vnum);

    if (!zone) {
        snprintf(buf1, sizeof(buf1), "&CQUEST&n : &YWhere &U%s&Y is, I don't know, either.&n\n\r", mob_name);
        snprintf(buf2, sizeof(buf2), "&CQUEST&n : &U%s&Y? 어디 있는 걸까? 모르겠는데...&n\n\r", mob_name);
        log("QUEST : INVALID mobile (or zone not found).");
    } else {
        snprintf(buf1, sizeof(buf1), "&CQUEST&n : &U%s&Y is in &#%s&Y probably.&n\n\r", mob_name, zone);
        snprintf(buf2, sizeof(buf2), "&CQUEST&n : &Y아마도 &U%s&Y는 &#%s&Y에 있을 걸요.&n\n\r", mob_name, zone);
    }

    send_to_char_han(buf1, buf2, ch);
}