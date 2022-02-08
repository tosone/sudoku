CC      = gcc
CFLAGS  = -Os -std=c99 -D_POSIX_C_SOURCE=199309L
LDFLAGS = #-I/usr/local/opt/ncurses/include -L/usr/local/opt/ncurses/lib

TARGET  = $(shell basename $(abspath $(dir $$PWD)))
PREFIX  = $(shell pwd)/install

.PHONY: all
all: clean
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET).test $(TARGET).c -lncurses -lpthread
	strip $(TARGET).test

.PHONY: clean
clean:
	$(RM) *.test

.PHONY: test
test:
	cat puzzles.txt | ./$(TARGET).test

.PHONY: verbose
verbose:
	cat puzzles.txt | TERMINFO=$(PREFIX)/share/terminfo ./$(TARGET).test -v
