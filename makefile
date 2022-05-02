CC=gcc
CFLAGS=-Wall -Wextra -pedantic -std=c99 -O2
LDFLAGS=-lelf

DESTDIR=
PREFIX=$(DESTDIR)/usr/local
BINDIR=$(PREFIX)/bin

INSTALL=install -p

all: rpkgnote

install: all
	mkdir -p $(BINDIR)
	$(INSTALL) -m 0755 rpkgnote $(BINDIR)

uninstall:
	rm -f $(BINDIR)/rpkgnote

clean:
	rm -f rpkgnote
