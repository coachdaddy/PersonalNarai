#ifndef __PROTOTYPES_H__
#define __PROTOTYPES_H__

/* 
   코드 정리와 리팩토링을 위해, 
   함수 선언부 모음
   251208 by Komo
*/

#include <stdio.h>

/* --- act.comm.c --- */
void do_say(struct char_data *ch, char *argument, int cmd);
void do_sayh(struct char_data *ch, char *argument, int cmd);
void save_chat_history(struct char_data *ch, char *argument);
void do_shout(struct char_data *ch, char *argument, int cmd);
void do_chat(struct char_data *ch, char *argument, int cmd);
void do_lastchat(struct char_data *ch, char *argument, int cmd);
void do_tell(struct char_data *ch, char *argument, int cmd);
void do_reply(struct char_data *ch, char *argument, int cmd);
void do_send(struct char_data *ch, char *argument, int cmd);
void do_gtell(struct char_data *ch, char *argument, int cmd);
void do_whisper(struct char_data *ch, char *argument, int cmd);
void do_ask(struct char_data *ch, char *argument, int cmd);
void do_write(struct char_data *ch, char *argument, int cmd);

/* --- act.informative.c --- */
void argument_split(char *argument, char *first_arg, char *second_arg);
struct obj_data *get_object_in_equip_vis(struct char_data *ch, char *arg, struct obj_data *equipment[], int *j);
char *find_ex_description(char *word, struct extra_descr_data *list);
void show_obj_to_char(struct obj_data *object, struct char_data *ch, int mode);
void list_obj_to_char(struct obj_data *list, struct char_data *ch, int mode, bool show);
void show_char_to_char(struct char_data *i, struct char_data *ch, int mode);
void list_char_to_char(struct char_data *list, struct char_data *ch, int mode);
void do_look(struct char_data *ch, char *argument, int cmd);
void do_bank(struct char_data *ch, char *argument, int cmd);
void do_read(struct char_data *ch, char *argument, int cmd);
void do_examine(struct char_data *ch, char *argument, int cmd);
void do_report(struct char_data *ch, char *argument, int cmd);
void do_title(struct char_data *ch, char *argument, int cmd);
void do_exits(struct char_data *ch, char *argument, int cmd);
void do_score(struct char_data *ch, char *argument, int cmd);
void do_attribute(struct char_data *ch, char *argument, int cmd);
void do_time(struct char_data *ch, char *argument, int cmd);
void do_weather(struct char_data *ch, char *argument, int cmd);
void do_help(struct char_data *ch, char *argument, int cmd);
void do_spells(struct char_data *ch, char *argument, int cmd);
void do_wizhelp(struct char_data *ch, char *argument, int cmd);
void do_who(struct char_data *ch, char *argument, int cmd);
void do_users(struct char_data *ch, char *argument, int cmd);
void do_inventory(struct char_data *ch, char *argument, int cmd);
void do_equipment(struct char_data *ch, char *argument, int cmd);
void do_credits(struct char_data *ch, char *argument, int cmd);
void load_news_if_changed();
void do_news(struct char_data *ch, char *argument, int cmd);
void do_plan(struct char_data *ch, char *argument, int cmd);
void do_wizards(struct char_data *ch, char *argument, int cmd);
void do_where(struct char_data *ch, char *argument, int cmd);
void do_levels(struct char_data *ch, char *argument, int cmd);
void do_consider(struct char_data *ch, char *argument, int cmd);
void do_police(struct char_data *ch, char *argument, int cmd);
void do_siteban(struct char_data *ch, char *argument, int cmd);
void do_nochat(struct char_data *ch, char *argument, int cmd);
void do_notell(struct char_data *ch, char *argument, int cmd);
void do_data(struct char_data *ch, char *argument, int cmd);
void do_hangul(struct char_data *ch, char *argument, int cmd);
void do_solo(struct char_data *ch, char *argument, int cmd);

/* --- act.movement.c --- */
int do_simple_move(struct char_data *ch, int cmd, int following);
void do_move(struct char_data *ch, char *argument, int cmd);
int find_door(struct char_data *ch, char *type, char *dir);
void do_open(struct char_data *ch, char *argument, int cmd);
void do_close(struct char_data *ch, char *argument, int cmd);
int has_key(struct char_data *ch, int key);
void do_lock(struct char_data *ch, char *argument, int cmd);
void do_unlock(struct char_data *ch, char *argument, int cmd);
void do_pick(struct char_data *ch, char *argument, int cmd);
void do_enter(struct char_data *ch, char *argument, int cmd);
void do_leave(struct char_data *ch, char *argument, int cmd);
void do_stand(struct char_data *ch, char *argument, int cmd);
void do_sit(struct char_data *ch, char *argument, int cmd);
void do_rest(struct char_data *ch, char *argument, int cmd);
void do_sleep(struct char_data *ch, char *argument, int cmd);
void do_wake(struct char_data *ch, char *argument, int cmd);
void do_follow(struct char_data *ch, char *argument, int cmd);

