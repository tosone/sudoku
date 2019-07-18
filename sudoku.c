#include <getopt.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <ncurses.h>

#define SQRT_CELL 3
#define N (SQRT_CELL * SQRT_CELL)

WINDOW *win;
WINDOW *infowin;

int backtrace = 0;
bool verbose  = false;

#ifdef CLOCK_PROCESS_CPUTIME_ID
#define CLOCKTYPE CLOCK_PROCESS_CPUTIME_ID
#else
#define CLOCKTYPE CLOCK_MONOTONIC
#endif

typedef struct {
  int number;
  int column;
  int row;
  int cell;
} sudoku_box;

typedef struct {
  sudoku_box sudoku_box[N][N];
  int inserted;
  int rule_column[N][N + 1];
  int rule_row[N][N + 1];
  int rule_cell[N][N + 1];
} sudoku;

int sudoku_get(sudoku const s, int row, int column) {
  return s.sudoku_box[row][column].number;
}

void sudoku_erase(sudoku *s, int row, int column) {
  int val = s->sudoku_box[row][column].number;
  if (val != 0) {
    s->inserted--;
  }
  s->sudoku_box[row][column].number                  = 0;
  s->rule_row[row][val]                              = 0;
  s->rule_column[column][val]                        = 0;
  s->rule_cell[s->sudoku_box[row][column].cell][val] = 0;
}

void sudoku_set(sudoku *s, int row, int column, int val) {
  s->sudoku_box[row][column].number = val;
  s->sudoku_box[row][column].column = column;
  s->sudoku_box[row][column].row    = row;
  s->sudoku_box[row][column].cell   = row / SQRT_CELL * SQRT_CELL + column / SQRT_CELL;

  s->rule_row[row][val]                              = 0;
  s->rule_column[column][val]                        = 0;
  s->rule_cell[s->sudoku_box[row][column].cell][val] = 0;

  if (val == 0) {
    return;
  }

  s->inserted++;
  s->rule_row[s->sudoku_box[row][column].row][val]       = 1;
  s->rule_cell[s->sudoku_box[row][column].cell][val]     = 1;
  s->rule_column[s->sudoku_box[row][column].column][val] = 1;
}

void sudoku_get_rule_at(sudoku *s, int row, int column, int **possibilites, int *poss_count) {
  int rule_poss[N + 1] = {0};
  *poss_count          = 0;
  for (int rule = 0; rule < N + 1; rule++) {
    if (s->rule_row[row][rule] == 1) {
      rule_poss[rule] = 1;
    }
  }
  for (int rule = 0; rule < N + 1; rule++) {
    if (s->rule_column[column][rule] == 1) {
      rule_poss[rule] = 1;
    }
  }
  for (int rule = 0; rule < N + 1; rule++) {
    if (s->rule_cell[s->sudoku_box[row][column].cell][rule] == 1) {
      rule_poss[rule] = 1;
    }
  }
  for (int i = 1; i < N + 1; i++) {
    if (rule_poss[i] == 0) {
      (*possibilites)[*poss_count] = i;
      (*poss_count)++;
    }
  }
}

void sudoku_get_most_poss(sudoku *s, int *row, int *column, int **possibilites, int *poss_count) {
  int min = N + 1;
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      if (s->sudoku_box[i][j].number == 0) {
        sudoku_get_rule_at(s, i, j, possibilites, poss_count);
        if (*poss_count < min) {
          min     = *poss_count;
          *row    = i;
          *column = j;
        }
      }
    }
  }
  sudoku_get_rule_at(s, *row, *column, possibilites, poss_count);
}

void sleep_ms(int ms) {
  struct timespec tim, tim2;
  tim.tv_sec  = 0;
  tim.tv_nsec = ms * 1000000L;
  nanosleep(&tim, &tim2);
}

void *timeout_thread() {
  sleep_ms(300);
  exit(0);
}

void sudoku_input(sudoku *s) {
  s->inserted = 0;
  for (int i = 0; i < N * N; i++) {
    int row    = i / N;
    int column = i % N;

    pthread_t thread_id;
    pthread_create(&thread_id, NULL, timeout_thread, NULL);
    auto char c = getchar();
    pthread_cancel(thread_id);
    if (c == '\n' || c == '\r') {
      i--;
      continue;
    }
    if (c == -1) {
      return;
    }
    if (c == 10) {
      break;
    }
    if (c >= '0' && c <= '9') {
      sudoku_set(s, row, column, c - '0');
    }
  }
  return;
}

