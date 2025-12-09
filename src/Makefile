#
# Makefile for Narai MUD
# $Id$
# For GNU make only. For BSD make, use Makefile-bsdmake.
# PROG, CFLAGS and LDFLAGS adjusted by komoyon@gmail.com, 2025-12-02

PROG    = personal-narai
INCDIR  = ../include
UTLSRC  = ../utils
OBJDIR  = ./obj
INSTDIR = ../bin
VPATH   = .:$(INCDIR):$(UTLSRC):

CC      = cc

MAKE    = make
LD      = $(CC)

OFLAGS  = -O -g -Wall -pipe

INCLUDE = -I$(INCDIR)

# Set initial gold and quest in "db.c"
DFLAGS=

# 메모리 버그 추적을 위해 AddressSanitizer, -fsanitize=address -g 추가, 251202
CFLAGS  = $(OFLAGS) $(DFLAGS) $(INCLUDE) -g -rdynamic -fsanitize=address -fsanitize-recover=address -g
LIBS = -fsanitize=address -fsanitize-recover=address

LDFLAGS  = -lcrypt -g -rdynamic

# 메모리 버그 추적을 위해 ASan, -fsanitize=address 추가, 251202
LIBS = -fsanitize=address

SRCS    = \
	act.comm.c act.informative.c act.movement.c act.obj1.c act.obj2.c \
	act.offensive.c act.other.c act.social.c act.wizard.c board.c comm.c \
	constants.c db.c fight.c guild_command.c guild_constant.c \
	guild_informative.c guild_skills.c handler.c interpreter.c limit.c \
	magic.c magic2.c magic3.c magic_weapon.c mail.c mob_magic.c mobact.c \
	modify.c newcom.c quest.c shop.c spec_assign.c spec_procs.c \
	spec_procs2.c spec_procs3.c spell_parser.c spells1.c spells2.c \
	spells3.c utility.c version.c weather.c daerimsa.c gbisland.c

UTILS       = change_ac change_class change_dice change_exp change_level change_max change_pass \
			  check_god check_pass check_player chstashform \
              fix_noname init_remortal list_player mob_balance mob_create mob_edit \
              reset_quest unchstashform zone_item zone_list zone_print 

# SCRIPTS     = everun backup RESTART RUNME shutmud
SCRIPTS     = runmud.sh killmud.sh