/* --- act.obj1.c --- */
void get(struct char_data *ch, struct obj_data *obj_object, struct obj_data *sub_object);
void do_get(struct char_data *ch, char *argument, int cmd);
void do_drop(struct char_data *ch, char *argument, int cmd);
void do_put(struct char_data *ch, char *argument, int cmd);
void do_give(struct char_data *ch, char *argument, int cmd);
void do_reload(struct char_data *ch, char *argument, int cmd);

/* --- act.obj2.c --- */
void weight_change_object(struct obj_data *obj, int weight);
void name_from_drinkcon(struct obj_data *obj);
void name_to_drinkcon(struct obj_data *obj, int type);
void do_drink(struct char_data *ch, char *argument, int cmd);
void do_eat(struct char_data *ch, char *argument, int cmd);
void do_junk(struct char_data *ch, char *argument, int cmd);
void do_pour(struct char_data *ch, char *argument, int cmd);
void do_sip(struct char_data *ch, char *argument, int cmd);
void do_taste(struct char_data *ch, char *argument, int cmd);
void perform_wear(struct char_data *ch, struct obj_data *obj_object, int keyword);
void wear(struct char_data *ch, struct obj_data *obj_object, int keyword);
int where_wear(struct obj_data *obj_object);
void do_wear(struct char_data *ch, char *argument, int cmd);
void do_wield(struct char_data *ch, char *argument, int cmd);
void do_grab(struct char_data *ch, char *argument, int cmd);
void do_unwield(struct char_data *ch, char *argument, int cmd);
void do_unhold(struct char_data *ch, char *argument, int cmd);
void do_remove(struct char_data *ch, char *argument, int cmd);

/* --- act.offensive.c --- */
void do_hit(struct char_data *ch, char *argument, int cmd);
void do_kill(struct char_data *ch, char *argument, int cmd);
void do_backstab(struct char_data *ch, char *argument, int cmd);
void do_order(struct char_data *ch, char *argument, int cmd);
void do_flee(struct char_data *ch, char *argument, int cmd);
void do_bash(struct char_data *ch, char *argument, int cmd);
void do_rescue(struct char_data *ch, char *argument, int cmd);
void do_multi_kick(struct char_data *ch, char *argument, int cmd);
void do_kick(struct char_data *ch, char *argument, int cmd);
void do_punch(struct char_data *ch, char *argument, int cmd);
void do_tornado(struct char_data *ch, char *argument, int cmd);
void do_light_move(struct char_data *ch, char *argument, int cmd);
void do_flash(struct char_data *ch, char *argument, int cmd);
void shoot(struct char_data *ch, struct char_data *victim, int type);
void do_shoot(struct char_data *ch, char *argument, int cmd);
void do_temptation(struct char_data *ch, char *argument, int cmd);

/* --- act.other.c --- */
void do_quit(struct char_data *ch, char *argument, int cmd);
void do_wimpy(struct char_data *ch, char *argument, int cmd);
void do_save(struct char_data *ch, char *argument, int cmd);
void do_not_here(struct char_data *ch, char *argument, int cmd);
void do_sneak(struct char_data *ch, char *argument, int cmd);
void do_hide(struct char_data *ch, char *argument, int cmd);
void do_steal(struct char_data *ch, char *argument, int cmd);
void do_practice(struct char_data *ch, char *arg, int cmd);
void do_brief(struct char_data *ch, char *argument, int cmd);
void do_compact(struct char_data *ch, char *argument, int cmd);
void do_group(struct char_data *ch, char *argument, int cmd);
void do_quaff(struct char_data *ch, char *argument, int cmd);
void do_recite(struct char_data *ch, char *argument, int cmd);
void do_use(struct char_data *ch, char *argument, int cmd);
void do_post(struct char_data *ch, char *argument, int cmd);
void do_assault(struct char_data *ch, char *argument, int cmd);
void do_disarm(struct char_data *ch, char *argument, int cmd);
void do_spin_bird_kick(struct char_data *ch, char *argument, int cmd);
void do_shouryuken(struct char_data *ch, char *argument, int cmd);
void do_throw_object(struct char_data *ch, char *argument, int cmd);

/* --- act.social.c --- */
char *fread_action(FILE *fl);
void boot_social_messages(void);
int find_action(int cmd);
void do_action(struct char_data *ch, char *argument, int cmd);
void do_insult(struct char_data *ch, char *argument, int cmd);

