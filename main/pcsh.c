#include "pcsh.h"

// lets of TODOs :tired_wojack:

int main(int argc, char **argv) {
  // CLI Arguments
  for (int i = 0; i < argc; i++) {
    if (cmpstr(argv[i], "--version")) {
      printf(VERSION "\n");
      return 0;
    } else if (cmpstr(argv[i], "--help")) {
      printf(VERSION " usage\n\
\n\
--help    Shows this help page\n\
--version Shows the version\n\
\n\
Sharing is caring, no copyright.\n");
      return 0;
    }
  }

  // Ignore SIGINT
  signal(SIGINT, SIG_IGN);
  static struct termios _told, _tnew;
  tcgetattr(STDIN_FILENO, &_told);
  _tnew = _told;
  _tnew.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &_tnew);
  // tcsetattr(STDIN_FILENO, TCSANOW, &_told);

  char *input = malloc((INPUT_LIMIT + 1) * sizeof(char));
  char *prompt = malloc(LIMIT_SMALL * sizeof(char));
  int row = 0, col = 0;

  // Read the env
  char *s_username = malloc(LIMIT_SMALL * sizeof(char));
  char *s_hostname = malloc(LIMIT_SMALL * sizeof(char));
  char *s_pwd = malloc(LIMIT_BIG * sizeof(char));
  __uid_t s_uid = getuid();
  getlogin_r(s_username, LIMIT_SMALL);
  gethostname(s_hostname, LIMIT_SMALL);
  getcwd(s_pwd, LIMIT_BIG);

  // TODO: change this to the actual path, it shouldnt matter anyway
  setenv("SHELL", "/usr/bin/pcsh", 1);

  while (1) {
    // TODO: add prompt/theme support
    getcwd(s_pwd, LIMIT_BIG);
    sprintf(prompt, "\r%s@%s %s> ", s_username, s_hostname, s_pwd);
    printf("\r%s", prompt);

    // TODO: support for rollback, ctrl+l, etc.
    // By making this a for loop, we can avoid a buffer overflow because
    // anything past INPUT_LIMIT doesnt get put into input
    for (int i = 0; i < INPUT_LIMIT; i++) {
      getcursorpos(&row, &col);
      char nextc = fgetc(stdin);

      if (nextc >= '!' && nextc != 127) {
        input[i] = nextc;
        putchar(nextc);
      } else if (nextc == 10) { // return / enter
        input[i] = '\x00';
        putchar('\n');
        break;
      } else if (nextc == 27 && fgetc(stdin) == 91) { // escape
        switch (fgetc(stdin)) {
        case 65: // up
          // scrollback
          break;
        case 66: // down
          // scrollback
          break;
        // TODO: make text insert and not replace
        // TODO: limit select to only the input, restrict boundaries
        case 68: // left
          i--, i--;
          printf("\e[1D");
          break;
        case 67: // right
          printf("\e[1C");
          break;
        }
      } else if (nextc == ' ') {
        input[i] = ' ';
        putchar(' ');
      } else if (nextc == 127) { // backspace
        // hacky, i know.
        if (col <= strlen(prompt) + 1)
          printf("\e[%luG", strlen(prompt) + 1), i = 1;
        input[(i--)] = '\x00';
        printf("\b \b");
        i--;
      } else {
        i--;
      }
    }

    if (input == NULL)
      continue;

    char **args = malloc((LIMIT_BIG * sizeof(char)) * 100);
    args[0] = malloc(LIMIT_BIG * sizeof(char));
    int argsc = 0; // args count

    // seperate args with delim ' ' and '\x00'
    for (int i = 0, j = 0, k = 0; i <= strlen(input); i++, k++) {
      if (input[i] != ' ' && input[i] != '\x00' && input[i] != 10) {
        args[j][k] = input[i];
      } else {
        args[j][i] = '\x00';
        k = -1;
        j++, argsc++;
        args[j] = malloc(LIMIT_BIG * sizeof(char));
      }
    }

    // execute command
    // TODO: aliases
    if (cmpstr(args[0], "cd")) {
      if (args[1] != NULL) {
        if (chdir(args[1]) != 0)
          perror("cd");
      }
      continue;
      // FIXME: when spamming 2048 'a's into the input, typing exit afterwards
      // resulted in "malloc(): corrupted top size"
    } else if (cmpstr(args[0], "exit"))
      break;

    // TODO: variables and substitution

    // craft a system command
    char *cmd = malloc(LIMIT_BIG * sizeof(char));
    for (int i = 0, j = 0, k = 0; i < strlen(input); i++, k++)
      if (args[j][k] == ' ' || args[j][k] == '\x00')
        j++, k = -1, cmd[i] = ' ';
      else
        cmd[i] = args[j][k];

    // TODO: instead of relying on sh to execute commands, do it ourselves
    system(cmd);

    free(args);
    free(cmd);
  }

  free(input);
  free(prompt);
  free(s_hostname);
  free(s_username);
  free(s_pwd);

  return 0;
}
