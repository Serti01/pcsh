#pragma once

#include "../lib/pcsh.h"
#include <errno.h>
#include <memory.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define INPUT_LIMIT (8 << 8) // 2048
#define LIMIT_BIG (8 << 7)   // 1024
#define LIMIT_SMALL (8 << 4) // 128
