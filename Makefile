PRG=darkstar-tools
VERSION=14.1
CFLAGS=-g -O3 -ansi -Wall -Wextra -Werror -pedantic
CPPFLAGS=-D_XOPEN_SOURCE=500
LDFLAGS=-L.
LIBS=-ldst
CC=gcc
CPP=g++
PREFIX?=/usr/local
MANDIR?=$(PREFIX)/man
BINDIR?=$(PREFIX)/bin
SHELL=/bin/sh
CC=gcc
CPP=g++
INSTALL=install
INSTALL_PROGRAM=$(INSTALL)
INSTALL_DATA=$(INSTALL) -m644

.SUFFIXES:
.SUFFIXES: .o .c
.SUFFIXES: .oo .cc
.SUFFIXES: "" .sh
.SUFFIXES: "" .py
.SUFFIXES: "" .tcl

SRC = \
	darkstar-packname.c \
	darkstar-packcontent.c \
	darkstar-installed.c \
	cugfd.c \
	elfdeps.cc \
	darkstar-notinpkg.sh \
	darkstar-makedep.sh \
	darkstar-missingdeps.sh \
	darkstar-whoneeds.sh \
	mkslack-desc.sh \
	darkstar-deptree.py

LIB = \
	packname.c \
	findpkg.c \
	hr.c \

HDR=util.h arg.h

PRG=$(shell ls $(SRC) | cut -d. -f1)
OBJ=$(shell ls $(SRC) | grep '\.c*$$' | sed -e 's/\.c$$/.o/' -e 's/\.cc$$/.oo/' )
MAN=$(PRG:=.1)

all: libdst.a $(OBJ) $(PRG) $(HDR)

libdst.a: $(LIB:.c=.o)
	ar rc $@ $^

.c.o:
	$(CC)  $(CFLAGS) $(CPPFLAGS) -c -o $@ $< $(INCS)
.cc.oo:
	$(CPP) $(CFLAGS) $(CPPFLAGS) -c -o $@ $< $(INCS)
.o: libdst.a
	$(CC)  $(LDFLAGS) -o $@ $< $(LIBS)
.oo: libdst.a
	$(CPP) $(LDFLAGS) -o $@ $< $(LIBS)
.py:
	$(INSTALL_PROGRAM) $<  $@
.sh:
	$(INSTALL_PROGRAM) $<  $@
.tcl:
	$(INSTALL_PROGRAM) $<  $@

install: all
	mkdir -p $(DESTDIR)$(BINDIR)
	         $(DESTDIR)$(MANDIR)/man1
	$(INSTALL_PROGRAM) $(PRG) $(DESTDIR)$(BINDIR)
	$(INSTALL_DATA) $(MAN) $(DESTDIR)$(MANDIR)
	( cd $(DESTDIR)$(BINDIR) ; \
	    ln -sf darkstar-notinpkg darkstar-notonsys ; \
	)

clean:
	rm -f $(OBJ) $(PRG) $(LIB:.c=.o) libdst.a

dist-clean: clean
	rm -f $(PRG)-$(VERSION).tar.gz
	
dist:
	mkdir $(PRG)-$(VERSION)
	cp $(SRC) $(LIB) $(HDR) Makefile $(PRG)-$(VERSION)
	tar czf $(PRG)-$(VERSION).tar.gz $(PRG)-$(VERSION)
	rm -r $(PKG)-$(VERSION)

.PHONY:
	all install clean dist dist-clean
