/* ************************************************************************
*  file: comm.c , Communication module.                   Part of DIKUMUD *
*  Usage: Communication, central game loop.                               *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete information. *
*  All Rights Reserved                                                    *
*  Using *any* part of DikuMud without having read license.doc is         *
*  violating our copyright.
************************************************************************* */

#include <errno.h>
#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "comm2.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "colors.h" // 색상 매크로

#ifndef DFLT_DIR
#define DFLT_DIR "lib"		/* default port */
#endif				// DFLT_DIR
#ifndef DFLT_PORT
#define DFLT_PORT 5101		/* default port */
#endif				// DFLT_PORT

#define MAX_NAME_LENGTH 15
#define MAX_HOSTNAME    256
#define OPT_USEC        200000 /* time delay corresponding to 5 passes/sec */
#define MAXFDALLOWED    200
#define MAXOCLOCK       1200
#define A_DAY           86400
#define MINUTES(m)      (60 * (m))
#define TICS_PER_SEC    5
#define SAVE_PERIOD_MINUTES 10 /* 10분마다 플레이어 저장 */
#define PLAYER_SAVE_PERIOD  (MINUTES(SAVE_PERIOD_MINUTES) * TICS_PER_SEC)

extern int errno;
extern int no_echo;
extern struct room_data *world;			/* In db.c */
extern int top_of_world;				/* In db.c */
extern struct time_info_data time_info;	/* In db.c */
extern char help[];
extern char *connected_types[];

extern void no_echo_telnet(struct descriptor_data *d);
extern void echo_telnet(struct descriptor_data *d);
extern void no_echo_local(int fd);
extern void weather_and_time(int mode);

/* local globals */
struct descriptor_data *descriptor_list, *next_to_process;

int s;
int slow_death = 0;		/* Shut her down, Martha, she's sucking mud */
int shutdowngame = 0;		/* clean shutdown */
int reboot_game = 0;
int reboot_counter = -1;
struct timeval null_time;
int boottime;
unsigned long reboot_time = REBOOT_TIME; // 현재 4일

int maxdesc, avail_descs;
int tics = 0; /* for extern checkpointing */

char pidfile[256] = {0};

int nochatflag = 0;
int nodisarmflag = 0;		/* for disarm .Chase written */
int noenchantflag = 0;		/* noenchant!! */
int nokillflag = 0;
int nonewplayers = 0;
int no_specials = 0;		/* Suppress ass. of special routines */
int nostealflag = 0;
int noshoutflag = 0;

/*
 * move to db.c
int regen_percent = 50;
int regen_time_percent = 66;
int regen_time = 200;
*/

struct descriptor_data *xo;

void game_loop(int s);
void signal_setup(void);
void handle_graceful_shutdown(int sig);
void handle_immediate_shutdown(int sig);
void run_the_game(int port);

/* write_to_q is in comm.h for the macro */
int get_from_q(struct txt_q *queue, char *dest);
int init_socket(int port);
int new_connection(int s);
int new_descriptor(int s);
int unfriendly_domain(char *h);
int process_output(struct descriptor_data *t);
int process_input(struct descriptor_data *t);
void saveallplayers(void);
void zapper(void);
void checkpointing(int sig);
void nonblock(int s);
void on_echo_local(int fd);
void transall(int room);
int move_stashfile_safe(const char *victim); // 251016
void close_sockets(int s);
void close_socket(struct descriptor_data *d);
void stash_char(struct char_data *ch);
void flush_queues(struct descriptor_data *d);
void parse_name(struct descriptor_data *desc, char *arg);
void log_abnormal_disconnect(struct descriptor_data *d);
struct timeval timediff(struct timeval *a, struct timeval *b);

/* extern fcnts */
int is_korean(struct descriptor_data *d);
void boot_db(void);
void zone_update(void);
void record_player_number(void);
void affect_update(void);	/* In spells.c */
void point_update(void);	/* In limits.c */
void free_char(struct char_data *ch);
void mudlog(const char *str);
void mobile_activity(void);
void mobile_activity2(void);
void string_add(struct descriptor_data *d, char *str);
void perform_violence(void);
void stop_fighting(struct char_data *ch);
void show_string(struct descriptor_data *d, char *input);
void save_char(struct char_data *ch, sh_int load_room);
void do_assist(struct char_data *ch, char *argument, int cmd);
void process_color_string(const char *input, char *output, int max_out_len); // Komo
struct char_data *make_char(char *name, struct descriptor_data *desc);

/* *********************************************************************
*  main game loop and related stuff               *
********************************************************************* */

int main(int argc, char **argv)
{
	int port;
	char buf[512];

	close(0);
	close(1);
	baddoms = 0;
	port = DFLT_PORT;

	if (argc == 2) {
		char *p = argv[1];
        while (*p) {
            if (!isdigit(*p)) {
                fprintf(stderr, "(main) Port number must contain only digits. Usage: %s [ port # ]\n", argv[0]);
                exit(1);
            }
            p++;
        }
        if ((port = atoi(argv[1])) <= 1024) {
            fprintf(stderr, "Illegal port number.\n");
            exit(0);
		}
	}

	srandom(boottime = time(0));

	snprintf(buf, sizeof(buf), "mud-%d.pid", port);
	if (access(buf, F_OK) == 0) {
		mudlog("Port busy: pid file already exists.");
		exit(port);
	}

	umask(0077);

	mudlog("(main) Signal trapping.");

	signal_setup();

	snprintf(buf, sizeof(buf), "(main) Running game on port %d.", port);
	mudlog(buf);

	snprintf(buf, sizeof(buf), "Running game on port %d.", port);
	mudlog(buf);

	run_the_game(port);

	// PID 파일 삭제 (정상 종료 시)
    if (pidfile[0] != '\0') {
        unlink(pidfile);
        mudlog("(main) PID file removed on normal termination.");
    }
    mudlog("(main) Normal termination of game.");

	return (0);
}

