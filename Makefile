CC = gcc
CFLAGS = -Os -std=c99

all: clean
	@for file in *.c; do ${CC} ${CFLAGS} -o `echo $$file | cut -d. -f1`.test $$file; done

clean:
	@for file in *.c; do if [[ -f `echo $$file | cut -d. -f1`.test ]]; then rm `echo $$file | cut -d. -f1`.test; fi done

test:
	cat puzzles.txt | ./sudoku.test