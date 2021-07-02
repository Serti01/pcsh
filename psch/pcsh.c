#include <errno.h>
#include <memory.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
// TODO: add all builtins
#include "../builtins/builtins.h"

#define INPUT_LIMIT 2096

int main(int argc, char **argv) {
  // Ignore SIGINT
  signal(SIGINT, SIG_IGN);

  char *input = malloc(INPUT_LIMIT * sizeof(char));

  // Read the env
  char *s_username = malloc(128 * sizeof(size_t));
  char *s_hostname = malloc(128 * sizeof(size_t));
  char *s_pwd = malloc(1028 * sizeof(size_t));
  __uid_t s_uid = getuid();
  getlogin_r(s_username, 128);
  gethostname(s_hostname, 128);
  getcwd(s_pwd, 1028);

  // TODO: change this from an infinite condition
  while (1) {
    // TODO: add prompt/theme support
    getcwd(s_pwd, 1028);
    printf("%s@%s %s> ", s_username, s_hostname, s_pwd);
    fgets(input, INPUT_LIMIT, stdin);

    // Check input in this order: builtins > path

    return 0;
  }

  return 0;
}