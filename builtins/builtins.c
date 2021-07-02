#include "builtins.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

int cd(char *path) {
  struct stat *buf;
  buf = malloc(sizeof(struct stat));

  stat(path, buf);

  if (!(buf->st_mode & S_IFDIR)) {
    errno = ENOTDIR;
    return 1;
  }

  free(buf);
  chdir(path);
  return 0;
}