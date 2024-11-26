#include "header.h"

char ch = '\0';

void print_spaces(int n, WINDOW *win) {
  for (int i = 0; i < n; i++) {
    wprintw(win, " ");
  }
}

int month_start_day(int y, int m) {
  int total_days = 1;

  total_days += (y - 1) * 365;
  int leaps = ((y - 1) / 4) + ((y - 1) / 400) - ((y - 1) / 100);
  total_days += leaps;

  for (int i = 0; i < m; i++) {
    total_days += num_days[i];
  }

  return total_days % 7;
}

int month_start_week(int y, int m) {
  int total_days = month_start_day(y, 0);
  int week = 1;
  for (int i = 0; i < m; i++) {
    total_days += num_days[i];
  }

  week += total_days / 7;
  return week;
}

int is_leap_year(int y) {
  return ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0);
}

int *get_current_date() {
  static int date[3];
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  date[0] = tm.tm_mday;
  date[1] = tm.tm_mon;
  date[2] = tm.tm_year + 1900;
  return date;
}

int year_char_len(int y) {
  int count = 0;
  while (y != 0) {
    y /= 10;
    count++;
  }

  return count;
}

void print_heading(int y, int m, int n, int w, int s, WINDOW *win) {
  int col = 0;
  int row = 4 + (m / 3) * 10;

  for (int i = 0; i < n; i++) {
    char bruh[222] = "";
    char year[21];
    strcat(bruh, month_name[m + i]);

    snprintf(year, 6, " %d", y);
    strcat(bruh, year);

    col += (int)(xMax / MENU_SIZE) / 6 - strlen(bruh) / 2;
    wattron(win, COLOR_PAIR(2));
    if (!inMenu) {
      print_centered(win, 2, bruh);
    } else {
      mvwprintw(win, row, col, "%s", bruh);
    }
    wattroff(win, COLOR_PAIR(2));
    col += (int)(xMax / MENU_SIZE) / 6 + strlen(bruh) / 2;
  }

  col = 0;
  row++;
  wmove(win, 6, xMax / 6 - 10);
  for (int j = 0; j < n; j++) {
    col += (int)(xMax / MENU_SIZE) / 6 - 10;
    if (w)
      print_spaces(3, win);
    if (!inMenu) {
      print_centered(win, 4, day_names);
    } else {
      mvwprintw(win, row, col, "%s", day_names);
    }
    col += (int)(xMax / MENU_SIZE) / 6 + 10;
  }
}

void print_day_numbers(int y, int m, int n, int w, WINDOW *win) {

  int remaining_days = 0;
  int start_day[n];
  int days_printed[n];
  int week[n];
  int *date = get_current_date();

  for (int i = 0; i < n; i++) {
    remaining_days += num_days[m + i];
    start_day[i] = month_start_day(y, m + i);
    days_printed[i] = 1;
    if (w)
      week[i] = month_start_week(y, m + i);
  }

  n--;

  int month_pointer = 0;
  int day_pointer = 0;
  int lines = 5 + (m / 3) * 10;
  if (inMenu)
    lines++;

  int col = 0;
  col += (int)(xMax / MENU_SIZE) / 6 - 10;

  (inMenu) ? wmove(win, lines++, col) : wmove(win, lines++, xMax / 6 - 10);

  while (remaining_days > 0) {
    if (w) {
      if (day_pointer == 0 &&
          days_printed[month_pointer] <= num_days[month_pointer + m]) {
        if (week[month_pointer] < 10) {
          print_spaces(1, win);
        }
        wprintw(win, "%d*", week[month_pointer]);
        week[month_pointer]++;
      } else if (day_pointer == 0) {
        print_spaces(3, win);
      }
    }

    if (start_day[month_pointer] > 0) {
      print_spaces(start_day[month_pointer] * 3, win);
      day_pointer = start_day[month_pointer];
      start_day[month_pointer] = -1;
    } else if (days_printed[month_pointer] > num_days[month_pointer + m]) {
      print_spaces((7 - day_pointer) * 3, win);
      day_pointer = 7;
    } else {
      if (y == date[2] && m + month_pointer == date[1] &&
          days_printed[month_pointer] == date[0]) {
        wattron(win, COLOR_PAIR(5));
        if (days_printed[month_pointer] < 10) {
          print_spaces(1, win);
        }
        wprintw(win, "%d", days_printed[month_pointer]);
        wattroff(win, COLOR_PAIR(5));
        // printf(RST);
        print_spaces(1, win);
        days_printed[month_pointer]++;
        day_pointer++;
        remaining_days--;
      } else {
        if (days_printed[month_pointer] < 10) {
          print_spaces(1, win);
        }
        wprintw(win, "%d ", days_printed[month_pointer]);
        days_printed[month_pointer]++;
        day_pointer++;
        remaining_days--;
      }
    }
    if (day_pointer % 7 == 0 && month_pointer != n) {
      print_spaces((int)(xMax / MENU_SIZE) / 6 * 2 - 21, win);
      month_pointer++;
      day_pointer = 0;
    } else if (day_pointer % 7 == 0 && month_pointer == n) {
      (inMenu) ? wmove(win, lines++, col) : wmove(win, lines++, xMax / 6 - 10);
      month_pointer = 0;
      day_pointer = 0;
    }
  }
}