/* Init sockets, run game, and cleanup sockets */
void run_the_game(int port)
{
	descriptor_list = NULL;

	mudlog("(run_the_game) Opening mother connection.");
	mudlog("Signal trapping.");
	signal_setup();
	mudlog("Opening mother connection.");
	s = init_socket(port);

	boot_db();

	FILE *pidfp;
	mudlog("(run_the_game) Writing pid file.");

	if (!(pidfp = fopen(pidfile, "w+"))) {
        perror("(run_the_game) ERROR: can't open pid file.");
    } else {
        fprintf(pidfp, "%d\n", getpid());
        fclose(pidfp);
    }

	mudlog("(run_the_game) Entering game loop.");
	no_echo_local(s);

	game_loop(s);
	mudlog("(run_the_game) DOWN??? SAVE ALL CHARS???");
	char pidfile[256];
	sprintf(pidfile, "writing pid file: mud-%d.pid", port);
	mudlog(pidfile);

	sprintf(pidfile, "mud-%d.pid", port);
	if (!(pidfp = fopen(pidfile, "w+"))) {
		perror("ERROR: can't open pid file.");
	} else {
		fprintf(pidfp, "%ld\n", (long)getpid());
		fclose(pidfp);
	}

	mudlog("Entering game loop.");
	no_echo_local(s);

	game_loop(s);
	mudlog("DOWN??????????SAVE ALL CHARS???????");
	transall(3001);
	mudlog("(run_the_game) Game loop ended. Saving all characters.");
	saveallplayers();
	close_sockets(s);
	shutdown(s, 2);

	mudlog("(run_the_game) Normal termination of game.");
	unlink(pidfile);
	mudlog("Normal termination of game.");
}

// 신호 처리 로직 통합, by Komo
void handle_graceful_shutdown(int sig)
{
    reboot_game = 1; // '리붓' 플래그 설정
    mudlog("Received SIGUSR1 - Graceful shutdown/reboot request.");
}


// SIGUSR2, SIGINT, SIGTERM: 즉시 종료
void handle_immediate_shutdown(int sig)
{
    if (!shutdowngame) {
        mudlog("Received signal for immediate shutdown.");
        if (pidfile[0] != '\0') {
            unlink(pidfile);
            mudlog("PID file removed by signal handler.");
        }
        saveallplayers();
        exit(0);
    }
}


void signal_setup(void)
{
    // checkpointing을 위한 타이머 설정 추가
    struct itimerval itime;
    struct timeval interval;

    interval.tv_sec = 900; // 15분
    interval.tv_usec = 0;
    itime.it_interval = interval;
    itime.it_value = interval;
    setitimer(ITIMER_REAL, &itime, 0); // 타이머 시작
    signal(SIGALRM, checkpointing);

    signal(SIGPIPE, SIG_IGN);
    signal(SIGUSR1, handle_graceful_shutdown);  // 1분 후 리붓
    signal(SIGUSR2, handle_immediate_shutdown); // 즉시 종료
    signal(SIGINT, handle_immediate_shutdown);  
    signal(SIGTERM, handle_immediate_shutdown); 
    signal(SIGHUP, handle_immediate_shutdown);  // 기타 시스템 신호
}


