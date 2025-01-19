// TODO
//  1. On change direction if next direction is opposite current return ### DONE ###
//  2. Collision with environment ### DONE ###
//  3. Eating fruit, update position of fruit ### DONE ###
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
struct termios orig_termios;

typedef struct position {
  int x;
  int y;
  struct position * next;
} Position;
Position snake;
Position fruit = {10, 10};
Position* last_section;

void icanon_terminal_mode();
void not_icanon_terminal_mode();
void update_snake_position();
void update_snake_body();
int detect_collision(int max_widt_pos, int max_heigth_pos, Position* snake);
int eating_fruit(Position* snake, Position* fruit);
void calcualte_new_fruit_position(Position *fruit);
Position* create_new_node(Position* from);
void draw_elements();
int kbhit();
int getch();

  

int main() {

  snake.x = 30;
  snake.y = 12;
  snake.next = NULL;
  last_section = &snake;

  do {
    printf("\033c");
    update_snake_position();
    game_over = detect_collision(F_WIDTH, F_HEIGHT, &snake);
    int required_new_fruit = eating_fruit(&snake, &fruit);
    if(required_new_fruit == 1){
      Position* new_section = create_new_node(last_section);
      last_section->next = new_section;
      last_section = new_section;
      calcualte_new_fruit_position(&fruit);
    }
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
  Position* element = &snake;
  for (int h = 0; h <= F_HEIGHT; h++) {
    for (int w = 0; w <= F_WIDTH; w++) {
      if (w == F_WIDTH) {
        printf("H\n");
      } else {
        if (h != 0 && h != F_HEIGHT && w != 0 && w != F_WIDTH) {

          // Error on draw?

          if(element->x == w && element->y == h){
              printf("@");
              if(element->next != NULL){
                element = element->next;
              }
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

  Position* temp = &snake;

  while(temp->next != NULL){
        temp->next->x = temp->x;
        temp->next->y = temp->y;
        temp = temp->next;
      }

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

int detect_collision(int max_width_pos, int max_heigth_pos, Position* snake){

  if(snake->x == max_width_pos || snake->x == 0 || snake->y == max_heigth_pos || snake->y == 0){
    return 1;
  }
  
  return 0;
}

int eating_fruit(Position* snake, Position* fruit){
  if(snake->x == fruit->x && snake->y == fruit->y){
    return 1;
  }
  return 0;
}

void calcualte_new_fruit_position(Position *fruit){
  int random_width = rand() % ((F_WIDTH -1) + 1) + 1;
  fruit->x = random_width;
  int random_height = rand() % ((F_HEIGHT -1) + 1) + 1;
  fruit->y = random_height;
}

Position* create_new_node(Position* from){
  Position* temp = malloc(sizeof(Position));
  temp->x = 0;
  temp->y = 0;
  temp->next = NULL;
  return temp;
}