void print_calendar(int y, int m, int n, int w, int s, WINDOW *win) {
  num_days[1] = (is_leap_year(y)) ? 29 : 28;
  print_heading(y, m, n, w, s, win);
  print_day_numbers(y, m, n, w, win);
}

void print_year(int y, int w, WINDOW *win) {
  int heading_len = (w) ? 78 : 64;

  char year[6];
  snprintf(year, 6, "%d", y);
  print_centered(win, 2, year);
  for (int i = 0; i < 4; i++) {
    print_calendar(y, i * 3, 3, w, 0, win);
  }
}

int init(int y, int m, int n, int w, WINDOW *win) {
  int *date = get_current_date();
  int s = 0;
  if (y > 0 && m < 0) {
    print_year(y, w, win);
    return 0;
  }
  if (y < 1) {
    y = date[2];
  }
  if (m < 0) {
    m = date[1];
  }
  if (n == 12) {
    print_year(y, w, win);
    return 0;
  } else if (m + n > 12) {
    n = 12 - m;
  } else if (n < 1) {
    n = 1;
  }
  if (n > 3) {
    while (n > 3) {
      wprintw(win, "\n");
      print_calendar(y, m, 3, w, 0, win);
      m += 3;
      n -= 3;
    }
    wprintw(win, "\n");
    print_calendar(y, m, n, w, 0, win);
    return 0;
  }
  if (n == 1) {
    s = 1;
  }
  print_calendar(y, m, n, w, s, win);
  return 0;
}

