#include "pcsh.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

int cmpstr(char *s1, char *s2) {
  int s1l = strlen(s1);
  int s2l = strlen(s2);

  if (s1l != s2l)
    return 0;

  for (int i = 0; i < s1l; i++)
    if (s1[i] != s2[i])
      return 0;

  return 1;
}
