CC ?= cc
CFLAGS ?= -O2 -pipe
LDFLAGS ?= -s
DESTDIR ?=
BIN_DIR ?= /bin
MAN_DIR ?= /usr/share/man
DOC_DIR ?= /usr/share/doc

CFLAGS += -Wall -pedantic
INSTALL = install -v
LIBS = -ltinyalsa

all: ogg122

ogg122: ogg122.o
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)

ogg122.o: ogg122.c stb_vorbis.c
	$(CC) -c -o $@ $< $(CFLAGS)

install: all
	$(INSTALL) -D -m 755 ogg122 $(DESTDIR)/$(BIN_DIR)/ogg122
	$(INSTALL) -D -m 644 ogg122.1 $(DESTDIR)/$(MAN_DIR)/man1/ogg122.1
	$(INSTALL) -D -m 644 README $(DESTDIR)/$(DOC_DIR)/ogg122/README
	$(INSTALL) -m 644 AUTHORS $(DESTDIR)/$(DOC_DIR)/ogg122/AUTHORS
	$(INSTALL) -m 644 UNLICENSE $(DESTDIR)/$(DOC_DIR)/ogg122/UNLICENSE

clean:
	rm -f ogg122 ogg122.o