void update_appointment() {
  char str[100];
  FILE *appointments_file = fopen("../data/appointments.txt", "r");
  if (appointments_file == NULL) {
    printf("Error: Could not open appointments file\n");
    return;
  }

  mvwprintw(win_menu, yMax / MENU_SIZE - 3, 4, "%s",
            "Enter the line number to update: ");
  echo();
  wgetnstr(win_menu, str, sizeof(str));
  noecho();

  int line_to_update = atoi(str);

  FILE *new_appointments_file = fopen("../data/new_appointments.txt", "w");
  if (new_appointments_file == NULL) {
    printf("Error: Could not open new appointments file\n");
    return;
  }

  appointments_file = fopen("../data/appointments.txt", "r");
  if (appointments_file == NULL) {
    printf("Error: Could not open appointments file\n");
    return;
  }

  char *line = malloc(100);
  int line_number = 1;
  while (fgets(line, 100, appointments_file) != NULL) {
    if (line_number != line_to_update) {
      fprintf(new_appointments_file, "%s", line);
    } else {
      char *start_time = strtok(line, "~");
      char *end_time = strtok(NULL, "~");
      char *description = strtok(NULL, "~");

      struct tm start_time_tm;
      struct tm end_time_tm;

      strptime(start_time, "%Y-%m-%d %H:%M", &start_time_tm);
      strptime(end_time, "%Y-%m-%d %H:%M", &end_time_tm);

      char start_time_str[100];
      char end_time_str[100];

      strftime(start_time_str, 100, "%A, %d %B %Y - %H:%M", &start_time_tm);
      strftime(end_time_str, 100, "%A, %d %B %Y - %H:%M", &end_time_tm);

      remove_enter(start_time_str);
      remove_enter(end_time_str);
      remove_enter(description);

      wclear(win_menu);
      draw_window_base(inTitle);

      line_number = 3;
      mvwprintw(win_menu, line_number++, 3, "start date : %s", start_time_str);
      mvwprintw(win_menu, line_number++, 3, "end date   : %s", end_time_str);
      mvwprintw(win_menu, line_number++, 3, "description: %s", description);
      line_number++;

      mvwprintw(win_menu, line_number++, 3, "%s",
                "Enter the field to update (start, end, desc): ");
      wrefresh(win_menu);

      char field_to_update[100];
      echo();
      wgetnstr(win_menu, field_to_update, sizeof(field_to_update));

      if (strcmp(field_to_update, "start") == 0) {
        mvwprintw(win_menu, line_number++, 3, "%s",
                  "Enter the new start time: ");
        char new_start_time[100];
        wgetnstr(win_menu, new_start_time, sizeof(new_start_time));
        fprintf(new_appointments_file, "%s~%s~%s\n", new_start_time, end_time,
                description);
      } else if (strcmp(field_to_update, "end") == 0) {
        mvwprintw(win_menu, line_number++, 3, "%s", "Enter the new end time: ");
        char new_end_time[100];
        wgetnstr(win_menu, new_end_time, sizeof(new_end_time));
        fprintf(new_appointments_file, "%s~%s~%s\n", start_time, new_end_time,
                description);
      } else if (strcmp(field_to_update, "desc") == 0) {
        mvwprintw(win_menu, line_number++, 3, "%s",
                  "Enter the new description: ");
        char new_description[100];
        wgetnstr(win_menu, new_description, sizeof(new_description));
        fprintf(new_appointments_file, "%s~%s~%s\n", start_time, end_time,
                new_description);
      } else {
        mvwprintw(win_menu, line_number++, 3, "%s",
                  "Error: Invalid field to update\n");
        return;
      }
      noecho();
    }
    line_number++;
  }
  fclose(appointments_file);
  fclose(new_appointments_file);
  remove("../data/appointments.txt");
  rename("../data/new_appointments.txt", "../data/appointments.txt");
  free(line);
}

void sort_file() {
  char **lines = NULL;
  int n = 0;
  char line[256];

  FILE *in_file = fopen("../data/appointments.txt", "r");
  FILE *out_file = fopen("../data/new_appointments.txt", "w");

  while (fgets(line, sizeof(line), in_file)) {
    lines = realloc(lines, (n + 1) * sizeof(char *));
    lines[n++] = strdup(line);
  }
  fclose(in_file);

  qsort(lines, n, sizeof(char *), compare);

  for (int i = 0; i < n; i++) {
    fprintf(out_file, "%s", lines[i]);
  }
  fclose(out_file);

  remove("../data/appointments.txt");
  rename("../data/new_appointments.txt", "../data/appointments.txt");

  for (int i = 0; i < n; i++) {
    free(lines[i]);
  }
  free(lines);
}