/* --- act.wizard.c --- */
void do_advance(struct char_data *ch, char *argument, int cmd);
void do_at(struct char_data *ch, char *argument, int cmd);
void do_banish(struct char_data *ch, char *argument, int cmd);
void do_demote(struct char_data *ch, char *argument, int cmd);
void do_echo(struct char_data *ch, char *argument, int cmd);
void do_emote(struct char_data *ch, char *argument, int cmd);
void do_invis(struct char_data *ch, char *argument, int cmd);
void do_flag(struct char_data *ch, char *argument, int cmd);
void do_flick(struct char_data *ch, char *argument, int cmd);
void do_force(struct char_data *ch, char *argument, int cmd);
void do_goto(struct char_data *ch, char *argument, int cmd);
void do_load(struct char_data *ch, char *argument, int cmd);
void do_noaffect(struct char_data *ch, char *argument, int cmd);
void do_noshout(struct char_data *ch, char *argument, int cmd);
void do_purge(struct char_data *ch, char *argument, int cmd);
void do_reroll(struct char_data *ch, char *argument, int cmd);
void do_restore(struct char_data *ch, char *argument, int cmd);
void do_return(struct char_data *ch, char *argument, int cmd);
void do_set(struct char_data *ch, char *argument, int cmd);
void do_shutdow(struct char_data *ch, char *argument, int cmd);
void do_shutdown(struct char_data *ch, char *argument, int cmd);
void do_snoop(struct char_data *ch, char *argument, int cmd);
void do_start(struct char_data *ch);
void perform_stat_room(struct char_data *ch, struct room_data *rm, char *buf, size_t size);
void perform_stat_object(struct char_data *ch, struct obj_data *j, char *buf, size_t size);
void perform_stat_char(struct char_data *ch, struct char_data *k, char *buf, size_t size);
void do_stat(struct char_data *ch, char *argument, int cmd);
void do_switch(struct char_data *ch, char *argument, int cmd);
void do_sys(struct char_data *ch, char *argument, int cmd);
void do_trans(struct char_data *ch, char *argument, int cmd);
void do_transform(struct char_data *ch, char *argument, int cmd);
void do_wall(struct char_data *ch, char *argument, int cmd);
void do_wiznet(struct char_data *ch, char *argument, int cmd);
int compare_to_descending_order(const void *a, const void *b);
void roll_abilities(struct char_data *ch);
void do_zreload(struct char_data *ch, char *argument, int cmd);
void do_wreload(struct char_data *ch, char *argument, int cmd);
void do_zonelist(struct char_data *ch, char *argument, int cmd);

/* --- board.c --- */
struct board_data *init_a_board(struct char_data *ch);
struct board_data *find_board(struct char_data *ch);
void load_board(struct board_data *cb);
void save_board(struct board_data *cb);
int board(struct char_data *ch, int cmd, char *arg);
int show_board(struct char_data *ch, struct board_data *cb, char *arg);
int remove_board(struct char_data *ch, struct board_data *cb, char *arg);
int read_board(struct char_data *ch, struct board_data *cb, char *arg);
int write_board(struct char_data *ch, struct board_data *cb, char *arg);
int post_board(struct char_data *ch, struct board_data *cb, char *arg);

/* --- comm.c --- */
void run_the_game(int port);
void handle_graceful_shutdown(int sig);
void handle_immediate_shutdown(int sig);
void signal_setup(void);
void game_loop(int s);
void transall(int room);
void saveallplayers();
void record_player_number();
int get_from_q(struct txt_q *queue, char *dest);
void write_to_q(char *txt, struct txt_q *queue);
void send_to_q_color(const char *messg, struct descriptor_data *desc);
struct timeval timediff(struct timeval *a, struct timeval *b);
void flush_queues(struct descriptor_data *d);
int init_socket(int port);
int new_connection(int s);
int new_descriptor(int s);
int unfriendly_domain(char *h);
int process_output(struct descriptor_data *t);
int write_to_descriptor(int desc, char *txt);
int process_input(struct descriptor_data *t);
void close_sockets(int s);
void close_socket(struct descriptor_data *d);
void nonblock(int s);
void send_to_char(char *messg, struct char_data *ch);
void send_to_char_han(char *msgeng, char *msghan, struct char_data *ch);
void send_to_all(char *messg);
void send_to_room(char *messg, int room);
void act(char *str, int hide_invisible, struct char_data *ch, struct obj_data *obj, void *vict_obj, int type);
void acthan(char *streng, char *strhan, int hide_invisible, struct char_data *ch, struct obj_data *obj, void *vict_obj, int type);
void log_abnormal_disconnect(struct descriptor_data *d);
void zapper(void);
void checkpointing(int sig);

/* --- daerimsa.c --- */
int son_ogong_func(struct char_data *ch, int cmd, char *arg);
int fourth_jangro_func(struct char_data *ch, int cmd, char *arg);
int teleport_daerimsa_tower(struct char_data *ch, int cmd, char *arg);
int saint_water(struct char_data *ch, int cmd, char *arg);

