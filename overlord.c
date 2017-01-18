#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "network.h"

#define MAX_INPUT 1024

#define EXIT 0
#define DEFAULT 1
#define ORDER 2
#define SPY 3
#define MANAGE 4

char *prompt(char *prefix) {
  printf("%s", prefix);
	char *input = (char *) calloc(MAX_INPUT, sizeof(char));
	fgets(input, MAX_INPUT, stdin);
	input[strcspn(input, "\n")] = 0;
	return input;
}

void subServer(int connection) {

}

int startsWith(char *s1, char *s2) {
  return strncmp(s1, s2, strlen(s2)) == 0;
}

int main() {
  printf("Welcome, overlord.\n");
  printf("Type help for available commands.\n");

  int state = DEFAULT;
  char *input;
  socket_list *list;

	while (state) {
    switch (state) {
    case DEFAULT:
      input = prompt("[DEFAULT]> ");

      if (startsWith(input, "help")) {
      
      } else if (startsWith(input, "order")) {
        state = ORDER;
      } else if (startsWith(input, "spy")) {
        state = SPY;
      } else if (startsWith(input, "manage")) {
        state = MANAGE;
      } else if (startsWith(input, "exit")) {
        state = EXIT;
      }

      break;

    case ORDER:

      break;

    case SPY:

      break;

    case MANAGE:
      input = prompt("[MANAGE]> ");

      if (startsWith(input, "add")) {
        int sock = serverSocket(5001);

        int connection = serverConnect(sock);

        int pid = fork();

        if (pid == 0) {
          // child
          close(sock);
          subServer(connection);
          exit(0);
        } else {
          // parent
          close(connection);
        }
      } else if (startsWith(input, "remove")) {

      } else if (startsWith(input, "back")) {
        state = DEFAULT;
      }
      
      break;

    default:

      break;
    }
	}
	
	return 0;
}