void appointment_wrapper() {
  char str[10];
  int line;
  char selection = '\0';

  selection = wgetch(win_menu);

  if (selection == 'q')
    return;

  switch (selection) {
  case '1':
    wclear(win_menu);
    draw_window_base(inTitle);

    echo();
    add_appointment();
    noecho();
    draw_window_menu(inTitle);
    break;

  case '2':
    wclear(win_menu);
    draw_window_base(inTitle);
    ui_appointment(win_menu, 3);
    update_appointment();
    draw_window_menu(inTitle);
    break;

  case '3':
    wclear(win_menu);
    draw_window_base(inTitle);
    ui_appointment(win_menu, 3);
    mvwprintw(win_menu, yMax / MENU_SIZE - 3, 4, "%s",
              "Delete/check completed task number (0 to cancel): ");
    wrefresh(win_menu);

    echo();
    wgetnstr(win_menu, str, sizeof(str));
    noecho();

    line = atoi(str);
    remove_line(line, "../data/appointments.txt");
    draw_window_menu(inTitle);
    break;

  default:
    draw_window_menu(inTitle);
    break;
  }
  appointment_wrapper();
}

void tdl_wrapper() {
  char selection = '\0';
  selection = wgetch(win_menu);

  if (selection == 'q')
    return;

  switch (selection) {
  case '1':
    wclear(win_menu);
    draw_window_base(inTitle);
    ui_todo(win_menu, 3);
    mvwprintw(win_menu, yMax / MENU_SIZE - 4, 4, "%s", "Add a task: ");

    wrefresh(win_menu);
    echo();
    add_todo();
    noecho();
    draw_window_menu(inTitle);
    break;

  case '2':
    wclear(win_menu);
    draw_window_base(inTitle);
    ui_todo(win_menu, 3);
    mvwprintw(win_menu, yMax / MENU_SIZE - 3, 4, "%s",
              "Delete/check completed task number (0 to cancel): ");
    wrefresh(win_menu);

    echo();
    char str[10];
    wgetnstr(win_menu, str, sizeof(str));
    noecho();

    int line = atoi(str);
    remove_line(line, "../data/todolist.txt");
    draw_window_menu(inTitle);
    break;

  default:
    draw_window_menu(inTitle);
    break;
  }
  tdl_wrapper();
}

int main() {
  system("stty sane");

  initscr();
  cbreak();
  noecho();
  curs_set(0);
  signal(SIGWINCH, handle_winch);
  signal(SIGINT, handle_int);
  signal(SIGALRM, input_timeout);

  if (has_colors() == FALSE) {
    endwin();
    printf("Your terminal does not support color\n");
    exit(1);
  } else {
    start_color();
    use_default_colors();
    draw_window_main();
    inMenu = false;
  }

  while (ch != 'q') {
    setlocale(LC_ALL, "en_US.UTF-8");
    refresh();
    alarm(1);

    ch = getch();

    switch (ch) {
    case 'a':
      inMenu = true;
      strcpy(inTitle, " APPOINTMENT ");
      draw_window_menu(inTitle);
      appointment_wrapper();
      break;
    case 't':
      strcpy(inTitle, " TO DO LIST ");
      draw_window_menu(inTitle);
      tdl_wrapper();
      break;
    case 'c':
      inMenu = true;
      strcpy(inTitle, " CALENDAR ");
      draw_window_menu(inTitle);
      print_centered(win_menu, yMax / MENU_SIZE - 3,
                     "press any key to continue");
      refresh();
      wgetch(win_menu);
      break;
    default:
      draw_window_main();
      inMenu = false;
      break;
    }
    draw_window_main();
    inMenu = false;
  }
  end_screen();

  return 0;
}

void handle_winch(int sig) {
  endwin();
  refresh();
  clear();

  resizeterm(LINES, COLS);

  (inMenu) ? draw_window_menu(inTitle) : draw_window_main();
  doupdate();
}

void handle_int(int sig) {
  // endwin();
  exit(1);
}

void input_timeout(int sig) {
  alarm(1);
  clock_thread(win_clock);
}