/* --- db.c --- */
void boot_db(void);
void reset_time(void);
void build_player_index(void);
struct index_data *generate_indices(FILE *fl, int *top);
void load_rooms(FILE *fl, int zone_rnum, int *room_nr);
void boot_world(void);
void allocate_room(int new_top);
void clean_memory_for_room(int rnum);
void reload_world_file(FILE *fl, int zone_rnum);
void setup_dir(FILE *fl, int room, int dir);
void renum_world(void);
void renum_zone_table(void);
void parse_zone_file(FILE *fl, int zon);
void load_zones(int zon);
void boot_zones(void);
struct char_data *read_mobile(int nr, int type);
struct obj_data *read_object(int nr, int type);
void zone_update(void);
struct char_data *get_mobile_index(int index);
void reset_zone(int zone);
int is_empty(int zone_nr);
int load_char(char *name, struct char_file_u *char_element);
void store_to_char(struct char_file_u *st, struct char_data *ch);
void store_to_char_for_transform(struct char_file_u *st, struct char_data *ch);
void char_to_store(struct char_data *ch, struct char_file_u *st);
int create_entry(char *name);
void save_char_nocon(struct char_data *ch, sh_int load_room);
void save_char(struct char_data *ch, sh_int load_room);
void delete_char(struct char_data *ch);
int remove_entry(struct char_data *ch);
int compare(struct player_index_element *arg1, struct player_index_element *arg2);
char *fread_string(FILE *fl);
void free_char(struct char_data *ch);
void free_obj(struct obj_data *obj);
int file_to_string(char *name, char *buf);
void reset_char(struct char_data *ch);
void clear_char(struct char_data *ch);
void clear_object(struct obj_data *obj);
void init_char(struct char_data *ch);
int real_room(int virtual);
int real_mobile(int virtual);
int real_object(int virtual);
int real_zone_by_number(int virtual);
int move_stashfile_safe(const char *victim);
void stash_char(struct char_data *ch);
void stash_contents(FILE *fl, struct obj_data *p, int wear_flag);
void unstash_char(struct char_data *ch, char *filename);
void wipe_stash(char *filename);
void do_checkrent(struct char_data *ch, char *argument, int cmd);
void do_extractrent(struct char_data *ch, char *argument, int cmd);
void do_replacerent(struct char_data *ch, char *argument, int cmd);
void do_rent(struct char_data *ch, char *arg, int cmd);
void wipe_obj(struct obj_data *obj);

/* --- fight.c --- */
void appear(struct char_data *ch);
void load_messages(void);
void update_pos(struct char_data *victim);
void set_fighting(struct char_data *ch, struct char_data *vict);
void stop_fighting(struct char_data *ch);
void make_corpse(struct char_data *ch, int level);
void change_alignment(struct char_data *ch, struct char_data *victim);
void death_cry(struct char_data *ch);
void raw_kill(struct char_data *ch, int level);
void die(struct char_data *ch, int level, struct char_data *who);
void group_gain(struct char_data *ch, struct char_data *victim);
char *replace_string(char *str, char *weapon);
void dam_message(int dam, struct char_data *ch, struct char_data *victim, int w_type);
void damage(struct char_data *ch, struct char_data *victim, int dam, int attacktype);
void hit(struct char_data *ch, struct char_data *victim, int type);
void perform_violence(void);

/* --- gbisland.c --- */
void gbisland_move_seashore(struct char_data *ch);
int gbisland_sea(struct char_data *ch, int cmd, char *arg);
void gbisland_false_move(struct char_data *ch, int dir);
void gbisland_go_out_barrier(struct char_data *ch);
void gbisland_go_back(struct char_data *ch);
int gbisland_saint_mirror(struct char_data *ch, int cmd, char *arg);
int gbisland_lanessa(struct char_data *ch, int cmd, char *arg);
int gbisland_carpie(struct char_data *ch, int cmd, char *arg);
int gbisland_magic_paper(struct char_data *ch, int cmd, char *arg);
int gbisland_seed_evil_power(struct char_data *ch, int cmd, char *arg);

/* --- get_mobile_stat.c --- */
int get_simplified_tier(int raw_tier);
double get_base_hp_tier1(int level);
int get_mob_hit(int level, int tier);
int get_mob_ac(int level, int tier);
int get_mob_hr(int level, int tier);
int get_mob_dr(int level, int tier);
int get_mob_exp(int level, int tier);

/* --- guild_command.c --- */
int taxi(struct char_data *ch, int cmd, char *arg);
int guild_entry(struct char_data *ch, int cmd, char *arg);
int locker_room(struct char_data *ch, int cmd, char *arg);
int guild_practice_yard(struct char_data *ch, int cmd, char *arg);

/* --- guild_informative.c --- */
void do_cant(struct char_data *ch, char *argument, int cmd);
void do_query(struct char_data *ch, char *argument, int cmd);

/* --- guild_skills.c --- */
void do_power_bash(struct char_data *ch, char *argument, int cmd);
void do_whistle(struct char_data *ch, char *argument, int cmd);
void do_simultaneous(struct char_data *ch, char *argument, int cmd);
void do_arrest(struct char_data *ch, char *argument, int cmd);
void do_charge(struct char_data *ch, char *argument, int cmd);
void do_angry_yell(struct char_data *ch, char *argument, int cmd);
void do_smoke(struct char_data *ch, char *argument, int cmd);
void do_inject(struct char_data *ch, char *argument, int cmd);
void do_shadow(struct char_data *ch, char *argument, int cmd);
void do_solace(struct char_data *ch, char *argument, int cmd);
void do_evil_strike(struct char_data *ch, char *argument, int cmd);

