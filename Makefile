VERSION  = 14.2
CFLAGS   = -g -O2 -fPIC -pipe -ansi -Wall -Wextra -Werror -pedantic -Wno-parentheses
CFLAGS  += -D_XOPEN_SOURCE=700 
LDFLAGS  = -L.
CC       = gcc
INSTALL  = install

PREFIX  ?= /usr
MANDIR  ?= $(PREFIX)/man
BINDIR  ?= $(PREFIX)/bin

.SUFFIXES:
.SUFFIXES: "" .c
.SUFFIXES: .o .c
.SUFFIXES: "" .sh

C_SRC = \
	cugfd.c			\
	darkstar-dups.c		\
	darkstar-installed.c	\
	darkstar-notinpkg.c	\
	darkstar-packcontent.c	\
	darkstar-packname.c	\
	darkstar-packsize.c	\
	elfdeps.c

SH_SRC = \
	mkslack-desc.sh

LIB_SRC = \
	util/admdir.c	\
	util/cbn.c	\
	util/ealloc.c	\
	util/estrtoul.c	\
	util/slist.c	\
	util/strtoid.c

OBJ     = $(LIB_SRC:.c=.o)
LIB     = dst-util.a
C_PRG   = $(C_SRC:.c=)
SH_PRG  = $(SH_SRC:.sh=)
PRG     = $(C_PRG) $(SH_PRG)

elfdeps: CFLAGS += -std=c99

all: $(PRG)

$(LIB): $(OBJ)
	ar rcD $@ $^
	
$(C_PRG): $(LIB)

.c:
	$(CC)  $(LDFLAGS) $(CFLAGS) -o $@ $^ $(LDLIBS)
.c.o:
	$(CC)  $(CFLAGS) -c -o $@ $^

.sh:
	$(INSTALL) $<  $@

install: all
	$(INSTALL) -D -m 755 -t $(DESTDIR)$(BINDIR)/ $(PRG)
	$(INSTALL) -D -m 644 -t $(DESTDIR)$(MANDIR)/man1 $(PRG:=.1)
	( cd $(DESTDIR)$(BINDIR) ; \
	    ln -sf darkstar-notinpkg darkstar-notonsys ; \
	)

clean:
	@rm -vf $(OBJ) $(PRG) $(LIB)

.PHONY:
	all install clean