void game_loop(int s)
{
	fd_set input_set, output_set, exc_set;
	struct timeval last_time, now, timespent, timeout;
	static struct timeval opt_time;
	char comm[MAX_INPUT_LENGTH];
	struct descriptor_data *point, *next_point;
	struct char_data *ch;
	int pulse = 0;
	char prmpt[32];
	int pulse_per_mud_hour;

	null_time.tv_sec = 0;
	null_time.tv_usec = 0;
	opt_time.tv_usec = OPT_USEC;	/* Init time values */
	opt_time.tv_sec = 0;
	gettimeofday(&last_time, (struct timezone *)0);

	maxdesc = s;
	avail_descs = MAXFDALLOWED;

	/* Main loop */
	while (!shutdowngame) {
		/* Check what's happening out there */
		FD_ZERO(&input_set);
		FD_ZERO(&output_set);
		FD_ZERO(&exc_set);
		FD_SET(s, &input_set);
		for (point = descriptor_list; point; point = point->next) {
			FD_SET(point->descriptor, &input_set);
			FD_SET(point->descriptor, &exc_set);
			FD_SET(point->descriptor, &output_set);
		}

		/* check out the time */
		gettimeofday(&now, (struct timezone *)0);
		timespent = timediff(&now, &last_time);
		timeout = timediff(&opt_time, &timespent);
		last_time.tv_sec = now.tv_sec + timeout.tv_sec;
		last_time.tv_usec = now.tv_usec + timeout.tv_usec;

		if (last_time.tv_usec >= 1000000) {
			last_time.tv_usec -= 1000000;
			last_time.tv_sec++;
		}

		if (select(maxdesc + 1, &input_set, &output_set, &exc_set, &null_time) < 0) {
			perror("Select poll");
			return;
		}
		if (select(0, (fd_set *)0, (fd_set *)0, (fd_set *)0, &timeout) < 0) {
			if (errno != EINTR) {
                perror("Select sleep");
                exit(1);
            }
		}

		/* Respond to whatever might be happening */
		/* New connection? */
		if (FD_ISSET(s, &input_set))
			if (new_descriptor(s) < 0)
				perror("New connection");

		/* Handle network exceptions */
		for (point = descriptor_list; point; point = next_point) {
			next_point = point->next;
			if (FD_ISSET(point->descriptor, &exc_set)) {
				log_abnormal_disconnect(point);
				FD_CLR(point->descriptor, &input_set);
				FD_CLR(point->descriptor, &output_set);
				mudlog("Kicked out a freaky folk.\n\r");
				close_socket(point);
			}
		}
		for (point = descriptor_list; point; point = next_point) {
			next_point = point->next;
			if (FD_ISSET(point->descriptor, &input_set))
				if (process_input(point) < 0) {
					FD_CLR(point->descriptor, &input_set);
					FD_CLR(point->descriptor, &output_set);
					close_socket(point);
				}
		}
		/* process_commands; */
		for (point = descriptor_list; point; point = next_to_process) {
			next_to_process = point->next;
			if ((--(point->wait) <= 0) && get_from_q(&point->input, comm)) {
				if (no_echo)
					SEND_TO_Q("\n", point);
				echo_telnet(point);
				no_echo = 0;
				if (point->character && point->connected ==
				    CON_PLYNG &&
				    point->character->specials.was_in_room != NOWHERE) {
					if (point->character->in_room != NOWHERE)
						char_from_room(point->character);
					char_to_room(point->character, point->character->specials.was_in_room);
					point->character->specials.was_in_room = NOWHERE;
					act("$n has returned.", TRUE, point->character, 0, 0, TO_ROOM);
				}
				point->wait = 1;
				if (point->character)
					point->character->specials.timer = 0;
				point->prompt_mode = 1;
				if (point->str)
					string_add(point, comm);
				else if (!point->connected) {
					if (point->showstr_point)
						show_string(point, comm);
					else {
						/* ASan, 251204 */
                        int cmd_wait;
                        struct descriptor_data *d;
                        bool still_alive = FALSE;

                        cmd_wait = command_interpreter(point->character, comm);

                        for (d = descriptor_list; d; d = d->next) {
                            if (d == point) {
                                still_alive = TRUE;
                                break;
                            }
                        }

                        if (still_alive) {
                            point->wait += cmd_wait;
                            ++point->ncmds;
                        }
					}
				} else {
					nanny(point, comm);
					if (no_echo) {
						no_echo_telnet(point);
					}
				}
			}
		}
		for (point = descriptor_list; point; point = next_point) {
			next_point = point->next;
			if (FD_ISSET(point->descriptor, &exc_set)) {
				log_abnormal_disconnect(point);
				FD_CLR(point->descriptor, &input_set);
				FD_CLR(point->descriptor, &output_set);
				mudlog("Kicked out a freaky folk.\n\r");
				close_socket(point);
			}
		}
		for (point = descriptor_list; point; point = next_point) {
			next_point = point->next;
			if (FD_ISSET(point->descriptor, &output_set) && point->output.head) {
				if (process_output(point) < 0) {
					FD_CLR(point->descriptor, &input_set);
					FD_CLR(point->descriptor, &output_set);
					close_socket(point);
				} else
					point->prompt_mode = 1;
			}
		}
		/* --- PROMPT - give the people some prompts --- */
		for (point = descriptor_list; point; point = point->next) {
			if (point->prompt_mode) {
				if (point->str)
					write_to_descriptor(point->descriptor, "] ");
				else if (!point->connected) {
					if (point->showstr_point)
						write_to_descriptor(point->descriptor, "*** Press return ***");
					else {
						sprintf(prmpt, "\[%ld,%ld,%ld] ",
							GET_HIT(point->character),
							GET_MANA(point->character),
							GET_MOVE(point->character));
						write_to_descriptor(point->descriptor, prmpt);
					}
				}
				point->prompt_mode = 0;
			}
		}

		/* handle heartbeat stuff */
		pulse++;

		zapper();
		/* 
         * zapper()가 reboot_game=1로 만들면 아래 로직이 실행됨
         */
        if (reboot_game && reboot_counter < 0) {
            // 리붓 절차 시작
            reboot_counter = TICS_PER_SEC * 60; // 60초 카운트다운
            send_to_all("\n\r\n\r### SYSTEM: Server is rebooting in 1 minute. Please log off safely. ###\n\r\n\r");
            mudlog("Graceful shutdown sequence initiated (via zapper or SIGUSR1).");
            reboot_game = 0; // 플래그 초기화
        }

        if (reboot_counter >= 0) {
            if (pulse % TICS_PER_SEC == 0) { // Approx. every second
                int seconds_left = reboot_counter / TICS_PER_SEC;
                if (seconds_left == 30 || seconds_left == 10 || seconds_left <= 5) {
                    char buf[100];
                    snprintf(buf, sizeof(buf), "### SYSTEM: Rebooting in %d seconds. ###\n\r", seconds_left);
                    send_to_all(buf);
                }
            }
            if (--reboot_counter < 0) {
                shutdowngame = 1; // 1분 경과, 실제 종료 플래그 설정
            }
        } /* new shutdown logic */
		if (!(pulse % PULSE_ZONE)) {
			zone_update();
			record_player_number();
		}
		if (!(pulse % PULSE_MOBILE)) {
			mobile_activity();
		}
		/* added by atre */
		else if (!(pulse % PULSE_MOBILE2)) {
			mobile_activity2();
		}

		if (!(pulse % PULSE_VIOLENCE)) {
			perform_violence();
		}
		/* 
         * --- 게임내 시간 흐름(TICK)에 따른 처리 ---
         * ctmp라는 모호한 변수명을 pulse_per_mud_hour로 변경하고,
         * TICK 계산에 정의된 상수를 활용 - 251121 by Komo
         */
        pulse_per_mud_hour = SECS_PER_MUD_HOUR * TICS_PER_SEC;
		if (!(pulse % pulse_per_mud_hour)) {
			weather_and_time(1);
			affect_update();
			point_update();
		}
		/* 정의된 주기마다 플레이어 저장 (tics 기준) */
        if (tics > 0 && (tics % PLAYER_SAVE_PERIOD == 0)) {
            struct descriptor_data *d;
            int player_count = 0;
            char buf[256];

            for (d = descriptor_list; d; d = d->next) {
                if (d->connected == CON_PLYNG && d->character) {
                    player_count++;
                }
            }

            if (player_count > 0) {
                saveallplayers();
                snprintf(buf, sizeof(buf), "Periodic player save executed (%d min interval). %d players saved.",
                         SAVE_PERIOD_MINUTES, player_count);
                mudlog(buf);
            } else {
                snprintf(buf, sizeof(buf), "Periodic player save skipped (No active players, %d min interval).", 
                         SAVE_PERIOD_MINUTES);
                mudlog(buf);
            }
        }
		tics++;
	}
}

