CC      = gcc
CFLAGS  = -Os -std=c99
LDFLAGS = -D_POSIX_C_SOURCE=199309L -I./3rdParty/ncurses/include -L./3rdParty/ncurses/lib
Target  = $(shell basename $(abspath $(dir $$PWD)))

all: clean
	${CC} ${CFLAGS} ${LDFLAGS} -o ${Target}.test ${Target}.c -lncurses

clean:
	${RM} *.test

test:
	cat puzzles.txt | ./${Target}.test

dep:
	git submodule update --init
	cd 3rdParty/ncurses && ./configure && make
