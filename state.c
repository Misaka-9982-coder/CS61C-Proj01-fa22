#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t* state, unsigned int snum);
static char next_square(game_state_t* state, unsigned int snum);
static void update_tail(game_state_t* state, unsigned int snum);
static void update_head(game_state_t* state, unsigned int snum);

/* Task 1 */
game_state_t* create_default_state() {
  // TODO: Implement this function.
  snake_t* snake = malloc(sizeof(snake_t));
  snake->head_row = 2;
  snake->head_col = 4;
  snake->tail_row = 2;
  snake->tail_col = 2;
  snake->live = 1;

  game_state_t* game_state = malloc(sizeof(game_state_t));
  game_state->num_snakes = 1;
  game_state->snakes = snake;

  game_state->num_rows = 18;
  game_state->board = malloc(sizeof(char*) * game_state->num_rows);
  for(int i = 0; i < game_state->num_rows; i ++ ) {
    game_state->board[i] = malloc(sizeof(char) * 21);
    strcpy(game_state->board[i], "#                  #");
  }
  strcpy(game_state->board[0], "####################");
  strcpy(game_state->board[2], "# d>D    *         #");
  strcpy(game_state->board[17], "####################");
  
  return game_state;
}

/* Task 2 */
void free_state(game_state_t* state) {
  // TODO: Implement this function.
  for (int i = 0; i < state->num_rows; i ++ ) {
    free(state->board[i]);
  }
  free(state->board);
  free(state->snakes);
  free(state);
  return;
}

/* Task 3 */
void print_board(game_state_t* state, FILE* fp) {
  // TODO: Implement this function.
  for (int i = 0; i < state->num_rows; i ++ ) {
    fprintf(fp, "%s\n", state->board[i]);
  }
  return;
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t* state, char* filename) {
  FILE* f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t* state, unsigned int row, unsigned int col) {
  return state->board[row][col];
}

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  // TODO: Implement this function.
  char *judge = "wasd";
  for(int i = 0; i < strlen(judge); i ++ ) {
    if (c == judge[i]) {
      return true;
    }
  }
  return false;
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  // TODO: Implement this function.
  char *judge = "WASD";
  for(int i = 0; i < strlen(judge); i ++ ) {
    if (c == judge[i]) {
      return true;
    }
  }
  return false;
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  // TODO: Implement this function.
  char *judge = "wasd^<v>WASDx";
  for(int i = 0; i < strlen(judge); i ++ ) {
    if (c == judge[i]) {
      return true;
    }
  }
  return false;
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  // TODO: Implement this function.
  char *tails = "wasd";
  char *bodys = "^<v>";
  for (int i = 0; i < strlen(bodys); i ++ ) {
    if(c == bodys[i]) {
      c = tails[i];
      break;
    }
  }
  return c;
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implement this function.
  char *heads = "WASD";
  char *bodys = "^<v>";
  for (int i = 0; i < strlen(heads); i ++ ) {
    if(c == heads[i]) {
      c = bodys[i];
      break;
    }
  }
  return c;
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implement this function.
  char *increase = "vsS";
  char *decrease = "^wW";
  
  for(int i = 0; i < strlen(increase); i ++ ) {
    if(c == increase[i]) {
      cur_row ++ ;
      return cur_row;
    }
  }

  for(int i = 0; i < strlen(decrease); i ++ ) {
    if(c == decrease[i]) {
      cur_row -- ;
      break;
    }
  }
  
  return cur_row;
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implement this function.
  char *increase = ">dD";
  char *decrease = "<aA";

  for(int i = 0; i < strlen(increase); i ++ ) {
    if(c == increase[i]) {
      cur_col ++ ;
      return cur_col;
    }
  }

  for(int i = 0; i < strlen(decrease); i ++ ) {
    if(c == decrease[i]) {
      cur_col -- ;
      break;
    }
  }

  return cur_col;
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  snake_t snake = state->snakes[snum];
  unsigned int head_row = snake.head_row;
  unsigned int head_col = snake.head_col;
  char head = state->board[head_row][head_col];
  char next_square = '?';
  unsigned int next_row = get_next_row(head_row, head);
  unsigned int next_col = get_next_col(head_col, head);
  next_square = state->board[next_row][next_col];

  return next_square;
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  snake_t *snake = &state->snakes[snum];
  unsigned int head_row = snake->head_row;
  unsigned int head_col = snake->head_col;
  char head = state->board[head_row][head_col];

  unsigned int next_row = get_next_row(head_row, head);
  unsigned int next_col = get_next_col(head_col, head);

  char body = head_to_body(head);

  state->board[next_row][next_col] = head;
  state->board[head_row][head_col] = body;

  snake->head_row = next_row;
  snake->head_col = next_col;

  return;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  snake_t *snake = &state->snakes[snum];
  unsigned int tail_row = snake->tail_row;
  unsigned int tail_col = snake->tail_col;
  char tail = state->board[tail_row][tail_col];

  unsigned int next_row = get_next_row(tail_row, tail);
  unsigned int next_col = get_next_col(tail_col, tail);
  
  char body_next_to_tail = state->board[next_row][next_col];
  char next_tail = body_to_tail(body_next_to_tail);

  state->board[next_row][next_col] = next_tail;
  state->board[tail_row][tail_col] = ' ';

  snake->tail_row = next_row;
  snake->tail_col = next_col;

  return;
}