void *clock_thread(void *arg) {
  if (inMenu)
    return NULL;

  WINDOW *win = (WINDOW *)arg;
  start_color();
  use_default_colors();
  init_pair(1, COLOR_YELLOW, -1);
  init_pair(2, COLOR_GREEN, -1);
  init_pair(3, COLOR_CYAN, -1);
  init_pair(4, COLOR_MAGENTA, -1);
  init_pair(5, COLOR_RED, -1);
  time_t current_time = time(NULL);
  struct tm *time_info = localtime(&current_time);

  int day = time_info->tm_mday;
  char suffix[5];

  if (day % 10 == 1 && day != 11) {
    snprintf(suffix, 5, "%dst", day);
  } else if (day % 10 == 2 && day != 12) {
    snprintf(suffix, 5, "%dnd", day);
  } else if (day % 10 == 3 && day != 13) {
    snprintf(suffix, 5, "%drd", day);
  } else {
    snprintf(suffix, 5, "%dth", day);
  }

  char time_string[9];
  char date_string[30];
  strftime(date_string, sizeof(date_string), "%A, %B", time_info);
  strftime(time_string, sizeof(time_string), "%H:%M:%S", time_info);

  strncat(date_string, " ", sizeof(date_string) - strlen(date_string) - 1);
  strncat(date_string, suffix, sizeof(date_string) - strlen(date_string) - 1);

  wattron(win, COLOR_PAIR(3));
  mvwprintw(win, 2, len_col(win, time_string), "%s", time_string);
  wattroff(win, COLOR_PAIR(3));

  strftime(date_string, sizeof(date_string), "%A, %B", time_info);

  strncat(date_string, " ", sizeof(date_string) - strlen(date_string) - 1);
  strncat(date_string, suffix, sizeof(date_string) - strlen(date_string) - 1);

  wattron(win_clock, COLOR_PAIR(1));
  mvwprintw(win_clock, 4, len_col(win_calendar, date_string), "%s",
            date_string);
  wattroff(win_clock, COLOR_PAIR(1));

  refresh();
  wrefresh(win);

  return NULL;
}

void ui_todo(WINDOW *win, int column) {
  struct list td;

  FILE *fp;
  fp = fopen("../data/todolist.txt", "r");

  int line = 2;
  while (fgets(td.list, sizeof(td.list), fp)) {
    size_t len = strcspn(td.list, "\n");
    memset(td.list + len, 0, 1);

    mvwprintw(win, line, column, "%d. %s", line - 1, td.list);
    line++;
  }

  fclose(fp);
}

void add_todo() {
  struct list td;

  FILE *fp;
  fp = fopen("../data/todolist.txt", "a");

  wmove(win_menu, yMax / MENU_SIZE - 3, 4);
  wgetnstr(win_menu, td.list, sizeof(td.list));

  fprintf(fp, "%s\n", td.list);
  fclose(fp);
}

void add_appointment() {
  struct tm *tm;
  struct Appointment *appointment;
  char tmp[101];

  FILE *appointments_file = fopen("../data/appointments.txt", "a");
  if (appointments_file == NULL) {
    printf("Error: Could not open appointments file\n");
    return;
  }

  mvwprintw(win_menu, 3, 3, "Enter start time (YYYY-MM-DD HH:MM): ");
  wrefresh(win_menu);
  wgetnstr(win_menu, tmp, sizeof(tmp));
  sscanf(tmp, "%d-%d-%d %d:%d", &appointment->startTime.tm_year,
         &appointment->startTime.tm_mon, &appointment->startTime.tm_mday,
         &appointment->startTime.tm_hour, &appointment->startTime.tm_min);

  mvwprintw(win_menu, 4, 3, "Enter end time (YYYY-MM-DD HH:MM): ");
  wrefresh(win_menu);
  wgetnstr(win_menu, tmp, sizeof(tmp));
  sscanf(tmp, "%d-%d-%d %d:%d", &appointment->endTime.tm_year,
         &appointment->endTime.tm_mon, &appointment->endTime.tm_mday,
         &appointment->endTime.tm_hour, &appointment->endTime.tm_min);

  char *description = malloc(100);
  mvwprintw(win_menu, 5, 3, "Enter description: ");
  wrefresh(win_menu);
  wgetnstr(win_menu, description, sizeof(description));
  appointment->description = description;

  fprintf(appointments_file, "%d-%d-%d %d:%d~%d-%d-%d %d:%d~%s\n",
          appointment->startTime.tm_year, appointment->startTime.tm_mon,
          appointment->startTime.tm_mday, appointment->startTime.tm_hour,
          appointment->startTime.tm_min, appointment->endTime.tm_year,
          appointment->endTime.tm_mon, appointment->endTime.tm_mday,
          appointment->endTime.tm_hour, appointment->endTime.tm_min,
          appointment->description);
  fclose(appointments_file);
  free(description);
}