/* --- handler.c --- */
int is_same_group(struct char_data *ch, struct char_data *victim);
char *fname(char *namelist);
int isname(char *str, char *namelist);
int isexactname(char *str, char *namelist);
void affect_modify(struct char_data *ch, byte loc, short mod, long bitv, bool add);
void affect_total(struct char_data *ch);
void affect_to_char(struct char_data *ch, struct affected_type *af);
void affect_remove(struct char_data *ch, struct affected_type *af);
void affect_from_char(struct char_data *ch, byte skill);
bool affected_by_spell(struct char_data *ch, byte skill);
void affect_join(struct char_data *ch, struct affected_type *af, bool avg_dur, bool avg_mod);
void char_from_room(struct char_data *ch);
void char_to_room(struct char_data *ch, int room);
void obj_to_char(struct obj_data *object, struct char_data *ch);
void obj_from_char(struct obj_data *object);
int apply_ac(struct char_data *ch, int eq_pos);
void equip_char(struct char_data *ch, struct obj_data *obj, int pos);
struct obj_data *unequip_char(struct char_data *ch, int pos);
int get_number(char **name);
struct obj_data *get_obj_in_list(char *name, struct obj_data *list);
struct obj_data *get_obj_in_list_num(int num, struct obj_data *list);
struct obj_data *get_obj(char *name);
struct obj_data *get_obj_num(int nr);
struct char_data *get_char_room(char *name, int room);
struct char_data *get_char(char *name);
struct char_data *get_char_num(int nr);
void obj_to_room(struct obj_data *object, int room);
void obj_from_room(struct obj_data *object);
void obj_to_obj(struct obj_data *obj, struct obj_data *obj_to);
void obj_from_obj(struct obj_data *obj);
void object_list_new_owner(struct obj_data *list, struct char_data *ch);
void extract_obj(struct obj_data *obj);
void update_object(struct obj_data *obj, int use);
void update_char_objects(struct char_data *ch);
void extract_char(struct char_data *ch, int drop_items);
struct char_data *get_char_room_vis(struct char_data *ch, char *name);
struct char_data *get_specific_vis(struct char_data *ch, char *name, int type);
struct char_data *get_char_vis(struct char_data *ch, char *name);
struct char_data *get_char_vis_zone(struct char_data *ch, char *name);
struct obj_data *get_obj_in_list_vis(struct char_data *ch, char *name, struct obj_data *list);
struct obj_data *get_obj_vis(struct char_data *ch, char *name);
struct obj_data *create_money(int amount);
int generic_find(char *arg, int bitvector, struct char_data *ch, struct char_data **tar_ch, struct obj_data **tar_obj);

/* --- interpreter.c --- */
void echo_local(int fd);
void no_echo_local(int fd);
void echo_local(int fd);
void no_echo_local(int fd);
void no_echo_telnet(struct descriptor_data *d);
void echo_telnet(struct descriptor_data *d);
int search_block(char *arg, char **list, bool exact);
int old_search_block(char *argument, int begin, int length, char **list, int mode);
int improve_status(struct char_data *ch, char arg);
int command_interpreter(struct char_data *ch, char *argument);
void argument_interpreter(char *argument, char *first_arg, char *second_arg);
int is_number(char *str);
char *one_argument(char *argument, char *first_arg);
int fill_word(char *argument);
int is_abbrev(char *arg1, char *arg2);
void half_chop(char *string, char *arg1, char *arg2);
int special(struct char_data *ch, int cmd, char *arg);
void assign_command_pointers(void);
void query_status(struct descriptor_data *d);
int find_name(char *name);
int _parse_name(char *arg, char *name);
void nanny(struct descriptor_data *d, char *arg);

/* --- limit.c --- */
int graf(int age, int p0, int p1, int p2, int p3, int p4, int p5, int p6);
long int hit_limit(struct char_data *ch);
long int mana_limit(struct char_data *ch);
long int move_limit(struct char_data *ch);
int hit_gain(struct char_data *ch);
int mana_gain(struct char_data *ch);
int move_gain(struct char_data *ch);
void advance_level(struct char_data *ch, int level_up);
void set_title(struct char_data *ch);
void gain_exp(struct char_data *ch, int gain);
void gain_exp_regardless(struct char_data *ch, int gain);
void gain_condition(struct char_data *ch, int condition, int value);
void check_idling(struct char_data *ch);
void point_update(void);

