#include <cstdlib>
#include <ctime>
#include <ncurses.h>
#include <unistd.h>

// TODO:Organize code and improve efficiency
#define HEIGHT 50
#define WIDTH 100

// Declare functions to link later
void input(WINDOW *win, int &dirY, int &dirX, char tailY[], char tailX[],
           int &posY, int &posX, bool &gameover);
void logic(int &foodY, int &foodX, int posY, int posX, char tailY[],
           char tailX[], unsigned int &score, int &length, bool &gameover,
           bool &appleEaten);
void draw(WINDOW *win, WINDOW *text, int posY, int posX, int length,
          char tailY[], char tailX[], int foodY, int foodX, unsigned int score,
          struct phrases phrases, bool &appleEaten);

// Snake phrases
struct phrases {
  // Eaten apple
  char apple1[7] = {"Yummy!"};
  char apple2[25] = {"Anoother oneee~ pleease~"};
  char apple3[11] = {"Soo tasty!"};
  char apple4[12] = {"Keep it up!"};
  char apple5[10] = {"Delicious"};
  char apple6[33] = {"Best apple ever eaten in my life"};
  // Gameover
  char gameover1[8] = {"Ouch..."};
  char gameover2[16] = {"AHII, MY TAIL!?"};
} phrases;

int main() {
  // Set random time seed
  srand(time(NULL));
  // Inizialize screen
  initscr();
  if (!has_colors()) {
    printw("Terminal doesn't support colors!");
    getch();
    return -1;
  }
  start_color();

  /*
    1. Stop when ^C is pressed
    2. Don't echo getch charaters
    3. Disble cursor
  */
  cbreak();
  noecho();
  curs_set(0);

  // Inizialize new windows
  WINDOW *win = newwin(HEIGHT, WIDTH, 0, 70);
  WINDOW *text = newwin(HEIGHT, WIDTH, HEIGHT, 70);
  /*
    1. Enable keys such as arrow keys, ...
    2. Make so getch is a non-blocking call
  */
  keypad(win, true);
  nodelay(win, false);

  // Refresh the main screen
  refresh();

  // Create color pairs
  init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(2, COLOR_GREEN, COLOR_BLACK);
  init_pair(3, COLOR_RED, COLOR_BLACK);

  // Main menu loop
  while (true) {
    // Set wgetch a blocking call
    nodelay(win, false);
    // Clear window screen
    wclear(win);
    wclear(text);
    // Draw a box
    wborder(win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    // Ascii art
    // Copyright (c) Jennifer E. Swofford
    mvwaddstr(win, 1, 0, R"(
                                      __    __    __    __                                         
                                     /  \  /  \  /  \  /  \                                        
____________________________________/  __\/  __\/  __\/  __\_______________________________________
___________________________________/  /__/  /__/  /__/  /__________________________________________
                                   | / \   / \   / \   / \  \____                                  
                                   |/   \_/   \_/   \_/   \    o \                                 
                                                           \_____/--<                              
              )");

    mvwaddstr(win, 10, 0, R"(
                       ad88888ba                            88                     
                      d8"     "8b                           88                     
                      Y8,                                   88                     
                      `Y8aaaaa,    8b,dPPYba,   ,adPPYYba,  88   ,d8    ,adPPYba,  
                        `"""""8b,  88P'   `"8a  ""     `Y8  88 ,a8"    a8P_____88  
                              `8b  88       88  ,adPPPPP88  8888[      8PP"""""""  
                      Y8a     a8P  88       88  88,    ,88  88`"Yba,   "8b,   ,aa  
                       "Y88888P"   88       88  `"8bbdP"Y8  88   `Y8a   `"Ybbd8"'  
              )");
    // Menu entries
    mvwprintw(win, (HEIGHT / 2), ((WIDTH / 2) - 7), "1 -> New Game");
    mvwprintw(win, ((HEIGHT / 2) + 1), ((WIDTH / 2) - 7), "q -> Quit");
    // Refresh window
    wrefresh(win);
    wrefresh(text);

    // TODO: Full menu
    int pressed = wgetch(win);
    switch (pressed) {
    case 'q':
      return 0;
    case 27:
      return 0;
    case '2': // Leaadboards
    case '1':
      // Tutorial
      mvwaddstr(win, (HEIGHT / 2), ((WIDTH / 2) - 7), " How To Play:");
      mvwaddstr(
          win, ((HEIGHT / 2) + 1), ((WIDTH / 2) - 31),
          "1) Push *ONE TIME* any of the arrow keys to move the snake head.");
      mvwaddstr(win, ((HEIGHT / 2) + 2), ((WIDTH / 2) - 25),
                "The movment is secured so you can't go to the opposite");
      mvwaddstr(win, ((HEIGHT / 2) + 3), ((WIDTH / 2) - 15),
                "direction of where you're going.");

      mvwaddstr(win, ((HEIGHT / 2) + 5), ((WIDTH / 2) - 22),
                "2) You can always quit the game by pressing 'q'");

      mvwaddstr(win, ((HEIGHT / 2) + 7), ((WIDTH / 2) - 12),
                "3) Hit you tail and it's");
      wattrset(win, COLOR_PAIR(3));
      mvwaddstr(win, ((HEIGHT / 2) + 9), ((WIDTH / 2) - 4), "GAMEOVER");
      wattroff(win, COLOR_PAIR(3));

      wgetch(win);

      // Declare and define variables
      int posY = HEIGHT / 2;
      int posX = WIDTH / 2;
      int foodY = rand() % (HEIGHT - 2) + 1;
      int foodX = rand() % (WIDTH - 2) + 1;
      int dirY = 0;
      int dirX = 1;
      unsigned int sleepTime = 100000;
      unsigned int score = 0;
      int length = 0;
      char tailY[(HEIGHT - 2) * (WIDTH - 2) + 1];
      char tailX[(WIDTH - 2) * (HEIGHT - 2) + 1];
      bool gameover = false;
      bool appleEaten = false;

      // Set wgetch a non-blockable call
      nodelay(win, true);

      // Main game loop
      while (!gameover) {
        // Clear window screen
        wclear(win);
        // Handle input
        input(win, dirY, dirX, tailY, tailX, posY, posX, gameover);
        // Handle game logic
        logic(foodY, foodX, posY, posX, tailY, tailX, score, length, gameover,
              appleEaten);
        // Handle game UI
        draw(win, text, posY, posX, length, tailY, tailX, foodY, foodX, score,
             phrases, appleEaten);

        // Difficulty meter
        if (length < 10) {
          sleepTime = 150000;
        }
        if (length >= 10 and length < 20) {
          sleepTime = 100000;
        }
        if (length >= 20 and length < 30) {
          sleepTime = 75000;
        }
        if (length >= 30) {
          sleepTime = 50000;
        }

        // Prevent the terminal from buffering
        usleep(sleepTime);
      }
      mvwprintw(win, (HEIGHT / 2), ((WIDTH / 2) - 5), "Game Over");
      mvwprintw(win, ((HEIGHT / 2) + 2), ((WIDTH / 2) - 6), "Score:%d", score);
      wrefresh(win);

      wclear(text);
      int n = rand() % 2 + 1;
      wattron(text, COLOR_PAIR(2));
      switch (n) {
      case 1:
        mvwprintw(text, 3, 25, "%s", phrases.gameover1);
      case 2:
        mvwprintw(text, 3, 25, "%s", phrases.gameover2);
      };
      wattroff(text, COLOR_PAIR(2));

      // Noo, snaky!! ;(
      wattron(text, COLOR_PAIR(1));
      mvwaddstr(text, 0, 1, "           /^\\/^\\ ");
      mvwaddstr(text, 1, 1, "         _|__|  X| ");
      mvwaddstr(text, 2, 1, "       /~     \\_/ \\ ");
      mvwaddstr(text, 3, 1, "   ____|__________  \\ ");
      mvwaddstr(text, 4, 1, "  /    \\_______   \\   \\ ");
      mvwaddstr(text, 5, 1, "              `\\     \\ ");
      mvwaddstr(text, 5, 1, "                 |     | ");
      wattroff(text, COLOR_PAIR(1));
      wrefresh(text);

      getch();
      break;
    }
    usleep(100000);
  }

  // Close the main window and screen aand end the program
  endwin();
  return 0;
}

void input(WINDOW *win, int &dirY, int &dirX, char tailY[], char tailX[],
           int &posY, int &posX, bool &gameover) {

  // Get window input as character
  int pressed = wgetch(win);

  // Movement logic
  if (pressed == KEY_LEFT and dirX != 1) {
    dirY = 0;
    dirX = -1;
  }
  if (pressed == KEY_RIGHT and dirX != -1) {
    dirY = 0;
    dirX = 1;
  }
  if (pressed == KEY_UP and dirY != 1) {
    dirY = -1;
    dirX = 0;
  }
  if (pressed == KEY_DOWN and dirY != -1) {
    dirY = 1;
    dirX = 0;
  }
  if (pressed == 'q') {
    gameover = true;
  }

  tailY[0] = posY;
  tailX[0] = posX;

  posY += dirY;
  posX += dirX;

  // Teleport snake when it suprass the window borders
  if (posY >= HEIGHT - 1) {
    posY = 1;
  }
  if (posY <= 0) {
    posY = HEIGHT - 1;
  }
  if (posX >= WIDTH - 1) {
    posX = 1;
  }
  if (posX <= 0) {
    posX = WIDTH - 1;
  }
}

void logic(int &foodY, int &foodX, int posY, int posX, char tailY[],
           char tailX[], unsigned int &score, int &length, bool &gameover,
           bool &appleEaten) {
  // Generate new apple when last one has been ate
  if (foodY == posY and foodX == posX) {
    foodY = rand() % (HEIGHT - 2) + 1;
    foodX = rand() % (WIDTH - 2) + 1;
    score += 120;
    length += 1;
    appleEaten = true;
  }

  // Make the apple spawn on the map and not on the snake tail
  for (int i = length; i != 0; i--) {
    if (foodY == tailY[i] and foodX == tailX[i]) {
      foodY = rand() % (HEIGHT - 2) + 1;
      foodX = rand() % (WIDTH - 2) + 1;
    }
  }

  // Check for gameover
  for (int i = length; i != 0; i--) {
    if (posY == tailY[i] and posX == tailX[i]) {
      gameover = true;
    }
  }
}

void draw(WINDOW *win, WINDOW *text, int posY, int posX, int length,
          char tailY[], char tailX[], int foodY, int foodX, unsigned int score,
          struct phrases phrases, bool &appleEaten) {
  // Set colors and print chaarachers on the window box
  wattrset(win, COLOR_PAIR(1));
  mvwaddstr(win, posY, posX, "*");
  wattrset(win, COLOR_PAIR(2));
  for (int i = length; i != 0; i--) {
    tailY[i] = tailY[i - 1];
    tailX[i] = tailX[i - 1];
    mvwaddstr(win, tailY[i], tailX[i], "*");
  }

  wattrset(win, COLOR_PAIR(3));
  mvwaddstr(win, foodY, foodX, "@");
  wattroff(win, COLOR_PAIR(3));

  // Draw the min box with the text in the appropiate windows
  box(win, 0, 0);
  mvwprintw(win, 0, (WIDTH / 2) - 8, "Welcome to snake!");

  if (appleEaten) {
    int n = rand() % 6 + 1;
    wclear(text);
    wattron(text, COLOR_PAIR(2));
    switch (n) {
    case 1:
      mvwprintw(text, 3, 25, "%s", phrases.apple1);
      break;
    case 2:
      mvwprintw(text, 3, 25, "%s", phrases.apple2);
      break;
    case 3:
      mvwprintw(text, 3, 25, "%s", phrases.apple3);
      break;
    case 4:
      mvwprintw(text, 3, 25, "%s", phrases.apple4);
      break;
    case 5:
      mvwprintw(text, 3, 25, "%s", phrases.apple5);
      break;
    case 6:
      mvwprintw(text, 3, 25, "%s", phrases.apple6);
      break;
    }
    wattroff(text, COLOR_PAIR(2));
    appleEaten = false;
  };

  // Hi snaky!
  wattrset(text, COLOR_PAIR(1));
  mvwaddstr(text, 0, 1, "           /^\\/^\\ ");
  mvwaddstr(text, 1, 1, "         _|__|  O| ");
  mvwaddstr(text, 2, 1, "\\/     /~     \\_/ \\ ");
  mvwaddstr(text, 3, 1, " \\____|__________/  \\ ");
  mvwaddstr(text, 4, 1, "        \\_______      \\ ");
  mvwaddstr(text, 5, 1, "                `\\     \\ ");
  mvwaddstr(text, 5, 1, "                 |     | ");
  wattroff(text, COLOR_PAIR(1));

  wattrset(text, COLOR_PAIR(3));
  mvwaddstr(text, 1, 65, "@");
  wattroff(text, COLOR_PAIR(3));
  mvwprintw(text, 1, 67, "-> X: %d", foodX);

  wattrset(text, COLOR_PAIR(3));
  mvwaddstr(text, 2, 65, "@");
  wattroff(text, COLOR_PAIR(3));
  mvwprintw(text, 2, 67, "-> Y: %d", foodY);

  wattrset(text, COLOR_PAIR(1));
  mvwaddstr(text, 1, 85, "*");
  wattroff(text, COLOR_PAIR(1));
  mvwprintw(text, 1, 87, "-> X: %d", posX);

  wattrset(text, COLOR_PAIR(1));
  mvwaddstr(text, 2, 85, "*");
  wattroff(text, COLOR_PAIR(1));
  mvwprintw(text, 2, 87, "-> Y: %d", posY);

  mvwprintw(text, 0, 65, "Apples: %d", score / 120);
  mvwprintw(text, 0, 85, "Score: %d", score);

  // Refresh windows
  wrefresh(win);
  wrefresh(text);
}
