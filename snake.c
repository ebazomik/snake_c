#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

const int F_WIDTH = 65;
const int F_HEIGHT = 25;
struct termios orig_termios;
void reset_terminal_mode();
void set_conio_terminal_mode();
int kbhit();
int getch();

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

struct Position snake;

int main() {

  int game_over = 0;

  snake.x = 30;
  snake.y = 12;

  do {
    printf("\033c");
    set_conio_terminal_mode();
    
    if(kbhit()){
       int c = getch();
       printf("pressed -> %i", c);
    }
    
    reset_terminal_mode();

    usleep(80000);

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
  } while (!game_over);

  return 0;
}

void reset_terminal_mode() { tcsetattr(0, TCSANOW, &orig_termios); };
void set_conio_terminal_mode() {
  struct termios new_termios;

  /* take two copies - one for now, one for later */
  tcgetattr(0, &orig_termios);
  memcpy(&new_termios, &orig_termios, sizeof(new_termios));

  /* register cleanup handler, and set the new terminal mode */
  atexit(reset_terminal_mode);
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
