CC      = gcc
CFLAGS  = -Os -std=c99 -D_POSIX_C_SOURCE=199309L 
LDFLAGS = #-I/usr/local/opt/ncurses/include -L/usr/local/opt/ncurses/lib

TARGET  = $(shell basename $(abspath $(dir $$PWD)))
PREFIX  = $(shell pwd)/install

all: clean
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET).test $(TARGET).c -lncurses -lpthread
	strip $(TARGET).test

clean:
	$(RM) *.test

test:
	cat puzzles.txt | ./$(TARGET).test

verbose:
	cat puzzles.txt | TERMINFO=$(PREFIX)/share/terminfo ./$(TARGET).test -v

app_name                := sudoku
docker_name             := $(app_name)
docker_tag              := dev
docker_container        := $(app_name)

.PHONY: upgrade
upgrade:
	docker pull alpine:edge

.PHONY: build
build:
	docker-compose build

.PHONY: run
run:
	docker-compose up --force-recreate -d

.PHONY: exec
exec:
	docker-compose exec $(docker_container) /usr/bin/fish
