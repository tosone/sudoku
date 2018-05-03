CC      = gcc
CFLAGS  = -Os -std=c11
LDFLAGS = -D_POSIX_C_SOURCE=199309L
Target  = $(shell basename $(abspath $(dir $$PWD)))

all: clean
	${CC} ${CFLAGS} ${LDFLAGS} -o ${Target}.test ${Target}.c

clean:
	${RM} *.test

test:
	cat puzzles.txt | ./${Target}.test