/* Task 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
  // TODO: Implement this function.
  unsigned int snum = state->num_snakes;

  for(unsigned int i = 0; i < snum; i ++ ) {
    snake_t *snake = &state->snakes[i];
    
    bool is_alive = snake->live;
    if(!is_alive) {
      continue;
    }
    
    unsigned int head_row = snake->head_row;
    unsigned int head_col = snake->head_col;

    char next_squ = next_square(state, i);
    if(is_snake(next_squ) || next_squ == '#') {
      snake->live = false;
      state->board[head_row][head_col] = 'x';
      continue;
    }

    if(next_squ == '*') {
      update_head(state, i);
      add_food(state);
      continue;
    }

    update_head(state, i);
    update_tail(state, i);
  }
  return;
}

/* Task 5 */
game_state_t* load_board(char* filename) {
  // TODO: Implement this function.
  FILE *fp = NULL;
  fp = fopen(filename, "r");
  if(fp == NULL) {
    return NULL;
  }

  game_state_t* game_state = malloc(sizeof(game_state_t));

  char *buff = malloc(1024 * 1024 * sizeof(char));
  unsigned int cnt = 0;
  while(!feof(fp)) {
    fgets(buff, 1024 * 1024, fp);
	  cnt ++ ;
	}
  fclose(fp);
  free(buff);

  fp = fopen(filename, "r");
  game_state->num_rows = cnt - 1;
  game_state->board = malloc(sizeof(char*) * game_state->num_rows);

  for(int i = 0; i < cnt; i ++ ) {
    buff = malloc(1024 * 1024 * sizeof(char));
    fgets(buff, 1024 * 1024, fp);
    game_state->board[i] = malloc((strlen(buff)) * sizeof(char));
    strcpy(game_state->board[i], buff);
    game_state->board[i][strlen(buff) - 1] = '\0';
    free(buff);
  }
  fclose(fp);

  return game_state;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  snake_t* snake = &state->snakes[snum];
  unsigned int cur_row = snake->tail_row;
  unsigned int cur_col = snake->tail_col;
  char cur_char = state->board[cur_row][cur_col];

  while(!is_head(cur_char)) {
    cur_row = get_next_row(cur_row, cur_char);
    cur_col = get_next_col(cur_col, cur_char);
    cur_char = state->board[cur_row][cur_col];
    // printf("row: %d, col: %d, char: %c\n", cur_row, cur_col, cur_char);
  }

  snake->head_row = cur_row;
  snake->head_col = cur_col;

  return;
}

/* Task 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
  // TODO: Implement this function.
  if(state == NULL) {
    return NULL;
  }

  unsigned int snake_num = 0;
  unsigned int *tail_rows = malloc(sizeof(unsigned int) * 1000);
  unsigned int *tail_cols = malloc(sizeof(unsigned int) * 1000);

  for (unsigned int i = 0; i < state->num_rows; i ++ ) {
    for(unsigned int j = 0; j < strlen(state->board[i]); j ++ ) {
      if(is_tail(state->board[i][j])) {
        tail_rows[snake_num] = i;
        tail_cols[snake_num] = j;
        snake_num ++ ;
      }
    }
  }

  snake_t* snakes = malloc(sizeof(snake_t) * snake_num);
  state->snakes = snakes;
  state->num_snakes = snake_num;

  for(unsigned int i = 0; i < snake_num; i ++ ) {
    snakes[i].tail_row = tail_rows[i];
    snakes[i].tail_col = tail_cols[i];
    snakes[i].live = 1;
    find_head(state, i);
  }

  free(tail_cols);
  free(tail_rows);
  
  return state;
}
