CC ?= gcc
CFLAGS ?= -std=c11 -O2 -Wall -Wextra -pedantic
CPPFLAGS += -Iinclude

BIN := sysalias
SRCS := $(wildcard src/*.c)
OBJS := $(SRCS:.c=.o)

PREFIX ?= /usr/local
BINDIR := $(PREFIX)/bin
DISTDIR := dist
VERSION ?= 0.1.0

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

src/%.o: src/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

install: $(BIN)
	mkdir -p $(DESTDIR)$(BINDIR)
	cp $(BIN) $(DESTDIR)$(BINDIR)/$(BIN)

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/$(BIN)

clean:
	rm -f $(OBJS) $(BIN)

dist: $(BIN)
	mkdir -p $(DISTDIR)
	cp $(BIN) $(DISTDIR)/$(BIN)

test: all
	@echo "No tests configured"

.PHONY: all clean install uninstall dist test

