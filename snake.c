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
struct termios orig_termios;
void icanon_terminal_mode();
void not_icanon_terminal_mode();
void update_snake_position();
void draw_elements();
int kbhit();
int getch();
int current_direction = 119;

struct Pos {
  int x;
  int y;
};

struct Position {
  int x;
  int y;
  struct Pos next;
  struct Pos prev;
};

struct Pos snake;

int main() {

  int game_over = 0;

  snake.x = 30;
  snake.y = 12;

  do {
    printf("\033c");
    not_icanon_terminal_mode();
    if (kbhit()) {
      current_direction = getch();
    }
    icanon_terminal_mode();
    update_snake_position();
    usleep(100000);
    draw_elements();

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
          // TODO add logic for print snake

          if (snake.y == h && snake.x == w) {
            printf("@");
          } else {
            printf(" ");
          }
        } else {
          printf("H");
        }
      }
    }
  }

  printf("\nDirection = %i", current_direction);
}

void update_snake_position() {
  //TODO fix change direction
  switch (current_direction) {
  case 119:
    snake.x++;
  case 100:
    snake.y++;
  case 115:
    snake.x--;
  case 97:
    snake.y--;
  default:
    return;
  };
}