void remove_line(int line_number, char dir[]) {
  FILE *fp, *temp;
  char ch;
  int current_line = 1;

  fp = fopen(dir, "r");
  temp = fopen("../data/temp.txt", "w");

  while ((ch = fgetc(fp)) != EOF) {
    if (current_line != line_number) {
      fputc(ch, temp);
    }
    if (ch == '\n')
      current_line++;
  }

  fclose(fp);
  fclose(temp);

  remove(dir);
  rename("../data/temp.txt", dir);
}

void menu_todo(WINDOW *win) {
  mvwprintw(win, yMax / MENU_SIZE - 6, 4, "%s", "1. Add Task");
  mvwprintw(win, yMax / MENU_SIZE - 5, 4, "%s", "2. Remove Task");
  mvwprintw(win, yMax / MENU_SIZE - 3, 4, "%s", "press 'q' to go back");
}

void menu_appointment(WINDOW *win) {
  mvwprintw(win, yMax / MENU_SIZE - 7, 4, "%s", "1. Add appointment");
  mvwprintw(win, yMax / MENU_SIZE - 6, 4, "%s", "2. Update appointment");
  mvwprintw(win, yMax / MENU_SIZE - 5, 4, "%s", "3. Remove appointment");
  mvwprintw(win, yMax / MENU_SIZE - 3, 4, "%s", "press 'q' to go back");
}

void ui_appointment(WINDOW *win, int lines) {
  time_t current_time = time(NULL);

  FILE *appointments_file = fopen("../data/appointments.txt", "r");
  if (appointments_file == NULL) {
    printf("Error: Could not open appointments file\n");
    return;
  }

  char *line = malloc(100);
  int line_number = 2;
  while (fgets(line, 100, appointments_file) != NULL) {
    char *start_time = strtok(line, "~");
    char *end_time = strtok(NULL, "~");
    char *description = strtok(NULL, "~");

    struct tm start_time_tm;
    struct tm end_time_tm;

    strptime(start_time, "%Y-%m-%d %H:%M", &start_time_tm);
    strptime(end_time, "%Y-%m-%d %H:%M", &end_time_tm);

    char start_time_str[100];
    char end_time_str[100];

    strftime(start_time_str, 100, "%A, %d %B %Y - %H:%M", &start_time_tm);
    strftime(end_time_str, 100, "%A, %d %B %Y - %H:%M", &end_time_tm);

    remove_enter(start_time_str);
    remove_enter(end_time_str);
    remove_enter(description);

    mvwprintw(win, line_number, lines, "start date: %s", start_time_str);
    line_number++;
    mvwprintw(win, line_number, lines, "end date: %s", end_time_str);
    line_number++;
    mvwprintw(win, line_number, lines, "description: %s", description);
    line_number++;
    line_number++;
  }
  fclose(appointments_file);
  free(line);
}

void remove_enter(char str[]) {
  size_t len = strcspn(str, "\n");
  memset(str + len, 0, 1);
}

