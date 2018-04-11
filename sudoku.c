#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define SQRT_CELL 3
#define N (SQRT_CELL * SQRT_CELL)

typedef struct {
  int number;
  int column;
  int row;
  int cell;
} box;

typedef struct {
  box box[N][N];
  int inserted;
  int rule_column[N][N + 1];
  int rule_row[N][N + 1];
  int rule_cell[N][N + 1];
} sudoku;

int sudoku_get(sudoku const s, int row, int column) {
  return s.box[row][column].number;
}

void sudoku_erase(sudoku *s, int row, int column) {
  int val = s->box[row][column].number;
  if (val != 0) {
    s->inserted--;
  }
  s->box[row][column].number                  = 0;
  s->rule_row[row][val]                       = 0;
  s->rule_column[column][val]                 = 0;
  s->rule_cell[s->box[row][column].cell][val] = 0;
}

void sudoku_set(sudoku *s, int row, int column, int val) {
  s->box[row][column].number = val;
  s->box[row][column].column = column;
  s->box[row][column].row    = row;
  s->box[row][column].cell   = row / SQRT_CELL * SQRT_CELL + column / SQRT_CELL;

  s->rule_row[row][val]                       = 0;
  s->rule_column[column][val]                 = 0;
  s->rule_cell[s->box[row][column].cell][val] = 0;

  if (val == 0) {
    return;
  }

  s->inserted++;
  s->rule_row[s->box[row][column].row][val]       = 1;
  s->rule_cell[s->box[row][column].cell][val]     = 1;
  s->rule_column[s->box[row][column].column][val] = 1;
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
    if (s->rule_cell[s->box[row][column].cell][rule] == 1) {
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
      if (s->box[i][j].number == 0) {
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

void sudoku_input(sudoku *s) {
  s->inserted = 0;
  for (int i = 0; i < N * N; i++) {
    int row    = i / N;
    int column = i % N;

    auto char c = getchar();
    if (c == -1) {
      exit(0);
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
    found_solution = solve(s);
    if (found_solution == 1) {
      break;
    }
    sudoku_erase(s, row, column);
  }
  free(possibilities);
  return found_solution;
}

int main(int argc, char const *argv[]) {
  for (;;) {
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
      continue;
    }
    printf("origin: ");
    for (int i = 0; i < N * N; i++) {
      printf("%d", s.box[i / 9][i % 9].number);
    }
    printf("\n");
    if (solve(&s)) {
      printf("solved: ");
      for (int i = 0; i < N * N; i++) {
        printf("%d", s.box[i / 9][i % 9].number);
      }
    } else {
      printf("%s\n", "No answer!");
    }
    printf("\n");
  }
  return 0;
}