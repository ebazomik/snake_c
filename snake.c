// TODO
//  1. On change direction if next direction is opposite current return ### DONE ###
//  2. Collision with environment
//  3. Eating fruit, update position of fruit
//  4. Eating fruit, update snake length + calcualate new position of each body
//  item
//  5. Add menu (tutorial movement + start game)
//  6. Organize and cleaning code

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <termios.h>
#include <threads.h>
#include <time.h>
#include <unistd.h>

const int F_WIDTH = 65;
const int F_HEIGHT = 25;
int current_direction;
int new_direction;
int game_over = 0;

void icanon_terminal_mode();
void not_icanon_terminal_mode();
void update_snake_position();
void draw_elements();
int kbhit();
int getch();

struct termios orig_termios;
struct Position {
  int x;
  int y;
};
struct Position snake;
struct Position snake_body[0];
struct Position fruit = {10, 10};

int main() {

  snake.x = 30;
  snake.y = 12;

  do {
    printf("\033c");
    update_snake_position();
    not_icanon_terminal_mode();
    if (kbhit()) {
      new_direction = getch();
    }
    icanon_terminal_mode();
    draw_elements();
    usleep(100000);

  } while (!game_over);

  return 0;
}

void icanon_terminal_mode() { tcsetattr(0, TCSANOW, &orig_termios); };
void not_icanon_terminal_mode() {
  struct termios new_termios;

  /* take two copies - one for now, one for later */
  tcgetattr(0, &orig_termios);
  memcpy(&new_termios, &orig_termios, sizeof(new_termios));

  /* register cleanup handler, and set the new terminal mode */
  atexit(icanon_terminal_mode);
  cfmakeraw(&new_termios);
  tcsetattr(0, TCSANOW, &new_termios);
}

int kbhit() {
  struct timeval tv = {0L, 0L};
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(0, &fds);
  return select(1, &fds, NULL, NULL, &tv) > 0;
}

int getch() {
  int r;
  unsigned char c;
  if ((r = read(0, &c, sizeof(c))) < 0) {
    return r;
  } else {
    return c;
  }
}

void draw_elements() {
  for (int h = 0; h <= F_HEIGHT; h++) {
    for (int w = 0; w <= F_WIDTH; w++) {
      if (w == F_WIDTH) {
        printf("H\n");
      } else {
        if (h != 0 && h != F_HEIGHT && w != 0 && w != F_WIDTH) {
          if (snake.y == h && snake.x == w) {
            printf("@");
          } else if (fruit.y == h && fruit.x == w) {
            printf("*");
          } else {
            printf(" ");
          }
        } else {
          printf("H");
        }
      }
    }
  }
}

void update_snake_position() {
  switch (new_direction) {
  case 119:
    if (current_direction == 115) {
      snake.y++;
      return;
    }
    snake.y--;
    current_direction = new_direction;
    break;
  case 100:
    if (current_direction == 97) {
      snake.x--;
      return;
    }
    snake.x++;
    current_direction = new_direction;
    break;
  case 115:
    if (current_direction == 119) {
      snake.y--;
      return;
    }
    snake.y++;
    current_direction = new_direction;
    break;
  case 97:
    if (current_direction == 100) {
      snake.x++;
      return;
    }
    snake.x--;
    current_direction = new_direction;
    break;
  case 113:
    game_over = 1;
    break;
  default:
    return;
  };
}