void transall(int room)
{
	struct descriptor_data *pt, *npt;

	for (pt = descriptor_list; pt; pt = npt) {
		npt = pt->next;
		if (pt->connected == CON_PLYNG)
			if (pt->character) {
				char_from_room(pt->character);
				char_to_room(pt->character, real_room(room));
				send_to_char(
						    "\n\r\n\r\n\r\n\rShutdown System has transferred you!\n\r\n\r",
						    pt->character);
				send_to_all("&H   MUD will be up in 65 seconds :p&n\n\r\n\r");
				process_output(pt);
			}
	}
}

void saveallplayers()
{
	struct descriptor_data *pt, *npt;

	for (pt = descriptor_list; pt; pt = npt) {
		npt = pt->next;
		if (pt->connected == CON_PLYNG)
			if (pt->character) {
				save_char(pt->character,
					  world[pt->character->in_room].number);
				stash_char(pt->character);
				move_stashfile_safe(pt->character->player.name);
			}
	}
}


void record_player_number()
{
	char line[256];
	int t, tod;		// , in_d=0, out_d=0;
	struct descriptor_data *d;
	int m = 0, n = 0;
	static int most = 0;
	t = time(0) + 32400;	/* 86400 is a day */
	tod = (t % 3600);

	// zone period is about 48 sec, but rather irregular.
	static int hour = -1;
	int th = (t % A_DAY) / MINUTES(60);
	if (tod < MINUTES(1) && hour != th) {
		hour = th;
		line[0] = 0;
		for (d = descriptor_list; d; d = d->next) {
			++m;
			if (d->original) {
				sprintf(line + strlen(line), "%3d%2d:", d->descriptor,
					d->original->specials.timer);
				sprintf(line + strlen(line), "%-14s%2d ",
					GET_NAME(d->original), GET_LEVEL(d->original));
			} else if (d->character) {
				sprintf(line + strlen(line), "%3d%2d:", d->descriptor,
					d->character->specials.timer);
				sprintf(line + strlen(line), "%-14s%2d ",
					(d->connected == CON_PLYNG) ? GET_NAME(d->character)
					: "Not in game",
					GET_LEVEL(d->character));
			} else
				sprintf(line + strlen(line), "%3d%9s%10s ",
					d->descriptor, "  UNDEF  ",
					connected_types[d->connected]);
			sprintf(line + strlen(line), "%-15s", d->host);
			
			if (!(n % 2)) {
				strcat(line, "|");
			} else {
				mudlog(line);
				line[0] = 0;
			}
			++n;
		}
		if (n % 2) {
			mudlog(line);
		}

		if (m > most)
			most = m;
		sprintf(line, "%s%d/%d active connections",
			(n % 2) ? "\n\r" : "", m, most);
		mudlog(line);
		t = 30 + time(0) - boottime;
		sprintf(line, "Running time %d:%02d", t / 3600, (t % 3600) / 60);
		mudlog(line);

#ifdef REBOOT_WHEN
		static bool adjust = FALSE;
		if (!adjust && reboot_time >= A_DAY * 3) {
			reboot_time += A_DAY - (boottime + reboot_time - 1000)
			    % A_DAY -
			    1000 - TIME_ZONE + MINUTES(REBOOT_WHEN);
			adjust = TRUE;
		}
#endif
	}
}
/* ******************************************************************
*  general utility stuff (for local use)                   *
****************************************************************** */

int get_from_q(struct txt_q *queue, char *dest)
{
	struct txt_block *tmp;

	/* Q empty? */
	if (!queue->head)
		return (0);

	tmp = queue->head;
	strcpy(dest, queue->head->text);
	queue->head = queue->head->next;

	free(tmp->text);
	free(tmp);

	return (1);
}

void write_to_q(char *txt, struct txt_q *queue)
{
	struct txt_block *new;

	CREATE(new, struct txt_block, 1);
	CREATE(new->text, char, strlen(txt) + 1);

	strcpy(new->text, txt);

	/* Q empty? */
	if (!queue->head) {
		new->next = NULL;
		queue->head = queue->tail = new;
	} else {
		queue->tail->next = new;
		queue->tail = new;
		new->next = NULL;
	}
}

/* SEND_TO_Q 매크로 구현부 (색상 처리 및 전송) */
void send_to_q_color(const char *messg, struct descriptor_data *desc)
{
    char color_buf[MAX_STRING_LENGTH * 3];

    if (!desc || !messg)
        return;

    process_color_string(messg, color_buf, sizeof(color_buf));
    write_to_q(color_buf, &desc->output);
}

struct timeval timediff(struct timeval *a, struct timeval *b)
{
	struct timeval rslt, tmp;

	tmp = *a;