/* --- magic.c --- */
void spell_magic_missile(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_chill_touch(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_burning_hands(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_shocking_grasp(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_lightning_bolt(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_color_spray(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_energy_drain(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_fireball(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_disintegrate(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_corn_of_ice(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_sunburst(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_energyflow(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_full_fire(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_throw(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_earthquake(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_all_heal(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_sanctuary_cloud(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_hand_of_god(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_fire_storm(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_trick(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_dispel_evil(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_call_lightning(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_harm(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_identify(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
int ac_applicable(struct obj_data *obj_object);
void spell_fire_breath(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_frost_breath(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_gas_breath(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_lightning_breath(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);

/* --- magic2.c --- */
void spell_armor(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_teleport(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_far_look(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_relocate(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_damage_up(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_crush_armor(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_bless(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_blindness(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_create_food(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_create_water(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_create_nectar(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_create_golden_nectar(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_cure_blind(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_cure_critic(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_cause_critic(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_mana_boost(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_vitalize(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_cure_light(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_cause_light(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_curse(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_detect_evil(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_detect_invisibility(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_recharger(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_enchant_person(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_enchant_weapon(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_enchant_armor(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_pray_for_armor(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_self_heal(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_restore_move(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_heal(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_full_heal(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_entire_heal(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_invisibility(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_locate_object(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_poison(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_protection_from_evil(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_haste(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_improved_haste(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_remove_curse(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_remove_poison(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_infravision(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_mirror_image(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_sanctuary(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_death(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_love(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_reraise(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_sleep(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_strength(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_ventriloquate(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_phase(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_word_of_recall(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_summon(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_charm_person(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_preach(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_sense_life(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_reanimate(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_clone(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);

/* --- magic3.c --- */
void spell_spell_block(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_reflect_damage(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_dumb(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_holy_shield(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_mana_transfer(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_kiss_of_process(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void spell_thunder_bolt(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);

/* --- magic_weapon.c --- */
void magic_weapon_hit(struct char_data *ch, struct char_data *victim, struct obj_data *weapon);
void sweapon_smash(struct char_data *ch, struct char_data *victim, struct obj_data *weapon);
void sweapon_flame(struct char_data *ch, struct char_data *victim, struct obj_data *weapon);
void sweapon_ice(struct char_data *ch, struct char_data *victim, struct obj_data *weapon);
void sweapon_bombard(struct char_data *ch, struct char_data *victim, struct obj_data *weapon);
void sweapon_shot(struct char_data *ch, struct char_data *victim, struct obj_data *weapon);
void sweapon_dragon_slayer(struct char_data *ch, struct char_data *victim, struct obj_data *weapon);
void sweapon_anti_good(struct char_data *ch, struct char_data *victim, struct obj_data *weapon);
void sweapon_anti_evil(struct char_data *ch, struct char_data *victim, struct obj_data *weapon);
void sweapon_god(struct char_data *ch, struct char_data *victim, struct obj_data *weapon);

/* --- mail.c --- */
void init_mbox();
struct mbox_data *init_a_mbox(struct char_data *ch);
struct mbox_data *find_mbox(struct char_data *ch);
void load_mbox(struct mbox_data *cb);
void save_mbox(struct mbox_data *cb);
int mbox(struct char_data *ch, int cmd, char *arg);
int show_mail(struct char_data *ch, struct mbox_data *cb, char *arg);
void remove_mail(struct mbox_data *cb, int m_num);
int get_mail(struct char_data *ch, struct mbox_data *cb, char *arg);
int post_mail(struct char_data *ch, struct mbox_data *cb, char *arg);
void mail_error_log(char *str);

/* --- mob_magic.c --- */
void mob_spell_fire_storm(byte level, struct char_data *ch, struct char_data *victim, struct obj_data *obj);
void mob_light_move(struct char_data *ch, char *argument, int cmd);
void mob_punch_drop(struct char_data *ch, struct char_data *victim);
struct char_data *choose_victim(struct char_data *mob, int fightmode, int mode);
struct char_data *choose_rescue_mob(struct char_data *mob);

/* --- mobact.c --- */
int check_stat(struct char_data *ch);
void mobile_activity2(void);
void mobile_activity(void);

/* --- modify.c --- */
void string_add(struct descriptor_data *d, char *str);
void quad_arg(char *arg, int *type, char *name, int *field, char *string);
void do_string(struct char_data *ch, char *arg, int cmd);
char *one_word(char *argument, char *first_arg);
struct help_index_element *build_help_index(FILE *fl, int *num);
void page_string(struct descriptor_data *d, char *str, int keep_internal);
void show_string(struct descriptor_data *d, char *input);

/* --- newcom.c --- */
void do_assist(struct char_data *ch, char *argument, int cmd);
void stop_group(struct char_data *ch);
void do_ungroup(struct char_data *ch, char *argument, int cmd);
void do_version(struct char_data *ch, char *argument, int cmd);
void do_resetzone(struct char_data *ch, char *argument, int cmd);
void do_reloadzone(struct char_data *ch, char *argument, int cmd);

/* --- quest.c --- */
char *find_zone(int number);
int get_quest(struct char_data *ch);
void do_request(struct char_data *ch, char *arg, int cmd);
void do_hint(struct char_data *ch, char *arg, int cmd);
void do_quest(struct char_data *ch, char *arg, int cmd);
void init_quest(void);
int give_reward_for_quest(int level);
void check_quest_mob_die(struct char_data *ch, int mob);
int quest_room(struct char_data *ch, int cmd, char *arg);
void do_challenge(struct char_data *ch, char *argument, int cmd);
void do_begin(struct char_data *ch, char *argument, int cmd);
void do_rejoin(struct char_data *ch, char *argument, int cmd);
void do_challenge_abort(struct char_data *ch, char *argument, int cmd);

/* --- shop.c --- */
int is_ok(struct char_data *keeper, struct char_data *ch, int shop_nr);
int trade_with(struct obj_data *item, int shop_nr);
int shop_producing(struct obj_data *item, int shop_nr);
void shopping_buy(char *arg, struct char_data *ch, struct char_data *keeper, int shop_nr);
void shopping_sell(char *arg, struct char_data *ch, struct char_data *keeper, int shop_nr);
void shopping_value(char *arg, struct char_data *ch, struct char_data *keeper, int shop_nr);
void shopping_list(char *arg, struct char_data *ch, struct char_data *keeper, int shop_nr);
int shop_keeper(struct char_data *ch, int cmd, char *arg);
void boot_the_shops();
void assign_the_shopkeepers();

/* --- spec_assign.c --- */
void assign_mobiles(void);
void assign_objects(void);
void assign_rooms(void);

/* --- spec_procs.c --- */
char *how_good(int p1, int p2);
int guild(struct char_data *ch, int cmd, char *arg);
int dump(struct char_data *ch, int cmd, char *arg);
int mayor(struct char_data *ch, int cmd, char *arg);
void npc_tornado(struct char_data *ch);
void npc_steal(struct char_data *ch, struct char_data *victim);
int snake(struct char_data *ch, int cmd, char *arg);
void first_attack(struct char_data *ch, struct char_data *victim);
struct char_data *select_victim(struct char_data *ch);
int warrior(struct char_data *ch, int cmd, char *arg);
int thief(struct char_data *ch, int cmd, char *arg);
int cleric(struct char_data *ch, int cmd, char *arg);
int magic_user(struct char_data *ch, int cmd, char *arg);
int thief(struct char_data *ch, int cmd, char *arg);
int cleric(struct char_data *ch, int cmd, char *arg);
int magic_user(struct char_data *ch, int cmd, char *arg);
int paladin(struct char_data *ch, int cmd, char *arg);
int dragon(struct char_data *ch, int cmd, char *arg);

/* --- spec_procs2.c --- */
int puff(struct char_data *ch, int cmd, char *arg);
int super_deathcure(struct char_data *ch, int cmd, char *arg);
int deathcure(struct char_data *ch, int cmd, char *arg);
int perhaps(struct char_data *ch, int cmd, char *arg);
int Quest_bombard(struct char_data *ch, int cmd, char *arg);
int mud_message(struct char_data *ch, int cmd, char *arg);
int musashi(struct char_data *ch, int cmd, char *arg);
int super_musashi(struct char_data *ch, int cmd, char *arg);
int mom(struct char_data *ch, int cmd, char *arg);
int singer(struct char_data *ch, int cmd, char *arg);
int fido(struct char_data *ch, int cmd, char *arg);
int janitor(struct char_data *ch, int cmd, char *arg);
int cityguard(struct char_data *ch, int cmd, char *arg);
int rescuer(struct char_data *ch, int cmd, char *arg);
int superguard(struct char_data *ch, int cmd, char *arg);
int pet_shops(struct char_data *ch, int cmd, char *arg);
int hospital(struct char_data *ch, int cmd, char *arg);
int metahospital(struct char_data *ch, int cmd, char *arg);
int remortal(struct char_data *ch, int cmd, char *arg);
int jale_room(struct char_data *ch, int cmd, char *arg);
int safe_house(struct char_data *ch, int cmd, char *arg);
int shooter(struct char_data *ch, int cmd, char *arg);
int finisher(struct char_data *ch, int cmd, char *arg);
int bank(struct char_data *ch, int cmd, char *arg);
int totem(struct char_data *ch, int cmd, char *arg);
int kickbasher(struct char_data *ch, int cmd, char *arg);
int spell_blocker(struct char_data *ch, int cmd, char *arg);
int archmage(struct char_data *ch, int cmd, char *arg);
int spitter(struct char_data *ch, int cmd, char *arg);
int portal(struct char_data *ch, int cmd, char *arg);
int magicseed(struct char_data *ch, int cmd, char *arg);

/* --- spec_procs3.c --- */
int level_gate(struct char_data *ch, int cmd, char *arg);
int neverland(struct char_data *ch, int cmd, char *arg);
int helper(struct char_data *ch, int cmd, char *arg);
int electric_shock(struct char_data *ch, int cmd, char *arg);
int great_mazinga(struct char_data *ch, int cmd, char *arg);
int string_machine(struct char_data *ch, int cmd, char *arg);
int slot_machine(struct char_data *ch, int cmd, char *arg);

/* --- spell_parser.c --- */
int use_mana(struct char_data *ch, int sn);
void affect_update(void);
bool circle_follow(struct char_data *ch, struct char_data *victim);
void stop_follower(struct char_data *ch);
void die_follower(struct char_data *ch);
void add_follower(struct char_data *ch, struct char_data *leader);
void say_spell(struct char_data *ch, int si);
bool saves_spell(struct char_data *ch, int save_type);
char *skip_spaces(char *string);
void do_cast(struct char_data *ch, char *argument, int cmd);
void assign_spell_pointers(void);

/* --- spells1.c --- */
void cast_burning_hands(byte level, struct char_data *ch, char *arg, int type, struct char_data *victim, struct obj_data *tar_obj);
void cast_call_lightning(byte level, struct char_data *ch, char *arg, int type, struct char_data *victim, struct obj_data *tar_obj);
void cast_chill_touch(byte level, struct char_data *ch, char *arg, int type, struct char_data *victim, struct obj_data *tar_obj);
void cast_shocking_grasp(byte level, struct char_data *ch, char *arg, int type, struct char_data *victim, struct obj_data *tar_obj);
void cast_color_spray(byte level, struct char_data *ch, char *arg, int type, struct char_data *victim, struct obj_data *tar_obj);
void cast_earthquake(byte level, struct char_data *ch, char *arg, int type, struct char_data *victim, struct obj_data *tar_obj);
void cast_all_heal(byte level, struct char_data *ch, char *arg, int type, struct char_data *victim, struct obj_data *tar_obj);
void cast_sanctuary_cloud(byte level, struct char_data *ch, char *arg, int type, struct char_data *victim, struct obj_data *tar_obj);
void cast_hand_of_god(byte level, struct char_data *ch, char *arg, int type, struct char_data *victim, struct obj_data *tar_obj);
void cast_fire_storm(byte level, struct char_data *ch, char *arg, int type, struct char_data *victim, struct obj_data *tar_obj);
void cast_energy_drain(byte level, struct char_data *ch, char *arg, int type, struct char_data *victim, struct obj_data *tar_obj);
void cast_disintegrate(byte level, struct char_data *ch, char *arg, int type, struct char_data *victim, struct obj_data *tar_obj);
void cast_sunburst(byte level, struct char_data *ch, char *arg, int type, struct char_data *victim, struct obj_data *tar_obj);
void cast_energyflow(byte level, struct char_data *ch, char *arg, int type, struct char_data *victim, struct obj_data *tar_obj);
void cast_full_fire(byte level, struct char_data *ch, char *arg, int type, struct char_data *victim, struct obj_data *tar_obj);
void cast_throw(byte level, struct char_data *ch, char *arg, int type, struct char_data *victim, struct obj_data *tar_obj);
void cast_fireball(byte level, struct char_data *ch, char *arg, int type, struct char_data *victim, struct obj_data *tar_obj);
void cast_corn_of_ice(byte level, struct char_data *ch, char *arg, int type, struct char_data *victim, struct obj_data *tar_obj);
void cast_harm(byte level, struct char_data *ch, char *arg, int type, struct char_data *victim, struct obj_data *tar_obj);
void cast_lightning_bolt(byte level, struct char_data *ch, char *arg, int type, struct char_data *victim, struct obj_data *tar_obj);
void cast_magic_missile(byte level, struct char_data *ch, char *arg, int type, struct char_data *victim, struct obj_data *tar_obj);
void cast_clone(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);

/* --- spells2.c --- */
void cast_armor(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_teleport(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_damage_up(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_crush_armor(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_bless(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_blindness(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_create_food(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_create_water(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_create_nectar(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_create_golden_nectar(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_cure_blind(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_mana_boost(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_vitalize(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_trick(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_cause_critic(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_cure_critic(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_cause_light(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_cure_light(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_curse(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_detect_evil(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_detect_invisibility(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_preach(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_dispel_evil(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_recharger(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_enchant_weapon(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_enchant_person(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_enchant_armor(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_pray_for_armor(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_self_heal(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_restore_move(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_heal(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_full_heal(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_entire_heal(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_invisibility(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_locate_object(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_poison(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_protection_from_evil(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_haste(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_improved_haste(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_remove_curse(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_remove_poison(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_infravision(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_mirror_image(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_death(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_sanctuary(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_love(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_reraise(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_sleep(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_strength(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_phase(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_ventriloquate(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_word_of_recall(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_summon(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_far_look(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_relocate(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_charm_person(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_sense_life(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_identify(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_fire_breath(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_frost_breath(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_gas_breath(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_lightning_breath(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_reanimate(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);

/* --- spells3.c --- */
void cast_spell_block(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_dumb(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_reflect_damage(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_holy_shield(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_mana_transfer(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_kiss_of_process(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_thunder_bolt(byte level, struct char_data *ch, char *arg, int type, struct char_data *tar_ch, struct obj_data *tar_obj);

/* --- utility.c --- */
int number(int from, int to);
int dice(int number, int size);
int str_cmp(char *arg1, char *arg2);
int strn_cmp(char *arg1, char *arg2, int n);
void sprintbit(long vektor, char *names[], char *result);
void sprinttype(int type, char *names[], char *result);
struct time_info_data real_time_passed(time_t t2, time_t t1);
struct time_info_data mud_time_passed(time_t t2, time_t t1);
struct time_info_data age(struct char_data *ch);
void print_increased_skilled(struct char_data *ch, int sk_no);
void mudlog(const char *str);
void start_progress_bar(struct char_data *ch);
void process_color_string(const char *input, char *output, int max_out_len);
void do_colortest(struct char_data *ch, char *argument, int cmd);
size_t strlcat(char *dest, const char *src, size_t size);
void prune_crlf(char *txt);
void utf8_safe_strncpy(char *dest, const char *src, size_t n);
size_t strlcpy(char *dst, const char *src, size_t siz);

/* --- weather.c --- */
void weather_and_time(int mode);
void another_hour(int mode);
void weather_change(int change);

#endif /* __PROTOTYPES_H__ */
