#include "../lib/pcsh.h"
#include <errno.h>
#include <memory.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define VERSION "pcsh 0.1.0-alpha"

#define INPUT_LIMIT (8 << 8) // 2048
#define LIMIT_BIG (8 << 7)   // 1024
#define LIMIT_SMALL (8 << 4) // 128

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

  char *input = malloc(INPUT_LIMIT * sizeof(char));

  // Read the env
  char *s_username = malloc(LIMIT_SMALL * sizeof(size_t));
  char *s_hostname = malloc(LIMIT_SMALL * sizeof(size_t));
  char *s_pwd = malloc(LIMIT_BIG * sizeof(size_t));
  __uid_t s_uid = getuid();
  getlogin_r(s_username, LIMIT_SMALL);
  gethostname(s_hostname, LIMIT_SMALL);
  getcwd(s_pwd, LIMIT_BIG);

  // TODO: change this to the actual path, it shouldnt matter anyway
  setenv("SHELL", "/usr/bin/pcsh", 1);

  while (1) {
    // TODO: add prompt/theme support
    // TODO: support for rollback, ctrl+l, etc. (needs to get indiviual chars
    // instead of deliming at '\n')
    getcwd(s_pwd, LIMIT_BIG);
    printf("\r%s@%s %s> ", s_username, s_hostname, s_pwd);
    fgets(input, INPUT_LIMIT, stdin);

    if (input == NULL)
      continue;

    // 100 args each with 8<<7 (1024) characters
    char **args = malloc((LIMIT_BIG * sizeof(char)) * 100);
    for (int i = 0; i < 100; i++)
      args[i] = malloc(LIMIT_BIG * sizeof(char));
    int argsc = 0; // args count

    // seperate args with delim ' ' and '\x00'
    for (int i = 0, j = 0, k = 0; i <= strlen(input); i++, k++) {
      if (input[i] != ' ' && input[i] != '\x00' && input[i] != 10) {
        args[j][k] = input[i];
      } else {
        args[j][i] = '\x00';
        k = -1;
        j++, argsc++;
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
  }

  return 0;
}