	if ((rslt.tv_usec = tmp.tv_usec - b->tv_usec) < 0) {
		rslt.tv_usec += 1000000;
		--(tmp.tv_sec);
	}
	if ((rslt.tv_sec = tmp.tv_sec - b->tv_sec) < 0) {
		rslt.tv_usec = 0;
		rslt.tv_sec = 0;
	}
	return (rslt);
}

/* Empty the queues before closing connection */
void flush_queues(struct descriptor_data *d)
{
	char dummy[MAX_STRING_LENGTH];

	while (get_from_q(&d->output, dummy)) ;
	while (get_from_q(&d->input, dummy)) ;
}

/* ******************************************************************
*  socket handling               *
****************************************************************** */

int init_socket(int port)
{
	int s;
	char *opt;
	char hostname[MAX_HOSTNAME + 1];
	struct sockaddr_in sa;
	struct hostent *hp;
	struct linger ld;

	bzero(&sa, sizeof(struct sockaddr_in));
	gethostname(hostname, MAX_HOSTNAME);
	mudlog(hostname);
	hp = gethostbyname(hostname);
	if (hp == NULL) {
		perror("gethostbyname");
		exit(1);
	}
	sa.sin_family = hp->h_addrtype;
	sa.sin_port = htons(port);
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0) {
		perror("Init-socket");
		exit(1);
	}
	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR,
		       (char *)&opt, sizeof(opt)) < 0) {
		perror("setsockopt REUSEADDR");
		exit(1);
	}
	ld.l_onoff = 0;
	ld.l_linger = 0;
	if (setsockopt(s, SOL_SOCKET, SO_LINGER, &ld, sizeof(ld)) < 0) {
		perror("setsockopt LINGER");
		exit(1);
	}
	if (bind(s, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
		perror("bind");
		close(s);
		exit(1);
	}
	listen(s, 3);
	return (s);
}

int new_connection(int s)
{
	struct sockaddr_in isa;
	unsigned int i;
	int t;

	i = sizeof(isa);
	getsockname(s, (struct sockaddr *)&isa, &i);
	if ((t = accept(s, (struct sockaddr *)&isa, &i)) < 0) {
		perror("Accept");
		return (-1);
	}
	nonblock(t);
	return (t);
}

int new_descriptor(int s)
{
	int desc;
	struct descriptor_data *newd;
	int i;
	unsigned int size;
	struct sockaddr_in sock;

	if ((desc = new_connection(s)) < 0)
		return (-1);

	CREATE(newd, struct descriptor_data, 1);
	size = sizeof(sock);
	if (getpeername(desc, (struct sockaddr *)&sock, &size) < 0) {
		perror("getpeername");
		*newd->host = 0;
	} else {
		strncpy(newd->host, (char *)inet_ntoa(sock.sin_addr), 16);
	}

	if ((maxdesc + 1) >= avail_descs) {
		write_to_descriptor(desc, "Sorry.. The game is full...\n\r");
		write_to_descriptor(desc,
				    "Wait at least one minute before retrying.\n\r");
		close(desc);
		flush_queues(newd);
		free((char *)newd);
		return (0);
	} else if (desc > maxdesc)
		maxdesc = desc;
	if (unfriendly_domain(newd->host)) {
		write_to_descriptor(desc,
				    "Sorry, the game is unavailable from your site.\n\r");
		fprintf(stderr, "Reject from %s\n", newd->host);
		close(desc);
		flush_queues(newd);
		free((char *)newd);
		return (-1);
	}

	/* init desc data */
	newd->ncmds = 0;
	newd->contime = time(0);
	newd->descriptor = desc;
	newd->connected = 1;
	newd->wait = 1;
	newd->prompt_mode = 0;
	*newd->buf = '\0';
	newd->str = 0;
	newd->showstr_head = 0;
	newd->showstr_point = 0;
	for (i = 0; i < MAX_LAST_INPUT_COUNT; i++)
		newd->last_input[i][0] = '\0';
	newd->last_input_count = 0;
	newd->output.head = NULL;
	newd->input.head = NULL;
	newd->next = descriptor_list;
	newd->character = 0;
	newd->original = 0;
	newd->snoop.snooping = 0;
	newd->snoop.snoop_by = 0;

	/* prepend to list */
	descriptor_list = newd;

	SEND_TO_Q(GREETINGS, newd);

	if (nonewplayers) {
		SEND_TO_Q("WARNING:\n\r", newd);
		SEND_TO_Q("No NEW characters are being accepted right now.\n\r\n\r", newd);
	}
	SEND_TO_Q("&CBy what name do you wish to be known? &n", newd);

	return (0);
}

int unfriendly_domain(char *h)
{
	int i;

	for (i = 0; i < baddoms; ++i) {
		if (strncmp(h, baddomain[i], strlen(baddomain[i])) == 0) {
			return (1);
		}
	}
	return (0);
}

int process_output(struct descriptor_data *t)
{
	char i[MAX_STRING_LENGTH + 1];

	if (!t->prompt_mode && !t->connected)
		if (write_to_descriptor(t->descriptor, "\n\r") < 0)
			return (-1);
	while (get_from_q(&t->output, i)) {
		if (t->snoop.snoop_by) {
			write_to_q("% ", &t->snoop.snoop_by->desc->output);
			write_to_q(i, &t->snoop.snoop_by->desc->output);
		}
		if (write_to_descriptor(t->descriptor, i))
			return (-1);
	}
	if (!t->connected && !(t->character && !IS_NPC(t->character) &&
			       IS_SET(t->character->specials.act, PLR_COMPACT)))
		if (write_to_descriptor(t->descriptor, "\n\r") < 0)
			return (-1);
	return (1);
}