void ui_calendar() {
  start_color();
  use_default_colors();
  init_pair(1, COLOR_YELLOW, -1);
  init_pair(2, COLOR_GREEN, -1);

  time_t current_time = time(NULL);
  struct tm *tm = localtime(&current_time);

  int year = tm->tm_year + 1900;
  int month = tm->tm_mon + 1;
  int day = tm->tm_mday;

  int days_in_month[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  if (month == 2 && year % 4 == 0) {
    if (year % 100 == 0 && year % 400 != 0) {
      days_in_month[2] = 28;
    } else {
      days_in_month[2] = 29;
    }
  }
  wattron(win_calendar, COLOR_PAIR(2));
  print_centered(win_calendar, 2, months[month - 1]);
  wattroff(win_calendar, COLOR_PAIR(2));
  print_centered(win_calendar, 4, "Mo Tu We Th Fr Sa Su");

  int first_day_of_month = tm->tm_wday - tm->tm_mday % 7;
  if (first_day_of_month < 0) {
    first_day_of_month += 7;
  }

  char date[7][25] = {"", "", "", "", "", "", ""};

  for (int i = 0; i < first_day_of_month; i++) {
    strcat(date[0], "   ");
  }

  int col = 0;
  int sisa;
  char i_str[15];
  char curr_date[20] = "Current date: ";
  for (int i = 1; i <= days_in_month[month]; i++) {
    if (i == day) {
      snprintf(i_str, sizeof i_str, "%2d", i);
      strcat(date[col], i_str);
      strcat(date[col], " ");
      strcat(curr_date, i_str);
    } else {
      snprintf(i_str, sizeof i_str, "%2d", i);
      strcat(date[col], i_str);
      strcat(date[col], " ");
    }

    if ((i + first_day_of_month) % 7 == 0) {
      col++;
    }

    sisa = 7 - (i + first_day_of_month) % 7;
  }

  for (int i = 0; i < sisa; i++) {
    strcat(date[col], "   ");
  }

  for (int i = 0; i < 6; i++) {
    print_centered(win_calendar, i + 5, date[i]);
  }

  char suffix[5];

  if (day % 10 == 1 && day != 11) {
    snprintf(suffix, 5, "%dst", day);
  } else if (day % 10 == 2 && day != 12) {
    snprintf(suffix, 5, "%dnd", day);
  } else if (day % 10 == 3 && day != 13) {
    snprintf(suffix, 5, "%drd", day);
  } else {
    snprintf(suffix, 5, "%dth", day);
  }

  char date_string[30];
  strftime(date_string, sizeof(date_string), "%A, %B", tm);

  strncat(date_string, " ", sizeof(date_string) - strlen(date_string) - 1);
  strncat(date_string, suffix, sizeof(date_string) - strlen(date_string) - 1);

  wattron(win_calendar, COLOR_PAIR(1));
  mvwprintw(win_calendar, 12, len_col(win_calendar, date_string), "%s",
            date_string);
  wattroff(win_calendar, COLOR_PAIR(1));
}

void ui_legend(int yWin, int xWin) {
  mvwprintw(win_legend, 1, xWin / 4 - 5, "%-16s", "q quit");
  mvwprintw(win_legend, 2, xWin / 4 - 5, "%-16s", "t to-do-list");
  mvwprintw(win_legend, 2, xWin - xWin / 4 - 8, "%-16s", "a appointment");
  mvwprintw(win_legend, 1, xWin - xWin / 4 - 8, "%-16s", "c yearly calendar");

  wattron(win_legend, COLOR_PAIR(4));
  mvwprintw(win_legend, 1, xWin / 4 - 5, "%-2s", "q");
  mvwprintw(win_legend, 2, xWin / 4 - 5, "%-2s", "t");
  mvwprintw(win_legend, 2, xWin - xWin / 4 - 8, "%-2s", "a");
  mvwprintw(win_legend, 1, xWin - xWin / 4 - 8, "%-2s", "c");
  wattroff(win_legend, COLOR_PAIR(4));
}

void draw_window_main() {
  clear();
  refresh();

  getmaxyx(stdscr, yMax, xMax);

  if (xMax % 2 != 0) {
    xMax--;
  }
  if (yMax % 2 != 0) {
    yMax--;
  }

  win_todolist = newwin(yMax - 20, xMax / 3, 0, 0);
  win_clock = newwin(7, xMax / 3, yMax - 20, 0);
  win_calendar = newwin(13, xMax / 3, yMax - 13, 0);
  win_appointment = newwin(yMax - 4, xMax - (xMax / 3) - 1, 0, xMax / 3 + 1);
  win_legend = newwin(4, xMax - (xMax / 3) - 1, yMax - 4, xMax / 3 + 1);

  box(win_todolist, 0, 0);
  box(win_appointment, 0, 0);
  box(win_calendar, 0, 0);
  box(win_clock, 0, 0);
  box(win_legend, 0, 0);
  refresh();

  sort_file();

  print_centered(win_todolist, 0, " TO DO LIST ");
  ui_todo(win_todolist, 3);
  print_centered(win_appointment, 0, " APPOINTMENT ");
  ui_appointment(win_appointment, 3);
  print_centered(win_calendar, 0, " CALENDAR ");
  init(0, -1, 0, 0, win_calendar);
  print_centered(win_clock, 0, " CLOCK ");
  clock_thread(win_clock);
  print_centered(win_legend, 0, " LEGENDS ");
  ui_legend(4, xMax - (xMax / 3) - 1);

  wrefresh(win_todolist);
  wrefresh(win_appointment);
  wrefresh(win_calendar);
  wrefresh(win_clock);
  wrefresh(win_legend);
}

void draw_window_menu(char title[]) {
  draw_window_base(title);
  if (!strcmp(" TO DO LIST ", title)) {
    inMenu = true;
    ui_todo(win_menu, 3);
    menu_todo(win_menu);
    wrefresh(win_menu);
  } else if (!strcmp(" APPOINTMENT ", title)) {
    inMenu = true;
    ui_appointment(win_menu, 3);
    menu_appointment(win_menu);
    wrefresh(win_menu);
  } else if (!strcmp(" CALENDAR ", title)) {
    inMenu = true;
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    int current_year = tm->tm_year + 1900;

    init(current_year, -1, 0, 0, win_menu);
    wrefresh(win_menu);
  } else {
    printf("something's wrong\n");
    exit(1);
  }
}

void draw_window_base(char title[]) {
  clear();
  refresh();
  getmaxyx(stdscr, yMax, xMax);

  win_menu = newwin(yMax / MENU_SIZE, xMax / MENU_SIZE,
                    (yMax - (yMax / MENU_SIZE)) / 2,
                    (xMax - (xMax / MENU_SIZE)) / 2 + 1);
  box(win_menu, 0, 0);
  print_centered(win_menu, 0, title);

  wrefresh(win_menu);
}

void print_centered(WINDOW *win, int start_row, char str[]) {
  int center_col = getmaxx(win) / 2;
  int center_str = strlen(str) / 2;
  int adjusted_col = center_col - center_str;

  mvwprintw(win, start_row, adjusted_col, "%s", str);
}

int len_col(WINDOW *win, char str[]) {
  int center_col = getmaxx(win) / 2;
  int center_str = strlen(str) / 2;
  return abs(center_col - center_str);
}

void end_screen() {
  inMenu = true;

  int width, height;
  initscr();
  clear();
  refresh();
  getmaxyx(stdscr, height, width);
  endwin();
  system("clear");
  printf("\e[?25l");
  setvbuf(stdout, NULL, _IOLBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);

  printf("\033[%d;%ldH%s", (height / 2 - 14),
         (width - strlen("ありがとうございます！\n") + 14) / 2,
         RED "ありがとうございます！\n" RESET);

  for (int i = 12; 0 <= i; i--)
    printf("\033[%d;%dH%s", (height / 2 - i), (width - 27) / 2, ayaya[12 - i]);

  printf("\033[%d;%ldH%s", (height / 2 + 2),
         (width - strlen("⭐ AYAYA! ⭐\n") + 4) / 2, YEL "⭐ AYAYA! ⭐\n" RESET);

  printf("\033[%d;%ldH%s", (height / 2 + 4),
         (width - strlen("press enter to continue\n") + 2) / 2,
         CYN "press enter to continue\n" RESET);

  printf("\033[%d;%ldH%s", (height),
         (width - strlen("© GNU GENERAL PUBLIC LICENSE Version 3\n") + 2) / 2,
         GRN "© GNU GENERAL PUBLIC LICENSE Version 3\n" RESET);
  getch();
  system("clear");
  printf("\e[?25h");
  system("stty sane");
}