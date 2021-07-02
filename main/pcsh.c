#include <errno.h>
#include <memory.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
// TODO: add all builtins
#include "../lib/pcsh.h"

#define INPUT_LIMIT 2096

int main(int argc, char **argv) {
  // Ignore SIGINT
  signal(SIGINT, SIG_IGN);

  char *input = malloc(INPUT_LIMIT * sizeof(char));

  // Read the env
  char *s_username = malloc(128 * sizeof(size_t));
  char *s_hostname = malloc(128 * sizeof(size_t));
  char *s_pwd = malloc(1024 * sizeof(size_t));
  __uid_t s_uid = getuid();
  getlogin_r(s_username, 128);
  gethostname(s_hostname, 128);
  getcwd(s_pwd, 1024);

  // TODO: change this from an infinite condition
  while (1) {
    // TODO: add prompt/theme support
    getcwd(s_pwd, 1024);
    printf("%s@%s %s> ", s_username, s_hostname, s_pwd);
    fgets(input, INPUT_LIMIT, stdin);

    if (input == NULL)
      continue;

    // 100 args each with 1024 characters
    char **args = malloc((1024 * sizeof(char)) * 100);
    for (int i = 0; i < 100; i++)
      args[i] = malloc(1024 * sizeof(char));
    int argsc = 0; // args count

    // seperate args with delim ' ' and '\x00'
    for (int i = 0, j = 0, k = 0; i <= strlen(input); i++, k++) {
      if (input[i] != ' ' && input[i] != '\x00') {
        args[j][k] = input[i];
      } else {
        args[j][i] = '\x00';
        k = -1;
        j++, argsc++;
      }
    }

    printf("$ %s\n", args[0]);

    // execute command
    // strmcp is fucking cursed
    // if () {
    //   if (args[1] != NULL) {
    //     printf("cd %s\n", args[1]);
    //     if (!cd(args[1])) {
    //       perror("cd: ");
    //     }
    //   }
    // }
    return 0;
  }

  return 0;
}
