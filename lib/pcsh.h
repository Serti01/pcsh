#pragma once

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

int cd(char *path);
int cmpstr(char *s1, char *s2);
int getcursorpos(int *const row, int *const col);

#define VERSION "pcsh 0.2.0-alpha"

// TODO: add config file
#define SCROLLBACK_LIMIT 128