int write_to_descriptor(int desc, char *txt)
{
	int sofar, thisround, total;

	total = strlen(txt);
	if (total == 0)
		return 0;
	sofar = 0;
	do {
		thisround = write(desc, txt + sofar, total - sofar);
		if (thisround < 0) {
			perror("Write to socket");
			return (-1);
		} else {
			sofar += thisround;
		}
	} while (sofar < total);
	return (0);
}

int process_input(struct descriptor_data *t)
{
	int sofar, thisround, begin, i, k, flag;
	char tmp[MAX_INPUT_LENGTH + 2], buffer[MAX_INPUT_LENGTH + 60];
	int lic, j;

	sofar = 0;
	flag = 0;
	begin = strlen(t->buf);
	/* Read in some stuff */
	do {
		if ((thisround = read(t->descriptor, t->buf + begin + sofar,
				      MAX_STRING_LENGTH - (begin + sofar) -
		    1)) > 0)
			sofar += thisround;
		else if (thisround < 0)
			if (errno != EWOULDBLOCK) {
				perror("Read1 - ERROR");
				return (-1);
			} else
				break;
		else {
			mudlog("EOF encountered on socket read.");
			return (-1);
		}
	} while (!ISNEWL(*(t->buf + begin + sofar - 1)));

	*(t->buf + begin + sofar) = 0;

	/* if no newline is contained in input, return without proc'ing */
	for (i = begin; !ISNEWL(*(t->buf + i)); i++)
		if (!*(t->buf + i))
			return (0);

	/* input contains 1 or more newlines; process the stuff */
	for (i = 0, k = 0; *(t->buf + i);) {
		if (!ISNEWL(*(t->buf + i)) &&
		    !(flag = (k >= (MAX_INPUT_LENGTH - 2)))) {
			if (*(t->buf + i) == '\b') {	/* backspace */
				if (k) {	/* more than one char ? */
					if (*(tmp + --k) == '$')
						k--;
				}
				i++;	/* no or just one char.. Skip backsp */
			}
			/* strange stuff, i can't understand! */
			else if (*(t->buf + i) == -1) {
				if (*(t->buf + i) && !ISNEWL(*(t->buf + i)))
					i++;
				if (*(t->buf + i) && !ISNEWL(*(t->buf + i)))
					i++;
				if (*(t->buf + i) && !ISNEWL(*(t->buf + i)))
					i++;
			}
			/* hangul routine - deleted, 251130 by Komo */
            /* strange stuff, i can't understand! */
            else if ((isascii(*(t->buf + i)) && isprint(*(t->buf + i))) || !isascii(*(t->buf + i))) {
                if ((*(tmp + k) = *(t->buf + i)) == '$')
                    *(tmp + ++k) = '$';
                k++;
                i++;
            } else
                i++;
		} else {
			*(tmp + k) = 0;
			if (tmp[0] == '!') {
				if (tmp[1] == '!' || tmp[1] == 0) {
					lic = t->last_input_count;
					if (lic == 0)
						lic = MAX_LAST_INPUT_COUNT - 1;
					else
						lic--;
					strcpy(tmp, t->last_input[lic]);
				} else {
					lic = t->last_input_count - 1;
					for (j = 0; j < MAX_LAST_INPUT_COUNT;
					     j++, lic--) {
						if (lic < 0)
							lic =
							    MAX_LAST_INPUT_COUNT
							    - 1;
						if
						    (strncmp(t->last_input[lic],
						     &tmp[1],
						     strlen(&tmp[1])) == 0) {
							strcpy(tmp,
							       t->last_input[lic]);
							break;
						}
					}
				}
			} else {
				strcpy(t->last_input[t->last_input_count], tmp);
				t->last_input_count++;
				t->last_input_count %= 10;
			}
			write_to_q(tmp, &t->input);
			if (t->snoop.snoop_by) {
				write_to_q("% ", &t->snoop.snoop_by->desc->output);
				write_to_q(tmp, &t->snoop.snoop_by->desc->output);
				write_to_q("\n\r", &t->snoop.snoop_by->desc->output);
			}
			if (flag) {
				snprintf(buffer, sizeof(buffer),
					"Line too long. Truncated to:\n\r%s\n\r", tmp);
				if (write_to_descriptor(t->descriptor, buffer)
				    < 0)
					return (-1);
				/* skip the rest of the line */
				for (; !ISNEWL(*(t->buf + i)); i++) ;
			}
			/* find end of entry */
			for (; ISNEWL(*(t->buf + i)); i++) ;
			/* 메모리 overlap 방지, 251202 */
            size_t remain_len = strlen(t->buf + i);
            memmove(t->buf, t->buf + i, remain_len + 1);
			k = 0;
			i = 0;
		}
	}
	return (1);
}

void close_sockets(int s)
{
	mudlog("Closing all sockets.");
	while (descriptor_list)
		close_socket(descriptor_list);
	close(s);
}

