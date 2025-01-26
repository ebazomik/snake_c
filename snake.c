/*----------------------------------------------------------------------------------------
This is a simple implementation of the Snake game to start getting hands-on with the
language that has always intrigued and always scared me.
For the implementation I planned to use a single linked list
(good opportunity to study this data structure as well).
Happy to receive any advice or constructive critique asnd for those like me who are
trying to understand something about this language, I hope these lines will be useful ;)

ebazomik
-----------------------------------------------------------------------------------------*/


#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <termios.h>
#include <threads.h>
#include <time.h>
#include <unistd.h>

/* Struct for cartesian points 2D */
typedef struct position {
  int x;
  int y;
} Position;

/* Struct for create every node of snake */
typedef struct node {
  Position axis;
  struct node *next;
} Node;

/*----------------------------------------------------------------------------------------
 * Functions for mange terminal state
 * This solutions was found on Stack Overflow in "C non blocking keyboard input topic"
 * https://stackoverflow.com/questions/448944/c-non-blocking-keyboard-input
 * Thanks Alnitak for solution ;)
 * https://stackoverflow.com/users/6782/alnitak
 *----------------------------------------------------------------------------------------*/

/* Set terminal in canonical mode  */
void icanon_terminal_mode(struct termios *origin_termios) {
  tcsetattr(0, TCSANOW, origin_termios);
};

/* Set terminal in non canonical mode */
void not_icanon_terminal_mode(struct termios *origin_termios) {
  struct termios new_termios;
  void *canonical_terminal_mode = &icanon_terminal_mode;

  /* Take two copies - one for now, one for later */
  tcgetattr(0, origin_termios);
  memcpy(&new_termios, origin_termios, sizeof(new_termios));

  /* Register cleanup handler, and set the new terminal mode */
  atexit(canonical_terminal_mode);
  cfmakeraw(&new_termios);
  tcsetattr(0, TCSANOW, &new_termios);
}

/* Detect if keyboard is hitted */
int kbhit() {
  struct timeval tv = {0L, 0L};
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(0, &fds);
  return select(1, &fds, NULL, NULL, &tv) > 0;
}

/* Return which char was hitted */
int getch() {
  int r;
  unsigned char c;
  if ((r = read(0, &c, sizeof(c))) < 0) {
    return r;
  } else {
    return c;
  }
}

/*----------------------------------------------------------------------------------------
 * Snake functions
 *----------------------------------------------------------------------------------------*/

/* Update position of each node based on position of the previous node */
int update_snake_position(int new_direction, int current_direction, Node *snake) {

  /* Take snake node pointer for start point */
  Node *snake_node = snake;
  Position temp_pos = {-1, -1};

  /* ------ Before update body nodes position -------*/

  /* Crosses the single linked list as long as the node is not null */
  while (snake_node != NULL) {
    if (temp_pos.x != -1 && temp_pos.y != -1) {

      /* If cycle is in not in first step save coordinate temporarily for not to lose the information during the update */
      Position current_node_position = snake_node->axis;
      snake_node->axis.x = temp_pos.x;
      snake_node->axis.y = temp_pos.y;
      temp_pos = current_node_position;
    } else {
      temp_pos = snake_node->axis;
    }
    /* Set next node for cycle */
    snake_node = snake_node->next;
  }

  /* ------ After update head position -------*/

  /* Update snake head position based on current direction and new direction selected */
  /* If return 1, player has hit 'q' for quit the game */
  switch (new_direction) {
  case 119:
    if (current_direction == 115) {
      snake->axis.y++;
      return 0;
    }
    snake->axis.y--;
    current_direction = new_direction;
    break;
  case 100:
    if (current_direction == 97) {
      snake->axis.x--;
      return 0;
    }
    snake->axis.x++;
    current_direction = new_direction;
    break;
  case 115:
    if (current_direction == 119) {
      snake->axis.y--;
      return 0;
    }
    snake->axis.y++;
    current_direction = new_direction;
    break;
  case 97:
    if (current_direction == 100) {
      snake->axis.x++;
      return 0;
    }
    snake->axis.x--;
    current_direction = new_direction;
    break;
    /* Quit the game 'q' */
  case 113:
    return 1;
    break;
  default:
    return 0;
  };
  return 0;
};

/* Detect collision from field */
int detect_collision(int max_width_pos, int max_heigth_pos, Node *snake) {
  if (
    snake->axis.x == max_width_pos ||
    snake->axis.x == 0 ||
    snake->axis.y == max_heigth_pos ||
    snake->axis.y == 0
    ) {
      return 1;
  }
  return 0;
}