OBJS        = $(SRCS:%.c=$(OBJDIR)/%.o)
PROG_BIN    = $(OBJDIR)/$(PROG)
U_BINS      = $(UTILS:%=$(OBJDIR)/%)
CTAGS_SRC   = $(SRCS) $(INCDIR)/*.h

$(OBJDIR)/%.o :  %.c
	$(CC) -c $(CFLAGS) -o $@ $<

# ASan
$(OBJDIR)/% :  %.o
	$(LD) -o $@ $< $(LDFLAGS) $(LIBS)

default :    objdir $(PROG_BIN) 

all:    $(PROG_BIN) utils

$(PROG_BIN) : $(OBJS)
	$(LD) -o $@ $^ $(LDFLAGS) $(LIBS)

objdir:
	if [ ! -d $(OBJDIR) ] ; then mkdir $(OBJDIR); fi

utils : $(U_BINS)

clean:
	- /bin/rm -f $(OBJS) $(UTILS:%=$(OBJDIR)/%.o) tags core 
	- /bin/rm -f $(PROG_BIN) $(U_BINS)

install : $(PROG_BIN) $(U_BINS)
	- if [ -x $(INSTDIR)/$(PROG).run ] ; then \
	      /bin/mv -f $(INSTDIR)/$(PROG).run $(INSTDIR)/$(PROG).old ; fi
	/bin/cp -p $(PROG_BIN) $(INSTDIR)/$(PROG).new
	/bin/cp -p $(U_BINS) $(INSTDIR)
	/bin/cp -p $(SCRIPTS:%=$(UTLSRC)/%) $(INSTDIR)
	/bin/chmod 700 $(SCRIPTS:%=$(INSTDIR)/%)

uninstall :
	- /bin/rm -f $(INSTDIR)/$(PROG).new $(INSTDIR)/$(PROG).run $(INSTDIR)/$(PROG).old
	- /bin/rm -f $(UTILS:%=$(INSTDIR)/%) $(SCRIPTS:%=$(INSTDIR)/%)

ctags :
	- ctags $(CTAGS_SRC)


#======================================================================

# DO NOT DELETE

# act.comm.c act.informative.c act.movement.c act.obj1.c act.obj2.c act.offensive.c act.other.c act.social.c act.wizard.c board.c comm.c constants.c daerimsa.c db.c fight.c gbisland.c guild_command.c guild_constant.c guild_informative.c guild_skills.c handler.c interpreter.c limit.c mabact.c magic_weapon.c magic.c magic2.c magic3.c mail.c mob_bal.c mob_magic.c mobact.c modify.c newcom.c newshop.c quest.c shop.c spec_assign.c spec_procs.c spec_procs2.c spec_procs3.c spell_parser.c spells1.c spells2.c spells3.c utility.c version.c weather.c $(UTLSRC)/change_level.c $(UTLSRC)/change_pass.c $(UTLSRC)/change_stashform.c $(UTLSRC)/check_pass.c $(UTLSRC)/check_player.c $(UTLSRC)/list_player.c $(UTLSRC)/mob_balace.c $(UTLSRC)/mob_create.c $(UTLSRC)/mob_edit.c $(UTLSRC)/zone_item.c $(UTLSRC)/zone_list.c $(UTLSRC)/zone_print.c -I../include

$(OBJDIR)/act.comm.o: act.comm.c \
    $(INCDIR)/structs.h \
    $(INCDIR)/utils.h \
    $(INCDIR)/comm.h $(INCDIR)/interpreter.h $(INCDIR)/handler.h \
    $(INCDIR)/db.h $(INCDIR)/spells.h
$(OBJDIR)/act.informative.o: act.informative.c \
    $(INCDIR)/structs.h \
    $(INCDIR)/utils.h $(INCDIR)/comm.h \
    $(INCDIR)/interpreter.h $(INCDIR)/handler.h $(INCDIR)/db.h \
    $(INCDIR)/spells.h $(INCDIR)/limit.h $(INCDIR)/guild_list.h
$(OBJDIR)/act.movement.o: act.movement.c \
    $(INCDIR)/structs.h \
    $(INCDIR)/utils.h $(INCDIR)/comm.h \
    $(INCDIR)/interpreter.h $(INCDIR)/handler.h $(INCDIR)/db.h \
    $(INCDIR)/spells.h $(INCDIR)/guild_list.h
$(OBJDIR)/act.obj1.o: act.obj1.c \
    $(INCDIR)/structs.h \
    $(INCDIR)/utils.h \
    $(INCDIR)/comm.h $(INCDIR)/interpreter.h $(INCDIR)/handler.h \
    $(INCDIR)/db.h $(INCDIR)/spells.h
$(OBJDIR)/act.obj2.o: act.obj2.c \
    $(INCDIR)/structs.h \
    $(INCDIR)/utils.h \
    $(INCDIR)/comm.h $(INCDIR)/interpreter.h $(INCDIR)/handler.h \
    $(INCDIR)/db.h $(INCDIR)/spells.h
$(OBJDIR)/act.offensive.o: act.offensive.c \
    $(INCDIR)/structs.h \
    $(INCDIR)/utils.h $(INCDIR)/comm.h \
    $(INCDIR)/interpreter.h $(INCDIR)/handler.h $(INCDIR)/db.h \
    $(INCDIR)/spells.h $(INCDIR)/limit.h \
    $(INCDIR)/guild_list.h
$(OBJDIR)/act.other.o: act.other.c \
    $(INCDIR)/structs.h \
    $(INCDIR)/utils.h $(INCDIR)/comm.h \
    $(INCDIR)/interpreter.h $(INCDIR)/handler.h $(INCDIR)/db.h \
    $(INCDIR)/spells.h $(INCDIR)/guild_list.h
$(OBJDIR)/act.social.o: act.social.c \
    $(INCDIR)/structs.h \
    $(INCDIR)/utils.h \
    $(INCDIR)/comm.h $(INCDIR)/interpreter.h $(INCDIR)/handler.h \
    $(INCDIR)/db.h $(INCDIR)/spells.h
$(OBJDIR)/act.wizard.o: act.wizard.c \
    $(INCDIR)/structs.h $(INCDIR)/utils.h \
    $(INCDIR)/comm.h $(INCDIR)/interpreter.h $(INCDIR)/handler.h \
    $(INCDIR)/db.h $(INCDIR)/spells.h \
    $(INCDIR)/limit.h
$(OBJDIR)/board.o: board.c \
    $(INCDIR)/structs.h \
    $(INCDIR)/utils.h \
    $(INCDIR)/comm.h $(INCDIR)/db.h
$(OBJDIR)/comm.o: comm.c \
    $(INCDIR)/structs.h \
    $(INCDIR)/utils.h $(INCDIR)/comm.h $(INCDIR)/interpreter.h \
    $(INCDIR)/handler.h $(INCDIR)/db.h
$(OBJDIR)/constants.o: constants.c $(INCDIR)/structs.h \
    $(INCDIR)/limit.h
$(OBJDIR)/daerimsa.o: daerimsa.c \
    $(INCDIR)/structs.h \
    $(INCDIR)/utils.h $(INCDIR)/comm.h \
    $(INCDIR)/interpreter.h $(INCDIR)/handler.h $(INCDIR)/db.h \
    $(INCDIR)/spells.h $(INCDIR)/limit.h
$(OBJDIR)/db.o: db.c \
    $(INCDIR)/structs.h $(INCDIR)/utils.h \
    $(INCDIR)/db.h $(INCDIR)/comm.h \
    $(INCDIR)/handler.h $(INCDIR)/limit.h $(INCDIR)/spells.h mob_bal.c
$(OBJDIR)/fight.o: fight.c \
    $(INCDIR)/structs.h \
    $(INCDIR)/utils.h \
    $(INCDIR)/comm.h $(INCDIR)/handler.h $(INCDIR)/interpreter.h \
    $(INCDIR)/db.h $(INCDIR)/spells.h \
    $(INCDIR)/guild_list.h
$(OBJDIR)/gbisland.o: gbisland.c \
    $(INCDIR)/structs.h \
    $(INCDIR)/utils.h $(INCDIR)/comm.h \
    $(INCDIR)/interpreter.h $(INCDIR)/handler.h $(INCDIR)/db.h \
    $(INCDIR)/spells.h $(INCDIR)/limit.h
$(OBJDIR)/guild_command.o: guild_command.c \
    $(INCDIR)/structs.h \
    $(INCDIR)/utils.h $(INCDIR)/comm.h \
    $(INCDIR)/interpreter.h $(INCDIR)/handler.h $(INCDIR)/db.h \
    $(INCDIR)/spells.h $(INCDIR)/limit.h \
    $(INCDIR)/guild_list.h
$(OBJDIR)/guild_constant.o: guild_constant.c $(INCDIR)/structs.h \
    $(INCDIR)/limit.h \
    $(INCDIR)/guild_list.h
$(OBJDIR)/guild_informative.o: guild_informative.c \
    $(INCDIR)/structs.h \
    $(INCDIR)/utils.h \
    $(INCDIR)/comm.h $(INCDIR)/interpreter.h $(INCDIR)/handler.h \
    $(INCDIR)/db.h $(INCDIR)/spells.h \
    $(INCDIR)/limit.h $(INCDIR)/guild_list.h
$(OBJDIR)/guild_skills.o: guild_skills.c \
    $(INCDIR)/structs.h \
    $(INCDIR)/utils.h $(INCDIR)/comm.h \
    $(INCDIR)/spells.h $(INCDIR)/handler.h $(INCDIR)/limit.h \
    $(INCDIR)/db.h $(INCDIR)/guild_list.h
$(OBJDIR)/handler.o: handler.c \
    $(INCDIR)/structs.h \
    $(INCDIR)/utils.h \
    $(INCDIR)/comm.h $(INCDIR)/db.h \
    $(INCDIR)/handler.h $(INCDIR)/spells.h $(INCDIR)/guild_list.h
$(OBJDIR)/interpreter.o: interpreter.c \
    $(INCDIR)/structs.h \
    $(INCDIR)/comm.h $(INCDIR)/interpreter.h $(INCDIR)/db.h \
    $(INCDIR)/utils.h $(INCDIR)/limit.h
$(OBJDIR)/limit.o: limit.c \
    $(INCDIR)/structs.h \
    $(INCDIR)/limit.h \
    $(INCDIR)/utils.h $(INCDIR)/spells.h $(INCDIR)/comm.h \
    $(INCDIR)/guild_list.h
$(OBJDIR)/mabact.o: mabact.c \
    $(INCDIR)/utils.h $(INCDIR)/structs.h \
    $(INCDIR)/db.h \
    $(INCDIR)/comm.h $(INCDIR)/spells.h
$(OBJDIR)/magic_weapon.o: magic_weapon.c $(INCDIR)/structs.h \
    $(INCDIR)/comm.h \
    $(INCDIR)/magic_weapon.h $(INCDIR)/utils.h $(INCDIR)/spells.h
$(OBJDIR)/magic.o: magic.c \
    $(INCDIR)/structs.h \
    $(INCDIR)/utils.h \
    $(INCDIR)/comm.h $(INCDIR)/spells.h $(INCDIR)/handler.h \
    $(INCDIR)/limit.h $(INCDIR)/db.h \
    $(INCDIR)/guild_list.h $(INCDIR)/magic_weapon.h
$(OBJDIR)/magic2.o: magic2.c \
    $(INCDIR)/structs.h \
    $(INCDIR)/utils.h \
    $(INCDIR)/comm.h $(INCDIR)/spells.h $(INCDIR)/handler.h \
    $(INCDIR)/limit.h $(INCDIR)/db.h \
    $(INCDIR)/magic_weapon.h $(INCDIR)/guild_list.h
$(OBJDIR)/magic3.o: magic3.c \
    $(INCDIR)/structs.h \
    $(INCDIR)/utils.h \
    $(INCDIR)/comm.h $(INCDIR)/spells.h $(INCDIR)/handler.h \
    $(INCDIR)/limit.h $(INCDIR)/db.h \
    $(INCDIR)/guild_list.h
$(OBJDIR)/mail.o: mail.c \
    $(INCDIR)/structs.h \
    $(INCDIR)/utils.h \
    $(INCDIR)/comm.h $(INCDIR)/db.h \
    $(INCDIR)/handler.h $(INCDIR)/interpreter.h
$(OBJDIR)/mob_bal.o: mob_bal.c
$(OBJDIR)/mob_magic.o: mob_magic.c \
    $(INCDIR)/structs.h \
    $(INCDIR)/utils.h \
    $(INCDIR)/comm.h $(INCDIR)/spells.h $(INCDIR)/handler.h \
    $(INCDIR)/db.h \
    $(INCDIR)/mob_magic.h
$(OBJDIR)/mobact.o: mobact.c \
    $(INCDIR)/structs.h \
    $(INCDIR)/db.h \
    $(INCDIR)/comm.h $(INCDIR)/spells.h \
    $(INCDIR)/utils.h
$(OBJDIR)/modify.o: modify.c \
    $(INCDIR)/structs.h \
    $(INCDIR)/utils.h $(INCDIR)/interpreter.h \
    $(INCDIR)/handler.h $(INCDIR)/db.h \
    $(INCDIR)/comm.h
$(OBJDIR)/newcom.o: newcom.c \
    $(INCDIR)/structs.h \
    $(INCDIR)/utils.h \
    $(INCDIR)/comm.h $(INCDIR)/interpreter.h $(INCDIR)/handler.h \
    $(INCDIR)/db.h $(INCDIR)/spells.h
$(OBJDIR)/newshop.o: newshop.c $(INCDIR)/structs.h \
    $(INCDIR)/newshop.h
$(OBJDIR)/quest.o: quest.c \
    $(INCDIR)/structs.h \
    $(INCDIR)/utils.h $(INCDIR)/db.h
$(OBJDIR)/shop.o: shop.c \
    $(INCDIR)/structs.h \
    $(INCDIR)/comm.h \
    $(INCDIR)/handler.h $(INCDIR)/db.h \
    $(INCDIR)/interpreter.h $(INCDIR)/utils.h
$(OBJDIR)/spec_assign.o: spec_assign.c \
    $(INCDIR)/structs.h \
    $(INCDIR)/db.h \
    $(INCDIR)/guild_list.h
$(OBJDIR)/spec_procs.o: spec_procs.c \
    $(INCDIR)/structs.h \
    $(INCDIR)/utils.h $(INCDIR)/comm.h \
    $(INCDIR)/interpreter.h $(INCDIR)/handler.h $(INCDIR)/db.h \
    $(INCDIR)/spells.h $(INCDIR)/limit.h
$(OBJDIR)/spec_procs2.o: spec_procs2.c \
    $(INCDIR)/structs.h \
    $(INCDIR)/utils.h $(INCDIR)/comm.h \
    $(INCDIR)/interpreter.h $(INCDIR)/handler.h $(INCDIR)/db.h \
    $(INCDIR)/spells.h $(INCDIR)/limit.h \
    $(INCDIR)/mob_magic.h
$(OBJDIR)/spec_procs3.o: spec_procs3.c \
    $(INCDIR)/structs.h \
    $(INCDIR)/utils.h $(INCDIR)/comm.h \
    $(INCDIR)/interpreter.h $(INCDIR)/handler.h $(INCDIR)/db.h \
    $(INCDIR)/spells.h $(INCDIR)/limit.h \
    $(INCDIR)/mob_magic.h
$(OBJDIR)/spell_parser.o: spell_parser.c \
    $(INCDIR)/structs.h \
    $(INCDIR)/utils.h \
    $(INCDIR)/comm.h $(INCDIR)/db.h \
    $(INCDIR)/interpreter.h $(INCDIR)/spells.h $(INCDIR)/handler.h
$(OBJDIR)/spells1.o: spells1.c \
    $(INCDIR)/structs.h \
    $(INCDIR)/utils.h \
    $(INCDIR)/comm.h $(INCDIR)/db.h \
    $(INCDIR)/interpreter.h $(INCDIR)/spells.h $(INCDIR)/handler.h \
    $(INCDIR)/guild_list.h
$(OBJDIR)/spells2.o: spells2.c \
    $(INCDIR)/structs.h \
    $(INCDIR)/utils.h \
    $(INCDIR)/comm.h $(INCDIR)/db.h \
    $(INCDIR)/interpreter.h $(INCDIR)/spells.h $(INCDIR)/handler.h \
    $(INCDIR)/guild_list.h
$(OBJDIR)/spells3.o: spells3.c \
    $(INCDIR)/structs.h \
    $(INCDIR)/utils.h \
    $(INCDIR)/comm.h $(INCDIR)/db.h \
    $(INCDIR)/interpreter.h $(INCDIR)/spells.h $(INCDIR)/handler.h
$(OBJDIR)/utility.o: utility.c \
    $(INCDIR)/structs.h \
    $(INCDIR)/utils.h
$(OBJDIR)/version.o: version.c
$(OBJDIR)/weather.o: weather.c \
    $(INCDIR)/structs.h \
    $(INCDIR)/utils.h \
    $(INCDIR)/comm.h $(INCDIR)/handler.h $(INCDIR)/interpreter.h \
    $(INCDIR)/db.h
$(OBJDIR)/change_level.o: $(UTLSRC)/change_level.c \
    $(INCDIR)/structs.h
$(OBJDIR)/change_pass.o: $(UTLSRC)/change_pass.c \
    $(INCDIR)/structs.h
$(OBJDIR)/chstashform.o: $(UTLSRC)/chstashform.c
$(OBJDIR)/unchstashform.o: $(UTLSRC)/unchstashform.c
$(OBJDIR)/check_pass.o: $(UTLSRC)/check_pass.c \
    $(INCDIR)/structs.h
$(OBJDIR)/check_player.o: $(UTLSRC)/check_player.c \
    $(INCDIR)/structs.h
$(OBJDIR)/list_player.o: $(UTLSRC)/list_player.c \
    $(INCDIR)/structs.h
$(OBJDIR)/mob_balance.o: $(UTLSRC)/mob_balance.c
$(OBJDIR)/mob_create.o: $(UTLSRC)/mob_create.c \
    $(UTLSRC)/mob_create.h
$(OBJDIR)/mob_edit.o: $(UTLSRC)/mob_edit.c \
    $(UTLSRC)/mob_create.h
$(OBJDIR)/zone_item.o: $(UTLSRC)/zone_item.c
$(OBJDIR)/zone_list.o: $(UTLSRC)/zone_list.c
$(OBJDIR)/zone_print.o: $(UTLSRC)/zone_print.c

$(OBJDIR)/check_god.o: $(UTLSRC)/check_god.c $(INCDIR)/structs.h
$(OBJDIR)/fix_noname.o: $(UTLSRC)/fix_noname.c $(INCDIR)/structs.h
