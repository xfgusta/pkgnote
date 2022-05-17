CC=gcc
CFLAGS=-Wall -Wextra -pedantic -std=c99 -O2
LDFLAGS=-lelf -ljson-c

DESTDIR=
PREFIX=$(DESTDIR)/usr/local
BINDIR=$(PREFIX)/bin

INSTALL=install -p

all: pkgnote

install: all
	mkdir -p $(BINDIR)
	$(INSTALL) -m 0755 pkgnote $(BINDIR)

uninstall:
	rm -f $(BINDIR)/pkgnote

clean:
	rm -f pkgnote
