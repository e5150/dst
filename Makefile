VERSION=14.1
CFLAGS=-D_XOPEN_SOURCE=500 -g -ansi -fPIC -O3 -I. -Wall -Wextra -Werror -pedantic -Wno-parentheses
LDFLAGS=
CC=gcc
CPP=g++
PREFIX?=/usr/local
MANDIR?=$(PREFIX)/man
BINDIR?=$(PREFIX)/bin

LLTSRC = \
	darkstar-packname.c \
	darkstar-packcontent.c \
	darkstar-installed.c \
	cugfd.c

CPPSRC = elfdeps.cc

SHSRC = darkstar-notinpkg.sh \
        mkslack-desc.sh

HDR=packname.h arg.h

EXEBIN=$(LLTSRC:.c=)   $(CPPSRC:.cc=)    $(SHSRC:.sh=)
ALLMAN=$(LLTSRC:.c=.1) $(CPPSRC:.cc=.1)  $(SHSRC:.sh=.1)
ALLSRC=$(LLTSRC)       $(CPPSRC)         $(SHSRC)         $(HDR)
ALLOBJ=$(LLTSRC:.c=.o) $(CPPSRC:.cc=.oo)                  
ALLBIN=$(EXEBIN)

all: $(ALLOBJ) $(ALLBIN) $(HDR)

%.o: %.c
	$(CC)  $(CFLAGS) -c -o $@ $<
%.oo: %.cc
	$(CPP) $(CFLAGS) -Wno-long-long -c -o $@ $<
%: %.o
	$(CC)  $(LDFLAGS) -o $@ $<
%: %.oo
	$(CPP) $(LDFLAGS) -o $@ $<
darkstar-notinpkg: darkstar-notinpkg.sh
	install -m 0755 $<  $@
darkstar-%: darkstar-%.o packname.o
	$(CC)  $(LDFLAGS) -o $@ $< packname.o

install: all
	mkdir -p $(DESTDIR)/$(BINDIR) \
	         $(DESTDIR)/$(MANDIR)/man1
	install -m 0755 -t $(DESTDIR)/$(BINDIR) $(EXEBIN)
	install -m 0644 -t $(DESTDIR)/$(MANDIR)/man1 $(ALLMAN)
	( cd $(DESTDIR)/$(BINDIR) ; \
	    ln -sf darkstar-notinpkg darkstar-notonsys ; \
	)

clean:
	echo cleaning
	rm -f $(ALLOBJ) $(ALLBIN) packname.o darkstar-tools-$(VERSION).tar.gz

dist:
	mkdir -p darkstar-tools-$(VERSION)
	tar cf - $(ALLSRC) Makefile | tar -xf - -C darkstar-tools-$(VERSION)/
	tar czf darkstar-tools-$(VERSION).tar.gz darkstar-tools-$(VERSION)
	rm -rf darkstar-tools-$(VERSION)

pkg: all
	rm -fr /tmp/package-dst
	mkdir -p /tmp/package-dst/install
	make install PREFIX=/usr DESTDIR=/tmp/package-dst
	strip --strip-unneeded /tmp/package-dst/usr/bin/* 2>/dev/null || true
	gzip /tmp/package-dst/usr/man/man1/*
	/tmp/package-dst/usr/bin/mkslack-desc darkstar-tools "small collection of slackware related programs" < desc > /tmp/package-dst/install/slack-desc
	cd /tmp/package-dst ; makepkg -l y -c y /tmp/darkstar-tools-$(VERSION)-$(shell uname -m)-1.tgz
