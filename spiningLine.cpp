#include <iostream>
#include <ncurses.h>
#include <string>
#include <unistd.h>

int defaultSpeed = 200000;

void spinnyRunner(bool *running);
void spinnySettings(bool *running);

int main() {
  bool running = true;

  // ncurses
  initscr();
  cbreak();
  noecho();
  nodelay(stdscr, TRUE);

  // the loop
  while (running == true) {
    spinnyRunner(&running);
    spinnySettings(&running);
  }
  endwin();

  return 0;
}

void spinnyRunner(bool *running) {
  char bars[4] = {'|', '/', '-', '\\'};
  const int num_frames = 4;
  std::string message =
      "Settings('s' = speed up, 'd' = decrease speed, 'q' = quit)";
  printw("%s\n", message.c_str());

  // The actual spining bar

  for (int i = 0; *running; i = (i + 1) % num_frames) {
    printw("[%c]\r", bars[i]);
    refresh();
    usleep(defaultSpeed);
    spinnySettings(running);
  }
}

void spinnySettings(bool *running) {
  int ch = getch();
  if (ch == ERR) {
    return;
  }

  switch (ch) {
  case 's':
    defaultSpeed -= 50000;
    if (defaultSpeed < 10000)
      defaultSpeed = 10000;
    break;
  case 'd':
    defaultSpeed += 50000;
    break;
  case 'q':
    *running = false;
    break;
  }
}
