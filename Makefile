CC      = gcc
CFLAGS  = -Os -std=c99
LDFLAGS = -D_POSIX_C_SOURCE=199309L -I./3rdParty/ncurses/include -L./3rdParty/ncurses/lib
Target  = $(shell basename $(abspath $(dir $$PWD)))

all: clean
	${CC} ${CFLAGS} ${LDFLAGS} -o ${Target}.test ${Target}.c -lncurses -lpthread

clean:
	${RM} *.test

test:
	cat puzzles.txt | ./${Target}.test

verbose:
	cat puzzles.txt | TERMINFO=/usr/share/terminfo TERM=xterm-basic ./sudoku.test -v

dep:
	git submodule update --init
	cd 3rdParty/ncurses && ./configure && make