void close_socket(struct descriptor_data *d)
{
	struct descriptor_data *tmp;
	char buf[100];

	if (!d)
		return;
	close(d->descriptor);
	flush_queues(d);
	if (d->descriptor == maxdesc)
		--maxdesc;
	/* Forget snooping */
	if (d->snoop.snooping)
		d->snoop.snooping->desc->snoop.snoop_by = 0;
	if (d->snoop.snoop_by) {
		send_to_char("Your victim is no longer among us.\n\r", d->snoop.snoop_by);
		d->snoop.snoop_by->desc->snoop.snooping = 0;
	}
	if (d->character) {
		if (d->connected == CON_PLYNG) {
			save_char(d->character, world[d->character->in_room].number);
			stash_char(d->character);
			move_stashfile_safe(d->character->player.name);
			act("$n has lost $s link.", TRUE, d->character, 0, 0, TO_ROOM);
			snprintf(buf, sizeof(buf), "Closing link to: %s.", GET_NAME(d->character));
			mudlog(buf);
			d->character->desc = 0;
		} else {
			snprintf(buf, sizeof(buf), "Losing player: %s.", GET_NAME(d->character));
/*
#ifdef  RETURN_TO_QUIT  
	save_char(d->character,world[d->character->in_room].number);
#else
      save_char(d->character, d->character->in_room);
#endif
*/
/*
#ifdef SYPARK
	  stash_char(d->character);
#endif
*/
			mudlog(buf);
			free_char(d->character);
		}
	} else
		mudlog("Losing descriptor without char.");

	if (next_to_process == d)	/* to avoid crashing the process loop */
		next_to_process = next_to_process->next;

	if (d == descriptor_list)	/* this is the head of the list */
		descriptor_list = descriptor_list->next;
	else
		/* This is somewhere inside the list */
	{
		/* Locate the previous element */
		for (tmp = descriptor_list; (tmp->next != d) && tmp; tmp = tmp->next) ;

		tmp->next = d->next;
	}
	if (d->showstr_head) {
		free(d->showstr_head);
	}
	free(d);
}

void nonblock(int s)
{
	if (fcntl(s, F_SETFL, O_NONBLOCK) == -1) {
		perror("Noblock");
		exit(1);
	}
}

/* ****************************************************************
*  Public routines for system-to-player-communication        *
**************************************************************** */

/* 메시지 & 색상 처리 통합                --- 251125 by Komo */
static void perform_send_to_char(const char *txt, struct char_data *ch)
{
    if (!ch || !ch->desc || !txt || !*txt) return;

    char color_buf[MAX_STRING_LENGTH * 3];

    process_color_string(txt, color_buf, sizeof(color_buf));
    write_to_q(color_buf, &ch->desc->output);
}

/* 일반 전송 */
void send_to_char(char *messg, struct char_data *ch)
{
    perform_send_to_char(messg, ch);
}

/* 한글 - 한글 메시지가 NULL이어도 영어 메시지가 있으면 전송 */
void send_to_char_han(char *msgeng, char *msghan, struct char_data *ch)
{
    if (!ch || !ch->desc) return;

    if (IS_SET(ch->specials.act, PLR_KOREAN) && msghan && *msghan) {
        perform_send_to_char(msghan, ch);
    } else {
        perform_send_to_char(msgeng, ch);
    }
}

void send_to_all(char *messg)
{
    struct descriptor_data *i;
    char color_buf[MAX_STRING_LENGTH * 3];

    if (messg)
        for (i = descriptor_list; i; i = i->next)
            if (!i->connected) {
                process_color_string(messg, color_buf, sizeof(color_buf));
                write_to_q(color_buf, &i->output);
            }
}

void send_to_room(char *messg, int room)
{
    struct char_data *i;
    char color_buf[MAX_STRING_LENGTH * 3];

    if (messg)
        for (i = world[room].people; i; i = i->next_in_room)
            if (i->desc) {
                process_color_string(messg, color_buf, sizeof(color_buf));
                write_to_q(color_buf, &i->desc->output);
            }
}

/* 
 * act, acthan에만 쓰이던 매크로 함수 삭제 & 헬퍼함수 도입 (utils.h) 
 *                                                  --- 251125 by Komo
 * ANA, SANA,       -> get_indefinite_article
 * HSHR, HSSH, HMHR -> get_pronoun
 * PERS             -> get_char_name (in utility.c)
 * OBJN, OBJS       -> get_obj_name
* */

/* 관사 a/an 처리 도우미 함수 : 모음(a,e,i,o,u)으로 시작하면 an을 반환 */
static const char *get_indefinite_article(const char *name, bool capitalize)
{
    // 이름의 첫 글자가 모음인지 확인 (strchr 사용)
    if (strchr("aeiouAEIOU", *name)) {
        return capitalize ? "An" : "an";
    }
    return capitalize ? "A" : "a";
}

/* 성별에 따른 대명사 반환 (he/she/it, him/her/it, his/her/its) */
static const char *get_pronoun(struct char_data *ch, char type)
{
    int sex = GET_SEX(ch);

    switch (type) {
        case 'm': // HMHR (him/her/it)
            if (sex == SEX_MALE) return "him";
            if (sex == SEX_FEMALE) return "her";
            return "it";
        
        case 's': // HSHR (his/her/its)
            if (sex == SEX_MALE) return "his";
            if (sex == SEX_FEMALE) return "her";
            return "its";
            
        case 'e': // HSSH (he/she/it)
            if (sex == SEX_MALE) return "he";
            if (sex == SEX_FEMALE) return "she";
            return "it";
    }
    return "it";
}

/* 아이템 이름 반환 (type: 'o'=name, 'p'=short_desc) */
static const char *get_obj_name(struct obj_data *obj, struct char_data *viewer, char type)
{
    if (!CAN_SEE_OBJ(viewer, obj)) {
        return "something";
    }
    
    if (type == 'o') // OBJN
        return fname(obj->name);
        
    return obj->short_description; // OBJS ('p')
}
/* higher-level communication */
/*
 * [통합 엔진 함수] 251125 by Komo
 * act와 acthan의 기능 처리
 * str_han이 NULL이면 무조건 str_eng를 사용
 */
