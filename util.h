#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "network.h"

int min(int a, int b);

int checkError(int descriptor, char *description);

void prompt(char **input_ptr, char *prefix, int history);

int startsWith(char *s1, char *s2);

void leftShift(char *str, int offset);