void print_sudoku(sudoku *s) {
  for (int i = 0; i < 19; i++) {
    for (int j = 0; j < 37; j++) {
      if (i % 2 == 0) {
        if (i % 6 == 0) {
          wattron(win, A_BOLD | COLOR_PAIR(2));
          if (j % 4 != 0) {
            wprintw(win, "-");
          } else {
            wprintw(win, "+");
          }
        } else {
          if (j % 4 != 0) {
            wattron(win, COLOR_PAIR(1));
            wprintw(win, "-");
          } else {
            if (j % 12 == 0) {
              wattron(win, COLOR_PAIR(2));
            } else {
              wattron(win, COLOR_PAIR(1));
            }
            wprintw(win, "+");
          }
        }
      } else {
        if (j % 4 == 2) {
          wattron(win, COLOR_PAIR(4));
          if (s->sudoku_box[i / 2][j / 4].number == 0) {
            wprintw(win, " "); // specified num
          } else {
            wprintw(win, "%d", s->sudoku_box[i / 2][j / 4].number); // specified num
          }
        } else if (j % 4 == 0) {
          if (j == 0 || j == 36) {
            wattron(win, A_BOLD | COLOR_PAIR(2));
          } else if (j % 12 == 0) {
            wattron(win, A_BOLD | COLOR_PAIR(2));
          } else {
            wattron(win, A_BOLD | COLOR_PAIR(1));
          }
          wprintw(win, "|");
        } else {
          wprintw(win, " ");
        }
      }
    }
  }
  wrefresh(win);
}

void print_sudoku_raw(sudoku *s) {
  for (int i = 0; i < N * N; i++) {
    printf("%d", s->sudoku_box[i / 9][i % 9].number);
  }
  printf("\n");
}

bool solve(sudoku *s) {
  if (s->inserted == N * N) {
    return true;
  }
  int row, column;
  int *possibilities = malloc(N * sizeof(int));
  int poss_count;

  sudoku_get_most_poss(s, &row, &column, &possibilities, &poss_count);
  if (poss_count == 0) {
    return false;
  }
  bool found_solution;
  for (int i = 0; i < poss_count; i++) {
    sudoku_set(s, row, column, possibilities[i]);
    if (verbose) {
      wclear(win);
      print_sudoku(s);
      sleep_ms(80);
    }
    found_solution = solve(s);
    if (found_solution == 1) {
      break;
    }
    backtrace++;
    if (verbose) {
      wclear(infowin);
      wprintw(infowin, "Backtrace: %d\n", backtrace);
      wrefresh(infowin);
    }
    sudoku_erase(s, row, column);
  }
  free(possibilities);
  return found_solution;
}

static void cleanup(void) {
  endwin();
}

int main(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "v")) != -1) {
    if (opt == 118) {
      verbose = true;
    }
  }

  if (verbose) {
    initscr();
    cbreak();
    noecho();
    atexit(cleanup);
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    init_pair(3, COLOR_CYAN, COLOR_BLACK);
    init_pair(4, COLOR_WHITE, COLOR_BLACK);

    WINDOW *verwin = newwin(2, 37, 1, 3);
    win            = newwin(19, 37, 3, 3);
    infowin        = newwin(3, 37, 23, 3);

    refresh();
    wrefresh(verwin);
    wrefresh(win);
    wrefresh(infowin);

    wclear(win);

    wprintw(verwin, "            sudoku solver");
    wrefresh(verwin);
  }

  struct timespec tsi, tsf;

  int try_read = 0;

  for (;;) {
    clock_gettime(CLOCKTYPE, &tsi);
    backtrace = 0;
    sudoku s;
    s.inserted = 0;
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < N + 1; j++) {
        s.rule_row[i][j]    = 0;
        s.rule_column[i][j] = 0;
        s.rule_cell[i][j]   = 0;
      }
    }
    sudoku_input(&s);
    if (s.inserted == 0) {
      if (try_read == 3) {
        getchar();
        return 0;
      }
      try_read++;
      continue;
    }

    if (verbose) {
      print_sudoku(&s);
      wclear(win);
    } else {
      printf("%s", "Origin: ");
      print_sudoku_raw(&s);
    }

    if (solve(&s)) {
      if (verbose) {
        print_sudoku(&s);
        wrefresh(win);
        wprintw(infowin, "%s\n", "Solved!");
      } else {
        printf("%s", "Solved: ");
        print_sudoku_raw(&s);
      }
    } else {
      wprintw(infowin, "%s\n", "No answer!");
    }

    clock_gettime(CLOCKTYPE, &tsf);
    double elaps_s = difftime(tsf.tv_sec, tsi.tv_sec);
    long elaps_ns  = tsf.tv_nsec - tsi.tv_nsec;
    if (verbose) {
      wprintw(infowin, "Resolve cost CPU time: %lfs\n", elaps_s + ((double)elaps_ns) / 1.0e9);
      wrefresh(infowin);

      wclear(win);
      wclear(infowin);

      sleep(3);
    } else {
      printf("Backtrace: %d\n", backtrace);
      printf("Resolve cost CPU time: %lfs\n\n", elaps_s + ((double)elaps_ns) / 1.0e9);
    }
  }

  return 0;
}
