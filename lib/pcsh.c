#include "pcsh.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

int cd(char *path) {
  struct stat *buf;
  buf = malloc(sizeof(struct stat));

  stat(path, buf);

  // If path is NOT dir
  if (!(buf->st_mode & S_IFDIR)) {
    errno = ENOTDIR;
    free(buf);
    return 1;
  } else
    chdir(path);

  free(buf);
  return 0;
}