static void perform_act(const char *str_eng, const char *str_han, int hide_invisible, struct char_data *ch, struct obj_data *obj, void *vict_obj, int type)
{
    const char *strp;
    const char *template;   // 현재 대상에게 보여줄 원본 문자열 (영어 or 한글)
    const char *replacement;// 치환될 문자열 ($n -> 이름)
    
    char *point;            // 버퍼에 쓸 포인터
    struct char_data *to;
    char buf[MAX_STRING_LENGTH];
    char color_buf[MAX_STRING_LENGTH * 3];

    // 영어 문장조차 없으면 리턴
    if (!str_eng || !*str_eng) return;

    // 대상 설정
    if (type == TO_VICT)       to = (struct char_data *)vict_obj;
    else if (type == TO_CHAR)  to = ch;
    else                       to = world[ch->in_room].people;

    // 루프 시작
    for (; to; to = to->next_in_room) {
        // 전송 조건 체크
        if (!to->desc || 
            ((to == ch) && (type != TO_CHAR)) ||
            (!CAN_SEE(to, ch) && hide_invisible) ||
            !AWAKE(to) ||
            ((type == TO_NOTVICT) && (to == (struct char_data *)vict_obj)))
        {
            continue;
        }

        // 언어 선택
        if (str_han && *str_han && IS_SET(to->specials.act, PLR_KOREAN)) {
            template = str_han;
        } else {
            template = str_eng;
        }

        // 문자열 파싱 - 기존 act 로직
        for (strp = template, point = buf; *strp; ++strp) {
            if (*strp == '$') {
                ++strp;
                replacement = NULL;

                switch (*strp) {
                    case 'n': replacement = get_char_name(ch, to); break;
                    case 'N': replacement = get_char_name((struct char_data *)vict_obj, to); break;
                    case 'm': replacement = get_pronoun(ch, 'm'); break;
                    case 'M': replacement = get_pronoun((struct char_data *)vict_obj, 'm'); break;
                    case 's': replacement = get_pronoun(ch, 's'); break;
                    case 'S': replacement = get_pronoun((struct char_data *)vict_obj, 's'); break;
                    case 'e': replacement = get_pronoun(ch, 'e'); break;
                    case 'E': replacement = get_pronoun((struct char_data *)vict_obj, 'e'); break;
                    case 'o': replacement = get_obj_name(obj, to, 'o'); break;
                    case 'O': replacement = get_obj_name((struct obj_data *)vict_obj, to, 'o'); break;
                    case 'p': replacement = get_obj_name(obj, to, 'p'); break;
                    case 'P': replacement = get_obj_name((struct obj_data *)vict_obj, to, 'p'); break;
                    case 'a': replacement = get_indefinite_article(obj->name, FALSE); break;
                    case 'A': replacement = get_indefinite_article(((struct obj_data *)vict_obj)->name, TRUE); break;
                    case 'T': replacement = (char *)vict_obj; break;
                    case 'F': replacement = fname((char *)vict_obj); break;
                    case '$': replacement = "$"; break;
                    default:
                        mudlog("SYSERR: Illegal $-code to act():");
                        mudlog(template);
                        break;
                }

                while (replacement && *replacement && (point - buf) < (MAX_STRING_LENGTH - 3)) {
                    *point++ = *replacement++;
                }
            } else {
                if ((point - buf) < (MAX_STRING_LENGTH - 3)) {
                    *point++ = *strp;
                }
            }
        }

        // 마무리 및 전송
        *point++ = '\n';
        *point++ = '\r';
        *point   = '\0';

        CAP(buf);

        process_color_string(buf, color_buf, sizeof(color_buf));
        write_to_q(color_buf, &to->desc->output);
        
        if ((type == TO_VICT) || (type == TO_CHAR))
            return;
    }
}

/* * [Wrapper 1] act : 한글 문장 자리에 NULL을 넘겨서 영어만 출력 */
void act(char *str, int hide_invisible, struct char_data *ch, struct obj_data *obj, void *vict_obj, int type)
{
    perform_act(str, NULL, hide_invisible, ch, obj, vict_obj, type);
}

/* * [Wrapper 2] acthan : 영어와 한글 문장을 모두 넘겨서 상황에 맞게 출력 */
void acthan(char *streng, char *strhan, int hide_invisible, struct char_data *ch, struct obj_data *obj, void *vict_obj, int type)
{
    perform_act(streng, strhan, hide_invisible, ch, obj, vict_obj, type);
}

/* 기존 freaky 함수 대체 */
void log_abnormal_disconnect(struct descriptor_data *d)
{
    char buf[256];
    char *name = "Unknown";

    if (d->original && d->original->player.name) {
        name = d->original->player.name;
    } else if (d->character && d->character->player.name) {
        name = d->character->player.name;
    }

    snprintf(buf, sizeof(buf), 
        "ABNORMAL DISCONNECT: Socket %d, State %d, Player '%s' kicked due to network exception.", 
        d->descriptor, 
        d->connected, 
        name
    );
    
    mudlog(buf);
}

void zapper(void)
{
    int t;
    static int zapper_triggered = 0; // 이 함수가 리붓을 실행했는지 확인

    t = time(0) - boottime; // 머드가 실행된 총 시간

    if (t > reboot_time && !zapper_triggered && reboot_counter < 0) {
        mudlog("(zapper) Automatic reboot time reached. Triggering graceful shutdown.");
        reboot_game = 1;
        zapper_triggered = 1;
    }
}

/* 서버 멈춤 감지 */
void checkpointing(int sig)
{
	extern int tics;
    static int last_tics = 0;

    if (tics == last_tics) { // 지난 검사와 tics가 같으면 멈춘 것
        mudlog("!!! CHECKPOINT shutdown: tics not updated. Server appears to be frozen.");
        mudlog("!!! Emergency saving all players before abort().");
        saveallplayers();
        abort();
    }
    else {
        last_tics = tics; // remember current tics
    }
    mudlog("(checkpointing) Checkpoint signal received. Tics saved.");
}
