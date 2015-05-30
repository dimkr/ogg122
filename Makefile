CC ?= cc
CFLAGS ?= -O2 -pipe
LDFLAGS ?= -Wl,-s
DESTDIR ?=
BIN_DIR ?= /bin
MAN_DIR ?= /usr/share/man
DOC_DIR ?= /usr/share/doc

CFLAGS += -Wall -pedantic
LIBS = -ltinyalsa

INSTALL = install -v

all: ogg122

ogg122: ogg122.c
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)

install: all
	$(INSTALL) -D -m 755 ogg122 $(DESTDIR)/$(BIN_DIR)/ogg122
	$(INSTALL) -D -m 644 ogg122.1 $(DESTDIR)/$(MAN_DIR)/man1/ogg122.1
	$(INSTALL) -D -m 644 README $(DESTDIR)/$(DOC_DIR)/ogg122/README
	$(INSTALL) -m 644 AUTHORS $(DESTDIR)/$(DOC_DIR)/ogg122/AUTHORS
	$(INSTALL) -m 644 COPYING $(DESTDIR)/$(DOC_DIR)/ogg122/COPYING

clean:
	rm -f ogg122
