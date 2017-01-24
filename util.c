#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "util.h"

int min(int a, int b) {
  if (a > b) {
    return b;
  }

  return a;
}

int checkError(int descriptor, char *description) {
  if (descriptor == -1) {
    printf("%s error %d: %s\n", description, errno, strerror(errno));
    return -1;
  }

  return descriptor;
}

void prompt(char **input_ptr, char *prefix, int history) {
  if (*input_ptr) {
    free(*input_ptr);
    *input_ptr = NULL;
  }

  *input_ptr = readline(prefix);

  if (history && *input_ptr && **input_ptr) {
    add_history(*input_ptr);
  }
}

int startsWith(char *s1, char *s2) {
  return strncmp(s1, s2, strlen(s2)) == 0;
}

int equals(char *s1, char *s2) {
  return strcmp(s1, s2) == 0;
}

void leftShift(char *str, int offset) {
	char *trail = str;
	str += offset;

	while (*str) {
		*(trail++) = *(str++);
	}

	*trail = 0;
}

int readMessage(int fd) {
  memset(message, 0, MAX_MESSAGE_LENGTH * sizeof(char));
  return read(fd, message, MAX_MESSAGE_LENGTH);
}