/* Detect collision from yourself */
int detect_yourself_collision(Node *snake) {
  Node *node = snake->next;
  while (node != NULL) {
    if (node->axis.x == snake->axis.x && node->axis.y == snake->axis.y) {
      return 1;
    } else {
      node = node->next;
    }
  }
  return 0;
};

/* Create new node and return pointer of this */
Node *create_new_node(Node *from) {
  Node *temp = malloc(sizeof(Node));
  temp->axis.x = from->axis.x;
  temp->axis.y = from->axis.y;
  temp->next = NULL;
  return temp;
}

/*----------------------------------------------------------------------------------------
 * Fruit functions
 *----------------------------------------------------------------------------------------*/

/* Check if fruit has been eaten, is simply a collision detection */
int eating_fruit(Node *snake, Position *fruit) {
  if (snake->axis.x == fruit->x && snake->axis.y == fruit->y) {
    return 1;
  }
  return 0;
}

/* Add new position to fruit*/
void calcualte_new_fruit_position(Position *fruit, int max_width, int max_heigth) {
  int random_width = rand() % ((max_width) + 1) + 1;
  fruit->x = random_width;
  int random_height = rand() % ((max_heigth) + 1) + 1;
  fruit->y = random_height;
}

/*----------------------------------------------------------------------------------------
 * Draw functions
 *----------------------------------------------------------------------------------------*/


/* Function to show points on top of screen*/
void draw_top_points(int current_points){
  printf("\n");
  printf("### POINTS = ");
  printf("%d ", current_points);
  printf("###");
  printf("\n");
};

/* Function for draw every element on game field on screen */
void draw_elements(int width, int height, Node *head, Position *fruit) {
  for (int h = 0; h <= height; h++) {
    for (int w = 0; w <= width; w++) {
      if (h == 0 || h == height || w == 0 || w == width) {
        printf("H");
      } else {
        Node *element = head;
        if (element->axis.x == w && element->axis.y == h) {
          printf("@");
        } else {
          int printed_snake_body = 0;

          /* Cycle to check if there is node body to draw */
          do {
            if (element->next != NULL) {
              element = element->next;
            }
            if (element->axis.x == w && element->axis.y == h) {
              printf("@");
              printed_snake_body = 1;
              break;
            }
          } while (element->next != NULL);

          if (!printed_snake_body) {

            /* Check if is time to draw a fruit */
            if (fruit->x == w && fruit->y == h) {
              printf("*");
            } else {
              printf(" ");
            }
          }
        }
        element = head;
      }
    }
    printf("\n");
  }
}

int main() {

  /* ------ Simple setup before start game ------ */
  
  const int F_WIDTH = 65;                               /* With of game field */
  const int F_HEIGHT = 25;                              /* Height of game field */
  const int U_SECONDS = 100000;                         /* Seconds for delay at end of main cycle */
  int current_direction;                                /* The direction selected before new direction */
  int new_direction;                                    /* The direction selected on hit wasd */
  int current_points = 0;                               /* How many fruits were eaten*/
  int game_over = 0;                                    /* Simply int to detect if the game has to stop */
  struct termios orig_termios;                          /* struct trermios definition for teminal functions */
  Node snake = {30, 12, NULL};    /* Start Node, head of snake */
  Position fruit = {10, 10};                      /* Fruit position */
  Node *last_node;                                      
  last_node = &snake;                                   /* Save last node for attach the next node when is create */

  do {
    printf("\033c"); /* Clear screen */
    int quit_game = update_snake_position(new_direction, current_direction, &snake);
    int field_collision = detect_collision(F_WIDTH, F_HEIGHT, &snake);
    int body_collision = detect_yourself_collision(&snake);
    game_over = field_collision || body_collision || quit_game;
    if (game_over == 0) {
      int required_new_fruit = eating_fruit(&snake, &fruit);
      if (required_new_fruit == 1) {
        Node *new_section = create_new_node(last_node);
        last_node->next = new_section;
        last_node = new_section;
        calcualte_new_fruit_position(&fruit, F_WIDTH - 1, F_HEIGHT - 1);
        current_points ++;
      }
      not_icanon_terminal_mode(&orig_termios);
      if (kbhit()) {
        new_direction = getch();
      }
      icanon_terminal_mode(&orig_termios);
      draw_top_points(current_points);
      draw_elements(F_WIDTH, F_HEIGHT, &snake, &fruit);
    }
    usleep(U_SECONDS); /* Delay before next cycle */
  } while (game_over != 1);

  return 0;
}
